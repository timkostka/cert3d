"""
This file shows some examples of our motion post-processing scheme.

"""

import matplotlib.pyplot as plt


# natural frequency (Hz)
omega_n = 500

# damping ratio
zeta = 0.15

# linear motion per step
mm_per_step = 1


def generate_ramp(ramp_time=0.25, amplitude=1, step_count=10, end_time=0.5):
    """Return a ramp step."""
    steps = []
    for i in range(step_count + 1):
        steps.append((i * ramp_time / step_count, amplitude * i / step_count))
    if end_time > ramp_time:
        steps.append((end_time, amplitude))
    return steps


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
    # post-process acceleration
    print(solution)
    t, x, v = zip(*solution)
    plt.plot(t, x)
    plt.ylabel("Position")
    plt.xlabel("Time")
    if steps:
        t, x = zip(*get_discrete_steps(steps))
        plt.plot(t, x)
    plt.show()


def solve_motion(steps):
    """Solve the system of equations and return (t, x, v) data."""
    assert steps
    # timestep
    dt0 = 1 / (omega_n * 10)
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
    steps = generate_ramp(
        ramp_time=0.010, amplitude=1, step_count=50, end_time=0.05
    )
    print("Ramp is %d points" % len(steps))
    solution = solve_motion(steps)
    print("Solution is %d points" % len(solution))
    plot_solution(solution, steps)


# run example if run as script
if __name__ == "__main__":
    example()
