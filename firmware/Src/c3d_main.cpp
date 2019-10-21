//
// Created by tdkostk on 4/27/2019.
//

#include "c3d_includes.h"

#include "stm32f4xx_ll_adc.h"
#include "stm32f4xx_ll_tim.h"

/*
[37.127065] TIM1, clock enabled
[37.130176] - SMCR: TS=011 SMS=110
[37.133660] - SR: CC4OF=1 CC3OF=1 CC2OF=1 CC1OF=1 CC4IF=1 CC3IF=1 CC2IF=1 CC1IF=1
[37.141798] - CCMR1: IC2F=0001 CC2S=01 IC1F=0001 CC1S=01
[37.147483] - CCMR2: IC4F=0001 CC4S=01 IC3F=0001 CC3S=01
[37.153169] - CCER: CC4NP=1 CC4P=1 CC4E=1 CC3NP=1 CC3P=1 CC3E=1 CC2NP=1 CC2P=1 CC2E=1 CC1NP=1 CC1P=1 CC1E=1
[37.163924] - ARR: ARR=65535
[37.166767] TIM2, clock enabled
[37.169878] - CR2: MMS=010
[37.172547] - SMCR: MSM=1 TS=011 SMS=110
[37.176638] - PSC: PSC=99
[37.179195] - ARR: ARR=8399
[37.181942] TIM3, clock enabled
[37.185053] - SMCR: TS=011 SMS=110
[37.188537] - CCMR1: IC2F=0001 CC2S=01 IC1F=0001 CC1S=01
[37.194223] - CCER: CC2NP=1 CC2P=1 CC2E=1 CC1NP=1 CC1P=1 CC1E=1
[37.200575] - ARR: ARR=32767
[37.203418] TIM4, clock enabled
[37.206529] - CR2: MMS=010
[37.209199] - DIER: UIE=1
[37.211755] - CNT: CNT=16382
[37.214598] - ARR: ARR=16383
[37.217441] TIM5, clock enabled
[37.220552] - CR1: CEN=1
[37.223013] - DIER: UIE=1
[37.225570] - SR: CC4IF=1 CC3IF=1 CC2IF=1 CC1IF=1
[37.230519] - CNT: CNT=1950
[37.233266] - PSC: PSC=9999
[37.236013] - ARR: ARR=8399
[37.238761] TIM8, clock enabled
[37.241872] - SMCR: TS=010 SMS=110
[37.245356] - CCMR1: IC2F=0001 CC2S=01
[37.249221] - CCMR2: IC4F=0001 CC4S=01
[37.253087] - CCER: CC4NP=1 CC4P=1 CC4E=1 CC2NP=1 CC2P=1 CC2E=1
[37.259440] - ARR: ARR=65535
 */

// output timer counts
void C3D_OutputTimerCounts(void) {
  __disable_irq();
  uint32_t c1 = TIM1->CNT;
  uint32_t c2 = TIM2->CNT;
  uint32_t c3 = TIM3->CNT;
  uint32_t c4 = TIM4->CNT;
  uint32_t c8 = TIM8->CNT;
  __enable_irq();
  LOG("\nc3d=", c3d_process_count);
  LOG(", TIM1->CNT=", c1, ", TIM2->CNT=", c2, ", TIM3->CNT=", c3);
  LOG(", TIM4->CNT=", c4, ", TIM8->CNT=", c8);
}

// initialize the master timer
void C3D_InitializeMasterTimer(void) {
  auto & TIMx = c3d_master_timer;
  // enable clock
  GSL_TIM_EnableClock(TIMx);
  // disable
  CLEAR_BIT(TIMx->CR1, TIM_CR1_CEN);
  // clear interrupts
  TIMx->SR = 0;
  // set up master timer base unit
  TIMx->PSC = 0;
  ASSERT(TIMx == TIM4);
  TIMx->ARR = 0x3FFF;
  // set counter to N - 1 so it generates an update immediately
  TIMx->CNT = TIMx->ARR - 1;
  // enable TRGO on update event
  MODIFY_REG(TIMx->CR2, TIM_CR2_MMS, TIM_TRGO_UPDATE);
  // enable update interrupt
  SET_BIT(TIMx->DIER, TIM_DIER_UIE);
  // enable IRQ
  ASSERT(TIMx == TIM4);
  HAL_NVIC_SetPriority(TIM4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM4_IRQn);
}

// initialize the signal timers and related DMA streams
void C3D_InitializeSignalTimers(void) {
  // enable clocks
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    GSL_TIM_EnableClock(c3d_signal[i].TIMx);
  }
  // clear interrupts
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    c3d_signal[i].TIMx->SR = 0;
  }
  // disable timer
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    CLEAR_BIT(c3d_signal[i].TIMx->CR1, TIM_CR1_CEN);
  }
  // set up timer base unit
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    c3d_signal[i].TIMx->PSC = 0;
    if (c3d_signal[i].TIMx == TIM1 || c3d_signal[i].TIMx == TIM8) {
      c3d_signal[i].TIMx->ARR = 0xFFFF;
    } else {
      ASSERT(c3d_signal[i].TIMx == TIM3);
      c3d_signal[i].TIMx->ARR = 0x7FFF;
    }
    c3d_signal[i].TIMx->CNT = 0;
  }
  // set up timer IC channels
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    auto & signal = c3d_signal[i];
    auto & TIMx = c3d_signal[i].TIMx;
    // set filter of 2 samples and set mode to 01
    {
      auto reg = &TIMx->CCMR1;
      reg += (signal.timer_channel / 4) / 2;
      uint32_t mask = 0xFF;
      uint32_t config = 0b00010001;
      *reg &= ~mask << (((signal.timer_channel / 4) % 2) * 8);
      *reg |= config << (((signal.timer_channel / 4) % 2) * 8);
    }
    // set CCxP = 1 and CCxE = 1
    {
      auto & reg = TIMx->CCER;
      uint32_t mask = 0b1111;
      uint32_t config = 0b0011;
      reg &= ~mask << ((signal.timer_channel / 4) * 4);
      reg |= config << ((signal.timer_channel / 4) * 4);
    }
  }
  // set to enable when master timer triggers
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    auto & TIMx = c3d_signal[i].TIMx;
    uint32_t source = GSL_TIM_GetInternalTrigger(TIMx, c3d_master_timer);
    MODIFY_REG(TIMx->SMCR, TIM_SMCR_TS, source);
    MODIFY_REG(TIMx->SMCR, TIM_SMCR_SMS, TIM_SLAVEMODE_TRIGGER);
  }
  // enable DIER_CCxDE bits to enable DMA request on CC event
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    auto & signal = c3d_signal[i];
    SET_BIT(signal.TIMx->DIER, TIM_DIER_CC1DE << (signal.timer_channel / 4));
  }
  // set up DMA signal channels
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    auto & signal = c3d_signal[i];
    auto & TIMx = signal.TIMx;
    auto & stream = signal.dma_stream;
    // enable clock
    GSL_DMA_EnableClock(GSL_DMA_GetBase(stream));
    // disable
    CLEAR_BIT(stream->CR, DMA_SxCR_EN);
    // clear register
    stream->CR = 0;
    // set channel selection
    MODIFY_REG(stream->CR, DMA_SxCR_CHSEL, signal.dma_channel);
    // set memory size
    MODIFY_REG(stream->CR, DMA_SxCR_MSIZE, DMA_MDATAALIGN_HALFWORD);
    // set peripheral size
    MODIFY_REG(stream->CR, DMA_SxCR_PSIZE, DMA_PDATAALIGN_HALFWORD);
    // set memory increment
    MODIFY_REG(stream->CR, DMA_SxCR_MINC, DMA_MINC_ENABLE);
    // set circular mode
    MODIFY_REG(stream->CR, DMA_SxCR_CIRC, DMA_CIRCULAR);
    // set highest priority
    MODIFY_REG(stream->CR, DMA_SxCR_PL, DMA_PRIORITY_VERY_HIGH);
    // set direction
    MODIFY_REG(stream->CR, DMA_SxCR_DIR, DMA_PERIPH_TO_MEMORY);
    // set number of transfers to do
    stream->NDTR = signal.buffer_capacity;
    // set peripheral address
    stream->PAR = (uint32_t) (&TIMx->CCR1 +
        (signal.timer_channel - TIM_CHANNEL_1) /
            (TIM_CHANNEL_2 - TIM_CHANNEL_1) * (TIMx->CCR2 - TIMx->CCR1));
    // set memory address
    stream->M0AR = (uint32_t) signal.buffer;
    // enable DMA stream
    SET_BIT(stream->CR, DMA_SxCR_EN);
  }
}

