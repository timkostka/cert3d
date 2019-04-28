#pragma once

// This file contains routines for using the DAC peripheral.

// TODO: clean this up a bit.  implement deinit and routines.

#include "gsl_includes.h"

// get the trigger for a given timer
uint32_t GSL_DAC_GetTrigger(TIM_TypeDef * TIMx) {
  if (TIMx == TIM2) {
    return DAC_TRIGGER_T2_TRGO;
  } else if (TIMx == TIM4) {
    return DAC_TRIGGER_T4_TRGO;
  } else if (TIMx == TIM5) {
    return DAC_TRIGGER_T5_TRGO;
  } else if (TIMx == TIM6) {
    return DAC_TRIGGER_T6_TRGO;
  } else if (TIMx == TIM7) {
    return DAC_TRIGGER_T7_TRGO;
  } else if (TIMx == TIM8) {
    return DAC_TRIGGER_T8_TRGO;
  } else {
    HALT("Unexpected value");
  }
}

// generate a signal on the given thing
void GSL_DAC_GenerateSignal(
    uint32_t dac_channel,
    GSLSignalEnum signal_type,
    float target_frequency) {

  // link the dma
  //__HAL_LINKDMA(&gsl_dac_hdac, DMA_Handle2, gsl_dac_hdma2);
  //__HAL_LINKDMA(&gsl_dac_hdac, DMA_Handle1, gsl_dac_hdma1);

  // convert channel to integer
  uint8_t dac_channel_integer = (dac_channel == DAC_CHANNEL_1) ? 0 : 1;
  // get this channel info
  GSL_DAC_ChannelStruct * channel_info =
      &GSL_DAC_Info.channel[dac_channel_integer];
  // store the timer
  TIM_TypeDef * TIMx = channel_info->TIMx;

  // maximum possible update frequency
  // (this is an estimate)
  uint32_t max_update_frequency = HAL_RCC_GetSysClockFreq() / 64;

  // find signal length at this update rate
  float max_signal_length = max_update_frequency / target_frequency;

  uint16_t signal_length = 0;

  // if we can use this length, then do it
  if (max_signal_length <= gsl_dac_buffer_capacity) {
    signal_length = max_signal_length + 0.5f;
  } else {
    // else use the maximum signal length and adjust the frequency
    signal_length = gsl_dac_buffer_capacity;
  }
  // for square waves, use a signal length of 2
  if (signal_type == kSignalSquare) {
    signal_length = 2;
  }
  // set the timer frequency
  GSL_TIM_SetFrequency(TIMx, target_frequency * signal_length, true);
  // initialize the pin
  GSL_PIN_Initialize(channel_info->pin, GPIO_MODE_ANALOG, GPIO_NOPULL);
  // get the number of samples
  // initialize the buffer
  if (channel_info->buffer == nullptr) {
    channel_info->buffer = (uint16_t *) GSL_BUF_Create(
        sizeof(channel_info->buffer[0]) * gsl_dac_buffer_capacity);
  }
  // establish signal bounds
  float voltage_min = 0.15f;
  float voltage_max = 3.15f;
  // hold the buffer
  uint16_t * buffer = channel_info->buffer;
  // create the waveform
  switch (signal_type) {
  case kSignalSine: {
    float offset = (voltage_min + voltage_max) / 2.0f;
    float amplitude = (voltage_max - voltage_min) / 2.0f;
    // create a sine wave pattern
    for (uint16_t i = 0; i < signal_length; ++i) {
      float theta = 2.0f * 3.14159265358979f * i / signal_length;
      float value = offset + amplitude * sinf(theta);
      buffer[i] = value * 4095.0f / 3.3f + 0.5f;
    }
    break;
  }
  case kSignalRampUp: {
    float amplitude = (voltage_max - voltage_min) / 2.0f;
    // create a sawtooth wave pattern
    for (uint16_t i = 0; i < signal_length; ++i) {
      float value = voltage_min +
          2.0f * amplitude * i / (signal_length - 1);
      buffer[i] = value * 4095.0f / 3.3f + 0.5f;
    }
    break;
  }
  case kSignalRampDown: {
    float amplitude = (voltage_max - voltage_min) / 2.0f;
    // create a sawtooth wave pattern
    for (uint16_t i = 0; i < signal_length; ++i) {
      float value = voltage_max -
          2.0f * amplitude * i / (signal_length - 1);
      buffer[i] = value * 4095.0f / 3.3f + 0.5f;
    }
    break;
  }
  case kSignalSquare:
  {
    // create a square wave pattern
    for (uint16_t i = 0; i < signal_length; ++i) {
      if (i < signal_length / 2) {
        buffer[i] = voltage_min * 4095.0f / 3.3f + 0.5f;
      } else {
        buffer[i] = voltage_max * 4095.0f / 3.3f + 0.5f;
      }
    }
    break;
  }
  case kSignalTriangle:
  {
    float amplitude = (voltage_max - voltage_min) / 2.0f;
    // create a square wave pattern
    for (uint16_t i = 0; i < signal_length; ++i) {
      if (i < signal_length / 2) {
        float value = voltage_min + 2.0f * amplitude * i / (signal_length / 2);
        buffer[i] = value * 4095.0f / 3.3f + 0.5f;
      } else {
        float value = voltage_max - 2.0f * amplitude * (i - signal_length / 2) / (signal_length / 2);
        buffer[i] = value * 4095.0f / 3.3f + 0.5f;
      }
    }
    break;
  }
  default:
    HALT("Unexpected signal type");
  }

  __HAL_RCC_DAC_CLK_ENABLE();

  HAL_RUN(HAL_DAC_Init(&gsl_dac_hdac));

  GSL_DMA_Initialize(channel_info->hdma);

  // TODO: disable interrupts for this DMA channel
  //GSL_DMA_DisableInterrupts(channel_info->hdma);

  {
    DAC_ChannelConfTypeDef sConfig;
    sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
    //sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_DISABLE;
    sConfig.DAC_Trigger = GSL_DAC_GetTrigger(channel_info->TIMx);
    HAL_RUN(HAL_DAC_ConfigChannel(&gsl_dac_hdac, &sConfig, dac_channel));
  }

  // start it
  HAL_RUN(
      HAL_DAC_Start_DMA(
          &gsl_dac_hdac,
          dac_channel,
          (uint32_t *) buffer,
          signal_length,
          DAC_ALIGN_12B_R));
  // disable interrupts
  // start the timer
  GSL_TIM_EnableTriggerUpdate(TIMx);
  GSL_TIM_Start(TIMx);
}

// stop the signal on the given dac channel
void GSL_DAC_Stop(uint32_t dac_channel) {
}

// error handler
extern "C" {

void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef *hdac) {
  //HALT("");
}

void HAL_DAC_HalfConvCpltCallbackCh1(DAC_HandleTypeDef *hdac) {
  //HALT("");
}

void HAL_DAC_ErrorCallbackCh1(DAC_HandleTypeDef *hdac) {
  //HALT("");
}

void HAL_DAC_DMAUnderrunCallbackCh1(DAC_HandleTypeDef *hdac) {
  HALT("");
}

void HAL_DAC_ConvCpltCallbackCh2(DAC_HandleTypeDef *hdac) {
  //HALT("");
}

void HAL_DAC_HalfConvCpltCallbackCh2(DAC_HandleTypeDef *hdac) {
  //HALT("");
}

void HAL_DAC_ErrorCallbackCh2(DAC_HandleTypeDef *hdac) {
  //HALT("");
}

void HAL_DAC_DMAUnderrunCallbackCh2(DAC_HandleTypeDef *hdac) {
  HALT("");
}

}
