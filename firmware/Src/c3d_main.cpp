//
// Created by tdkostk on 4/27/2019.
//

#include "c3d_includes.h"

// output clock tick counts
void C3D_OutputTimerTicks() {
  return;

  uint32_t c1 = TIM1->CNT;
  uint32_t c2 = TIM2->CNT;
  uint32_t c3 = TIM3->CNT;
  uint32_t c4 = TIM4->CNT;
  uint32_t c8 = TIM8->CNT;

  LOG("\n TIM1->CNT=", c1);
  LOG(",  TIM2->CNT=", c2);
  LOG(",  TIM3->CNT=", c3);
  LOG(",  TIM4->CNT=", c4);
  LOG(",  TIM8->CNT=", c8);
  LOG("\n TIM1->CR1=", GSL_OUT_Hex((uint16_t) TIM1->CR1));
  LOG(",  TIM2->CR1=", GSL_OUT_Hex((uint16_t) TIM2->CR1));
  LOG(",  TIM3->CR1=", GSL_OUT_Hex((uint16_t) TIM3->CR1));
  LOG(",  TIM4->CR1=", GSL_OUT_Hex((uint16_t) TIM4->CR1));
  LOG(",  TIM8->CR1=", GSL_OUT_Hex((uint16_t) TIM8->CR1));
  LOG("\n TIM1->CR2=", GSL_OUT_Hex((uint16_t) TIM1->CR2));
  LOG(",  TIM2->CR2=", GSL_OUT_Hex((uint16_t) TIM2->CR2));
  LOG(",  TIM3->CR2=", GSL_OUT_Hex((uint16_t) TIM3->CR2));
  LOG(",  TIM4->CR2=", GSL_OUT_Hex((uint16_t) TIM4->CR2));
  LOG(",  TIM8->CR2=", GSL_OUT_Hex((uint16_t) TIM8->CR2));
  LOG("\nTIM1->SMCR=", GSL_OUT_Hex((uint16_t) TIM1->SMCR));
  LOG(", TIM2->SMCR=", GSL_OUT_Hex((uint16_t) TIM2->SMCR));
  LOG(", TIM3->SMCR=", GSL_OUT_Hex((uint16_t) TIM3->SMCR));
  LOG(", TIM4->SMCR=", GSL_OUT_Hex((uint16_t) TIM4->SMCR));
  LOG(", TIM8->SMCR=", GSL_OUT_Hex((uint16_t) TIM8->SMCR));
  LOG("\n  TIM1->SR=", GSL_OUT_Hex((uint16_t) TIM1->SR));
  LOG(",   TIM2->SR=", GSL_OUT_Hex((uint16_t) TIM2->SR));
  LOG(",   TIM3->SR=", GSL_OUT_Hex((uint16_t) TIM3->SR));
  LOG(",   TIM4->SR=", GSL_OUT_Hex((uint16_t) TIM4->SR));
  LOG(",   TIM8->SR=", GSL_OUT_Hex((uint16_t) TIM8->SR));
  LOG("\n");

}