// initialize the ADC timer and related DMA stream
void C3D_InitializeADC(void) {
  auto & ADCx = c3d_adc;
  auto & TIMx = c3d_adc_timer;
  // set up the ADC itself
  {
    GSL_ADC_EnableClock(ADCx);
    // disable the ADC
    CLEAR_BIT(ADCx->CR2, ADC_CR2_ADON);
    // enable scan mode (convert all channels on trigger)
    SET_BIT(ADCx->CR1, ADC_CR1_SCAN);
    // trigger on TRGO rising edge
    MODIFY_REG(ADCx->CR2, ADC_CR2_EXTEN, ADC_EXTERNALTRIGCONVEDGE_RISING);
    // trigger off of TIM_TRGO signal
    MODIFY_REG(ADCx->CR2, ADC_CR2_EXTSEL, ADC_EXTERNALTRIGCONV_T2_TRGO);
    // enable DMA
    SET_BIT(ADCx->CR2, ADC_CR2_DDS);
    SET_BIT(ADCx->CR2, ADC_CR2_DMA);
    // configure channels
    for (uint16_t i = 0; i < c3d_adc_channel_count; ++i) {
      // set sampling time
      LL_ADC_SetChannelSamplingTime(ADCx, i, ADC_SAMPLETIME_480CYCLES);
      // set this rank to convert this channel
      GSL_ADC_SetRegularSequenceChannel(ADCx, i, c3d_adc_channel[i]);
    }
    // turn on the ADC
    SET_BIT(ADCx->CR2, ADC_CR2_ADON);
  }
  // set up ADC timer
  {
    // enable the clock
    GSL_TIM_EnableClock(TIMx);
    // clear interrupts
    TIMx->SR = 0;
    CLEAR_BIT(TIMx->CR1, TIM_CR1_CEN);
    // set up timer base unit
    TIMx->PSC = 99;
    TIMx->ARR = 8399;
    TIMx->CNT = 0;
    // set update event as TRGO output
    MODIFY_REG(TIMx->CR2, TIM_CR2_MMS, LL_TIM_TRGO_UPDATE);
    // set to enable when master timer triggers
    uint32_t source = GSL_TIM_GetInternalTrigger(TIMx, c3d_master_timer);
    MODIFY_REG(TIMx->SMCR, TIM_SMCR_TS, source);
    MODIFY_REG(TIMx->SMCR, TIM_SMCR_SMS, TIM_SLAVEMODE_TRIGGER);
  }
  // set up ADC DMA stream
  {
    auto & stream = c3d_adc_dma_monitor.dma_stream;
    // enable clock
    GSL_DMA_EnableClock(GSL_DMA_GetBase(stream));
    // disable
    CLEAR_BIT(stream->CR, DMA_SxCR_EN);
    // clear register
    stream->CR = 0;
    // set channel selection
    {
      ASSERT(c3d_adc == ADC1 || c3d_adc == ADC2 || c3d_adc == ADC3);
      uint32_t chsel =
          (c3d_adc == ADC1) ? DMA_CHANNEL_0 :
          (c3d_adc == ADC2) ? DMA_CHANNEL_1 :
          (c3d_adc == ADC3) ? DMA_CHANNEL_2 : 0;
      MODIFY_REG(stream->CR, DMA_SxCR_CHSEL, chsel);
    }
    // set memory size
    MODIFY_REG(stream->CR, DMA_SxCR_MSIZE, DMA_MDATAALIGN_HALFWORD);
    // set peripheral size
    MODIFY_REG(stream->CR, DMA_SxCR_PSIZE, DMA_PDATAALIGN_HALFWORD);
    // set memory increment
    MODIFY_REG(stream->CR, DMA_SxCR_MINC, DMA_MINC_ENABLE);
    // set circular mode
    MODIFY_REG(stream->CR, DMA_SxCR_CIRC, DMA_CIRCULAR);
    // set high priority (below signal channels)
    MODIFY_REG(stream->CR, DMA_SxCR_PL, DMA_PRIORITY_HIGH);
    // set direction
    MODIFY_REG(stream->CR, DMA_SxCR_DIR, DMA_PERIPH_TO_MEMORY);
    // set number of transfers to do
    stream->NDTR = c3d_adc_buffer_capacity;
    // set peripheral address
    stream->PAR = (uint32_t) &(c3d_adc->DR);
    // set memory address
    stream->M0AR = (uint32_t) c3d_adc_buffer;
    // enable DMA stream
    SET_BIT(stream->CR, DMA_SxCR_EN);
  }
}

// initialize signal timers and DMA stuff
void C3D_InitTimers(void) {
  // initialize the master clock
  {}
  // enable clocks
  __HAL_RCC_TIM1_CLK_ENABLE();
  __HAL_RCC_TIM3_CLK_ENABLE();
  __HAL_RCC_TIM4_CLK_ENABLE();
  __HAL_RCC_TIM8_CLK_ENABLE();
  // disable timer and
  // disable timers
  TIM1->CR1 &= ~TIM_CR1_CEN;
  TIM3->CR1 &= ~TIM_CR1_CEN;
  TIM4->CR1 &= ~TIM_CR1_CEN;
  TIM8->CR1 &= ~TIM_CR1_CEN;
  // zero out count
  TIM1->CNT = 0;
  TIM3->CNT = 0;
  TIM4->CNT = 0;
  TIM8->CNT = 0;
  // set prescalers
  TIM1->PSC = 0;
  TIM3->PSC = 0;
  TIM4->PSC = 0;
  TIM8->PSC = 0;
  // set ARR
  TIM1->ARR = 0xFFFF;
  TIM3->ARR = 0x7FFF;
  TIM4->ARR = 0x3FFF;
  TIM8->ARR = 0xFFFF;

  // set up IC channels
  TIM_TypeDef * TIMx = nullptr;
  // TIM1->CH4
  TIMx = TIM1;
  GSL_PIN_InitializeAF(kPinA11, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_AF1_TIM1);
  CLEAR_BIT(TIMx->CCMR2, TIM_CCMR2_OC4CE);
  MODIFY_REG(TIMx->CCMR2, TIM_CCMR2_OC4M, TIM_OCMODE_PWM1 << 8);
  SET_BIT(TIMx->CCMR2, TIM_CCMR2_OC4PE);
  TIMx->CCR4 = 0x8000;
  SET_BIT(TIMx->BDTR, TIM_BDTR_MOE);
  SET_BIT(TIMx->CCER, TIM_CCER_CC4E);
  // set to trigger when TIM4 enables
  MODIFY_REG(TIMx->SMCR, TIM_SMCR_TS, TIM_CLOCKSOURCE_ITR3);
  MODIFY_REG(TIMx->SMCR, TIM_SMCR_SMS, TIM_SLAVEMODE_TRIGGER);

  // TIM3->CH1
  TIMx = TIM3;
  GSL_PIN_InitializeAF(kPinB4, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_AF2_TIM3);
  CLEAR_BIT(TIMx->CCMR1, TIM_CCMR1_OC1CE);
  MODIFY_REG(TIMx->CCMR1, TIM_CCMR1_OC1M, TIM_OCMODE_PWM1);
  SET_BIT(TIMx->CCMR1, TIM_CCMR1_OC1PE);
  TIMx->CCR1 = 0x4000;
  SET_BIT(TIMx->CCER, TIM_CCER_CC1E);
  // set to trigger when TIM4 enables
  MODIFY_REG(TIMx->SMCR, TIM_SMCR_TS, TIM_CLOCKSOURCE_ITR3);
  MODIFY_REG(TIMx->SMCR, TIM_SMCR_SMS, TIM_SLAVEMODE_TRIGGER);

  // TIM8->CH4
  TIMx = TIM8;
  GSL_PIN_InitializeAF(kPinC9, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_AF3_TIM8);
  CLEAR_BIT(TIMx->CCMR2, TIM_CCMR2_OC4CE);
  MODIFY_REG(TIMx->CCMR2, TIM_CCMR2_OC4M, TIM_OCMODE_PWM1 << 8);
  SET_BIT(TIMx->CCMR2, TIM_CCMR2_OC4PE);
  TIMx->CCR4 = 0x8000;
  SET_BIT(TIMx->BDTR, TIM_BDTR_MOE);
  SET_BIT(TIMx->CCER, TIM_CCER_CC4E);
  // set to trigger when TIM4 enables
  MODIFY_REG(TIMx->SMCR, TIM_SMCR_TS, TIM_CLOCKSOURCE_ITR2);
  MODIFY_REG(TIMx->SMCR, TIM_SMCR_SMS, TIM_SLAVEMODE_TRIGGER);

  TIM1->CNT = 1;
  TIM3->CNT = 2;
  TIM8->CNT = 1;

  // enable master timer
  TIMx = TIM4;
  MODIFY_REG(TIMx->CR2, TIM_CR2_MMS, TIM_TRGO_UPDATE);

  // enable master timer
  SET_BIT(TIM4->CR1, TIM_CR1_CEN);
  //TIM4->EGR = TIM_EGR_UG;
  //SET_BIT(TIM4->DIER, TIM_DIER_UIE);

  GSL_DEL_MS(1);
  LOG("\nTIM4->CNT=", TIM4->CNT);
  GSL_DEL_MS(1);
  LOG("\nTIM4->CNT=", TIM4->CNT);
  GSL_DEL_MS(1);
  LOG("\nTIM4->CNT=", TIM4->CNT);
  GSL_DEL_MS(1);
  LOG("\nTIM4->CNT=", TIM4->CNT);

  //GSL_TIM_LogDetailedInformation();

  // wait for timer to roll over
  for (uint16_t i = 0; i < 10; ++i) {
    uint32_t count = TIM4->CNT;
    uint32_t next = TIM4->CNT;
    while (count < next) {
      count = next;
      next = TIM4->CNT;
    }
    C3D_OutputTimerCounts();
  }

  while (1) {
    LIMITED_LOG(5, "\nTIM1->CNT=", TIM1->CNT);
    GSL_PIN_Toggle(c3d_debug_pin[0]);
  }

}

