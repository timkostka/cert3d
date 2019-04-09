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
* (1 pin) Stepper motor monitoring
* (3 pins) X, Y, and Z end-stops
* (2 pins) SWD interface
* (2 pins) UART debug input/output
* (1 pin) Error-indicating LED
* (1 pin) monitor bed heating on/off
* (3 pins) monitor fans?
* (2 DAC pins) simulate bed and hot end thermistors
* (1 pin) monitor hot end heater on/off
* (1 pin) simulate EZABL type bed leveler

### How to simulate a thermistor?

The common thermistors used in 3D printers are 100kOhm at room temp and go lower than this as they heat up, becoming as low as 220 Ohm at 150C.

A 100kOhm digital potentiometer might work well.

On the board (Duet 2 Wifi), thermistors are connected in the following circuit:

```
                         [3.3V]
                           |
                     [4.7kOhm 0.1%]
                           | 
MCU PIN>---+---[100kOhm]---+---[THERMISTOR]---+
           |                                 |
           = [2.2uF]                         |
           |                                 |
          GND                               GND
```

So the voltage on the thermistor shouldn't go over 2.24V.  If shorted, the current through it will be 0.7mA.

There are a few issues with using digipots for this.  First, the tolerance is awful, typically 20-30% and quite nonlinear.  Second, the number of taps is limited.  With 256 taps, the resistance increment would be 39 Ohm.  This is the difference between 225C and 265C.

One option would be using two digipots in series to get better resolution.  If we had a 100kOhm and a 1kOhm in series.  But 1kOhm are expensive.  No good solution that I can see.

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

### Timer triggering

We need to start all timers at the same time in order to get relative timing correct between channels.  We can do this by setting up TIM3 and TIM8 in slave mode that trigger off of TIM1_TRGO signal.  For TIM3, this is the ITR0 trigger per Table 98 in RM90.  For TIM8, this is also ITRO0 per Table 94.

We want all timers to be in sync so that CNT register values are identical between them.  We can do this by triggering them to start at the same time.  I set up TIM1 in master mode and had TIM3 and TIM8 trigger off of it.  Doing this, there was a delay of 18-20 ticks between TIM1 and TIM8.  In other words, `TIM1->CNT = TIM8->CNT + (18 to 20)`.  This isn't desirable.

Can I use a different timer to trigger TIM1, TIM3 and TIM8 all at the same time?  The delay between the trigger and the timer starting should be the same for all three.

* TIM1 can be triggered from TIM5 (ITR0), TIM2 (ITR1), TIM3 (ITR2), and TIM4 (ITR3)
* TIM3 can be triggered from TIM1 (ITR0), TIM2 (ITR1), TIM5 (ITR2), and TIM4 (ITR3)
* TIM8 can be triggered from TIM1 (ITR0), TIM2 (ITR1), TIM4 (ITR2), and TIM5 (ITR3)

So yes, we can use TIM2 to trigger the other three.  Conveniently, they all use the same ITR1 signal to do so.  I did this and the timers appear to be perfectly in sync.  I got a difference of 8 ticks if I read them in one direction and -8 ticks if I reversed the order that I read them.  So probably exactly the same.

### Signal buffers

We need a buffer to store the timer CNT values at each edge change.  How large should this buffer be?  Our high speed timers reset at `65536/168MHz=2.5kHz`.  We want to process values at at least twice this rate in order to correctly interpret their CNT value.

