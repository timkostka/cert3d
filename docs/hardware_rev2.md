# Hardware changes for rev2

This file is to brainstorm possible changes to make to the rev1 hardware to create rev2.

## Ground pins

I need to add ground pins to the PCB so I can hook up grounds between this board and the printer board.  A few male header pins to the GND signal should be enough.  Possibly have these far enough away from other pins to be able to alligator clip onto them.

## Encoder channels (invalid option, see later)

The firmware is too slow to postprocess motor position from STEP/DIR edges when they're going quickly.  One possible solution would be to use the encoder function within the timer to process this via hardware.  This would require one channel per encoder.

There are a total of 6 timers that have encoder functionality.  We currently use 3 of these to process edges.  It is probably possible for us to use the other 3 as encoders.  This would look like the following:

* TIM1: 4 channels used to monitor STEP/DIR edges
* TIM8: 2 channels used to mointor STEP/DIR edges
  * Other 2 channels have a DMA conflict with TIM1 channels
  * Possible I could 
* TIM3: 2 channels used to monitor STEP/DIR edges
  * Other 2 channels can possibly be used as well.

* TIM2: encoder for X motor
  * Currently used to trigger ADC conversions.
  * Not sure how to free this up.
* TIM4: encoder for Y motor
  * Currently used as master timer.  I could quickly switch this from master timer to encoder mode after triggering other timers.  It may miss a few steps but this likely wouldn't even matter.  It is also possible the encoder mode will also trigger other channels, in which case switching wouldn't be necessary.
* TIM5: encoder for Z motor
  * Currently used for SECNT timer, which is easily moved to another timer.

### Encoder waveforms.

The following wavedrom script explains how we want the timer encoder to work.  Note how `CNT` should update right after a rising edge on `STEP` depending on the polarity of `DIR` at the time of the rising edge.

```
{signal: [
  {name: 'STEP', wave: 'lpppplppplpp'},
  {name: 'DIR', wave: 'h....l...h..'},
  {},
  {name: 'CNT', wave: '222222222222', data: ['0', '1', '2', '3', '4', '4', '3', '2', '1', '1', '2', '3']}
]}
```

![Example encoder waveform](http://wavedrom.craftware.info/rest/gen_image?type=svg&scale=1.0&c=34ADLAGhdAD2cyVX4n%2ByefOiEYt63aI8IHLdqN2%2F6uyUWTr9QRUUrXfZnZO1z5o70dB%2Bc5l%2FH%2FN2xNC2mM%2BMhxz3UaZocD2vMVoEzUib9q760Uk4o70PO%2FIqGzuGi9BgVR%2B%2FjjGke23O3Zo0Z8c69ACLvs76eZp5OAAGEAcwBAAAqkE1GscRn%2BwI%3D)

I think this can work using encoder mode 1:
```
001: Encoder mode 1 - Counter counts up/down on TI1FP1 edge depending on TI2FP2 level.
```

I would hook up `STEP` to `TI2` and `DIR` to `TI1`.

Actually, this doesn't work.  It looks like the timer changes value on both rising and falling edges, like a traditional encoder.  Hmm...

![](.hardware_rev2_images/54eb3f34.png)

## Encoder channels

What other way can we get motor position from processing STEP/DIR pins via hardware?  With unlimited timers, I can think of several ways, but we have limited timers.

I could add another chip to the board and dedicate that to handling this.  This complicates board layout, firmware, debugging, etc.  Not an attractive option.

This is only needed to simulate endstops.  I could simply only simulate endstops when necessary.  For example, when doing a homing operation.  Travel speeds during homing should be nowhere near the max speed.  I would expect speeds of under 20mm/s.  At 80 step/mm, this would be 1600 steps/s, which is very doable in software.

## HRTIM usage

It is possible to use the HRTIM to capture up to 5 channels at 480MHz, but we need to capture 10 channels.

## STM32H7 microprocessor

We will switch to a 480 MHz STM32H745 microprocessor, which runs a 480 MHz M7 core and a 240 MHz M4 core.

To start, we will only use the M7 core.  Perhaps the M4 core can act do the endstops.

### USB connectivity

The bandwidth of FS HSB (up to 12Mbps) is probably fine.  I would like to get higher, but it would involve adding an external PHY.

### ADCs

Same as before, we'll add pins to read ADCs.  The ADC will be 16 bit.  TIM8 will triggert the ADCs.  Transfer from ADC to memory will use 1 DMA stream.

## Programmer

I'll use the ST-Link V3 Mini programmer with the standard 14-pin adapter.

## Protection resistors

I'm going to remove all protection diodes/resistors.  This is just a development board, we can add them back on later.  They just make building it a bit harder and more space intensive.

## Timer channels

I'm not going to use the HRTIM.  Instead, I'll use the standard timers at 240 MHz, or maybe 200 MHz.  I need 10 channels, but I'll add 12:
* TIM1_CH1-4
* TIM2_CH1-4
* TIM3_CH1-4

Each of these channels uses a DMA stream.

Maybe HRTIM can be used as a high-speed timer between events?  If not, I'll use use TIM4/TIM5.

## Debug stream

The M7 core will send the debug stream to USART1 via DMA.  No debug stream on the M4 core, for now.  This uses 1-2 DMA channels, depending if we want input as well as output.

## Debug pins

I will have a number of pins used for debug purposes only.  Probably 4 is enough, but maybe 8 would be better.

## Endstop pins

Ideally, we will have 12 GPIO inputs for end stop pins.

## DMA streams

There are 16 DMA streams available which will be used as follows:
* (12) Input channels
* (2) USART debug stream
* (1) ADC transfer to memory
