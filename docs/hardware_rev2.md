# Hardware changes for rev2

This file is to brainstorm possible changes to make to the rev1 hardware to create rev2.

## Encoder channels

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