// called to process buffers
// the first call to this after we start streaming should
// have
void C3D_ProcessBuffers(void) {

  GSL_PIN_SetHigh(kPinA15);

  uint32_t c1 = TIM1->CNT;
  uint32_t c4 = c3d_master_timer->CNT;
  uint32_t c8 = TIM8->CNT;

  // the first call to this will set c3d_process_count = 0
  // in this first call, we should expect tick values in the lower half of the timer
  ++c3d_process_count;

  if (c3d_process_count < 2) {
    LOG("\nCall ", c3d_process_count, " to C3D_ProcessBuffers()");
    LOG("\n- TIM1->CNT=", c1, ", TIM4->CNT=", c4, ", TIM8->CNT=", c8);
  }

  //LOG_ONCE("\nLogging first output with count=", c3d_process_count);

  /*LOG_ONCE("\nTIM1->CNT=", TIM1->CNT);
  LOG_ONCE(", TIM2->CNT=", TIM2->CNT);
  LOG_ONCE(", TIM3->CNT=", TIM3->CNT);
  LOG_ONCE(", TIM4->CNT=", TIM4->CNT);
  LOG_ONCE(", TIM8->CNT=", TIM8->CNT);*/

  uint16_t byte_count = 0;

  // if we're not outputting data, just return
  if (!c3d_output_to_usb) {
    return;
  }

  // add random data
  // DEBUG
  /*for (uint16_t i = 0; i < 52; ++i) {
    uint32_t sync = 0xDEADBEEF;
    c3d_usb_buffer.StageData(&sync, 4);
    ++byte_count;
  }*/

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
    auto & monitor = c3d_signal_dma_monitor[s];
    if (!monitor.IsEmpty()) {
      channel_mask |= (1 << s);
    }
  }

  // output channel mask
  c3d_usb_buffer.StageVariable(channel_mask);
  ++byte_count;

  // process signals and move them into the staging buffer
  for (uint16_t s = 0; s < c3d_signal_count; ++s) {
    // if this isn't in the mask, don't output anything
    if ((channel_mask & (1 << s)) == 0) {
      continue;
    }
    // alias DMA buffer monitor for this channel
    auto & monitor = c3d_signal_dma_monitor[s];
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

    continue;


    // DEBUG
    //if (count) {
    //  LOG("\nFound ", count, " edges on channel ", s);
    //}
    // put values into the staging buffer
    // if no values, just return
    /*
    uint8_t * buffer = nullptr;
    uint16_t byte_count = 0;
    monitor.PopMany(buffer, byte_count);
    c3d_usb_buffer.StageData(buffer, byte_count);
    byte_count += byte_count;
    if (byte_count) {
      monitor.PopMany(buffer, byte_count);
      c3d_usb_buffer.StageData(buffer, byte_count);
      byte_count += byte_count;
    }*/

    /*for (uint16_t i = 0; i < count; ++i) {
      uint16_t value = monitor.Pop();
      c3d_usb_buffer.StageVariable(value);
      byte_count += 2;
    }*/
  }

  // output 0 ADC signals for now
  uint8_t zero = 0;
  c3d_usb_buffer.StageData(&zero, 1);
  ++byte_count;

  // TODO: process ADC signals

  // number of bytes added to the output buffer
  //uint16_t bytes_added = 6 + 2 * c3d_signal_count;

  /*uint16_t header = 0x0C77;
  c3d_output_buffer.AddBytes((uint8_t *) &header, sizeof(header));
  c3d_output_buffer.AddBytes((uint8_t *) &c3d_process_count, 2);

  for (uint16_t s = 0; s < c3d_signal_count; ++s) {
    // auto & signal = c3d_signal[s];
    auto & buffer = c3d_signal_staging_buffer[s];
    // add number of entries
    c3d_output_buffer.AddBytes((uint8_t *) &c3d_signal_staging_buffer_length[s],
        2);
    c3d_edge_count += c3d_signal_staging_buffer_length[s];
    // add tick numbers
    c3d_output_buffer.AddBytes((uint8_t *) buffer,
        c3d_signal_staging_buffer_length[s] * sizeof(*buffer));
    //bytes_added += 2 * c3d_signal_staging_buffer_length[s];
    // erase buffer
    c3d_signal_staging_buffer_length[s] = 0;
  }*/

  //c3d_output_bits_per_process.Update(bytes_added * 8);

  // move signals from staging buffer to output buffer if there is enough data
  /*
  for (uint16_t s = 0; s < c3d_signal_count; ++s) {
    // auto & signal = c3d_signal[s];
    auto & buffer = c3d_signal_staging_buffer[s];
    if (c3d_signal_staging_buffer_length[s] < 64) {
      continue;
    }
    // add sync word
    uint16_t header[3] = {0x0C77, s, c3d_signal_staging_buffer_length[s]};
    c3d_output_buffer.AddBytes((uint8_t *) header, 3 * sizeof(*header));
    c3d_output_buffer.AddBytes((uint8_t *) buffer,
        c3d_signal_staging_buffer_length[s] * sizeof(*buffer));
    c3d_signal_staging_buffer_length[s] = 0;
  }*/

  c3d_output_bits_per_process.Update(byte_count * 8);

  GSL_PIN_SetLow(kPinA15);
}