// create and send a new data packet
void C3D_CreatePacket(void) {

  // quickly, store the head index for each channel
  for (uint16_t s = 0; s < c3d_signal_count; ++s) {
    auto & monitor = c3d_signal_dma_monitor[s];
    monitor.head_index = monitor.GetHeadIndex();
  }

  // number of bytes added by this packet
  uint16_t byte_count = 0;

  // for each signal, increment middle_index to end of edges to send
  for (uint16_t s = 0; s < c3d_signal_count; ++s) {
    // alias DMA buffer monitor for this channel
    auto & monitor = c3d_signal_dma_monitor[s];
    const auto & capacity = monitor.capacity;
    const auto & middle_index = monitor.middle_index;
    // move middle edge to end of edges to send
    uint32_t head_index = monitor.head_index;
    // TODO: implement
    while (head_index != middle_index &&
        monitor.buffer[(head_index + capacity - 1) % capacity] <
            monitor.middle_count) {
      head_index = (head_index + capacity - 1) % capacity;
    }
    monitor.middle_index = head_index;
  }

  // output packet number
  c3d_usb_buffer.StageVariable((uint8_t) (c3d_process_count / 2));
  ++byte_count;

  // create channel mask
  uint8_t channel_mask = 0;
  for (uint16_t s = 0; s < c3d_signal_count; ++s) {
    auto & monitor = c3d_signal_dma_monitor[s];
    if (monitor.middle_index != monitor.tail_index) {
      channel_mask |= 1 << s;
    }
  }

  // add channel mask
  c3d_usb_buffer.StageVariable(channel_mask);
  ++byte_count;

  // add edges for each signal
  for (uint16_t s = 0; s < c3d_signal_count; ++s) {
    // skip channel if it has no edges
    if ((channel_mask & (1 << s)) == 0) {
      continue;
    }
    // alias DMA buffer monitor for this channel
    auto & monitor = c3d_signal_dma_monitor[s];
    // count edges
    uint16_t count =
        (monitor.middle_index + monitor.capacity - monitor.tail_index) %
        monitor.capacity;
    // ensure count is as expected
    if (count >= 256) {
      LIMITED_RUN(10) {
        LOG("\nc3d = ", c3d_process_count);
        LOG(", tail_index = ", monitor.tail_index);
        LOG(", middle_index = ", monitor.middle_index);
        LOG(", count = ", count);
        LOG(", capacity = ", monitor.capacity);
      }
      count = 255;
    }
    //ASSERT_GT(count, 0);
    //ASSERT_LT(count, 256);
    // add number of edges for this signal packet
    c3d_usb_buffer.StageVariable((uint8_t) count);
    ++byte_count;
    // add edge counts
    while (count--) {
      // store this edge
      c3d_usb_buffer.StageVariable(monitor.buffer[monitor.tail_index]);
      // increment index
      monitor.tail_index = (monitor.tail_index + 1) % monitor.capacity;
      byte_count += 2;
    }
    monitor.tail_index = monitor.middle_index;
  }

  // output ADC info
  if (c3d_output_adc_channels) {
    // add ADC signals
    uint8_t sample_count =
        c3d_adc_dma_monitor.GetAvailable() / c3d_adc_channel_count;
    c3d_usb_buffer.StageVariable(sample_count);
    ++byte_count;
    for (uint16_t i = 0; i < sample_count; ++i) {
      for (uint16_t s = 0; s < c3d_adc_channel_count; ++s) {
        uint16_t value = c3d_adc_dma_monitor.Pop();
        c3d_usb_buffer.StageVariable(value);
      }
    }
    byte_count += sample_count * c3d_adc_channel_count * sizeof(uint16_t);
  } else {
    c3d_usb_buffer.StageVariable((uint8_t) 0);
    ++byte_count;
  }

  // update bits per packet
  c3d_output_bits_per_process.Update(byte_count * 8);

}


// callback timer

extern "C" {

void TIM4_IRQHandler(void) {
  GSL_PIN_SetHigh(c3d_debug_pin[0]);
  LIMITED_RUN(5) {
    C3D_OutputTimerCounts();
  }
  // clear update bit
  TIM4->SR = ~TIM_SR_UIF;
  // if not outputting, just return
  if (!c3d_streaming_flag) {
    GSL_PIN_SetLow(c3d_debug_pin[0]);
    return;
  }
  // increment process count
  ++c3d_process_count;
  // on odd counters, update middle index
  if (c3d_process_count % 2) {
    for (uint16_t s = 0; s < c3d_signal_count; ++s) {
      // alias DMA buffer monitor for this channel
      auto & monitor = c3d_signal_dma_monitor[s];
      monitor.middle_index = monitor.GetHeadIndex();
    }
  } else {
    C3D_CreatePacket();
  }
  // update
  GSL_PIN_SetLow(c3d_debug_pin[0]);
}

}

// called to process buffers and send out new data as needed
/*void C3D_ProcessBuffers(void) {

  GSL_PIN_SetHigh(c3d_debug_pin[0]);

  // if we're not outputting data, just return
  if (!c3d_output_to_usb) {
    GSL_PIN_SetLow(c3d_debug_pin[0]);
    return;
  }

  // increment process count
  ++c3d_process_count;

  // after starting streaming, the first call to this will set
  // c3d_process_count = 0
  // an timers should be low

  // on odd calls, just store NDTR

  if (c3d_process_count < 10 && c3d_process_count % 3 == 0) {
    C3D_OutputTimerCounts();
  }

  uint16_t byte_count = 0;

  // add sync every 8th frame
  if (c3d_process_count % 8 == 0) {
    uint8_t sync = 0x77;
    c3d_usb_buffer.StageData(&sync, 1);
    ++byte_count;
  }
  // add frame counter
  c3d_usb_buffer.StageData(&c3d_process_count, 1);
  ++byte_count;

  // establish channel mask
  uint8_t channel_mask = 0;
  for (uint16_t s = 0; s < c3d_signal_count; ++s) {
    // alias DMA buffer monitor for this channel
    auto & monitor = c3d_signal_usb_dma_monitor[s];
    if (!monitor.IsEmpty()) {
      channel_mask |= (1U << s);
    }
  }

  // output channel mask
  c3d_usb_buffer.StageVariable(channel_mask);
  ++byte_count;

  // process signals and move them into the staging buffer
  for (uint16_t s = 0; s < c3d_signal_count; ++s) {
    // if this isn't in the mask, don't output anything
    if ((channel_mask & (1U << s)) == 0) {
      continue;
    }
    // alias the DMA buffer monitor for this channel
    auto & monitor = c3d_signal_usb_dma_monitor[s];
    // read out all available data
    uint8_t * buffer_1 = nullptr;
    uint16_t byte_count_1 = 0;
    uint8_t * buffer_2 = nullptr;
    uint16_t byte_count_2 = 0;
    monitor.PopMany(buffer_1, byte_count_1);
    monitor.PopMany(buffer_2, byte_count_2);

    uint16_t count = (byte_count_2 + byte_count_1) / sizeof(uint16_t);

    // if nothing is available, increase the no-edge count
    //uint16_t count = monitor.GetAvailable();
    ASSERT_GT(count, 0);
    ASSERT_LT(count, 256);
    c3d_usb_buffer.StageVariable((uint8_t) count);
    byte_count += 1;

    byte_count += byte_count_1;
    byte_count += byte_count_2;

    c3d_usb_buffer.StageData(buffer_1, byte_count_1);
    if (byte_count_2) {
      c3d_usb_buffer.StageData(buffer_2, byte_count_2);
    }

  }

  // add ADC signals
  uint8_t sample_count =
      c3d_adc_dma_monitor.GetAvailable() / c3d_adc_channel_count;
  c3d_usb_buffer.StageVariable(sample_count);
  ++byte_count;
  for (uint16_t i = 0; i < sample_count; ++i) {
    for (uint16_t s = 0; s < c3d_adc_channel_count; ++s) {
      uint16_t value = c3d_adc_dma_monitor.Pop();
      c3d_usb_buffer.StageVariable(value);
    }
  }
  byte_count += sample_count * c3d_adc_channel_count * sizeof(uint16_t);

  c3d_output_bits_per_process.Update(byte_count * 8);

  GSL_PIN_SetLow(c3d_debug_pin[0]);

}*/

