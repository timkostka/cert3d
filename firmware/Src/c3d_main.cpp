//
// Created by tdkostk on 4/27/2019.
//

#include "c3d_includes.h"

// create and send a new data packet
void C3D_CreatePacket(void) {

  // number of bytes added by this packet
  uint16_t byte_count = 0;

  // for each signal, increment middle_index to end of edges to send
  for (uint16_t s = 0; s < c3d_signal_count; ++s) {
    // alias DMA buffer monitor for this channel
    auto & monitor = c3d_signal_dma_monitor[s];
    const auto & capacity = monitor.capacity;
    const auto & middle_index = monitor.middle_index;
    // move middle edge to end of edges to send
    uint16_t head_index = monitor.GetHeadIndex();
    while (head_index > middle_index &&
        monitor.buffer[(head_index - 1) % capacity] < monitor.middle_count) {
      head_index = (head_index - 1) % capacity;
    }
    monitor.middle_index = head_index;
  }

  // output packet number
  c3d_usb_buffer.StageVariable((uint8_t) c3d_process_count / 2);
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
        (monitor.middle_index - monitor.tail_index) % monitor.capacity;
    // ensure count is as expected
    ASSERT_GT(count, 0);
    ASSERT_LT(count, 256);
    // add number of edges for this signal packet
    c3d_usb_buffer.StageVariable((uint8_t) count);
    ++byte_count;
    // add edge counts
    while (monitor.tail_index != monitor.middle_index) {
      // store this edge
      c3d_usb_buffer.StageVariable(monitor.buffer[monitor.tail_index]);
      // increment index
      monitor.tail_index = (monitor.tail_index + 1) % monitor.capacity;
      byte_count += 2;
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

  // update bits per packet
  c3d_output_bits_per_process.Update(byte_count * 8);

}


// callback timer

extern "C" {

void TIM4_IRQHandler(void) {

  // clear update bit
  TIM4->SR = ~TIM_SR_UIF;

  GSL_PIN_SetHigh(c3d_debug_pin[0]);

  // increment process count
  ++c3d_process_count;

  // after starting streaming, the first call to this will set
  // c3d_process_count = 0
  // and timers should be low

  // on odd counters, store middle index
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

// called to process buffers and send out new data as needed
void C3D_ProcessBuffers(void) {

  GSL_PIN_SetHigh(c3d_debug_pin[0]);

  //LOG_ONCE("\nASDF");

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

  // if we're not outputting data, just return
  if (!c3d_output_to_usb) {
    GSL_PIN_SetLow(c3d_debug_pin[0]);
    return;
  }

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
  CLEAR_BIT(c3d_master_timer->CR1, TIM_CR1_CEN);
  CLEAR_BIT(c3d_adc_timer->CR1, TIM_CR1_CEN);
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    CLEAR_BIT(c3d_signal[i].TIMx->CR1, TIM_CR1_CEN);
  }

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
  // send info header packet
  C3D_SendInfoHeaderPacket();
  // set counters to 0
  c3d_master_timer->CNT = 0;
  c3d_adc_timer->CNT = 0;
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    c3d_signal[i].TIMx->CNT = 0;
  }
  // enable the master timer
  c3d_output_to_usb = true;
  // clear pending interrupt on master timer
  c3d_master_timer->SR = 0;
  // reset all timer interrupt flags
  c3d_adc_timer->SR = 0;
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    c3d_signal[i].TIMx->SR = 0;
  }
  // read all pending DMA values
  {
    auto & monitor = c3d_adc_dma_monitor;
    while (monitor.GetAvailable()) {
      monitor.Pop();
    }
  }
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    auto & monitor = c3d_signal_usb_dma_monitor[i];
    while (monitor.GetAvailable()) {
      monitor.Pop();
    }
  }
  // ensure all timers are disabled
  ASSERT(READ_BIT(c3d_master_timer->CR1, TIM_CR1_CEN) == 0);
  ASSERT(READ_BIT(c3d_adc_timer->CR1, TIM_CR1_CEN) == 0);
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    ASSERT(READ_BIT(c3d_signal[i].TIMx->CR1, TIM_CR1_CEN) == 0);
  }
  // ensure counters are at 0
  ASSERT(c3d_master_timer->CNT == 0);
  ASSERT(c3d_adc_timer->CNT == 0);
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    ASSERT(c3d_signal[i].TIMx->CNT == 0);
  }
  // reset process count
  c3d_process_count = 0xFFFFFFFF;
  // look at signals and if they are high, trigger the IC DMA request flag
  // (this is so signals have the correct polarity
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
  // start the master, which will start all chained timers
  //c3d_master_timer->ARR = 0x3FFF;
  c3d_master_timer->CNT = c3d_master_timer->ARR - 1;
  MODIFY_REG(c3d_master_timer->CR2, TIM_CR2_MMS, TIM_TRGO_UPDATE);

  //GSL_TIM_LogDetailedInformation();
  SET_BIT(c3d_master_timer->CR1, TIM_CR1_CEN);

  GSL_DEL_MS(1);

  // ensure all timers are enabled
  ASSERT(READ_BIT(c3d_master_timer->CR1, TIM_CR1_CEN));
  ASSERT(READ_BIT(c3d_adc_timer->CR1, TIM_CR1_CEN));
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    ASSERT(READ_BIT(c3d_signal[i].TIMx->CR1, TIM_CR1_CEN));
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