// stop streaming data
void C3D_StopStreaming(void) {
  // if we're not streaming, just return
  if (!c3d_output_to_usb) {
    LOG("\nERROR: streaming not enabled");
    return;
  }
  c3d_output_to_usb = false;

  // stop timers
  CLEAR_BIT(TIM4->CR1, TIM_CR1_CEN);
  CLEAR_BIT(TIM1->CR1, TIM_CR1_CEN);
  CLEAR_BIT(TIM2->CR1, TIM_CR1_CEN);
  CLEAR_BIT(TIM3->CR1, TIM_CR1_CEN);
  CLEAR_BIT(TIM8->CR1, TIM_CR1_CEN);

  // clear out DMA info
  LOG("\nData stream stopped.");
  // advance chunk so everything gets sent out
  c3d_usb_buffer.AdvanceChunk();
}

// set the start streaming flag
void C3D_EnableStartStreamingFlag(void) {
  c3d_start_streaming_flag = true;
}

// start streaming data
void C3D_StartStreaming(void) {

  // reset flag
  c3d_start_streaming_flag = false;

  // ensure we're not in an interrupt
  ASSERT(!GSL_GEN_InInterrupt());

  // if we're already streaming, just return
  if (c3d_output_to_usb) {
    LOG("\nERROR: streaming already enabled");
    return;
  }
  LOG("\nStarting data stream.");
  // send info packet
  C3D_SendInfoPacket();

  // reset stats
  c3d_process_count = 0xFFFFFFFF;

  C3D_OutputTimerTicks();

  // set counters to 0
  c3d_master_timer->CNT = 0;
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    c3d_signal[i].TIMx->CNT = 0;
  }

  // enable the master timer
  c3d_output_to_usb = true;

  // clear pending interrupt on master timer
  c3d_master_timer->SR = 0;

  // reset all timer IC DMA requests
  // CLEAR_BIT(c3d_adc_timer->SR, TIM_SR_TIF | TIM_SR_UIF);
  c3d_adc_timer->SR = 0;
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    //CLEAR_BIT(c3d_signal[i].TIMx->SR, TIM_SR_CC1IF | TIM_SR_CC2IF | TIM_SR_CC3IF | TIM_SR_CC4IF);
    //CLEAR_BIT(c3d_signal[i].TIMx->SR, TIM_SR_CC1OF | TIM_SR_CC2OF | TIM_SR_CC3OF | TIM_SR_CC4OF);
    c3d_signal[i].TIMx->SR = 0;
  }

  // clear trigger flag on slave timers
  //CLEAR_BIT(c3d_adc_timer->SR, TIM_SR_TIF | TIM_SR_UIF);
  //for (uint16_t i = 0; i < c3d_signal_count; ++i) {
  //  CLEAR_BIT(c3d_signal[i].TIMx->SR, TIM_SR_TIF | TIM_SR_UIF);
  //}

  // CLEAR_BIT(c3d_master_timer->SR, TIM_SR_UIF);

  // read all pending DMA values
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    auto & monitor = c3d_signal_dma_monitor[i];
    while (monitor.GetAvailable()) {
      monitor.Pop();
    }
  }

  // ensure all timers are disabled
  ASSERT(READ_BIT(c3d_master_timer->CR1, TIM_CR1_CEN) == 0)
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    ASSERT(READ_BIT(c3d_signal[i].TIMx->CR1, TIM_CR1_CEN) == 0)
  }

  // ensure counters are at 0
  ASSERT(c3d_master_timer->CNT == 0);
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    ASSERT(c3d_signal[i].TIMx->CNT == 0);
  }

  // reset process count
  c3d_process_count = 0xFFFFFFFF;

  // look at signals and if they are high, trigger the IC DMA request flag
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    if (GSL_PIN_GetValue(GSL_TIM_GetChannelPin(c3d_signal[i].TIMx, c3d_signal[i].timer_channel))) {
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

  GSL_TIM_LogDetailedInformation();

  // start the master, which should start all chained timers
  GSL_TIM_Start(c3d_master_timer);

  LOG("\nStream started.");

  // ensure all timers are enabled
  ASSERT(READ_BIT(c3d_master_timer->CR1, TIM_CR1_CEN))
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    ASSERT(READ_BIT(c3d_signal[i].TIMx->CR1, TIM_CR1_CEN))
  }

  C3D_OutputTimerTicks();

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

