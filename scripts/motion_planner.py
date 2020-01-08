"""
This file implements a constant jerk motion planner.

This is only a partial planner and implements XY motion with constant
constraints on all paths.

The way we route paths is the following:
* Start with a list of XY points, assume linear segments between them.
* For each segment:
* - Check max speed at corner if we accelerate fully.
* - Set transition distance at corner based on this max speed limit
* - Create path to accelerate to junction point
* - Create path along transition

Each corner will have a transition speed and distance.

"""

import math

import numpy

from point2d import Point2D


# steps per mm
steps_per_mm = 80

# hold points to traverse

# maximum speed (mm/s)
max_speed = 40

# maximum acceleration (mm/s^2)
max_accel = 6000

# maximum jerk (mm/s^3)
max_jerk = 80000

# maximum junction deviation (mm)
max_deviation = 0.2


class PathSegment:
    """A PathSegment is a constant-jerk movement of a stepper."""

    def __init__(self, dt=0.0, x0=0.0, v0=0.0, a0=0.0, j0=0.0):
        # segment duration
        self.dt = dt
        # initial position
        self.x0 = x0
        # initial velocity
        self.v0 = v0
        # initial acceleration
        self.a0 = a0
        # constant jerk for entire segment
        self.j0 = j0


def get_max_transition_entry_speed(angle, distance):
    """Return the max entry speed for the given transition."""
    test = max_speed
    result = get_transition(angle, test)
    # TODO: make this a better estimate
    if result > distance:
        test *= distance / result
        result2 = get_transition(angle, test)
    return test


def get_transition(angle, entry_speed):
    """
    Return the transition characteristics.

    angle: angle change in radiants
    entry_speed: initial speed in mm/s

    Return value is distance away from corner to start transition.

    """
    # alias variables
    s = entry_speed
    theta = angle
    # calculate constants
    vx = s * math.sin(theta / 2)
    vy = s * math.cos(theta / 2)
    # get max accel if path is of first type
    d = s * math.sqrt(2 * s * math.cos(theta / 2) / max_jerk)
    dt = (2 * d * math.sin(theta / 2)) / (s * math.sin(theta / 2))
    amax = 0.5 * dt * max_jerk
    # print("amax =", amax)
    # if this is below our limit, path is of first type
    # print("t1d =", d, "t1dt =", dt)
    dx1 = d * math.sin(theta / 2)
    if amax <= max_accel:
        dx = 2 * d * math.sin(theta / 2)
        dt2 = dx / (s * math.sin(theta / 2))
        vy2 = 0.125 * max_jerk * dt ** 2
        assert abs(dt2 / dt - 1) < 1e-6
        assert abs(vy2 / vy - 1) < 1e-6
        return d
    # else path is of the second type
    d = (
        s ** 2 * math.cos(theta / 2) / max_accel
        + 0.5 * s * max_accel / max_jerk
    )
    # dt3 = 2 * vy / max_accel + max_accel / max_jerk
    # tramp = max_accel / max_jerk
    # tflat1 = 2 * vy / max_accel - tramp
    # dx3 = vx * (tramp + tflat1 / 2)
    # dx = 2 * d * math.sin(theta / 2)
    # dt2 = dx / vx
    # assert abs(dt2 / dt - 1) < 1e-6
    # print("t2d =", d)
    return d


def get_max_exit_speed(entry_speed, distance):
    """
    Return the max speed we can achieve if we accelerate as much as allowed.

    For constant jerk, the max speed will be achieved by a symmetric triangle
    acceleration profile, with positive jerk in the first half and negative
    jerk in the second.  For this path, the average velocity is (vs + vf) / 2
    where vs is the starting velocity and vf is the final velocity.  If we let
    the time it takes to traverse the path be td, then:
    * d == td * (vs + vf) / 2     (1)
    The area under the acceleration curve is the change in velocity:
    * vf - vs == 1 / 4 * j * td^2     (2)
    This gives us two equations for two unknowns (vf, td).  We can solve:
    * vf == 2 * d / td - vs
    * 2 * d / td - 2 * vs = 1 / 4 * j * td^2
    * 0 = 1 / 4 * j * td^3 + 2 * vs * td - 2 * d
    This last equation has exactly one positive real solution, since vs, d and
    j are all positive.

    """
    d = distance
    vs = entry_speed
    j = max_jerk
    roots = numpy.roots([0.25 * j, 0, 2 * vs, -2 * d])
    # print(roots)
    roots = [x.real for x in roots if numpy.isreal(x) and x > 0]
    assert len(set(roots)) == 1
    td = roots[0]
    vf = 2 * d / td - vs
    # find max accel for this path
    a_max = j * td / 2
    # if this satisfies our max accel, we know ending velocity
    if a_max <= max_accel:
        return min(vf, max_speed)
    # we reach the max acceleration
    # calculate d1 to reach max accel
    d1 = max_accel / max_jerk
    # now find maximum possible d2
    d2 = (
        distance
        - entry_speed * d1
        - 1 / 2 * max_accel * d1 ** 2
        + 1 / 6 * max_jerk * d1 ** 3
    ) / (max_accel * d1)
    # return final speed
    exit_speed = (
        entry_speed + 1 / 2 * max_accel * d1 * (1 - d1) + max_accel * d2
    )
    return min(exit_speed, max_speed)