// stop streaming data
void C3D_StopStreaming(void) {
  LOG("\nStopping stream");
  // stop timers, disable DMA requests, clear flags
  __disable_irq();
  CLEAR_BIT(c3d_master_timer->CR1, TIM_CR1_CEN);
  CLEAR_BIT(c3d_adc_timer->CR1, TIM_CR1_CEN);
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    auto & TIMx = c3d_signal[i].TIMx;
    CLEAR_BIT(TIMx->CR1, TIM_CR1_CEN);
    TIMx->DIER = 0;
    TIMx->SR = 0;
  }
  // disable DMA
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    auto & TIMx = c3d_signal[i].TIMx;
    auto & stream = c3d_signal[i].dma_stream;
    CLEAR_BIT(stream->CR, DMA_SxCR_EN);
    while (READ_BIT(stream->CR, DMA_SxCR_EN)) {
    }
    stream->NDTR = 0;
  }
  // advance chunk so everything gets sent out
  c3d_usb_buffer.AdvanceChunk();
  __enable_irq();
}

// set the start streaming flag
void C3D_EnableStartStreamingFlag(void) {
  c3d_target_streaming_flag = true;
}

// set the start streaming flag
void C3D_EnableStopStreamingFlag(void) {
  c3d_target_streaming_flag = false;
}

// start streaming data
/*
[02.425740] DMA2 stream index 11 detailed description:
[02.431053] - CR: SxCR_CHSEL=111 SxCR_MSIZE=01 SxCR_PSIZE=01 SxCR_MINC=1 SxCR_CIRC=1 SxCR_TCIE=1 SxCR_DMEIE=1 SxCR_EN=1
[02.442848] - NDTR: SxNDT=0x0800
[02.446072] - PAR: SxPAR_PA=0x40010438
[02.449868] - M0AR: SxM0AR_M0A=0x2001248C
[02.453950] - FCR: SxFCR_FEIE=1 SxFCR_FS=100
[02.458370]
[02.459644] DMA2 stream index 8 detailed description:
[02.464861] - CR: SxCR_PL=10 SxCR_MSIZE=01 SxCR_PSIZE=01 SxCR_MINC=1 SxCR_CIRC=1 SxCR_DMEIE=1 SxCR_EN=1
[02.475088] - NDTR: SxNDT=0x0038
[02.478312] - PAR: SxPAR_PA=0x4001204C
[02.482108] - M0AR: SxM0AR_M0A=0x20001F84
[02.486190] - FCR: SxFCR_FEIE=1 SxFCR_FS=100
[02.490610]
[02.491884] ADC1 detailed description:
[02.495654] - CR1: OVRIE=1 SCAN=1
[02.499008] - CR2: EXTEN=01 EXTSEL=0110 DDS=1 DMA=1 ADON=1
[02.504884] - SMPR1: SMP18=010 SMP17=010 SMP16=010 SMP15=111 SMP14=111 SMP13=111 SMP12=111 SMP11=111 SMP10=111
[02.515977] - SMPR2: SMP9=111 SMP8=111 SMP7=111 SMP6=111 SMP3=111 SMP2=111 SMP1=111 SMP0=111
[02.525311] - HTR: HT=111111111111
[02.528821] - SQR1: L=1101 SQ14=01001 SQ13=01000
[02.533735] - SQR2: SQ12=01111 SQ11=01110 SQ10=00111 SQ9=00110 SQ8=00011 SQ7=00010
[02.542107] - SQR3: SQ6=00001 SQ4=01101 SQ3=01100 SQ2=01011 SQ1=01010
[02.549171] - DR: DATA=0x03C7
 */
void C3D_StartStreaming(void) {
  LOG("\nStarting data stream.");
  // ensure we're not in an interrupt
  ASSERT(!GSL_GEN_InInterrupt());
  // initialize master timer (but don't start it)
  C3D_InitializeMasterTimer();
  // initialize signal timers
  C3D_InitializeSignalTimers();
  // initialize adc timer
  C3D_InitializeADC();
  // start the timer

  // enable timer clocks (but not the timers themselves)
//  GSL_TIM_EnableClock(c3d_master_timer);
//  GSL_TIM_EnableClock(c3d_adc_timer);
//  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
//    GSL_TIM_EnableClock(c3d_signal[i].TIMx);
//  }
  // disable master timer and clear all interrupts
//  CLEAR_BIT(c3d_master_timer->CR1, TIM_CR1_CEN);
  // clear all pending interrupts
//  c3d_master_timer->SR = 0;
//  c3d_adc_timer->SR = 0;
//  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
//    c3d_signal[i].TIMx->SR = 0;
//  }
  // disable slave timers
//  CLEAR_BIT(c3d_adc_timer->CR1, TIM_CR1_CEN);
//  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
//    CLEAR_BIT(c3d_signal[i].TIMx->CR1, TIM_CR1_CEN);
//  }
  // ensure all timers are disabled
//  ASSERT(READ_BIT(c3d_master_timer->CR1, TIM_CR1_CEN) == 0);
//  ASSERT(READ_BIT(c3d_adc_timer->CR1, TIM_CR1_CEN) == 0);
//  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
//    ASSERT(READ_BIT(c3d_signal[i].TIMx->CR1, TIM_CR1_CEN) == 0);
//  }
  // disable DMA requests DIER_CCxDE bits to disable DMA requests
//  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
//    auto & signal = c3d_signal[i];
//    CLEAR_BIT(signal.TIMx->DIER, TIM_DIER_CC1DE << (signal.timer_channel / 4));
//  }
  // set up master timer base unit
//  c3d_master_timer->PSC = 0;
//  c3d_master_timer->ARR = 0x3FFF;
//  c3d_master_timer->CNT = c3d_master_timer->ARR - 1;
  // set up the ADC timer base unit
//  c3d_adc_timer->PSC = 99;
//  c3d_adc_timer->ARR = 8399;
//  c3d_adc_timer->CNT = 0;
  // set up signal timer base units
//  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
//    c3d_signal[i].TIMx->PSC = 0;
//    if (c3d_signal[i].TIMx == TIM1 || c3d_signal[i].TIMx == TIM8) {
//      c3d_signal[i].TIMx->ARR = 0xFFFF;
//    } else {
//      ASSERT(c3d_signal[i].TIMx == TIM3);
//      c3d_signal[i].TIMx->ARR = 0x7FFF;
//    }
//    c3d_signal[i].TIMx->CNT = 0;
//  }
  // set up ADC
  {
  }
  // set up ADC DMA channel
//  {
//    auto & stream = c3d_adc_dma_monitor.dma_stream;
//    // disable
//    CLEAR_BIT(stream->CR, DMA_SxCR_EN);
//    // clear register
//    stream->CR = 0;
//    // set channel selection
//    {
//      ASSERT(c3d_adc == ADC1 || c3d_adc == ADC2 || c3d_adc == ADC3);
//      uint32_t chsel =
//          (c3d_adc == ADC1) ? DMA_CHANNEL_0 :
//          (c3d_adc == ADC2) ? DMA_CHANNEL_1 :
//          (c3d_adc == ADC3) ? DMA_CHANNEL_2 : 0;
//      MODIFY_REG(stream->CR, DMA_SxCR_CHSEL, chsel);
//    }
//    // set memory size
//    MODIFY_REG(stream->CR, DMA_SxCR_MSIZE, DMA_MDATAALIGN_HALFWORD);
//    // set peripheral size
//    MODIFY_REG(stream->CR, DMA_SxCR_PSIZE, DMA_PDATAALIGN_HALFWORD);
//    // set memory increment
//    MODIFY_REG(stream->CR, DMA_SxCR_MINC, DMA_MINC_ENABLE);
//    // set circular mode
//    MODIFY_REG(stream->CR, DMA_SxCR_CIRC, DMA_CIRCULAR);
//    // set high priority (below signal channels)
//    MODIFY_REG(stream->CR, DMA_SxCR_PL, DMA_PRIORITY_HIGH);
//    // set direction
//    MODIFY_REG(stream->CR, DMA_SxCR_DIR, DMA_PERIPH_TO_MEMORY);
//    // set number of transfers to do
//    stream->NDTR = c3d_adc_buffer_capacity;
//    // set peripheral address
//    stream->PAR = (uint32_t) &(c3d_adc->DR);
//    // set memory address
//    stream->M0AR = (uint32_t) c3d_adc_buffer;
//  }
  /*{
    LIMITED_RUN(1) {
      GSL_TIM_LogDetailedInformation();
      GSL_DMA_LogDetailedInformation(DMA1);
      GSL_DMA_LogDetailedInformation(DMA2);
      for (uint16_t i = 0; i < c3d_signal_count; ++i) {
        GSL_DMA_LogDetailedStreamInformation(c3d_signal[i].dma_stream);
      }
      GSL_DMA_LogDetailedStreamInformation(c3d_adc_dma_monitor.dma_stream);
      GSL_ADC_LogDetailedInformation(c3d_adc);
    }
  }*/
  // ensure all timers are disabled
  ASSERT(READ_BIT(c3d_master_timer->CR1, TIM_CR1_CEN) == 0);
  ASSERT(READ_BIT(c3d_adc_timer->CR1, TIM_CR1_CEN) == 0);
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    ASSERT(READ_BIT(c3d_signal[i].TIMx->CR1, TIM_CR1_CEN) == 0);
  }
  // ensure counters are at 0
  ASSERT(c3d_adc_timer->CNT == 0);
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    ASSERT(c3d_signal[i].TIMx->CNT == 0);
  }
  // read DMA monitors
  {
    auto & monitor = c3d_adc_dma_monitor;
    while (monitor.GetAvailable()) {
      monitor.Pop();
    }
  }
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    auto & monitor = c3d_signal_dma_monitor[i];
    monitor.tail_index = monitor.GetHeadIndex();
    monitor.middle_index = monitor.GetHeadIndex();
  }
  // look at signals and if they are high, trigger the IC DMA request flag
  // (this is so signals have the correct polarity)
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    auto & signal = c3d_signal[i];
    auto pin = GSL_TIM_GetChannelPin(signal.TIMx, signal.timer_channel);
    if (GSL_PIN_GetValue(pin)) {
      LOG("\nChannel ", i, " pin is high!");
      if (c3d_signal[i].timer_channel == TIM_CHANNEL_1) {
        SET_BIT(c3d_signal[i].TIMx->EGR, TIM_EGR_CC1G);
      } else if (c3d_signal[i].timer_channel == TIM_CHANNEL_2) {
        SET_BIT(c3d_signal[i].TIMx->EGR, TIM_EGR_CC2G);
      } else if (c3d_signal[i].timer_channel == TIM_CHANNEL_3) {
        SET_BIT(c3d_signal[i].TIMx->EGR, TIM_EGR_CC3G);
      } else if (c3d_signal[i].timer_channel == TIM_CHANNEL_4) {
        SET_BIT(c3d_signal[i].TIMx->EGR, TIM_EGR_CC4G);
      } else {
        HALT("Unexpected value");
      }
    }
  }
  // enable ADC DMA stream
