"""
This file shows some examples of our motion post-processing scheme.

In contrast to the other script, this script solves for the position of the stepper motor first, and then solves for the position of the hot end.

"""

import math
import ctypes

import matplotlib
import matplotlib.pyplot as plt


# stepper motor steps per mm of hotend movement
steps_per_mm = 80

# natural frequency of the hotend (Hz)
hotend_frequency = 500

# damping ratio
hotend_damping = 0.15

# divisor for default timestep
# (set to 1 to use default timestep)
hotend_timestep_divider = 10

# natural frequency of the stepper motor (Hz)
stepper_frequency = 5000

# damping ratio of stepper motor
stepper_damping = 0.95

# natural frequency of the stepper motor (Hz)
stepper_frequency = 500

# damping ratio of stepper motor
stepper_damping = 1.0


# divisor for stepper timestamp
# (set to 1 to use default timestep)
stepper_timestep_divider = 10


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
    # read position and velocity
    if len(solution[0]) == 2:
        plt.plot(t, x)
        t, x = zip(*solution)
        plt.ylabel("Position")
        plt.xlabel("Time")
    else:
        # breka into subplots
        fig, ax = plt.subplots(nrows=3, ncols=1)
        plt.subplot(3, 1, 1)
        # plot position on top plot
        plt.plot(t, x)
        # get components
        t, x, v = zip(*solution)
        plt.plot(t, x)
        # calculate acceleration
        a = [
            (v2 - v1) / (t2 - t1)
            for t1, t2, v1, v2 in zip(t[:-1], t[1:], v[:-1], v[1:])
        ]
        ta = [x for pair in zip(t, t) for x in pair]
        del ta[0]
        del ta[-1]
        ta = [(t1 + t2) / 2 for t1, t2 in zip(t[:-1], t[1:])]
        # plot velocity
        plt.subplot(3, 1, 2)
        plt.plot(t, v)
        # plot acceleration
        plt.subplot(3, 1, 3)
        plt.plot(ta, a)

    # plot speed
    # s = [abs(x) for x in v]
    # plt.plot(t, v)
    # plot acceleration
    # a = [x for pair in zip(a, a) for x in pair]
    # ta = [x for pair in zip(t, t) for x in pair]
    # del ta[0]
    # del ta[-1]
    # ta = [(t1 + t2) / 2 for t1, t2 in zip(t[:-1], t[1:])]
    # plt.plot(ta, a)
    plt.show()


def plot_solution_2(steps, stepper, hotend):
    """Show a plot of the given (t, x, v) data."""
    # break into subplots
    plt.subplots(nrows=3, ncols=1, figsize=(12, 14))
    # plot steps behind everything
    t1, x1 = zip(*get_discrete_steps(steps))
    t2, x2 = zip(*stepper)
    t3, x3, v3 = zip(*hotend)
    # calculate acceleration
    a3 = [
        (v2 - v1) / (t2 - t1)
        for t1, t2, v1, v2 in zip(t3[:-1], t3[1:], v3[:-1], v3[1:])
    ]
    ta3 = [x for pair in zip(t3, t3) for x in pair]
    del ta3[0]
    del ta3[-1]
    ta3 = [(t1 + t2) / 2 for t1, t2 in zip(t3[:-1], t3[1:])]
    # plot position on top plot
    plt.subplot(3, 1, 1)
    plt.plot(t1, x1)
    plt.plot(t2, x2)
    plt.plot(t3, x3)
    plt.ylabel("Position (mm)")
    plt.legend(["Input", "Stepper", "Hotend"])
    # plot velocity
    plt.subplot(3, 1, 2)
    plt.plot(t3, v3)
    plt.ylabel("Velocity (mm/s)")
    # plot acceleration
    plt.subplot(3, 1, 3)
    plt.plot(ta3, a3)
    plt.ylabel("Acceleration (mm/s^2)")

    # get components
    # plt.plot(t, x)
    # # plot velocity
    # plt.subplot(3, 1, 2)
    # plt.plot(t, v)
    # # plot acceleration
    # plt.subplot(3, 1, 3)
    # plt.plot(ta, a)
    # plot speed
    # s = [abs(x) for x in v]
    # plt.plot(t, v)
    # plot acceleration
    # a = [x for pair in zip(a, a) for x in pair]
    # ta = [x for pair in zip(t, t) for x in pair]
    # del ta[0]
    # del ta[-1]
    # ta = [(t1 + t2) / 2 for t1, t2 in zip(t[:-1], t[1:])]
    # plt.plot(ta, a)
    plt.show()


