#pragma once

// This provides a interface to configure and use the ADC peripheral.
//
// Examples:
//
// To read a single channel at the default settings, use:
//   GSL_ADC_Read(ADC1, ADC_CHANNEL_0);
//
// To read multiple channels, they must be set up first
// To clear all currently defined channels on a given ADC, use
//   GSL_ADC_EmptyChannels(ADC1);
//
// To add a channel
//   GSL_ADC_AddChannel(ADC1, ADC_CHANNEL_0, ADC_SAMPLETIME_480CYCLES);
//
// To set automatic, continuous conversion (which is the default):
//   GSL_ADC_SetTrigger(ADC1, ADC_SOFTWARE_START)
//
// To set up triggered conversion on TIM1_CC1:
//   GSL_ADC_SetTrigger(ADC1, ADC_EXTERNALTRIGCONV_T1_CC1);
//
// To set up DMA transfer
//   GSL_ADC_Start_DMA_Circular(ADC1, data, length);
//
// HAL_ADC_Start_IT
// This interface assumes that ADC_CHANNEL_X is just the integer X
//
// e.g. ADC_CHANNEL_11 -> 11
//
// If this isn't the case, I'll need to rewrite things.

// so, to set up ADC1 to sample CHANNEL_17 (1.2V ref) and CHANNEL_18
// (temperature) every X ms and put them into a circular buffer, we would do
// the following:
//   GSL_ADC_SetTrigger(ADC1, ADC_EXTERNALTRIGCONV_T3_CC1);
//   GSL_ADC_Initialize(ADC1, 2);
//   GSL_ADC_SetChannel(ADC1, 0, ADC_CHANNEL_17, ADC_SAMPLETIME_480CYCLES);
//   GSL_ADC_SetChannel(ADC1, 1, ADC_CHANNEL_18, ADC_SAMPLETIME_480CYCLES);
//   GSL_TIM_SetFrequency(TIM3, 100);
//   GSL_TIM_Start(TIM3);
//   GSL_ADC_Start_DMA_Circular_Silent(ADC1, data, length);

#include "gsl_includes.h"

// convert the sample constant into the number of samples in integer form
uint16_t GSL_ADC_ConvertSampleCycles(uint32_t sample_time) {
  switch (sample_time) {
    case ADC_SAMPLETIME_3CYCLES:
      return 3;
    case ADC_SAMPLETIME_15CYCLES:
      return 15;
    case ADC_SAMPLETIME_28CYCLES:
      return 28;
    case ADC_SAMPLETIME_56CYCLES:
      return 56;
    case ADC_SAMPLETIME_84CYCLES:
      return 84;
    case ADC_SAMPLETIME_112CYCLES:
      return 112;
    case ADC_SAMPLETIME_144CYCLES:
      return 144;
    case ADC_SAMPLETIME_480CYCLES:
      return 480;
    default:
      HALT("Unexpected value");
  }
}

// convert an ADC pointer to a 0-based integer
// e.g. ADC1 -> 0
uint32_t GSL_ADC_GetNumber(ADC_TypeDef * ADCx) {
  for (uint32_t i = 0; i < GSL_ADC_PeripheralCount; ++i) {
    if (GSL_ADC_Info[i].ADCx == ADCx) {
      return i;
    }
  }
  HALT("Invalid parameter");
  return -1;
}

// return number of channels on this ADC
uint16_t GSL_ADC_ChannelCount(ADC_TypeDef * ADCx) {
  if (ADCx == ADC1) {
    return 19;
  } else {
    return 16;
  }
}

// enable the clock
void GSL_ADC_EnableClock(ADC_TypeDef * ADCx) {
  if (false) {
#ifdef ADC1
  } else if (ADCx == ADC1) {
    __HAL_RCC_ADC1_CLK_ENABLE();
#endif
#ifdef ADC2
  } else if (ADCx == ADC2) {
    __HAL_RCC_ADC2_CLK_ENABLE();
#endif
#ifdef ADC3
  } else if (ADCx == ADC3) {
    __HAL_RCC_ADC3_CLK_ENABLE();
#endif
  } else {
    HALT("Invalid parameter");
  }
}

