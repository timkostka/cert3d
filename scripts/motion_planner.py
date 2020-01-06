"""
This file implements a constant jerk motion planner.

This is only a partial planner and implements XY motion with constant
constraints on all paths.

"""

import math

import numpy

from point2d import Point2D


# steps per mm
steps_per_mm = 80

# hold points to traverse

# maximum speed (mm/s)
max_speed = 80

# maximum acceleration (mm/s^2)
max_accel = 600

# maximum jerk (mm/s^3)
max_jerk = 30000


class PathSegment:
    """A PathSegment is a constant-jerk movement of a stepper."""

    def __init__(self):
        pass


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


theta = [i * math.pi / 100 for i in reversed(range(1, 100))]
x = [(t, get_transition(t, 20)) for t in theta]
for x in x:
    print(x)
exit(0)


def get_final_speed(starting_speed, distance):
    """
    Return the max speed we can achieve if we accelerate as much as allowed.

    The starting and final acceleration are constrained to be zero.

    For constant jerk, the max spee will be achieved by a symmetric triangle
    acceleration profile, with positive jerk in the first half and negative
    jerk in the second.  For this path, the average velocity is (vs + vf) / 2
    where vs is the starting velocity and vf is the final velocity.  If we let
    the time it takes to traverse the path be td, then:
    * d == td * (vs + vf) / 2     (1)
    The area under the acceleration curve is the change in velocity:
    * vf - vs = 1 / 4 * j * td^2     (2)
    This gives us two equations for two unknowns (vf, td).  We can solve:
    * vf == 2 * d / td - vs
    * 2 * d / td - 2 * vs = 1 / 4 * j * td^2
    * 0 = 1 / 4 * j * td^3 + 2 * vs * td - 2 * d
    This last equation has exactly one positive real solution, since vs, d and
    j are all positive.

    """
    d = distance
    vs = starting_speed
    j = max_jerk
    roots = numpy.roots([0.25 * j, 0, 2 * vs, -2 * d])
    print(roots)
    roots = [x.real for x in roots if numpy.isreal(x) and x > 0]
    assert len(set(roots)) == 1
    td = roots[0]
    vf = 2 * d / td - vs
    # find max accel for this path
    a_max = j * td / 2
    # if this satisfies our max accel, we know ending velocity
    if a_max <= max_accel:
        return min(vf, max_speed)
    # need to truncate path
    raise NotImplementedError


def plan_route(points):
    print("Routing %d points" % len(points))
    # starting point
    p0 = points[0]
    # starting speed
    vs = 0
    for p1, p2 in zip(points[1:], points[2:]):
        print("%s -> %s -> %s" % (p0, p1, p2))
        # see max speed we can end at
        d = p0.distance_to(p1)
        vf = get_final_speed(vs, d)
        print("vf =", vf)
        p0 = p1
    # do last segment
    p1 = points[-1]
    print("%s -> %s" % (p0, p1))


def run_example():
    """Run some example functionality."""
    # hold Point2D points to traverse through
    points = []
    # add points in a square
    points.append(Point2D(0, 0))
    points.append(Point2D(10, 0))
    points.append(Point2D(10, 10))
    points.append(Point2D(0, 10))
    points.append(Point2D(0, 0))
    # plan the route
    segments = plan_route(points)


if __name__ == "__main__":
    run_example()
