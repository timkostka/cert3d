#pragma once

#include "gsl_includes.h"

// types of signals we can generate
enum GSLSignalEnum {
  kSignalSine,
  kSignalSquare,
  kSignalTriangle,
  kSignalRampUp,
  kSignalRampDown,
};

// ************
// *** DAC ***
// ************

// initialize DAC DMA settings

// DAC1
#define GSL_DAC1_DMA_STREAM DMA1_Stream5
#define GSL_DAC1_DMA_CHANNEL DMA_CHANNEL_7

// DAC2
#define GSL_DAC2_DMA_STREAM DMA1_Stream6
#define GSL_DAC2_DMA_CHANNEL DMA_CHANNEL_7

#ifndef GSL_DAC1_TIMER
#define GSL_DAC1_TIMER TIM7
#endif

#ifndef GSL_DAC2_TIMER
#define GSL_DAC2_TIMER TIM8
#endif

extern DAC_HandleTypeDef gsl_dac_hdac;

// DAC channel 1 DMA handle
DMA_HandleTypeDef gsl_dac_hdma1 = {
    GSL_DAC1_DMA_STREAM,
    {
        GSL_DAC1_DMA_CHANNEL,
        DMA_MEMORY_TO_PERIPH,
        DMA_PINC_DISABLE,
        DMA_MINC_ENABLE,
        DMA_PDATAALIGN_HALFWORD,
        DMA_MDATAALIGN_HALFWORD,
        DMA_CIRCULAR,
        DMA_PRIORITY_HIGH,
        DMA_FIFOMODE_ENABLE,
        DMA_FIFO_THRESHOLD_FULL,
        DMA_MBURST_SINGLE,
        DMA_PBURST_SINGLE},
    HAL_UNLOCKED,
    HAL_DMA_STATE_RESET,
    &gsl_dac_hdac,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    HAL_DMA_ERROR_NONE,
    0,
    0};

// DAC channel 2 DMA handle
DMA_HandleTypeDef gsl_dac_hdma2 = {
    GSL_DAC2_DMA_STREAM,
    {
        GSL_DAC2_DMA_CHANNEL,
        DMA_MEMORY_TO_PERIPH,
        DMA_PINC_DISABLE,
        DMA_MINC_ENABLE,
        DMA_PDATAALIGN_HALFWORD,
        DMA_MDATAALIGN_HALFWORD,
        DMA_CIRCULAR,
        DMA_PRIORITY_HIGH,
        DMA_FIFOMODE_ENABLE,
        DMA_FIFO_THRESHOLD_FULL,
        DMA_MBURST_SINGLE,
        DMA_PBURST_SINGLE},
    HAL_UNLOCKED,
    HAL_DMA_STATE_RESET,
    &gsl_dac_hdac,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    HAL_DMA_ERROR_NONE,
    0,
    0};

// DAC handle
DAC_HandleTypeDef gsl_dac_hdac = {
    DAC,
    HAL_DAC_STATE_RESET,
    HAL_UNLOCKED,
    &gsl_dac_hdma1,
    &gsl_dac_hdma2,
    HAL_DAC_ERROR_NONE};

// ***********************

// info for a given channel
struct GSL_DAC_ChannelStruct {
  uint32_t channel;
  PinEnum pin;
  TIM_TypeDef * TIMx;
  DMA_HandleTypeDef * hdma;
  uint16_t * buffer;
  uint16_t buffer_length;
};

// this holds pointers to the various structures we need
struct GSL_DAC_InfoStruct {
  DAC_TypeDef * DACx;
  DAC_HandleTypeDef * hadc;
  GSL_DAC_ChannelStruct channel[2];
};

// here we define the info structure which points to all the various other
// structures
GSL_DAC_InfoStruct GSL_DAC_Info = {
    DAC,
    &gsl_dac_hdac,
    {
        {
          DAC_CHANNEL_1,
          kPinA4,
          GSL_DAC1_TIMER,
          &gsl_dac_hdma1,
          nullptr,
          0},
      {
          DAC_CHANNEL_2,
          kPinA5,
          GSL_DAC2_TIMER,
          &gsl_dac_hdma2,
          nullptr,
          0},
      }
};

// number of DAC peripherals
const uint16_t GSL_DAC_PeripheralCount = 1;

// buffer capacity
const uint16_t gsl_dac_buffer_capacity = 1024;

// bytes of RAM used to store all the handler structs
const uint16_t GSL_DAC_StorageByteCount =
    sizeof(GSL_DAC_Info) +
    sizeof(DAC_HandleTypeDef) +
    2 * sizeof(DMA_HandleTypeDef);