// return pointer to the info struct
GSL_ADC_InfoStruct * GSL_ADC_GetInfo(ADC_TypeDef * ADCx) {
  return &GSL_ADC_Info[GSL_ADC_GetNumber(ADCx)];
}

// return the IRQ for the given ADC peripheral
// e.g. ADC1 -> ADC_IRQn
IRQn_Type GSL_ADC_GetIRQ(ADC_TypeDef * ADCx) {
  if (0) {
#ifdef ADC1
  } else if (ADCx == ADC1) {
    return ADC_IRQn;
#endif
#ifdef ADC2
  } else if (ADCx == ADC2) {
    return ADC_IRQn;
#endif
#ifdef ADC3
  } else if (ADCx == ADC3) {
    return ADC_IRQn;
#endif
  } else {
    HALT("Invalid parameter");
    return NonMaskableInt_IRQn;
  }
}

// set up the callback routine for ADC completion
// (set to nullptr to clear)
void GSL_ADC_SetCompleteCallback(
    ADC_TypeDef * ADCx,
    void (*callback_function)(void)) {
  // get the handle
  GSL_ADC_InfoStruct * info = GSL_ADC_GetInfo(ADCx);
  // set the routine
  info->ConvCpltCallback = callback_function;
}

// set up the callback routine for ADC half completion
// (set to nullptr to clear)
void GSL_ADC_SetHalfCompleteCallback(
    ADC_TypeDef * ADCx,
    void (*callback_function)(void)) {
  // get the handle
  GSL_ADC_InfoStruct * info = GSL_ADC_GetInfo(ADCx);
  // set the routine
  info->ConvHalfCpltCallback = callback_function;
}

// deinitialize the ADC peripheral
void GSL_ADC_Deinitialize(ADC_TypeDef * ADCx) {
  // get the handle
  GSL_ADC_InfoStruct * info = GSL_ADC_GetInfo(ADCx);
  // if already deinitialized, don't do anything
  if (info->handle->State == HAL_ADC_STATE_RESET) {
    return;
  }
  // deinitialize all pins
  for (uint32_t i = 0; i < gsl_adc_channel_count; ++i) {
    uint32_t mask = 1 << i;
    if (info->channel_init & mask) {
      GSL_PIN_Deinitialize(info->channel_pin[i]);
      info->channel_init &= ~mask;
    }
  }
  // disable interrupt
  HAL_NVIC_DisableIRQ(GSL_ADC_GetIRQ(ADCx));
  // deinitialize DMA if active
  GSL_DMA_Deinitialize(info->hdma);
  // deinitialize
  HAL_RUN(HAL_ADC_DeInit(info->handle));
}