// send info packet
void C3D_SendInfoHeaderPacket(void) {
  // if we're already streaming, just return
  if (c3d_output_to_usb) {
    LOG("\nERROR: cannot send info packet while streaming");
    return;
  }
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
  c3d_usb_buffer.StageVariable((uint8_t) c3d_adc_channel_count);
  // uint32_t ticks_per_adc_reading
  uint32_t ticks_per_adc_reading =
      clock / GSL_TIM_GetFrequency(c3d_adc_timer) + 0.5f;
  c3d_usb_buffer.StageVariable(ticks_per_adc_reading);
  // for each signal channel:
  // uint32_t signal_clock
  // uint32_t update_ticks
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    uint32_t frequency = GSL_TIM_GetTickFrequency(c3d_signal[i].TIMx);
    c3d_usb_buffer.StageVariable(frequency);
    uint32_t overflow =
        frequency / GSL_TIM_GetFrequency(c3d_signal[i].TIMx) + 0.5f;
    //LIMITED_LOG(c3d_signal_count, "\noverflow ", i, " = ", overflow);
    ASSERT(overflow == 65536 || overflow == 32768);
    c3d_usb_buffer.StageVariable(overflow);
  }
  // for each ADC channel:
  // float zero_value
  // float high_value
  for (uint16_t i = 0; i < c3d_adc_channel_count; ++i) {
    float zero_value = 0.0f;
    float high_value = 3.3f * (c3d_adc_high_resistor + c3d_adc_low_resistor) /
                       c3d_adc_low_resistor;
    c3d_usb_buffer.StageVariable(zero_value);
    c3d_usb_buffer.StageVariable(high_value);
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
    if (!c3d_ignore_usb_output) {
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
void C3D_UpdateMotorPositions(void) {
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
}

// initialize signal timers and DMA stuff
/*
void C3D_InitTimers(void) {
  // enable clocks
  __HAL_RCC_TIM1_CLK_ENABLE();
  __HAL_RCC_TIM3_CLK_ENABLE();
  __HAL_RCC_TIM4_CLK_ENABLE();
  __HAL_RCC_TIM8_CLK_ENABLE();
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

}*/

// main entry point for the C3D program
void C3D_Main(void) {

  GSL_DEL_Initialize();

  //C3D_InitTimers();

  GSL_PRO_Reset();

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

  // initialize signal DMA monitors
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    auto & signal = c3d_signal[i];
    auto & monitor = c3d_signal_dma_monitor[i];
    monitor.capacity = signal.buffer_capacity;
    monitor.buffer = signal.buffer;
    monitor.dma_stream = signal.dma_stream;
    monitor.tail_index = 0;
    monitor.middle_index = 0;
  }

  // initialize signal usb DMA monitors
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    auto & signal = c3d_signal[i];
    auto & monitor = c3d_signal_usb_dma_monitor[i];
    monitor.buffer_capacity = signal.buffer_capacity;
    monitor.buffer = signal.buffer;
    monitor.dma_stream = signal.dma_stream;
    monitor.last_NDTR = monitor.buffer_capacity;
  }

  // initialize signal step DMA monitors
  for (uint16_t i = 0; i < c3d_signal_count; ++i) {
    auto & master = c3d_signal_usb_dma_monitor[i];
    auto & slave = c3d_signal_step_dma_monitor[i];
    slave.buffer_capacity = master.buffer_capacity;
    slave.buffer = master.buffer;
    slave.dma_stream = master.dma_stream;
    slave.last_NDTR = master.buffer_capacity;
  }

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
  GSL_TIM_EnableClock(c3d_adc_timer);
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
      slave_config.InputTrigger =
          GSL_TIM_GetInternalTrigger(TIMx, c3d_master_timer);
      slave_config.TriggerPolarity = TIM_TRIGGERPOLARITY_NONINVERTED;
      slave_config.TriggerPrescaler = TIM_TRIGGERPRESCALER_DIV1;
      slave_config.TriggerFilter = 0;
      HAL_RUN(HAL_TIM_SlaveConfigSynchronization(info->handle, &slave_config));
    }
  }

  // set up ADC timer
  {
    auto TIMx = c3d_adc_timer;
    auto info = GSL_TIM_GetInfo(c3d_adc_timer);
    // set freqency
    GSL_TIM_SetFrequency(c3d_adc_timer, c3d_adc_frequency, 0.1f);
    HAL_RUN(HAL_TIM_Base_Init(info->handle));
    // set to trigger from master TRGO signal
    {
      TIM_SlaveConfigTypeDef slave_config;
      slave_config.SlaveMode = TIM_SLAVEMODE_TRIGGER;
      slave_config.InputTrigger =
          GSL_TIM_GetInternalTrigger(TIMx, c3d_master_timer);
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
    // This seems to work fine with no filtering, but let's do a 2-sample
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

  // set up the ADC to trigger on TIM2 TRGO signal
  ASSERT(c3d_adc_timer == TIM2);
  GSL_ADC_SetTrigger(c3d_adc, ADC_EXTERNALTRIGCONV_T2_TRGO);
  GSL_TIM_EnableTriggerUpdate(c3d_adc_timer);
  // add ADC channels
  GSL_ADC_RemoveAllChannels(c3d_adc);
  for (uint16_t i = 0; i < c3d_adc_channel_count; ++i) {
    GSL_ADC_AddChannel(c3d_adc, c3d_adc_channel[i]);
  }
  // start the DMA output
  // TODO: fix so it takes # values instead of bytes
  GSL_ADC_Start_DMA_Circular(
      c3d_adc,
      c3d_adc_buffer,
      c3d_adc_buffer_capacity * sizeof(*c3d_adc_buffer));
  // disable interrupt generation
  CLEAR_BIT(GSL_ADC_GetInfo(c3d_adc)->hdma->Instance->CR, DMA_SxCR_TCIE);
  CLEAR_BIT(GSL_ADC_GetInfo(c3d_adc)->hdma->Instance->CR, DMA_SxCR_HTIE);

  GSL_TIM_LogDetailedInformation();

  GSL_TIM_LogDescription();

  LOG("\n\nGSL_DMA_Reserved=", GSL_OUT_Binary(GSL_DMA_Reserved, 16));

  // now just output stuff
  while (true) {

    // enable streaming if requested
    if (c3d_start_streaming_flag) {
      C3D_StartStreaming();
    }

    // queue new USB packet
    C3D_SendUSBPacket();

    if (c3d_debug_flag) {
      C3D_OutputTimerCounts();
      c3d_debug_flag = false;
    }

    // process steps to determine motor positions
    //C3D_UpdateMotorPositions();

    // process endstops
    //C3D_UpdateEndstops();

  }

}