def get_velocity_change_distance(starting_speed, ending_speed):
    """
    Return the distance required to change between speeds.

    This is subject to the contrains of max_speed and max_jerk.

    """
    # trivial case
    if starting_speed == ending_speed:
        return 0.0
    # get absolute difference in speed
    delta = abs(starting_speed - ending_speed)
    assert delta > 0
    # get lower of the two speeds
    lower_speed = min(starting_speed, ending_speed)
    # get time required to ramp to this speed
    d1 = math.sqrt(delta / max_jerk)
    a_max = d1 * max_jerk
    d2 = 0
    # if we exceed accel limit, change durations to match
    if a_max > max_accel:
        # delta = max_accel * (d1 + d2)
        d1 = max_accel / max_jerk
        d2 = delta / max_accel - d1
    # calculate average velocity above minimum
    avg_vel = (2 * d1 * max_jerk + d2 * max_accel) / (2 * d1 + d2)
    # calculate total distance and time
    total_duration = 2 * d1 + d2
    total_distance = lower_speed + total_duration * avg_vel
    return total_distance


def get_junctions(segments, start_distance=0.0, start_speed=0.0):
    """Return junction distances and speeds for the given segments."""
    # segment entry speed
    entry_speed = start_speed
    # hold corner distances and speeds
    corners = []
    # segment entry distance
    entry_distance = start_distance
    for p1, p2, p3 in segments:
        # get lengths of legs
        l1 = p1.distance_to(p2)
        l2 = p2.distance_to(p3)
        assert l1 > 0 and l2 > 0
        # get angle
        cosine = (p1 - p2).dot(p3 - p2) / (l1 * l2)
        if cosine < -1:
            cosine = -1
        elif cosine > 1:
            cosine = 1
        theta = math.acos(cosine)
        # distance can't be more than half the sement length
        max_distance_1 = min(l1, l2) / 2
        # distance restriction based on max junction deviation
        max_distance_2 = 3 * max_deviation / math.cos(theta / 2)
        # get max speed at corner
        max_exit_speed_1 = get_max_exit_speed(entry_speed, l1 - entry_distance)
        # get distance
        max_distance_3 = get_transition(theta, max_exit_speed_1)
        # find distance
        delta = min(max_distance_1, max_distance_2, max_distance_3)
        # find max entry speed for this delta
        max_exit_speed_2 = get_max_transition_entry_speed(theta, delta)
        speed = min(max_exit_speed_1, max_exit_speed_2)
        # find entry speed
        corners.append((delta, speed))
    # return corner characteristics
    return corners


def plan_route(points):
    print("Routing %d points" % len(points))
    # starting point
    p0 = points[0]
    # starting speed
    # hold all corners
    # hold all path line segments
    segments = [
        (p1, p2, p3)
        for p1, p2, p3 in zip(points[:-2], points[1:-1], points[2:])
    ]
    # hold segments in reverse
    reversed_segments = [(p3, p2, p1) for p1, p2, p3 in reversed(segments)]
    # get junction speeds going in reverse
    reverse_junctions = list(reversed(get_junctions(reversed_segments)))
    # get junction speeds going forwards
    junctions = get_junctions(segments)
    print(junctions)
    print(reverse_junctions)
    # take minimum speed


def run_example():
    """Run some example functionality."""
    # hold Point2D points to traverse through
    points = []
    # add points in a square
    points.append(Point2D(0, 0))
    points.append(Point2D(10, 0))
    points.append(Point2D(10, 10))
    points.append(Point2D(0, 10))
    points.append(Point2D(0, 9))
    # plan the route
    segments = plan_route(points)


if __name__ == "__main__":
    run_example()
