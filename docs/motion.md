# Simulating the equations of motion

By monitoring the STEP and DIR signals, we know the target position of each motor.  This signal is discontinuous wich a jump occuring at every pulse of the STEP signal.  Since the derivative of this signal is infinite, we must use some other way to estimate the velocity of the motor.

This document looks at methods for deriving the position, velocity, and acceleration from the interpreted STEP/DIR signals.

## Equation of motion

We know the motor can't move instantly, so let's apply a force to the motor to make it move to its target position.

The force will be similar to the keeping force on a steppet motor.  It will be proportional to the difference between the target and actual position.  A damping force will also be present.

If the current position is x(t) and the target position is x_t(t), the equation of motion looks like the following:

* x"(t) = k * (x_t(t) - x(t)) - c * x'(t)
* v'(t) = k * (x_t(t) - x(t)) - c * v(t)

This equation can be solved even if x_t(t) is discontinuous.

We can solve this with the implicit midpoint method.

Let dt be the timestep.

* v_n+1 = v_n + dt * v'(t + dt / 2, (v_n + v_n+1) / 2)
* v_n+1 = v_n + dt * (k * (x_t(t + dt/2) - (x_n + v_n * dt / 2)) - c * ((v_n + v_n+1) / 2))


* v_n+1 = v_n + dt * k * x_t(t + dt/2) - dt * k * (x_n + v_n * dt / 2) - dt * c * v_n / 2 - dt * c * v_n+1 / 2
* v_n+1 * (1 + dt * c / 2) = v_n * (1 - dt * c / 2) + dt * k * x_t(t + dt/2) - dt * k * (x_n + v_n * dt / 2)

This works pretty well in Excel.

## Physical constants

We should ground our equation in reality.  We need to find values for m, k and c that make sense:

* m x" = k * x - c * x'

If we let x be the position of the nozzle, then m should be the weight of the hot end.  For direct drive printers, the hot end often has a NEMA 42-40 stepper motor on it.

Some NEMA 17 motors are listed here: https://reprap.org/wiki/NEMA_17_Stepper_motor

We will assume the following specs for the motor:
* 1.8 degree step angle
* 40 N*cm holding torque
* 370 grams (measured)

The stiffness translates to about 40 N*cm per 1.8 degrees.  This torque moves a belt over a pulley of diameter 0.7" (measured).  The pulley has 20 teeth and the belt has 1 tooth per 2mm, so the effective pulley diameter should be D = (40mm / pi). So this translates into a linear stiffness of 40 N*m / (40mm/2pi) / (1.8deg / 360deg * 40mm) = 180,000 lbf/in.  Seems a little high.

We will assume a mass of 150grams, which is probably about right for an Ender 3. 

This yields a natural frequency of sqrt(k/m) = 460 Hz.  This seems in the right ballpark.

We will calculate c based on the perceived damping ratio.  Based on ripples in the print after a 90 degree turn, the oscillations disappear after about 3-5 oscillations.  This gives us a damping ratio of around 0.2.

The unknowns k, c, m can be reduced to just the natural frequency (omega_n) and the damping ratio (zeta).  In these variables, the equation of motion above is equivalent to:
* x" = omega_n^2 * x - 2 * zeta * omega_n * x'

