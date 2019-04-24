# Motivation

After some observations of Marling and RepRap, it is clearly a nontrivial task to convet STEP/DIR readings into accurate POS/VEL/ACC curves, especially the last two.  Because of the quantized nature of stepping, there needs to be something to smooth out the process.  In the actual printer, this is achieved by mechanical means--inertia of the head and motors, etc.

With that thought, maybe it would be best to simulate the equation of motion to postprocess VEL/ACC curves from the STEP/DIR curves?  What would this look like?  What does the system look like?

Let's consider a simpified system:

* The hot end has some inertial mass.

* The belt act as a stiff spring.

* The stepper motor acts as another stiff spring.

* Damping between the hot end and the bar it's on.

The mechanics of the stepper motor are complicated--the current through the windings is very dynamic and nontrivial to simulate.  At a first cut, we will ignore this.  Maybe the mass of the hot end dominates the system.

The system of equations is a simple mass-damper system.  Let `x` be the position of the hot end, `e` be the position given by `STEP`/`DIR`, `k` be the stiffness of the belt and stepper motor, and `c` be the damping in the system.  Then our equation of motion is:

* `m x" + (x' - e') * c + (x - e) * k == 0`

where " means d/dt^2 and ' means d/dt.  We can simplify this to:

`m * x" + c * x' + k * x == c* e' + k * e`

What should the constants be?

* The mass is maybe 100g.

* The stiffness for the stepper motor will vary based on the max torque of the stepper.