//  SET_BIT(c3d_adc_dma_monitor.dma_stream->CR, DMA_SxCR_EN);
  // enable signal DMA streams
//  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
//    SET_BIT(c3d_signal[i].dma_stream->CR, DMA_SxCR_EN);
//  }
  // start the master, which will start all chained timers
//  c3d_master_timer->ARR = 0x3FFF;
//  c3d_master_timer->CNT = c3d_master_timer->ARR - 1;
//  MODIFY_REG(c3d_master_timer->CR2, TIM_CR2_MMS, TIM_TRGO_UPDATE);
  // reset process counter
  c3d_process_count = 0xFFFFFFFF;
  if (false) {
    LIMITED_RUN(1) {
      GSL_TIM_LogDetailedInformation();
      GSL_DMA_LogDetailedInformation(DMA1);
      GSL_DMA_LogDetailedInformation(DMA2);
      for (uint16_t i = 0; i < c3d_signal_count; ++i) {
        GSL_DMA_LogDetailedStreamInformation(c3d_signal[i].dma_stream);
      }
      GSL_DMA_LogDetailedStreamInformation(c3d_adc_dma_monitor.dma_stream);
      GSL_ADC_LogDetailedInformation(c3d_adc);
    }
  }
  // send info header packet
  C3D_SendInfoHeaderPacket();
  // enable the master timer
  SET_BIT(c3d_master_timer->CR1, TIM_CR1_CEN);
  // enable DIER_CCxDE bits to enable DMA requests on CC event
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    auto & signal = c3d_signal[i];
    SET_BIT(signal.TIMx->DIER, TIM_DIER_CC1DE << (signal.timer_channel / 4));
  }
  // verify all timers have started
  if (true) {
    GSL_DEL_MS(1);
    // ensure all timers are enabled
    ASSERT(READ_BIT(c3d_master_timer->CR1, TIM_CR1_CEN));
    ASSERT(READ_BIT(c3d_adc_timer->CR1, TIM_CR1_CEN));
    for (uint16_t i = 0; i < c3d_signal_count; ++i) {
      ASSERT(READ_BIT(c3d_signal[i].TIMx->CR1, TIM_CR1_CEN));
    }
  }

}

// send info packet
void C3D_Debug(void) {
  c3d_debug_flag = true;
}

// reset the board
void C3D_Reset(void) {
  GSL_DEL_MS(100);
  GSL_GEN_DisableInterrupts();
  NVIC_SystemReset();
}

// send status information
void C3D_Status(void) {
  LOG("\n\nLow-level status:");
  {
    GSL_TIM_LogDetailedInformation();
    GSL_DMA_LogDetailedInformation(DMA1);
    GSL_DMA_LogDetailedInformation(DMA2);
    for (uint16_t i = 0; i < c3d_signal_count; ++i) {
      GSL_DMA_LogDetailedStreamInformation(c3d_signal[i].dma_stream);
    }
    GSL_DMA_LogDetailedStreamInformation(c3d_adc_dma_monitor.dma_stream);
    GSL_ADC_LogDetailedInformation(c3d_adc);
  }
  LOG("\n\nStatus:");
  LOG("\nc3d_process_count = ", c3d_process_count);
  LOG(", c3d_streaming_flag = ", c3d_streaming_flag);
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    LOG("\nSignal ", i);
    auto & TIMx = c3d_signal[i].TIMx;
    auto & channel = c3d_signal[i].timer_channel;
    LOG(" on TIM", GSL_TIM_GetNumber(TIMx) + 1);
    LOG("_CH", 1 + (channel - TIM_CHANNEL_1) / (TIM_CHANNEL_2 - TIM_CHANNEL_1));
    LOG(", CEN=", TIMx->CR1 & TIM_CR1_CEN);
    LOG(", CNT=", TIMx->CNT);
    auto & stream = c3d_signal[i].dma_stream;
    LOG(", on DMA", GSL_DMA_GetIndex(stream));
    LOG(", NDTR=", stream->NDTR);
  }
  for (uint16_t i = 0; i < c3d_motor_count; ++i) {
    auto & motor = c3d_motor[i];
    LOG("\nMotor ", i + 1, ":");
    LOG(" mm=", GSL_OUT_FixedFloat(motor.GetPositionMM(), 3));
    LOG(", pos=", GSL_OUT_SignedInteger(motor.step_position));
    LOG(", step=", (motor.step_is_high) ? "high" : "low");
    LOG(", dir=", (motor.dir_is_high) ? "high" : "low");
  }
}

