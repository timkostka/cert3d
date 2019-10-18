# Streaming speed

This file is intended to figure out how much data needs to be sent across the USB interface.

## Methodology

It is required that all timers update at the same frequency.  However, they can have different individual tick frequencies.

For each timer update, a single packet should be sent to the computer with the ADC values and the timer count at each detected edge.  This packet will have the


## Header packet

Before streaming data, a header packet must be sent to the computer which holds the format of the streamed data.  This header has the following format:

* `char[9] start_string`
  * The string `InfoStart` to denote the start of this packet.
* `uint8_t header_packet_version`
  * Streaming data packet version number.
  * The version of the format given here is 1.
* `uint8_t streaming_packet_version`
  * Streaming data packet version number.
* `uint32_t system_clock`
  * System clock tick speed in Hz.
* `uint8_t signal_channel_count`
  * Number of signal channels in the packet.
* `uint8_t adc_channel_count`
  * Number of ADC channels in the packet.
* `uint32_t ticks_per_adc_reading`
  * Number of system ticks between ADC readings.
* For each signal channel:
  * `uint32_t signal_clock`
    * Clock tick speed in Hz of this channel.
  * `uint32_t update_ticks`
    * Number of ticks per timer update 
* For each ADC signal:
  * `float zero_value`
    * Voltage value corresponding to a zero count value on the channel.
  * `float high_value`
    * Voltage value corresponding to a max count (4095) on this channel.
* `char[8] end_string`
  * The string `InfoStop` to denote the start of this packet.

## Data packet

A data packet is sent for each update.  This data packet is sent at a frequency of `clock / ticks_per_update`.  This data packet version is 1.

* `uint8_t packet_number`
  * Incremental counter for the packet number
  * First is 0, second is 1, etc...
* `uint8_t channel_mask`
  * Mask showing which signal channels are present in this channel
    * If LSB (bit 0) is set, then channel 0 is present
    * If bit 1 is set, channel 1 is present
    * Etc...
* For each signal channel which is present (per the mask above)
  * `uint8_t edge_count`
    * Number of entries for this channel
  * For each edge:
    * `uint16_t timer_edge`
      * Value of timer at this edge.
* `uint8_t adc_sample_count`
  * Number of ADC samples present (typically 0 or 1)
* For each ADC sample:
  * For each ADC channel:
    * `uint16_t adc_value`
      * Raw value of the ADC of this channel

## Timer update

We set a primary timer that triggers at a rate of `2 * clock / ticks_per_update`.  This is tied to a callback routine.  In this routine, we do the following:

## Timer settings

Here are the timer settings from the previous firmware version:
```
[00.341359] Timers detailed description:
[00.345363] - Total timers defined: 14
[00.349194] TIM1, clock enabled
[00.352375] - SMCR: TS=011 SMS=110
[00.356014] - DIER: CC4DE=1 CC3DE=1 CC2DE=1 CC1DE=1
[00.361345] - SR: UIF=1
[00.363806] - CCMR1: OC2M=001 IC2F=0001 CC2S=01 OC1M=001 IC1F=0001 CC1S=01
[00.371675] - CCMR2: OC4M=001 IC4F=0001 CC4S=01 OC3M=001 IC3F=0001 CC3S=01
[00.379544] - CCER: CC4NP=1 CC4P=1 CC4E=1 CC3NP=1 CC3P=1 CC3E=1 CC2NP=1 CC2P=1 CC2E=1 CC1NP=1 CC1P=1 CC1E=1
[00.390768] - ARR: ARR=65535
[00.393706] TIM2, clock enabled
[00.396886] - CR2: MMS=010
[00.399660] - SMCR: MSM=1 TS=011 SMS=110
[00.403941] - SR: UIF=1
[00.406402] - PSC: PSC=9
[00.408950] - ARR: ARR=8399
[00.411793] TIM3, clock enabled
[00.414974] - SMCR: TS=011 SMS=110
[00.418614] - DIER: CC2DE=1 CC1DE=1
[00.422280] - SR: UIF=1
[00.424741] - CCMR1: OC2M=001 IC2F=0001 CC2S=01 OC1M=001 IC1F=0001 CC1S=01
[00.432611] - CCER: CC2NP=1 CC2P=1 CC2E=1 CC1NP=1 CC1P=1 CC1E=1
[00.439223] - ARR: ARR=32767
[00.442161] TIM4, clock enabled
[00.445342] - DIER: UIE=1
[00.447994] - ARR: ARR=65535
[00.450932] TIM5, clock enabled
[00.454112] - CR1: CEN=1
[00.456660] - DIER: UIE=1
[00.459313] - CNT: CNT=3872
[00.462155] - PSC: PSC=9999
[00.464998] - ARR: ARR=8399
[00.467840] TIM8, clock enabled
[00.471021] - SMCR: TS=010 SMS=110
[00.474661] - DIER: CC4DE=1 CC2DE=1
[00.478336] - SR: UIF=1
[00.480797] - CCMR1: OC2M=001 IC2F=0001 CC2S=01
[00.485780] - CCMR2: OC4M=001 IC4F=0001 CC4S=01
[00.490764] - CCER: CC4NP=1 CC4P=1 CC4E=1 CC2NP=1 CC2P=1 CC2E=1
[00.497376] - ARR: ARR=65535
```