def solve_stepper(steps):
    """
    Solve for the position of the stepper motor.

    steps is a list of (t, step) pairs where t is time and step is the absolte
    step position of the motor.

    Return value is a list of (t, x) points where t is time and x is the linear
    position of the motor.

    """
    assert steps
    # timestep
    dt0 = 2 * math.pi / (4 * stepper_frequency * stepper_timestep_divider)
    # initial conditions
    t0, x0 = steps[0]
    v0 = 0
    x_last = x0
    # solve for k and c (assuming m = 0)
    z = stepper_damping
    on = stepper_frequency
    # add first point
    points = [(t0, x0)]
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
            denom = 1 + dt * z * on + 0.25 * (dt * on) ** 2
            numer = (
                v0
                + 0.5 * dt * on ** 2 * x_last * 2
                - dt * on ** 2 * x0
                - dt * v0 * (1 / 4 * on ** 2 * dt + z * on)
            )
            v1 = numer / denom
            # find new position
            x1 = x0 + dt * (v0 + v1) / 2
            # cycle variables
            t0, x0, v0 = t1, x1, v1
            # add this point
            points.append((t0, x0))
        # save last x value
        x_last = x_next
    # return solution points
    return points


def solve_motion(steps):
    """
    Solve the system of equations and return rhe solution data.

    Solution is returned as two lists of (t, x) and (t, x, v) where the first
    is the position of the stepper motor and the second is the position of the
    hotend.

    """
    assert steps
    # find position of stepper motor
    stepper_position = solve_stepper(steps)
    # plot_solution(stepper_position, steps)
    # timestep
    dt0 = 2 * math.pi / (hotend_frequency * 4 * hotend_timestep_divider)
    # initial conditions
    t0, x0 = stepper_position[0]
    v0 = 0
    t_last = t0
    xt_last = x0
    # solve for k and c (assuming m = 0)
    k = hotend_frequency ** 2
    c = 2 * hotend_frequency * hotend_damping
    z = hotend_damping
    on = hotend_frequency
    # add first point
    points = [(t0, x0, v0)]
    # solve for each pair
    for t_next, xt_next in stepper_position[1:]:
        assert t_next >= t0
        # solve system until we reach the next time
        xt0 = xt_last
        while t0 != t_next:
            # find timestep
            if (t_next - t0) / dt0 < 1.0001:
                t1 = t_next
                dt = t1 - t0
            else:
                dt = dt0
                t1 = t0 + dt
            # solve for xt1
            alpha = (t1 - t_last) / (t_next - t_last)
            xt1 = xt_last + alpha * (xt_next - xt_last)
            # solve system
            denom = 1 + dt * z * on + 0.25 * (dt * on) ** 2
            numer = (
                v0
                + 0.5 * dt * on ** 2 * (xt0 + xt1)
                - dt * on ** 2 * x0
                - dt * v0 * (1 / 4 * on ** 2 * dt + z * on)
            )
            v1 = numer / denom
            # find new position
            x1 = x0 + dt * (v0 + v1) / 2
            # cycle variables
            t0, x0, v0 = t1, x1, v1
            xt0 = xt1
            # add this point
            points.append((t0, x0, v0))
            # save last input
        # save last x value
        xt_last = xt_next
    # return solution points
    return stepper_position, points


def example():
    """Run an example."""
    # steps = generate_motion()
    steps = generate_trapezoid_motion()
    steps.append((steps[-1][0] * 1.5, steps[-1][1]))
    stepper, hotend = solve_motion(steps)
    print("Stepper has %d timesteps" % len(stepper))
    print("Hot end has %d timesteps" % len(hotend))
    plot_solution_2(steps, stepper, hotend)


# run example if run as script
if __name__ == "__main__":
    matplotlib.rcParams.update({"font.size": 16})
    ctypes.windll.user32.SetProcessDPIAware()
    plt.close("all")
    example()