// initialize ADC using #defined values
void GSL_ADC_Initialize(ADC_TypeDef * ADCx) {
  // get the handle
  GSL_ADC_InfoStruct * info = GSL_ADC_GetInfo(ADCx);
  // enable the clock
  GSL_ADC_EnableClock(ADCx);
  // deinitialize
  GSL_ADC_Deinitialize(ADCx);
  // adjust clock prescaler so that the ADC clock runs at 36 MHz or under
  float min_prescaler = HAL_RCC_GetPCLK2Freq() / 36e6;
  if (min_prescaler > 2.0 &&
      info->handle->Init.ClockPrescaler == ADC_CLOCK_SYNC_PCLK_DIV2) {
    info->handle->Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  }
  if (min_prescaler > 4.0 &&
      info->handle->Init.ClockPrescaler == ADC_CLOCK_SYNC_PCLK_DIV4) {
    info->handle->Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV6;
  }
  if (min_prescaler > 6.0 &&
      info->handle->Init.ClockPrescaler == ADC_CLOCK_SYNC_PCLK_DIV6) {
    info->handle->Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV8;
  }
  // go through ranks and initialize pins where necessary
  for (uint32_t i = 0; i < gsl_adc_rank_count; ++i) {
    if (!info->rank_config[i].defined) {
      break;
    }
    // special channels don't have pins
    if (info->rank_config[i].channel >= 16) {
      continue;
    }
    uint32_t mask = 1 << info->rank_config[i].channel;
    if ((info->channel_init & mask) == 0) {
      GSL_PIN_Initialize(
          info->channel_pin[info->rank_config[i].channel],
          GPIO_MODE_ANALOG,
          info->rank_config[i].pull_configuration,
          GPIO_SPEED_FREQ_LOW);
      info->channel_init |= mask;
    }
  }
  // set number of channels to the number initialized, or 1
  // first undefined rank defines the number of conversion channels
  info->handle->Init.NbrOfConversion = 0;
  for (uint32_t i = 0; i < gsl_adc_rank_count; ++i) {
    if (!info->rank_config[i].defined) {
      break;
    }
    ++info->handle->Init.NbrOfConversion;
  }
  if (info->handle->Init.NbrOfConversion == 0) {
    // (to avoid an error)
    info->handle->Init.NbrOfConversion = 1;
  }
  // if more than one channel, enable scan mode
  if (info->handle->Init.NbrOfConversion == 1) {
    info->handle->Init.ScanConvMode = DISABLE;
  } else {
    info->handle->Init.ScanConvMode = ENABLE;
  }
  // enable interrupt
  {
    // DEBUG
    //IRQn_Type irq = GSL_ADC_GetIRQ(ADCx);
    //HAL_NVIC_SetPriority(irq, 15, 0);
    //HAL_NVIC_EnableIRQ(irq);
  }
  // initialize
  HAL_RUN(HAL_ADC_Init(info->handle));
  // link DMA
  __HAL_LINKDMA(info->handle, DMA_Handle, *(info->hdma));
  // configure regular channels
  for (uint32_t i = 0; i < gsl_adc_rank_count; ++i) {
    if (info->rank_config[i].defined) {
      ADC_ChannelConfTypeDef sConfig;
      sConfig.Channel = info->rank_config[i].channel;
      sConfig.Rank = i + 1;
      sConfig.SamplingTime = info->rank_config[i].sampling_time;
      sConfig.Offset = 0;
      HAL_RUN(HAL_ADC_ConfigChannel(info->handle, &sConfig));
    } else {
      break;
    }
  }
}

// initialize an adc channel
void GSL_ADC_AddChannel(
    ADC_TypeDef * ADCx,
    uint32_t channel,
    uint32_t rank = -1,
    uint32_t sample_time = GSL_ADC_SAMPLETIME_DEFAULT,
    uint32_t pull_configuration = GPIO_NOPULL) {
  // get the handle
  GSL_ADC_InfoStruct * info = GSL_ADC_GetInfo(ADCx);
  // ensure range is valid
  ASSERT(IS_ADC_CHANNEL(channel));
  // if we're measuring VREFINT, then disable VBAT
  if (ADCx == ADC1 &&
      channel == ADC_CHANNEL_TEMPSENSOR &&
      ADC_CHANNEL_TEMPSENSOR != ADC_CHANNEL_16 &&
      ADC_CHANNEL_TEMPSENSOR != ADC_CHANNEL_18)  {
    ADC->CCR &= ~ADC_CCR_VBATE;
  }
  // find automatic rank
  if (rank == (uint32_t) -1) {
    rank = 0;
    while (rank < gsl_adc_rank_count && info->rank_config[rank].defined) {
        ++rank;
    }
  }
  // ensure rank is appropriate
  if (rank >= gsl_adc_rank_count) {
    HALT("Invalid parameter");
  }
  // set up rank
  info->rank_config[rank].defined = true;
  info->rank_config[rank].channel = channel;
  info->rank_config[rank].sampling_time = sample_time;
  info->rank_config[rank].pull_configuration = pull_configuration;
}