Step rates for 3d printers (taken from https://reprap.org/wiki/Step_rates) top out at around 180kHz for 3 motors.  For a single motor, this is an edge generation rate of 360kHz.  Each edge needs 2 bytes.  If we process at 5kHz, then the buffer needs to be at least `360kHz * 2 byte / (5kHz) = 144 bytes`.  That's extremely small.  No problem.

Three motors running at 180kHz will produce 6 edges at 180kHz, which will create 8.64Mpbs of data.  This is too much to handle, but we could probably handle about half of that.  It's very unlikely to be running three motors this fast.

#### Buffer monitor callback

The buffers will be monitored by a callback at a frequency of around 5kHz.  On each pass, we need to monitor the signal lines and pass along information about any edges detected.

I clearly need an output buffer, where information goes when it's ready to be sent out.  The USB will read directly from this buffer when it is sending information.  This is the same way the debug log operates.

Maybe I need a staging buffer for each signal line where I translate the data so it's ready for output.  When these staging buffers get full enough, I can send out their info to the output buffer.  That seems good.

#### Output buffer

The output buffer information will be organized as follows:

The buffer will be primary in `uint16_t` format, occasionally putting two `uint8_t` values into a single `uint16_t`.

* Sync word at beginning `0x0B77`

* Channel number as a `uint16_t`

* Size of the of entries as a `uint16_t`

* Each entry as a single `uint16_t`

This structure repeats as necessary for each channel.

For signal channels, the `uint16_t` data entry represents the number of ticks between edges.  If the number of ticks is more than 65535, we encode it instead as a `uint16_t` of 0, then a `uint32_t` of the number of ticks.

On second thought, this requires us to do more calculations on the mcu.  I'd rather move processing of data to the computer.

#### Output processing try #2

We keep the output buffer.  However, a packet is sent on every processing step.  Each packet has the following info:

* Sync word `0x0B77`

* Packet number as `uint16_t`, first is 0, increments 1 each time, eventually flows over.

* For each signal channel (number of channels is known)

  * Number of entries as a `uint16_t` in channel `N`
  
  * Entry #1
  
  * Entry #2
  
  * etc...

The entries are the `TIMx->CNT` values.  Post-processing is done on the computer side to get the number of ticks between edges.

The computer can also detect missed packets (if any) since the packet number is included.

If no edges are detected, each packet is `2 + 2 + 8 * 2 = 20` bytes.  At a packet rate of 5kHz, this comes to 0.8 Mbps.  That's a bit much for purely overhead.

### ADC triggering

We should also set up the ADC to trigger off a timer in order to keep relative timing correct, although the rate of ADC triggers should be pretty slow in order to keep bandwidth reasonable.

I did this.  The ADC triggers off of `TIM2_TRGO` signal.  Maybe I should move it somewhere else so I can independently adjust the frequency of samples?  The only options for the are TIM1, TIM2, TIM3, and TIM8.  I'm keeping TIM1 and TIM8 for their high speed edge detection, so TIM2 will have to trigger the ADCs.  And rev1 hardware has TIM3 reserved as well.  So I'm stuck with TIM2.  Can I get the other three to trigger off of a different timer?  Yes, they can all be triggered off of TIM4.

### ADC sampling

How many samples should I take?

The ADC clock can run at a max of 36MHz.  If we have a system clock of 168 MHz and an APB2 clock of 84 MHz, we need to use a prescaler of 4, which gives us an ADC clock of 21 MHz.

It takes 15 clocks to convert a reading into a value.  This is in addition to the sampling time.

Say we wanted a reading on every channel at 10kHz.  With 14 channels, each sample needs to be done at 140kHz.  At a clock of 21MHz, this gives us 150 clocks per sample.  So we could choose the `ADC_SAMPLETIME_112CYCLES` sample time.  That seems fine.

At this rate, we would be generating `14 * 10kHz * 12 bit = 1.68Mbps`.  That still seems a bit high.  Let's sample at 1kHz to start.

### UART debug channel

This channel is used as a debug stream.

* USART3_RX - DMA1 Stream 1

* USART3_TX - DMA1 Stream 3

### Monitoring of on/off channels

There are a number of items we need to monitor for on/off.  Off, they should be pulled down to 0V.  On, they are set anywhere from 3.3V to 24V.  One option would be to use ADC channels to monitor the exact voltage for each of these channels.  If I used DMA, this works well to keep the timing between samples consistent.

If I expect voltages up to around 24V, the following circuit works well:

```

<ADC PIN>---+---[7.5kOhm]---<PRINTER PIN>
            |
            +---[1.0kOhm]---<GND>
```

With a 12-bit ADC, this gives a resolution of 6.8mV and a max voltage of 28.05V.  Since this will typically be used to monitor on/off peripherals, this seems fine.

### Ender3 thermistors

On the Ender3, thermistors are connected as follows:

```
          <5.0V>
            |
        [100kOhm]
            | 
<MCU PIN>---+---[THERMISTOR]---+
            |                  |
            = [2.2uF]          |
            |                  |
           GND                GND
```

### Simulating thermistor channels

We are using a DAC to simulate the thermistor.  I would like to protect it from being connected to 24V while still being able to simulate very high temperatures.

The BAT54C has a max power dissipation of 200mW.  At 24V, this is produced with a current of 8.3mA.  An inline resistor of 2.9 kOhm would prevent more than this.  However, this would prevent me from simulating very high temperatures.  No good.

Let's consider the following circuit diagram:

```
                          +---[R1=4.7kOhm]---<VH=3.3V>
                          |
<V1=DAC PIN>---+---[R3]---+---[R2=100kOhm]---<V2=PRINTER ADC PIN>
```

We can solve this for V2(V1, VH, R1, R2, R3):

```
I1 = (VH - V4) / R1
I2 = (V2 - V4) / R2 = 0
I3 = (V1 - V4) / R3
I1 + I2 + I3 = 0
--> V2 = (R1 * V1 + R3 * VH) / (R1 + R3)
```

The lowest this can go is with `V2(V1=0) = VH / (R1 / R3 + 1)`.  If I need to simulate a thermistor going as low as 200 Ohm, then I need the voltage to be `3.3V * 50 / (4700 + 40) = 0.135V`.  I could use an inline resistor of at most 200 Ohm to simulate this.

### Endstop simulating and detection

On the Ender3, the endstop pin is tied directly to the connector, and pulled up to VCC (5V) with a 10 kOhm resistor.

On the duet 2 wifi:

```
<3.3V>---[1kOhm]---[LED>|]---+
                             |
      <MCU PIN>---[10kOhm]---+---<ENDSTOP CONNECTOR>

```

In either case, we can simulate an endstop by using an open drain output and driving it to ground.  It needs to be protected from overvoltage.  A 100Ohm inline resistor would work fine, along with a diode clamp to the 3V3 rail.