// send info packet
void C3D_SendInfoHeaderPacket(void) {
  // construct info packet
  // char[9] start_string
  c3d_usb_buffer.StageData("InfoStart", 9);
  // uint8_t header_packet_version
  c3d_usb_buffer.StageVariable((uint8_t) 1);
  // uint8_t streaming_packet_version
  c3d_usb_buffer.StageVariable((uint8_t) 1);
  // uint32_t system_clock
  auto clock = (uint32_t) HAL_RCC_GetSysClockFreq();
  c3d_usb_buffer.StageVariable(clock);
  // uint8_t signal_channel_count
  c3d_usb_buffer.StageVariable((uint8_t) c3d_signal_count);
  // uint8_t adc_channel_count
  if (c3d_output_adc_channels) {
    c3d_usb_buffer.StageVariable((uint8_t) c3d_adc_channel_count);
  } else {
    c3d_usb_buffer.StageVariable((uint8_t) 0);
  }
  // uint32_t ticks_per_adc_reading
  uint32_t ticks_per_adc_reading =
      clock / GSL_TIM_GetFrequency(c3d_adc_timer) + 0.5f;
  c3d_usb_buffer.StageVariable(ticks_per_adc_reading);
  // for each signal channel:
  // uint32_t signal_clock
  // uint32_t update_ticks
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
//    uint32_t overflow =
//        frequency / GSL_TIM_GetFrequency(c3d_signal[i].TIMx) + 0.5f;
//    ASSERT(overflow == 65536 || overflow == 32768);
//    c3d_usb_buffer.StageVariable(overflow);
    uint32_t overflow = c3d_signal[i].TIMx->ARR + 1;
    uint32_t frequency = clock;
    ASSERT_LE(overflow, 65536);
    ASSERT_EQ(65536 % overflow, 0);
    frequency /= (65536 / overflow);
    c3d_usb_buffer.StageVariable(frequency);
    c3d_usb_buffer.StageVariable(overflow);
  }
  // for each ADC channel:
  // float zero_value
  // float high_value
  if (c3d_output_adc_channels) {
    for (uint16_t i = 0; i < c3d_adc_channel_count; ++i) {
      float zero_value = 0.0f;
      float high_value = 3.3f * (c3d_adc_high_resistor + c3d_adc_low_resistor) /
          c3d_adc_low_resistor;
      c3d_usb_buffer.StageVariable(zero_value);
      c3d_usb_buffer.StageVariable(high_value);
    }
  }
  // char[8]
  c3d_usb_buffer.StageData("InfoStop", 8);
  // advance chunk to get information to be sent
  c3d_usb_buffer.AdvanceChunk();
}

// a replacement for HAL_TIM_IC_Start_DMA without starting the timer
HAL_StatusTypeDef QUIETLY_HAL_TIM_IC_Start_DMA(
    TIM_HandleTypeDef *htim,
    uint32_t Channel,
    uint32_t *pData,
    uint16_t Length) {
  /* Check the parameters */
  assert_param(IS_TIM_CCX_INSTANCE(htim->Instance, Channel));
  assert_param(IS_TIM_DMA_CC_INSTANCE(htim->Instance));

  if (htim->State == HAL_TIM_STATE_BUSY) {
    return HAL_BUSY;
  } else if (htim->State == HAL_TIM_STATE_READY) {
    if ((pData == 0U) && (Length > 0)) {
      return HAL_ERROR;
    } else {
      htim->State = HAL_TIM_STATE_BUSY;
    }
  }

  switch (Channel)
  {
    case TIM_CHANNEL_1:
    {
      /* Set the DMA Period elapsed callback */
      htim->hdma[TIM_DMA_ID_CC1]->XferCpltCallback = TIM_DMACaptureCplt;

      /* Set the DMA error callback */
      htim->hdma[TIM_DMA_ID_CC1]->XferErrorCallback = TIM_DMAError ;

      /* Enable the DMA Stream */
      HAL_DMA_Start_IT(
          htim->hdma[TIM_DMA_ID_CC1],
          (uint32_t)&htim->Instance->CCR1,
          (uint32_t)pData, Length);

      /* Enable the TIM Capture/Compare 1 DMA request */
      __HAL_TIM_ENABLE_DMA(htim, TIM_DMA_CC1);
    }
      break;

    case TIM_CHANNEL_2:
    {
      /* Set the DMA Period elapsed callback */
      htim->hdma[TIM_DMA_ID_CC2]->XferCpltCallback = TIM_DMACaptureCplt;

      /* Set the DMA error callback */
      htim->hdma[TIM_DMA_ID_CC2]->XferErrorCallback = TIM_DMAError ;

      /* Enable the DMA Stream */
      HAL_DMA_Start_IT(
          htim->hdma[TIM_DMA_ID_CC2],
          (uint32_t)&htim->Instance->CCR2,
          (uint32_t)pData, Length);

      /* Enable the TIM Capture/Compare 2  DMA request */
      __HAL_TIM_ENABLE_DMA(htim, TIM_DMA_CC2);
    }
      break;

    case TIM_CHANNEL_3:
    {
      /* Set the DMA Period elapsed callback */
      htim->hdma[TIM_DMA_ID_CC3]->XferCpltCallback = TIM_DMACaptureCplt;

      /* Set the DMA error callback */
      htim->hdma[TIM_DMA_ID_CC3]->XferErrorCallback = TIM_DMAError ;

      /* Enable the DMA Stream */
      HAL_DMA_Start_IT(
          htim->hdma[TIM_DMA_ID_CC3],
          (uint32_t)&htim->Instance->CCR3,
          (uint32_t)pData, Length);

      /* Enable the TIM Capture/Compare 3  DMA request */
      __HAL_TIM_ENABLE_DMA(htim, TIM_DMA_CC3);
    }
      break;

    case TIM_CHANNEL_4:
    {
      /* Set the DMA Period elapsed callback */
      htim->hdma[TIM_DMA_ID_CC4]->XferCpltCallback = TIM_DMACaptureCplt;

      /* Set the DMA error callback */
      htim->hdma[TIM_DMA_ID_CC4]->XferErrorCallback = TIM_DMAError ;

      /* Enable the DMA Stream */
      HAL_DMA_Start_IT(
          htim->hdma[TIM_DMA_ID_CC4],
          (uint32_t)&htim->Instance->CCR4,
          (uint32_t)pData, Length);

      /* Enable the TIM Capture/Compare 4  DMA request */
      __HAL_TIM_ENABLE_DMA(htim, TIM_DMA_CC4);
    }
      break;

    default:
      break;
  }

  /* Enable the Input Capture channel */
  TIM_CCxChannelCmd(htim->Instance, Channel, TIM_CCx_ENABLE);

  /* DON'T Enable the Peripheral */
  //__HAL_TIM_ENABLE(htim);

  /* Return function status */
  return HAL_OK;
}


// read and process command coming in over USB
extern "C" {
void C3D_ProcessCommands(uint8_t * buffer, uint16_t length) {
  // zero-length commands do nothing
  if (length == 0) {
    return;
  }
  // look for commands
  for (uint16_t i = 0; i < c3d_command_count; ++i) {
    if (strncmp((const char *) buffer,
                c3d_command[i].text,
                strlen(c3d_command[i].text)) == 0) {
      LOG("\nReceived \"", c3d_command[i].text, "\" command");
      c3d_command[i].function();
      return;
    }
  }
  LOG("\nERROR: Ignoring unknown command of length", length, ".");
  GSL_OUT_LogHex(buffer, (length > 10) ? 10 : length);
}
}

// send out USB packet if we can
void C3D_SendUSBPacket(void) {
  // if USB is still active, just return
  auto hcdc = (USBD_CDC_HandleTypeDef*) hUsbDeviceHS.pClassData;
  if (hcdc->TxState != 0) {
    return;
  }
  // if a chunk is frozen, thaw it
  if (c3d_usb_buffer.frozen) {
    //LOG_ONCE("\nThawing first chunk");
    c3d_usb_buffer.Thaw();
  }
  // if first chunk is no longer active, send it out
  if (c3d_usb_buffer.first_chunk != c3d_usb_buffer.active_chunk) {
    //LOG_ONCE("\nSending first chunk");
    c3d_usb_buffer.Freeze();
    if (c3d_streaming_flag) {
      auto result = CDC_Transmit_HS(
          c3d_usb_buffer.chunk[c3d_usb_buffer.first_chunk],
          c3d_usb_buffer.chunk_length[c3d_usb_buffer.first_chunk]);
      ASSERT_EQ(result, USBD_OK);
    }
  }
}

// update endstops based on current position
void C3D_UpdateEndstops(void) {
  for (uint16_t i = 0; i < c3d_motor_count; ++i) {
    // motor alias
    auto & motor = c3d_motor[i];
    // skip stops which have no range
    if (motor.range_mm == -1.0f) {
      continue;
    }
    // get position
    float mm = motor.GetPositionMM();
    // update low stop
    if (mm <= 0.0f) {
      motor.low_stop.On();
    } else {
      motor.low_stop.Off();
    }
    // update high stop
    if (mm >= motor.range_mm) {
      motor.high_stop.On();
    } else {
      motor.high_stop.Off();
    }
  }
}