// send info packet
void C3D_SendInfoPacket(void) {
  // if we're already streaming, just return
  if (c3d_output_to_usb) {
    LOG("\nERROR: cannot send info packet while streaming");
    return;
  }
  // construct info packet
  c3d_usb_buffer.StageData("InfoStart", 9);
  uint32_t clock = (uint32_t) HAL_RCC_GetSysClockFreq();
  c3d_usb_buffer.StageVariable(clock);
  c3d_usb_buffer.StageVariable((uint8_t) c3d_signal_count);
  c3d_usb_buffer.StageVariable((uint8_t) c3d_adc_channel_count);
  uint32_t ticks_per_adc_reading = clock * GSL_TIM_GetFrequency(c3d_adc_timer) / GSL_TIM_GetTickFrequency(c3d_adc_timer) + 0.5f;
  LOG_ONCE("\nticks_per_adc_reading = ", ticks_per_adc_reading);
  c3d_usb_buffer.StageVariable(ticks_per_adc_reading);
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    uint32_t frequency = GSL_TIM_GetTickFrequency(c3d_signal[i].TIMx);
    c3d_usb_buffer.StageVariable(frequency);
    uint32_t overflow = frequency / GSL_TIM_GetFrequency(c3d_signal[i].TIMx) + 0.5f;
    LIMITED_LOG(c3d_signal_count, "\noverflow ", i, " = ", overflow);
    ASSERT(overflow == 65536 || overflow == 32768);
    c3d_usb_buffer.StageVariable(overflow);
  }
  for (uint16_t i = 0; i < c3d_adc_channel_count; ++i) {
    float zero_value = 0.0f;
    float high_value = 3.3f * (c3d_adc_high_resistor + c3d_adc_low_resistor) /
                       c3d_adc_low_resistor;
    c3d_usb_buffer.StageVariable(zero_value);
    c3d_usb_buffer.StageVariable(high_value);
  }
  c3d_usb_buffer.StageData("InfoStop", 8);
  // advance chunk to get information to be sent
  c3d_usb_buffer.AdvanceChunk();
}