// initialize an adc channel by pin number
void GSL_ADC_AddChannelByPin(
    ADC_TypeDef * ADCx,
    PinEnum pin,
    uint32_t rank = -1,
    uint32_t sample_time = GSL_ADC_SAMPLETIME_DEFAULT,
    uint32_t pull_configuration = GPIO_NOPULL) {
  // find the channel
  uint32_t channel = 0xFFFFFFFF;
  const auto & info = GSL_ADC_GetInfo(ADCx);
  for (uint16_t i = 0; i < 16; ++i) {
    if (info->channel_pin[i] == pin) {
      channel = ADC_CHANNEL_0 + i;
    }
  }
  ASSERT_NE(channel, 0xFFFFFFFF);
  // add this channel
  GSL_ADC_AddChannel(ADCx, channel, rank, sample_time, pull_configuration);
}

// remove all channels
void GSL_ADC_RemoveAllChannels(ADC_TypeDef * ADCx) {
  // deinitialize
  GSL_ADC_Deinitialize(ADCx);
  // get the handle
  GSL_ADC_InfoStruct * info = GSL_ADC_GetInfo(ADCx);
  // undefine all ranks
  for (uint16_t i = 0; i < gsl_adc_rank_count; ++i) {
    info->rank_config[i].defined = false;
  }
}

// estimate VREF value using the 1.2V VREFINT channel
float GSL_ADC_EstimateVRef(void) {
  // TODO
  HALT("");
  return 0.0f;
}

// set the trigger source
// e.g. GSL_ADC_SetTrigger(ADC1, ADC_SOFTWARE_START);
// ADC_SOFTWARE_START is special in that it will trigger continuous conversion
void GSL_ADC_SetTrigger(
    ADC_TypeDef * ADCx,
    uint32_t trigger_source,
    uint32_t edge_type = ADC_EXTERNALTRIGCONVEDGE_RISING) {
  // get the handle
  GSL_ADC_InfoStruct * info = GSL_ADC_GetInfo(ADCx);
  // deinitialize if necessary
  GSL_ADC_Deinitialize(ADCx);
  // set new trigger
  info->handle->Init.ExternalTrigConv = trigger_source;
  info->handle->Init.ExternalTrigConvEdge = edge_type;
  if (trigger_source == ADC_SOFTWARE_START) {
    info->handle->Init.ContinuousConvMode = ENABLE;
    info->handle->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  } else {
    info->handle->Init.ContinuousConvMode = DISABLE;
  }
}

// report an ADC error and halt the program
void GSL_ADC_ReportError(HAL_StatusTypeDef status, ADC_HandleTypeDef * hadc) {
  if (status == HAL_OK) {
    return;
  }
  LOG("\nERROR: Encountered HAL error during ADC code");
  LOG("\nHAL_StatusTypeDef: ", (uint32_t) status);
  LOG("\nADC ErrorCode: ", (uint32_t) hadc->ErrorCode);
  HALT("ERROR");
}

// wait until the given ADC bus is available or timeout expires
void GSL_ADC_Wait(ADC_HandleTypeDef *hadc, uint32_t timeout_ms = 1000) {
  // get the start time
  uint32_t start = GSL_DEL_Ticks();
  // initialize if not already done
  if (hadc->State == HAL_ADC_STATE_RESET) {
    GSL_ADC_Initialize(hadc->Instance);
  }
  // wait until it's ready or the timeout occurs
  while (GSL_DEL_ElapsedMS(start) < timeout_ms &&
      hadc->State != HAL_ADC_STATE_READY) {
  }
  if (hadc->State != HAL_ADC_STATE_READY) {
    LOG("\nTimeout on ADC");
    HALT("ERROR");
  }
}

// wait until the given ADC bus is available or timeout expires
void GSL_ADC_Wait(ADC_TypeDef * ADCx, uint32_t timeout_ms = 1000) {
  GSL_ADC_Wait(GSL_ADC_GetInfo(ADCx)->handle, timeout_ms);
}

