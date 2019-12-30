"""
This file shows some examples of our motion post-processing scheme.

"""

import math

import matplotlib.pyplot as plt


# natural frequency (Hz)
omega_n = 500

# damping ratio
zeta = 0.15

# steps per mm
steps_per_mm = 80

# divisor for default timestep
# (set to 1 to use default timestep)
timestep_divider = 1000


def generate_ramp(ramp_time=0.25, amplitude=1, step_count=10, end_time=0.5):
    """Return a ramp step."""
    steps = []
    for i in range(step_count + 1):
        steps.append((i * ramp_time / step_count, amplitude * i / step_count))
    if end_time > ramp_time:
        steps.append((end_time, amplitude))
    return steps


def generate_motion(distance=10, speed=100):
    """
    Return a step corresponding to a linear motion.

    distance: distance in mm
    speed: speed in mm/s

    """
    step_count = distance * steps_per_mm
    dt = 1 / (speed * steps_per_mm)
    return list((i * dt, i / steps_per_mm) for i in range(int(step_count + 1)))


def generate_trapezoid_motion(distance=10, speed=100, acceleration=1000):
    """
    Return a step corresponding to a linear motion.

    distance: distance in mm
    speed: max speed in mm/s
    acceleration: max acceleration in mm/s^2

    """
    # find number of steps
    step_count = distance * steps_per_mm
    # find accel if we accel fully
    # x = 1/2 * a * t^2 = distance / 2
    # v = a * t
    # --> t = sqrt(distance / a)
    # --> v = sqrt(distance * a)
    max_speed = math.sqrt(distance * acceleration)
    points = []
    if max_speed <= speed:
        # first part (x = 1/2 * a * t^2)
        a = acceleration
        for i in range(int(step_count / 2)):
            # find x at this step point
            x = i / steps_per_mm
            # find t at this step
            t = math.sqrt(2 * x / a)
            points.append((t, x))
        # second part: (x = xf - 1/2 * a * (tf-t)^2)
        # --> 2 * (xf - x) / a = (tf-t)^2
        tf = 2 * math.sqrt(distance / a)
        xf = distance
        for i in range(int(step_count / 2), int(step_count)):
            x = i / steps_per_mm
            t = tf - math.sqrt(2 * (xf - x) / a)
            points.append((t, x))
    else:
        pass
    return points


def get_discrete_steps(steps):
    """Return steps with points added to make it piecewise continuous."""
    assert steps
    new_steps = []
    new_steps.append(steps[0])
    for step in steps[1:]:
        if new_steps[-1][1] != step[1]:
            new_steps.append((step[0], new_steps[-1][-1]))
        new_steps.append(step)
    return new_steps


def plot_steps(steps):
    """Show a plot of the given (x, y) data."""
    # create new data points
    new_steps = get_discrete_steps(steps)
    x, y = zip(*new_steps)
    plt.plot(x, y)
    plt.ylabel("Position")
    plt.xlabel("Time")
    plt.show()


def plot_solution(solution, steps=None):
    """Show a plot of the given (t, x, v) data."""
    # plot steps behind everything
    if steps:
        t, x = zip(*get_discrete_steps(steps))
        plt.plot(t, x)
    # read position and velocity
    t, x, v = zip(*solution)
    # plot position
    # plt.plot(t, x)
    # plot speed
    # s = [abs(x) for x in v]
    # plt.plot(t, v)
    # plot acceleration
    a = [
        (v2 - v1) / (t2 - t1)
        for t1, t2, v1, v2 in zip(t[:-1], t[1:], v[:-1], v[1:])
    ]
    a = [x for pair in zip(a, a) for x in pair]
    ta = [x for pair in zip(t, t) for x in pair]
    del ta[0]
    del ta[-1]
    # ta = [(t1 + t2) / 2 for t1, t2 in zip(t[:-1], t[1:])]
    plt.plot(ta, a)
    plt.ylabel("Position")
    plt.xlabel("Time")
    plt.show()


def solve_motion(steps):
    """Solve the system of equations and return (t, x, v) data."""
    assert steps
    # timestep
    dt0 = 1 / (omega_n * 4 * timestep_divider)
    # initial conditions
    t0, x0 = steps[0]
    v0 = 0
    x_last = x0
    # solve for k and c (assuming m = 0)
    k = omega_n ** 2
    c = 2 * omega_n * zeta
    # add first point
    points = [(t0, x0, v0)]
    # solve for each pair
    for t_next, x_next in steps[1:]:
        assert t_next >= t0
        # if no change in time, just move on
        if t_next == t0:
            x_last = x_next
            continue
        # solve system until we reach the target time
        while t0 != t_next:
            # find timestep
            if (t_next - t0) / dt0 < 1.0001:
                t1 = t_next
                dt = t1 - t0
            else:
                dt = dt0
                t1 = t0 + dt
            # solve system
            v1 = (
                v0 * (1 - dt * c / 2)
                + dt * k * x_last
                - dt * k * (x0 + v0 * dt / 2)
            ) / (1 + dt * c / 2)
            # find new position
            x1 = x0 + dt * (v0 + v1) / 2
            # cycle variables
            t0, x0, v0 = t1, x1, v1
            # add this point
            points.append((t0, x0, v0))
        # save last x value
        x_last = x_next
    # return solution points
    return points


def example():
    """Run an example."""
    if False:
        steps = generate_ramp(
            ramp_time=0.010, amplitude=1, step_count=500, end_time=0.05
        )
        print("Ramp is %d points" % len(steps))
        solution = solve_motion(steps)
        print("Solution is %d points" % len(solution))
        plot_solution(solution, steps)
    if True:
        # plot_steps(generate_motion())
        # plot_steps(generate_trapezoid_motion())
        steps = generate_motion()
        steps = generate_trapezoid_motion()
        solution = solve_motion(steps)
        plot_solution(solution, steps)


# run example if run as script
if __name__ == "__main__":
    plt.close("all")
    example()