// update motor positions
// this must be called with a frequency
/*void C3D_UpdateMotorPositions(void) {
  // Note: Because these are populated via DMA, and because we have various
  // interrupts, it is possible for a new STEP edge to be added between
  // reads.  The algorithm ensures that each STEP edge is processed first.
  for (uint16_t i = 0; i < c3d_motor_count; ++i) {
    // motor alias
    auto & motor = c3d_motor[i];
    // monitor for motor STEP signal
    auto & step_monitor = c3d_signal_step_dma_monitor[2 * i];
    // monitor for motor DIR signal
    auto & dir_monitor = c3d_signal_step_dma_monitor[2 * i + 1];
    // get number of steps available on each signal
    uint16_t pre_dir_edge_count = dir_monitor.GetAvailable();
    uint16_t step_edge_count = step_monitor.GetAvailable();
    uint16_t dir_edge_count = dir_monitor.GetAvailable();
    ASSERT_GE(dir_edge_count, pre_dir_edge_count);
    // if no steps, just process DIR edges
    if (step_edge_count == 0 && pre_dir_edge_count == dir_edge_count) {
      dir_monitor.IgnoreMany(dir_edge_count);
      if (dir_edge_count % 2 == 1) {
        motor.dir_is_high = !motor.dir_is_high;
      }
      continue;
    }
    // if only steps, just process STEP edges
    if (dir_edge_count == 0) {
      // count number of steps to take
      uint16_t step_count = step_edge_count / 2;
      if (!motor.step_is_high && (step_edge_count % 2) == 1) {
        ++step_count;
      }
      // advance the motor position
      if (motor.dir_is_high) {
        motor.step_position += step_count;
      } else {
        motor.step_position -= step_count;
      }
      // flip signal if necessary
      if (step_edge_count % 2 == 1) {
        motor.step_is_high = !motor.step_is_high;
      }
      continue;
    }
    // else we have edges on both DIR and STEP, so we need to step through
    // them and process one at a time.  We do this by assuming each edge is
    // within half of the counter overflow of each other.
    // process all STEP edges
    while (step_edge_count && dir_edge_count) {
      // read the ticks at next DIR edge
      int32_t dir_edge = dir_monitor.Peek();
      while (step_edge_count) {
        bool step_before_edge;
        // read ticks at next STEP edge
        int32_t step_edge = step_monitor.Peek();
        if (dir_edge < motor.half_counter_overflow) {
          step_before_edge =
              (step_edge <= dir_edge)
              || (step_edge > dir_edge + motor.half_counter_overflow);
        } else {
          step_before_edge =
              (dir_edge - motor.half_counter_overflow < step_edge)
              && (step_edge <= dir_edge);
        }
        // if edge comes first, advance the DIR signal
        if (!step_before_edge) {
          break;
        }
        // else process the step edge
        step_monitor.Pop();
        --step_edge_count;
        // only advance on a rising edge of STEP
        if (!motor.step_is_high) {
          if (motor.dir_is_high) {
            ++motor.step_position;
          } else {
            --motor.step_position;
          }
        }
        motor.step_is_high = !motor.step_is_high;
      }
      // process DIR edge
      dir_monitor.Pop();
      --dir_edge_count;
      motor.dir_is_high = !motor.dir_is_high;
    }
    // process remaining STEP edges
    if (dir_edge_count == 0) {
    }
    // all STEP edges should be processed by now
    ASSERT_EQ(step_edge_count, 0);
    // process remaining DIR edges
    // we can only process DIR edges if no new STEP edges got added
    if (pre_dir_edge_count == dir_edge_count) {
      if (dir_edge_count % 2 == 1) {
        motor.dir_is_high = !motor.dir_is_high;
      }
      dir_monitor.IgnoreMany(dir_edge_count);
    }
  }
}*/

// main entry point for the C3D program
void C3D_Main(void) {

  GSL_DEL_Initialize();

  GSL_PRO_Reset();

  /*LOG_POSITION;
  GSL_DEL_MS(5000);
  LOG_POSITION;

  GSL_PIN_Unreserve(kPinA13);
  GSL_PIN_Unreserve(kPinA14);
  GSL_PIN_Initialize(kPinA13);
  GSL_PIN_Initialize(kPinA14);*/

  //C3D_InitTimers();

  LOG("\n\n\n");
  for (uint16_t line = 0; line < 3; ++line) {
    LOG("\n");
    for (uint16_t i = 0; i < 79; ++i) {
      LOG("*");
    }
  }
  LOG("\n\n\n");
  GSL_PRO_Add("Startup");

  GSL_GEN_GetBusyTime(-0.010f);
  GSL_GEN_GetBusyTime(-0.010f);
  GSL_GEN_GetBusyTime(-0.010f);
  GSL_PRO_Add("Startup - Busy time");

  GSL_GEN_LogBanner();
  GSL_PRO_Add("Startup - Banner generation");

  MX_USB_DEVICE_Init();

  c3d_usb_buffer.Initialize();

  LOG("\nGSL_DELOG speed is ", GSL_UART_GetClock(GSL_DELOG_PORT), " baud");

  LOG_POSITION;

//  // enable master timer clock
//  GSL_TIM_EnableClock(c3d_master_timer);
//  // set up master timer
//  GSL_TIM_SetFrequency(c3d_master_timer, 168e6f / 65536 * 2.0f, 1e-5f);
//  // set update callback with highest possible priority
//  GSL_TIM_SetUpdateCallback(c3d_master_timer, TIM4_IRQHandler, 0);

//  HAL_RUN(HAL_TIM_Base_Init(GSL_TIM_GetInfo(c3d_master_timer)->handle));

  // initialize buffers for signals
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    c3d_signal[i].buffer = (uint16_t *)
        GSL_BUF_Create(c3d_signal[i].buffer_capacity * sizeof(uint16_t));
  }

  // initialize signal DMA monitors
  // Note: timers must already be initialized
  C3D_InitializeSignalTimers();
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    auto & signal = c3d_signal[i];
    auto & monitor = c3d_signal_dma_monitor[i];
    monitor.middle_count = (signal.TIMx->ARR + 1) / 2;
    monitor.capacity = signal.buffer_capacity;
    monitor.buffer = signal.buffer;
    monitor.dma_stream = signal.dma_stream;
    monitor.tail_index = 0;
    monitor.middle_index = 0;
  }

  // initialize signal usb DMA monitors
//  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
//    auto & signal = c3d_signal[i];
//    auto & monitor = c3d_signal_usb_dma_monitor[i];
//    monitor.buffer_capacity = signal.buffer_capacity;
//    monitor.buffer = signal.buffer;
//    monitor.dma_stream = signal.dma_stream;
//    monitor.last_NDTR = monitor.buffer_capacity;
//  }

  // initialize signal step DMA monitors
//  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
//    auto & master = c3d_signal_usb_dma_monitor[i];
//    auto & slave = c3d_signal_step_dma_monitor[i];
//    slave.buffer_capacity = master.buffer_capacity;
//    slave.buffer = master.buffer;
//    slave.dma_stream = master.dma_stream;
//    slave.last_NDTR = master.buffer_capacity;
//  }

  // initialize ADC DMA monitor
  {
    auto & monitor = c3d_adc_dma_monitor;
    monitor.buffer_capacity = c3d_adc_buffer_capacity;
    monitor.buffer = c3d_adc_buffer;
    monitor.last_NDTR = monitor.buffer_capacity;
    monitor.dma_stream =
        GSL_ADC_GetInfo(c3d_adc)->handle->DMA_Handle->Instance;
  }

  // initialize timer pins
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    TIM_TypeDef * TIMx = c3d_signal[i].TIMx;
    uint32_t channel = c3d_signal[i].timer_channel;
    PinEnum pin = GSL_TIM_GetChannelPin(TIMx, channel);
    GSL_PIN_SetLow(pin);
    GSL_PIN_InitializeAF(
        pin,
        GPIO_MODE_AF_PP,
        GPIO_PULLDOWN,
        GSL_TIM_GetAF(TIMx));
  }

  // enable the timer clocks
//  GSL_TIM_EnableClock(c3d_adc_timer);
//  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
//    GSL_TIM_EnableClock(c3d_signal[i].TIMx);
//  }

  // call base init
