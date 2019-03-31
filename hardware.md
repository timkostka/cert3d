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

Another option is to output only the delta between pulses.  I could use a uint16_t, and use 0 to indicate that there was no change for 2^16 ticks.  That is way more memory efficient.  2^16 ticks at 180MHz is 0.4ms, which is a bit low.  So maybe use uint32_t to reduce the number of zeros.

### Plotting results

Results should be output in a variety of modes.  One of those should be an oscilloscope-type mode where edges are shown.  This may be fun to develop.

## Hardware pins

What is actually needed to monitor/simulate a 3d printer board?

* (8 high-speed pins, 4 slower pins) Monitor X, Y, Z, and E stepper motors
  * Per motor, need STEP and DIR hooked up to timer IC inputs.  Can monitor EN with a slower GPIO pin.
* (2 pins) X and Y end-stops
* (2 pins) SWD interface
* (2 pins) UART debug input/output
* (1 pin) Error-indicating LED
* (1 pin) monitor bed heating on/off
* (2 pins) monitor fans?
* (2 DAC pins) simulate bed and hot end thermistors
* (1 pin) monitor hot end heater on/off
* (1 pin) simulate EZABL type bed leveler

### How to simulate a thermistor?

The common thermistors used in 3D printers are 10kOhm at room temp and go lower than this as they heat up, becoming as low as 220 Ohm at 150C.

A 10kOhm digital potentiometer might work well.

On the board (Duet 2 Wifi), thermistors are connected in the following circuit:
```
                        [3.3V]
                          |
                    [4.7kOhm 0.1%]
                          | 
MCU PIN>---+---[10kOhm]---+---[THERMISTOR]---+
           |                                 |
           = [2.2uF]                         |
           |                                 |
          GND                               GND
```

So the voltage on the thermistor shouldn't go over 2.24V.  If shorted, the current through it will be 0.7mA.

There are a few issues with using digipots for this.  First, the tolerance is awful, typically 20-30% and quite nonlinear.  Second, the number of taps is limited.  With 256 taps, the resistance increment would be 39 Ohm.  This is the difference between 225C and 265C.

One option would be using two digipots in series to get better resolution.  If we had a 10kOhm and a 1kOhm in series.  But 1kOhm are expensive.  No good solution that I can see.

For the bed, a digipot is probably fine.

I could use a DAC.  The STM32F4 has a 12-bit DAC.  This gives a resolution of 0.8 mV.  This is way better.  This may vary depending on how boards are set up, but it's doable.

With 2 DACs, we can simualte 2 thermistors.

### STEP/DIR channels

What channels and DMA streams can we use for the STEP/DIR signal monitoring?

* TIM1_CH1 - DMA2 Stream 1

* TIM1_CH2 - DMA2 Stream 2

* TIM1_CH3 - DMA2 Stream 6

* TIM1_CH4 - DMA2 Stream 4

* TIM8_CH1 - DMA2 Stream 2

* TIM8_CH2 - DMA2 Stream 3

* TIM8_CH3 - DMA2 Stream 4

* TIM8_CH4 - DMA2 Stream 7

So out of these, we can have 6 enabled.  These can monitor X, Y and E.  All channel on TIM1 and channels 2 and 4 on TIM8.

We need 2 more slow speed channels to monitor Z.

* TIM3_CH1 - DMA1 Stream 4

* TIM3_CH2 - DMA1 Stream 5

### UART debug channel

This channel is used as a debug stream.

* USART3_RX - DMA1 Stream 1

* USART3_TX - DMA1 Stream 3

