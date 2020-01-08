# Path equations

This contains a reference for different types of paths

## Constant velocity

With a constant velocity of v0, the position vs time is given as:

* x(t) = x0 + v0 * t
* v(t) = v0

## Constant acceleration

* x(t) = x0 + v0 * t + 1/2 * a0 * t^2
* v(t) = v0 + a0 * t
* a(t) = a0

## Constant jerk

* x(t) = x0 + v0 * t + 1/2 * a0 * t^2 + 1/6 * j0 * t^3
* v(t) = v0 + a0 * t + 1/2 * j0 * t^2
* a(t) = a0 + j0 * t
* j(t) = j0

An equation for speeding up smoothly from 0 to to vf with a max jerk of j0 would be:

At the transition from j(t) = j0 to j(t) = -j0, we should be at half of the final speed, which lets us solve for the time of this transition:
* v(tt) = 1/2 * j0 * tt^2 = 1/2 * vf
* --> tt = sqrt(vf / j0)

Before this transition:
* x(t) = 1/6 * j0 * t^3
* v(t) = 1/2 * j0 * t^2
* a(t) = j0 * t
* j(t) = j0

At transition (t = tt):
* x(tt) = 1/6 * vf^1.5 * j0^-0.5
* v(tt) = 1/2 * vf
* a(tt) = vf^0.5 * j0^0.5

From tt <= t <= 2 * tt, we have j(t) = -j0
* x(t) = 1/6 * vf^1.5 * j0^-0.5 + 1/2 * vf * (t - tt) + 1/2 * vf^0.5 * j0^0.5 * (t - tt)^2 - 1/6 * j0 * (t - tt)^3
* v(t) = 1/2 * vf + vf^0.5 * j0^0.5 * (t - tt) - 1/2 * j0 * (t - tt)^2
* a(t) = vf^0.5 * j0^0.5 - j0 * (t - tt)
* j(t) = -j0

At t = 2 * tt. we have:
* x(t) = vf^1.5 * j0^-0.5
* v(t) = vf
* a(t) = 0
Which matches up with our expectations on the final velocity and acceleration.

Suppose we want to traverse a 90 degree angle by using this trajectory.  For 0 < t < tt, we would have
* x(t) = 1/6 * j0 * t^3 (accelerating from v0=0)
* vx(t) = 1/2 * j0 * t^2
* y(t) = vf * t - 1/6 * j0 * t^3 (decelerating from v0 = vf)
* vy(t) = vf - 1/2 * j0 * t^2
* s(t) = sqrt(vx(t)^2 + vy(t)^2)
* s(t) = sqrt((1/2 * j0 * t^2)^2 + (vf - 1/2 * j0 * t^2)^2)
* s(t) = sqrt(1/4 * j0^2 * t^4 + vf^2 + 1/4 * j0^2 * t^4 - vf * j0 * t^2)
* s(t) = sqrt(vf^2 - (j0 * t^2) * (vf - 1/2 * j0 * t^2))

At the transition (t=tt) the speed is:
* s(tt) = vf^2 / sqrt(2)

The speed is minimized halfway through the transition.  This distance from the corner is maximized at this time as well.

## Constant jerk with max accel

With initial velocity v0, max accel amax, max jerk jmax:

Within d1 segment:
* x(t) = v0 * t + 1 / 6 * jmax * t^3

At t = d1 (Note that d1 = amax / jmax):
* x(t = d1) = v0 * d1 + 1 / 6 * jmax * d1^3
* x'(t = d1) = v0 + 1 / 2 * jmax * d1^2

For d1 < t < d1 + d2:
* x(t) = (v0 + 1 / 2 * amax * d1 ^ 2) * (t - d1) + 1/2 * amax * (t - d1)^2
* x'(t) = (v0 + 1 / 2 * amax * d1 ^ 2) + amax * (t - d1)

At t = d1 + d2 (Note that d1 = amax / jmax):
* x(t = d1 + d2) = (v0 + 1 / 2 * amax * d1) * d2 + 1/2 * amax * d2^2
* x'(t = d1 + d2) = v0 + 1 / 2 * amax * d1 + amax * d2

For d1 + d2 < t < d1 + d2 + d3:
* x(t) = x'(t = d1 + d2) * (t - d1 - d2) - 1/6 * jmax * (t - d1 - d2)^3
* x'(t) = x'(t = d1 + d2) - 1/2 * jmax * (t - d1 - d2)^2

At t = d1 + d2 + d3:
* x(t = d1 + d2 + d3) = (v0 + 1/2 * amax * d1 + amax * d2) * d3 - 1/6 * jmax * d3 ^ 3
* x'(t = d1 + d2 + d3) = v0 + 1/2 * amax * d1 + amax * d2 - 1/2 * jmax * d3^2

Note that we must have d1 = d3.  We need to find d2 such that x(d1 + d2 + d3) meets the target distance.  This is easy:
* d2 = (dist - v0 * d1 - 1/2 * amax * d1^2 + 1/6 * jmax * d1 ^ 3) / (amax * d1)

## Nominal/starting values to for jerk

This section tries to find reasonable values for the jerk setting.  Note that many 3d printer controllers implement a change of velocity of around 10 mm/s which seems extreme but the printer handles it okay.

Say we want a printing speed of 40 mm/s and we want the printer to ramp up from rest to this max speed within dx = 2mm.  From our equations before:
* x(tf) = vf^1.5 * j0^-0.5 = dx
* --> j0 = vf^3 / dx^2

For our values in the previous paragraph, this translates to a max jerk of j0 = 16000 mm/s^3.  It also translates to a max acceleration of:
* a(tf / 2) = vf^0.5 * j0^0.5 = 800 mm/s^2

This is a reasonable max acceleration value.  I was okay using values around 2000-4000mm/s^2 when printing items and it worked okay.

With these settings, it will take 100ms to ramp from zero to 40 mm/s.

## Path segments

Path segments should be easy to calculate.  We will assume a constant jerk within each segment.  So each segment can look like:

```
strict PathSegment {
  float duration;
  float t0;
  float x0;
  float v0;
  float a0;
  float j0;
}
```

The planner will have a way to calculate the current time, so calculating the position and velocity will be easy:
* x(t) = x0 + v0 * t + 1/2 * a0 * t^2 + 1/6 * j0 * t^3
* v(t) = v0 + a0 * t + 1/2 * j0 * t^2