//  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
//    // only initialize once
//    uint16_t j = 0;
//    for (; j < i; ++j) {
//      if (c3d_signal[j].TIMx == c3d_signal[i].TIMx) {
//        break;
//      }
//    }
//    if (j != i) {
//      continue;
//    }
//    auto & TIMx = c3d_signal[i].TIMx;
//    // initialize this timer
//    GSL_TIM_InfoStruct * const info = GSL_TIM_GetInfo(c3d_signal[i].TIMx);
//    // set timer initialization
//    auto & htim = info->handle;
//    htim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
//    htim->Init.CounterMode = TIM_COUNTERMODE_UP;
//    // for high speed timers, set this to max
//    // for half speed, set this to half of max
//    if (TIMx == TIM3) {
//      htim->Init.Period = 0x7FFF;
//    } else {
//      htim->Init.Period = 0xFFFF;
//    }
//    htim->Init.Prescaler = 0;
//    htim->Init.RepetitionCounter = 0;
//    HAL_RUN(HAL_TIM_Base_Init(info->handle));
//    // config clock source
//    {
//      TIM_ClockConfigTypeDef sClockSourceConfig = {0};
//      sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
//      sClockSourceConfig.ClockPolarity = 0;
//      sClockSourceConfig.ClockPrescaler = 0;
//      sClockSourceConfig.ClockFilter = 0;
//      HAL_RUN(HAL_TIM_ConfigClockSource(info->handle, &sClockSourceConfig));
//    }
//    HAL_RUN(HAL_TIM_IC_Init(info->handle));
//    // set timer to trigger off of master timer's TRGO signal
//    {
//      TIM_SlaveConfigTypeDef slave_config;
//      slave_config.SlaveMode = TIM_SLAVEMODE_TRIGGER;
//      slave_config.InputTrigger =
//          GSL_TIM_GetInternalTrigger(TIMx, c3d_master_timer);
//      slave_config.TriggerPolarity = TIM_TRIGGERPOLARITY_NONINVERTED;
//      slave_config.TriggerPrescaler = TIM_TRIGGERPRESCALER_DIV1;
//      slave_config.TriggerFilter = 0;
//      HAL_RUN(HAL_TIM_SlaveConfigSynchronization(info->handle, &slave_config));
//    }
//  }
//
//  // set up ADC timer
//  {
//    auto TIMx = c3d_adc_timer;
//    auto info = GSL_TIM_GetInfo(c3d_adc_timer);
//    // set freqency
//    GSL_TIM_SetFrequency(c3d_adc_timer, c3d_adc_frequency, 0.1f);
//    HAL_RUN(HAL_TIM_Base_Init(info->handle));
//    // set to trigger from master TRGO signal
//    {
//      TIM_SlaveConfigTypeDef slave_config;
//      slave_config.SlaveMode = TIM_SLAVEMODE_TRIGGER;
//      slave_config.InputTrigger =
//          GSL_TIM_GetInternalTrigger(TIMx, c3d_master_timer);
//      slave_config.TriggerPolarity = TIM_TRIGGERPOLARITY_NONINVERTED;
//      slave_config.TriggerPrescaler = TIM_TRIGGERPRESCALER_DIV1;
//      slave_config.TriggerFilter = 0;
//      HAL_RUN(HAL_TIM_SlaveConfigSynchronization(info->handle, &slave_config));
//    }
//  }

//  // set up middle count
//  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
//    auto & signal = c3d_signal[i];
//    auto & monitor = c3d_signal_dma_monitor[i];
//    monitor.middle_count = (signal.TIMx->ARR + 1) / 2;
//  }

//  // set up channels
//  {
//    TIM_IC_InitTypeDef sConfigIC = {0};
//    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_BOTHEDGE;
//    sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
//    sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
//    // This seems to work fine with no filtering, but let's do a 2-sample
//    // filter to avoid potential issues.  It should not affect the data in any
//    // way other than throwing away pulses of 1 tick.
//    sConfigIC.ICFilter = 0b0001;
//    for (uint16_t i = 0; i < c3d_signal_count; ++i) {
//      GSL_TIM_InfoStruct * const info = GSL_TIM_GetInfo(c3d_signal[i].TIMx);
//      uint32_t channel = c3d_signal[i].timer_channel;
//      HAL_RUN(HAL_TIM_IC_ConfigChannel(info->handle, &sConfigIC, channel));
//    }
//  }

  // enable the DMA clocks
//  GSL_DMA_EnableClock(DMA1);
//  GSL_DMA_EnableClock(DMA2);

  // initialize the DMA streams
//  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
//    auto & signal = c3d_signal[i];
//    auto & hdma = *GSL_DMA_GetHandle(signal.dma_stream);
//    hdma.Instance = signal.dma_stream;
//    hdma.Init.Channel = signal.dma_channel;
//    hdma.Init.Direction = DMA_PERIPH_TO_MEMORY;
//    hdma.Init.PeriphInc = DMA_PINC_DISABLE;
//    hdma.Init.MemInc = DMA_MINC_ENABLE;
//    hdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
//    hdma.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
//    hdma.Init.Mode = DMA_CIRCULAR;
//    hdma.Init.Priority = DMA_PRIORITY_LOW;
//    hdma.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
//    GSL_DMA_ReserveStream(hdma.Instance);
//    HAL_RUN(HAL_DMA_Init(&hdma));
//  }

//  // link and start the DMA streams
//  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
//    auto & signal = c3d_signal[i];
//    DMA_HandleTypeDef * hdma_base = GSL_DMA_GetHandle(signal.dma_stream);
//    uint16_t id = GSL_TIM_GetDMACCID(signal.timer_channel);
//    TIM_HandleTypeDef * htim_base = GSL_TIM_GetInfo(signal.TIMx)->handle;
//    __HAL_LINKDMA(htim_base, hdma[id], *hdma_base);
//    HAL_RUN(QUIETLY_HAL_TIM_IC_Start_DMA(htim_base,
//                                         signal.timer_channel,
//                                         (uint32_t*) signal.buffer,
//                                         signal.buffer_capacity));
//    // set it to ready so we can use multiple channels on this timer
//    // (due to a HAL library bug)
//    htim_base->State = HAL_TIM_STATE_READY;
//    // stop the timer
//    htim_base->Instance->CR1 &= ~(TIM_CR1_CEN);
//  }

//  // set up the ADC to trigger on TIM2 TRGO signal
//  ASSERT(c3d_adc_timer == TIM2);
//  GSL_ADC_SetTrigger(c3d_adc, ADC_EXTERNALTRIGCONV_T2_TRGO);
//  GSL_TIM_EnableTriggerUpdate(c3d_adc_timer);
//  // add ADC channels
//  GSL_ADC_RemoveAllChannels(c3d_adc);
//  for (uint16_t i = 0; i < c3d_adc_channel_count; ++i) {
//    GSL_ADC_AddChannel(c3d_adc, c3d_adc_channel[i]);
//  }
  // start the DMA output
  // TODO: fix so it takes # values instead of bytes
//  GSL_ADC_Start_DMA_Circular(
//      c3d_adc,
//      c3d_adc_buffer,
//      c3d_adc_buffer_capacity * sizeof(*c3d_adc_buffer));
//  // disable interrupt generation
//  CLEAR_BIT(GSL_ADC_GetInfo(c3d_adc)->hdma->Instance->CR, DMA_SxCR_TCIE);
//  CLEAR_BIT(GSL_ADC_GetInfo(c3d_adc)->hdma->Instance->CR, DMA_SxCR_HTIE);

  //GSL_TIM_LogDetailedInformation();
  //GSL_TIM_LogDescription();

//  LOG("\n\nGSL_DMA_Reserved=", GSL_OUT_Binary(GSL_DMA_Reserved, 16));

  // main loop
  while (true) {

    // enable or disable streaming if flag is switched
    if (c3d_streaming_flag != c3d_target_streaming_flag) {
      c3d_streaming_flag = c3d_target_streaming_flag;
      if (c3d_streaming_flag) {
        C3D_StartStreaming();
      } else {
        C3D_StopStreaming();
      }
    }

    // queue new USB packet
    GSL_PIN_SetHigh(c3d_debug_pin[1]);
    C3D_SendUSBPacket();
    GSL_PIN_SetLow(c3d_debug_pin[1]);

    if (c3d_debug_flag) {
      C3D_OutputTimerCounts();
      c3d_debug_flag = false;
    }

    // process steps to determine motor positions
    GSL_PIN_SetHigh(c3d_debug_pin[2]);
    //C3D_UpdateMotorPositions();

    // process endstops
    C3D_UpdateEndstops();
    GSL_PIN_SetLow(c3d_debug_pin[2]);

  }

}