// start DMA conversion
// note that the number of readings is typically
// half of the buffer length in 9+ bit resolution mode
void GSL_ADC_Start_DMA_Circular(
    ADC_TypeDef * ADCx,
    void * buffer,
    uint16_t buffer_length) {
  // get the handle
  GSL_ADC_InfoStruct * info = GSL_ADC_GetInfo(ADCx);
  // counter number of channels
  info->handle->Init.NbrOfConversion = 0;
  for (uint16_t i = 0; i < gsl_adc_rank_count; ++i) {
    if (!info->rank_config[i].defined) {
      break;
    }
    ++info->handle->Init.NbrOfConversion;
  }
  // set up number of channels
  if (info->handle->Init.NbrOfConversion == 1) {
    // only one channel
    info->handle->Init.ScanConvMode = DISABLE;
  } else {
    // many channels, enable scanning between them
    info->handle->Init.ScanConvMode = ENABLE;
  }
  // TODO: remove trigger selection from this routine and put it somewhere else
  info->handle->Init.DiscontinuousConvMode = DISABLE;
  info->handle->Init.ContinuousConvMode = DISABLE;
  /*info->handle->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING; // DEBUG
  info->handle->Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC1;*/
  info->handle->Init.DMAContinuousRequests = ENABLE;
  // trigger at the end of all conversions
  info->handle->Init.EOCSelection = ADC_EOC_SEQ_CONV; // DEBUG
  // initialize the ADC
  GSL_ADC_Initialize(info->handle->Instance);
  // set up the byte length
  if (info->handle->Init.Resolution == ADC_RESOLUTION_6B ||
      info->handle->Init.Resolution == ADC_RESOLUTION_8B) {
    // single byte per sample
    info->handle->DMA_Handle->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    info->handle->DMA_Handle->Init.MemDataAlignment = DMA_PDATAALIGN_BYTE;
  } else {
    // two bytes per sample
    info->handle->DMA_Handle->Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    info->handle->DMA_Handle->Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    buffer_length /= 2;
  }
  // set up DMA settings
  info->handle->DMA_Handle->Init.Mode = DMA_CIRCULAR;
  // initialize the DMA
  GSL_DMA_Initialize(info->handle->DMA_Handle, 14);
  // start the conversion asynchronously
  HAL_RUN(HAL_ADC_Start_DMA(info->handle, (uint32_t*) buffer, buffer_length));
  //HAL_NVIC_SetPriority(GSL_ADC_GetIRQ(ADCx), 0, 1);
  //HAL_NVIC_EnableIRQ(GSL_ADC_GetIRQ(ADCx));
}

// start DMA conversion
// length is the buffer length in samples, not bytes
void GSL_ADC_Start_DMA(
    ADC_TypeDef * ADCx,
    void * buffer,
    uint16_t length) {
  // get the handle
  GSL_ADC_InfoStruct * info = GSL_ADC_GetInfo(ADCx);
  // counter number of channels
  info->handle->Init.NbrOfConversion = 0;
  for (uint16_t i = 0; i < gsl_adc_rank_count; ++i) {
    if (!info->rank_config[i].defined) {
      break;
    }
    ++info->handle->Init.NbrOfConversion;
  }
  // set up number of channels
  if (info->handle->Init.NbrOfConversion == 1) {
    // only one channel
    info->handle->Init.ScanConvMode = DISABLE;
  } else {
    // many channels, enable scanning between them
    info->handle->Init.ScanConvMode = ENABLE;
  }
  info->handle->Init.DiscontinuousConvMode = DISABLE;
  info->handle->Init.DMAContinuousRequests = ENABLE;
  // trigger at the end of all conversions
  info->handle->Init.EOCSelection = ADC_EOC_SEQ_CONV;
  // initialize the ADC
  GSL_ADC_Initialize(info->handle->Instance);
  // set up the byte length
  if (info->handle->Init.Resolution == ADC_RESOLUTION_6B ||
      info->handle->Init.Resolution == ADC_RESOLUTION_8B) {
    // single byte per sample
    info->handle->DMA_Handle->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    info->handle->DMA_Handle->Init.MemDataAlignment = DMA_PDATAALIGN_BYTE;
  } else {
    // two bytes per sample
    info->handle->DMA_Handle->Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    info->handle->DMA_Handle->Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
  }
  // set up DMA settings
  info->handle->DMA_Handle->Init.Mode = DMA_NORMAL;
  // initialize the DMA
  GSL_DMA_Initialize(info->handle->DMA_Handle);
  // start the conversion asynchronously
  HAL_RUN(HAL_ADC_Start_DMA(info->handle, (uint32_t*) buffer, length));
}