// a replacement for HAL_TIM_IC_Start_DMA without starting the timer
HAL_StatusTypeDef QUIETLY_HAL_TIM_IC_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t *pData, uint16_t Length)
{
  /* Check the parameters */
  assert_param(IS_TIM_CCX_INSTANCE(htim->Instance, Channel));
  assert_param(IS_TIM_DMA_CC_INSTANCE(htim->Instance));

  if((htim->State == HAL_TIM_STATE_BUSY))
  {
    return HAL_BUSY;
  }
  else if((htim->State == HAL_TIM_STATE_READY))
  {
    if((pData == 0U) && (Length > 0))
    {
      return HAL_ERROR;
    }
    else
    {
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
      HAL_DMA_Start_IT(htim->hdma[TIM_DMA_ID_CC1], (uint32_t)&htim->Instance->CCR1, (uint32_t)pData, Length);

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
      HAL_DMA_Start_IT(htim->hdma[TIM_DMA_ID_CC2], (uint32_t)&htim->Instance->CCR2, (uint32_t)pData, Length);

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
      HAL_DMA_Start_IT(htim->hdma[TIM_DMA_ID_CC3], (uint32_t)&htim->Instance->CCR3, (uint32_t)pData, Length);

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
      HAL_DMA_Start_IT(htim->hdma[TIM_DMA_ID_CC4], (uint32_t)&htim->Instance->CCR4, (uint32_t)pData, Length);

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
  LOG("\nERROR: Ignoring unknown command.");
  GSL_OUT_LogHex(buffer, (length > 10) ? 10 : length);
}
}

void C3D_DebugTestCallback(void) {
  static uint32_t calls = 0;
  ++calls;
  if (calls < 10) {
    C3D_OutputTimerTicks();
  }
}


void C3D_DebugTest(void) {
  gsl_tim1_htim.Init.Period = 65535;
  gsl_tim1_htim.Init.Prescaler = 3;
  //gsl_tim2_htim.Init.Period = 30000;
  //gsl_tim2_htim.Init.Prescaler = 0;
  GSL_TIM_SetFrequency(TIM2, 1e3f, 0.1f);
  gsl_tim3_htim.Init.Period = 32767;
  gsl_tim3_htim.Init.Prescaler = 3;
  gsl_tim4_htim.Init.Period = 32768 / 2 - 1;
  gsl_tim4_htim.Init.Prescaler = 3;
  gsl_tim8_htim.Init.Period = 65535;
  gsl_tim8_htim.Init.Prescaler = 3;


  LOG("\n\nAll timers should be off");
  C3D_DebugTestCallback();

  LOG("\n\nSetting update callback on master timer");
  GSL_TIM_SetUpdateCallback(c3d_master_timer, C3D_DebugTestCallback);
  GSL_DEL_MS(1);
  C3D_DebugTestCallback();


  // set update callback on master timer
  ASSERT(c3d_master_timer == TIM4);

  // set up the timer to send trigger on update
  LOG("\n\nSetting master to output TRGO signal");
  //GSL_TIM_EnableTriggerOutput(c3d_master_timer);
  // clear any pending interrupts
  //c3d_master_timer->SR = 0;
  /*
  {
    // send enable as trigger output
    TIM_MasterConfigTypeDef config;
    config.MasterOutputTrigger = TIM_TRGO_UPDATE;
    config.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE;
    HAL_TIMEx_MasterConfigSynchronization(GSL_TIM_GetInfo(c3d_master_timer)->handle, &config);
  }*/
  GSL_DEL_MS(1);
  C3D_DebugTestCallback();

  LOG("\n\nSetting up slaves to trigger from master");
  GSL_TIM_WaitForTrigger(TIM1, GSL_TIM_GetInternalTrigger(TIM1, c3d_master_timer));
  GSL_TIM_WaitForTrigger(TIM2, GSL_TIM_GetInternalTrigger(TIM2, c3d_master_timer));
  GSL_TIM_WaitForTrigger(TIM3, GSL_TIM_GetInternalTrigger(TIM3, c3d_master_timer));
  GSL_TIM_WaitForTrigger(TIM8, GSL_TIM_GetInternalTrigger(TIM8, c3d_master_timer));
  GSL_DEL_MS(1);
  C3D_DebugTestCallback();

  LOG("\n\nStarting master timer");
  GSL_TIM_Start(c3d_master_timer);

  GSL_DEL_MS(500);
  GSL_TIM_LogDescription();

  while (1);

}

// main entry point for the C3D program
void C3D_Main(void) {

  GSL_PIN_SetHigh(kPinB6);
  GSL_PIN_Initialize(kPinB6, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL);
  GSL_PIN_SetHigh(kPinB7);
  GSL_PIN_Initialize(kPinB7, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL);

  MX_USB_DEVICE_Init();

  c3d_usb_buffer.Initialize();

  LOG("\nGSL_DELOG speed is ", GSL_UART_GetClock(GSL_DELOG_PORT), " baud");

  LOG_POSITION;

  // enable master timer clock
  GSL_TIM_EnableClock(c3d_master_timer);
  // set up master timer
  GSL_TIM_SetFrequency(c3d_master_timer, 168e6f / 65536 * 2.0f, 1e-5f);
  // set update callback with highest possible priority
  GSL_TIM_SetUpdateCallback(c3d_master_timer, C3D_ProcessBuffers, 0);
  //HAL_RUN(HAL_TIM_Base_Init(GSL_TIM_GetInfo(c3d_master_timer)->handle));

  // initialize buffers for signals
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    c3d_signal[i].buffer = (uint16_t *)
        GSL_BUF_Create(c3d_signal[i].buffer_capacity * sizeof(uint16_t));
  }

  // initialize DMA monitors
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    auto & signal = c3d_signal[i];
    auto & monitor = c3d_signal_dma_monitor[i];
    monitor.buffer_capacity = signal.buffer_capacity;
    monitor.buffer = signal.buffer;
    monitor.last_NDTR = signal.buffer_capacity;
    monitor.dma_stream = signal.dma_stream;
  }

  // initialize timer pins
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    TIM_TypeDef * TIMx = c3d_signal[i].TIMx;
    uint32_t channel = c3d_signal[i].timer_channel;
    PinEnum pin = GSL_TIM_GetChannelPin(TIMx, channel);
    GSL_PIN_SetLow(pin);
    GSL_PIN_InitializeAF(pin, GPIO_MODE_AF_PP, GPIO_PULLDOWN, GSL_TIM_GetAF(TIMx));
  }

  // enable the timer clocks
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    GSL_TIM_EnableClock(c3d_signal[i].TIMx);
  }

  // call base init
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    // only initialize once
    uint16_t j = 0;
    for (; j < i; ++j) {
      if (c3d_signal[j].TIMx == c3d_signal[i].TIMx) {
        break;
      }
    }
    if (j != i) {
      continue;
    }
    auto & TIMx = c3d_signal[i].TIMx;
    // initialize this timer
    GSL_TIM_InfoStruct * const info = GSL_TIM_GetInfo(c3d_signal[i].TIMx);
    // set timer initialization
    auto & htim = info->handle;
    htim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim->Init.CounterMode = TIM_COUNTERMODE_UP;
    // for high speed timers, set this to max
    // for half speed, set this to half of max
    if (TIMx == TIM3) {
      htim->Init.Period = 0x7FFF;
    } else {
      htim->Init.Period = 0xFFFF;
    }
    htim->Init.Prescaler = 0;
    htim->Init.RepetitionCounter = 0;
    HAL_RUN(HAL_TIM_Base_Init(info->handle));
    // config clock source
    {
      TIM_ClockConfigTypeDef sClockSourceConfig = {0};
      sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
      sClockSourceConfig.ClockPolarity = 0;
      sClockSourceConfig.ClockPrescaler = 0;
      sClockSourceConfig.ClockFilter = 0;
      HAL_RUN(HAL_TIM_ConfigClockSource(info->handle, &sClockSourceConfig));
    }
    HAL_RUN(HAL_TIM_IC_Init(info->handle));
    // set timer to trigger off of master timer's TRGO signal
    {
      TIM_SlaveConfigTypeDef slave_config;
      slave_config.SlaveMode = TIM_SLAVEMODE_TRIGGER;
      slave_config.InputTrigger = GSL_TIM_GetInternalTrigger(TIMx, c3d_master_timer);
      slave_config.TriggerPolarity = TIM_TRIGGERPOLARITY_NONINVERTED;
      slave_config.TriggerPrescaler = TIM_TRIGGERPRESCALER_DIV1;
      slave_config.TriggerFilter = 0;
      HAL_RUN(HAL_TIM_SlaveConfigSynchronization(info->handle, &slave_config));
    }
  }

  // set up channels
  {
    TIM_IC_InitTypeDef sConfigIC = {0};
    sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_BOTHEDGE;
    sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
    sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
    // This seems to work fine with no filtering, but let's do a 2-signal
    // filter to avoid potential issues.  It should not affect the data in any
    // way other than throwing away pulses of 1 tick.
    sConfigIC.ICFilter = 0b0001;
    for (uint16_t i = 0; i < c3d_signal_count; ++i) {
      GSL_TIM_InfoStruct * const info = GSL_TIM_GetInfo(c3d_signal[i].TIMx);
      uint32_t channel = c3d_signal[i].timer_channel;
      HAL_RUN(HAL_TIM_IC_ConfigChannel(info->handle, &sConfigIC, channel));
    }
  }

  // enable the DMA clocks
  GSL_DMA_EnableClock(DMA1);
  GSL_DMA_EnableClock(DMA2);

  // initialize the DMA streams
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    auto & signal = c3d_signal[i];
    auto & hdma = *GSL_DMA_GetHandle(signal.dma_stream);
    hdma.Instance = signal.dma_stream;
    hdma.Init.Channel = signal.dma_channel;
    hdma.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma.Init.MemInc = DMA_MINC_ENABLE;
    hdma.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma.Init.Mode = DMA_CIRCULAR;
    hdma.Init.Priority = DMA_PRIORITY_LOW;
    hdma.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    GSL_DMA_ReserveStream(hdma.Instance);
    HAL_RUN(HAL_DMA_Init(&hdma));
  }

  // link and start the DMA streams
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    auto & signal = c3d_signal[i];
    DMA_HandleTypeDef * hdma_base = GSL_DMA_GetHandle(signal.dma_stream);
    uint16_t id = GSL_TIM_GetDMACCID(signal.timer_channel);
    TIM_HandleTypeDef * htim_base = GSL_TIM_GetInfo(signal.TIMx)->handle;
    __HAL_LINKDMA(htim_base, hdma[id], *hdma_base);
    HAL_RUN(QUIETLY_HAL_TIM_IC_Start_DMA(htim_base,
                                         signal.timer_channel,
                                         (uint32_t*) signal.buffer,
                                         signal.buffer_capacity));
    // set it to ready so we can use multiple channels on this timer
    // (due to a HAL library bug)
    htim_base->State = HAL_TIM_STATE_READY;
    // stop the timer
    htim_base->Instance->CR1 &= ~(TIM_CR1_CEN);
  }

  // set up ADC timer
  GSL_TIM_SetFrequency(c3d_adc_timer, c3d_adc_frequency, 0.1f);

  // set up the ADC to trigger on TIM2 TRGO signal
  /*
  ASSERT(c3d_master_timer == TIM2);
  GSL_ADC_SetTrigger(c3d_adc, ADC_EXTERNALTRIGCONV_T2_TRGO);
  // add ADC channels
  for (uint16_t i = 0; i < c3d_adc_channel_count; ++i) {
    GSL_ADC_AddChannel(c3d_adc, c3d_adc_channel[i]);
  }
  // start the DMA output
  GSL_ADC_Start_DMA_Circular(
      c3d_adc, c3d_adc_buffer, c3d_adc_buffer_capacity * sizeof(*c3d_adc_buffer));
  */

  // initialize pin to drive TIM1->CH1
  GSL_PIN_SetLow(kPinG15);
  GSL_PIN_Initialize(kPinG15, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL);
  GSL_PIN_SetLow(kPinB3);
  GSL_PIN_Initialize(kPinB3, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL);

  // DEBUG
  GSL_UART_Initialize(UART4);

  // DEBUG
  GSL_PIN_Initialize(kPinA15, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL);

  // GSL_TIM_LogDetailedInformation();

  // GSL_TIM_LogDescription();

  C3D_OutputTimerTicks();

  // disable most interrupts from TIM
  //hdma->Instance->CR  |= DMA_IT_TC | DMA_IT_TE | DMA_IT_DME;
  //hdma->Instance->FCR |= DMA_IT_FE;


  // now just output stuff
  while (1) {
    // enable streaming if requested
    if (c3d_start_streaming_flag) {
      C3D_StartStreaming();
    }

    //static bool send_data = true;
    // DEBUG
    if (GSL_DEL_ElapsedS(0) > 1.0f) {
      static bool done = false;
      if (!done) {
        if (!c3d_output_to_usb) {
          C3D_StartStreaming();
          GSL_DEL_MS(3);
          for (uint16_t i = 0; i < 64; ++i) {
            GSL_UART_SendString(UART4, "Hello world!");
            GSL_DEL_MS((i % 8) + 1);
          }
          LOG("\nUART4 speed is ", GSL_UART_GetClock(UART4));
          GSL_DEL_MS(3);
          C3D_StopStreaming();
        }
        done = true;
      }
    }

    // DEBUG
    if (c3d_debug_flag) {
      //send_data = false;
      LOG_ONCE("\nSending debug stream");
      GSL_UART_SendString(UART4, "Hello world!");
      c3d_debug_flag = false;
      for (uint16_t i = 0; i < c3d_signal_count; ++i) {
        if (i == 0) {
          LOG("\n");
        } else {
          LOG(", ");
        }
        LOG("CH", i, "=", c3d_signal[i].dma_stream->NDTR);
      }
    }

    // make sure USB is no longer active
    {
      USBD_CDC_HandleTypeDef *hcdc =
          (USBD_CDC_HandleTypeDef*) hUsbDeviceHS.pClassData;
      if (hcdc->TxState != 0) {
        continue;
      }
    }
    // if a chunk is frozen, thaw it
    if (c3d_usb_buffer.frozen) {
      LOG_ONCE("\nThawing first chunk");
      c3d_usb_buffer.Thaw();
    }
    // if first chunk is no longer active, send it out
    if (c3d_usb_buffer.first_chunk != c3d_usb_buffer.active_chunk) {
      LOG_ONCE("\nSending first chunk");
      c3d_usb_buffer.Freeze();
      if (!c3d_ignore_usb_output) {
        auto result = CDC_Transmit_HS(
            c3d_usb_buffer.chunk[c3d_usb_buffer.first_chunk],
            c3d_usb_buffer.chunk_length[c3d_usb_buffer.first_chunk]);
        ASSERT_EQ(result, USBD_OK);
      }
    }
  }

  while (1);

}
