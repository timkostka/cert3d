# Cert3D Hardware module

This file is for laying out the requirements for the hardware module.

At high speed, we need to monitor the DIR and STEP lines on each stepper motor driver.

Number of stepper motors:

* X axis

* Y axis

* Z axis

* E (extruder) axis

* (maybe) E2 axis (second extruder)

In each motor takes 4 lines (2 lines with rising/falling for each).  If we use timers on the STM32F4, what does this look like?

With 2 DMAs with 8 streams each, we are limited to 16 DMA-based interrupt transfers.  This would be enough without E2, except that doesn't leave us any for other functions, such as a UART debug stream.

Another option would be to set triggers to both rising and falling, and then detect the state in an interrupt or something.  This is more prone to error, but would probably work.

Another option would be to eliminate DMA-based triggers and do it in software via interrupts.

## What timing is necessary?

Looking at the DRV8825 driver, the required timing on a pulse on the STEP or DIR lines is 1.9us.  There is also a 650ns setup time between DIR rising/falling and STEP rising.  It would be nice to be able to verify that timing.

With an STM32F4 clock speed of 180MHz, we can run some timers at 180 MHz, and some others at 90MHz.  At 90MHz, this gives a timer period of 11.1 ns.  That seems good enough.

## Plan forward

Two timer channels per extruder seems fine, with one hooked up to STEP and the other to DIR.  If we plan for a single extruder, this is a total of 8 timer channels, and 8 DMA streams.

Can we use only high speed (APB2) timers for these?  These are TIM1 and TIM8-11.  Yes, both TIM1 and TIM8 have 4 available input channels.  Actually, the DMA streams overlap, so this would only allow us 6 DMA requests.  Let's just set it up with TIM1 only for now and work out the details later.  We may have to use 6 channels on high speed timers and 2 channels on slower timers.

### Results

I managed to get this working just fine using the TIM1 timer in IC mode for two channels at once.  Easily extensible to 4.  Resoltion of 1 timer tick at 160 MHz.  The output is written via DMA to a circular buffer.  I can easily parse this to capture individual edges.

What resolution should results be stored as?  If i use uint32_t and a tick of 180 MHz, this will overflow around 24sec.  So I could use uint64_t instead, although it's a bit clunky.  If I convert to float, then I lose some resolution.

### Plotting results

Results should be output in a variety of modes.  One of those should be an oscilloscope-type mode where edges are shown.  This may be fun to develop.