// stop the DMA conversion
void GSL_ADC_Stop_DMA(ADC_TypeDef * ADCx) {
  // get the handle
  GSL_ADC_InfoStruct * info = GSL_ADC_GetInfo(ADCx);
  HAL_RUN(HAL_ADC_Stop_DMA(info->handle));
}

// get a single value in blocking mode and return it
uint16_t GSL_ADC_ReadChannel(
    ADC_TypeDef * ADCx,
    uint32_t channel,
    uint32_t sample_time = GSL_ADC_SAMPLETIME_DEFAULT) {
  // get the info
  GSL_ADC_InfoStruct * info = GSL_ADC_GetInfo(ADCx);
  GSL_ADC_RemoveAllChannels(ADCx);
  GSL_ADC_AddChannel(ADCx, channel, -1, sample_time);
  // set software trigger
  GSL_ADC_SetTrigger(ADCx, ADC_SOFTWARE_START);
  GSL_ADC_Initialize(ADCx);
  // start the conversion
  HAL_RUN(HAL_ADC_Start(info->handle));
  // wait for it to complete
  HAL_RUN(HAL_ADC_PollForConversion(info->handle, 100));
  // read the value
  uint16_t value = HAL_ADC_GetValue(info->handle);
  // stop the ADC
  HAL_RUN(HAL_ADC_Stop(info->handle));
  // return the result
  return value;
}

// get a single value in interrupt mode
void GSL_ADC_ReadChannel_IT(
    ADC_TypeDef * ADCx,
    uint32_t channel,
    uint32_t sample_time = GSL_ADC_SAMPLETIME_DEFAULT) {
  // get the info
  GSL_ADC_InfoStruct * info = GSL_ADC_GetInfo(ADCx);
  GSL_ADC_RemoveAllChannels(ADCx);
  GSL_ADC_AddChannel(ADCx, channel, -1, sample_time);
  GSL_ADC_Initialize(ADCx);
  // start the conversion
  HAL_RUN(HAL_ADC_Start_IT(info->handle));
}

// convert a 12-bit internal temperature reading into a temperature in celsius
// (this uses vref+ and the two calibration values in memory)
float GSL_ADC_ConvertTemperature(uint16_t counts) {
  const uint16_t * TS_CAL = (uint16_t *) 0x1FFF7A2C;
  const uint16_t TS_CAL1 = TS_CAL[0];
  const uint16_t TS_CAL2 = TS_CAL[1];
  // convert counts to counts as if we're at 3.3V
  float temp_c = counts * gsl_adc_vref / 3.3f;
  // use TS_CAL1 and TS_CAL2 to convert temperature
  temp_c = 30.0f + 80.0f * (temp_c - TS_CAL1) / (TS_CAL2 - TS_CAL1);
  return temp_c;
}

// return the temperature in degrees C
// LSB was 0.310 on a STM32F411 device
// we take many readings and average them to get a more accurate value
float GSL_ADC_GetTemperatureC(void) {
  // take many readings and average them
  float mean = 0.0f;
  //float lsb = 0.0f;

  for (uint16_t i = 0; i < GSL_ADC_REF_SAMPLE_COUNT; ++i) {
    uint16_t counts = GSL_ADC_ReadChannel(ADC1,
        ADC_CHANNEL_TEMPSENSOR,
                                          ADC_SAMPLETIME_28CYCLES);
    float temp = GSL_ADC_ConvertTemperature(counts);
    //lsb += GSL_ADC_ConvertTemperature(counts + 1) -
    //    GSL_ADC_ConvertTemperature(counts - 1);
    mean += temp;
  }
  //lsb /= reading_count * 2;
  //LOG("\nLSB is ", GSL_OUT_FixedFloat(lsb, 3));
  mean /= GSL_ADC_REF_SAMPLE_COUNT;
  return mean;
}

// get the counts of the VBAT channel
// note that this is internally divided.  on some devices, it's 2:1, on others
// it's 4:1.  we leave this to the user to interpret.
float GSL_ADC_GetVBatCounts(void) {
  // take many readings and average them
  float mean = 0.0f;
  //float lsb = 0.0f;
  for (uint16_t i = 0; i < GSL_ADC_REF_SAMPLE_COUNT; ++i) {
    mean += GSL_ADC_ReadChannel(ADC1,
                                ADC_CHANNEL_VBAT,
                                ADC_SAMPLETIME_28CYCLES);
  }
  //lsb /= reading_count * 2;
  //LOG("\nLSB is ", GSL_OUT_FixedFloat(lsb, 3));
  mean /= GSL_ADC_REF_SAMPLE_COUNT;
  return mean;
}

// get the counts of the VREFINT channel.  THis channel is at 1.21V nominally.
float GSL_ADC_GetVRefIntCounts(void) {
  // take many readings and average them
  float mean = 0.0f;
  for (uint16_t i = 0; i < GSL_ADC_REF_SAMPLE_COUNT; ++i) {
    mean += GSL_ADC_ReadChannel(ADC1,
                                ADC_CHANNEL_VREFINT,
                                ADC_SAMPLETIME_28CYCLES);
  }
  mean /= GSL_ADC_REF_SAMPLE_COUNT;
  return mean;
}

// error handler
extern "C" {

void HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef* hadc) {
  UNUSED(hadc);
  //LOG("Input voltage out of range");
}

// errors such as HAL_ADC_ERROR_NONE
void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc) {
  // clear the flag
  LOG("\nAn error occurred on ADC ",
      GSL_OUT_Hex((uint32_t) hadc->Instance));
  //volatile uint32_t code = hadc->ErrorCode;
  //volatile uint32_t state = hadc->State;
  LOG("\nError code: ", GSL_OUT_Hex(hadc->ErrorCode));
  HALT("ERROR");
}

// conversion complete
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
  // if this was a one-time DMA transfer, disable it
  // 2016-DEC-20: disabled this check.  it was triggering during a non-circular
  // DMA event
  /*if (hadc->DMA_Handle->State != HAL_DMA_STATE_RESET &&
      hadc->DMA_Handle->Init.Mode == DMA_NORMAL) {
    LOG("\nState = ", hadc->DMA_Handle->State);
    LOG("\nMode = ", hadc->DMA_Handle->Init.Mode);
    LOG("\n12431234");
    GSL_ADC_ReportError(HAL_ADC_Stop_DMA(hadc), hadc);
    LOG("5");
  }*/
  // get the info
  GSL_ADC_InfoStruct * info = GSL_ADC_GetInfo(hadc->Instance);
  // call the callback routine if one is defined
  if (info->ConvCpltCallback) {
    info->ConvCpltCallback();
  }
}

// conversion half complete
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc) {
  // get the info
  GSL_ADC_InfoStruct * info = GSL_ADC_GetInfo(hadc->Instance);
  // call the callback routine if one is defined
  if (info->ConvHalfCpltCallback) {
    info->ConvHalfCpltCallback();
  }
}

// ADC IRQ handler
void ADC_IRQHandler(void) {
  if (gsl_adc1_hadc.State != HAL_ADC_STATE_RESET) {
    HAL_ADC_IRQHandler(&gsl_adc1_hadc);
  }
#ifdef ADC2
  if (gsl_adc2_hadc.State != HAL_ADC_STATE_RESET) {
    HAL_ADC_IRQHandler(&gsl_adc2_hadc);
  }
#endif
#ifdef ADC3
  if (gsl_adc3_hadc.State != HAL_ADC_STATE_RESET) {
    HAL_ADC_IRQHandler(&gsl_adc3_hadc);
  }
#endif
}

}
