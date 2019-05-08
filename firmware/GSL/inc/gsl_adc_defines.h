#pragma once

// This provides a interface to configure and use the ADC peripherals.

#include "gsl_includes.h"

// voltage at the vref pin
#ifndef GSL_ADC_VREF
#define GSL_ADC_VREF 3.3f
#endif

// vref+ voltage
float gsl_adc_vref = GSL_ADC_VREF;

// number of samples to take for VBAT, VREFINT and TEMP calculations
#ifndef GSL_ADC_REF_SAMPLE_COUNT
#define GSL_ADC_REF_SAMPLE_COUNT 32
#endif

// defaults

// resolution
#ifndef GSL_ADC_RESOLUTION_DEFAULT
#define GSL_ADC_RESOLUTION_DEFAULT ADC_RESOLUTION_12B
#endif

// data alignment
#ifndef GSL_ADC_DATAALIGN_DEFAULT
#define GSL_ADC_DATAALIGN_DEFAULT ADC_DATAALIGN_LEFT
#endif

// sample cycles
#ifndef GSL_ADC_SAMPLETIME_DEFAULT
#define GSL_ADC_SAMPLETIME_DEFAULT ADC_SAMPLETIME_480CYCLES
#endif

// number of regular adc channels
const uint8_t gsl_adc_channel_count = 16;

// number of channels that can be converted in a row
const uint8_t gsl_adc_rank_count = 16;

// struct to hold channel information
struct GSL_ADC_ChannelStruct {
  bool defined;
  uint32_t channel;
  uint32_t sampling_time;
  uint32_t pull_configuration;
};

// ************
// *** ADC1 ***
// ************

// initialize ADC1 settings
#ifdef ADC1

// resolution
#ifndef GSL_ADC1_RESOLUTION
#define GSL_ADC1_RESOLUTION GSL_ADC_RESOLUTION_DEFAULT
#endif

// data alignment
#ifndef GSL_ADC1_DATAALIGN
#define GSL_ADC1_DATAALIGN GSL_ADC_DATAALIGN_DEFAULT
#endif

// ADC1 channel pins
const PinEnum gsl_adc1_channel_pin[gsl_adc_channel_count] = {
    kPinA0, kPinA1, kPinA2, kPinA3, kPinA4, kPinA5, kPinA6, kPinA7,
    kPinB0, kPinB1, kPinC0, kPinC1, kPinC2, kPinC3, kPinC4, kPinC5,
};

// ADC1 regular conversion channels
GSL_ADC_ChannelStruct gsl_adc1_channel_rank[gsl_adc_rank_count];

// ADC1 handle
ADC_HandleTypeDef gsl_adc1_hadc = {
    ADC1,
    {
        ADC_CLOCK_SYNC_PCLK_DIV2,
        ADC_RESOLUTION_12B,
        ADC_DATAALIGN_RIGHT,
        ENABLE,
        ADC_EOC_SEQ_CONV,
        DISABLE,
        1,
        DISABLE,
        0,
        ADC_SOFTWARE_START,
        ADC_EXTERNALTRIGCONVEDGE_NONE,
        DISABLE,
        },
    0,
    nullptr,
    HAL_UNLOCKED,
    HAL_ADC_STATE_RESET,
    HAL_ADC_ERROR_NONE};

// initialize ADC1 DMA settings

// ADC1 --> DMA 2 stream 0 or 4 channel 0
#ifndef GSL_ADC1_DMA_STREAM
#define GSL_ADC1_DMA_STREAM DMA2_Stream0
#endif
/*static_assert(GSL_ADC1_DMA_STREAM == DMA2_Stream0 ||
              GSL_ADC1_DMA_STREAM == DMA2_Stream4,
              "Invalid channel");*/
#define GSL_ADC1_DMA_CHANNEL DMA_CHANNEL_0

DMA_HandleTypeDef gsl_adc1_hdma = {
    GSL_ADC1_DMA_STREAM,
    {
        GSL_ADC1_DMA_CHANNEL,
        DMA_PERIPH_TO_MEMORY,
        DMA_PINC_DISABLE,
        DMA_MINC_ENABLE,
        DMA_PDATAALIGN_HALFWORD,
        DMA_MDATAALIGN_HALFWORD,
        DMA_NORMAL,
        DMA_PRIORITY_HIGH,
        DMA_FIFOMODE_DISABLE,
        DMA_FIFO_THRESHOLD_FULL,
        DMA_MBURST_SINGLE,
        DMA_PBURST_SINGLE},
    HAL_UNLOCKED,
    HAL_DMA_STATE_RESET,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    HAL_DMA_ERROR_NONE,
    0,
    0};

#endif // #ifdef ADC1

// ************
// *** ADC2 ***
// ************

// initialize ADC2 settings
#ifdef ADC2

// resolution
#ifndef GSL_ADC2_RESOLUTION
#define GSL_ADC2_RESOLUTION GSL_ADC_RESOLUTION_DEFAULT
#endif

// data alignment
#ifndef GSL_ADC2_DATAALIGN
#define GSL_ADC2_DATAALIGN GSL_ADC_DATAALIGN_DEFAULT
#endif

// ADC2 channel pins
const PinEnum gsl_adc2_channel_pin[gsl_adc_channel_count] = {
    kPinA0, kPinA1, kPinA2, kPinA3, kPinA4, kPinA5, kPinA6, kPinA7,
    kPinB0, kPinB1, kPinC0, kPinC1, kPinC2, kPinC3, kPinC4, kPinC5,
};

// ADC2 regular conversion channels
GSL_ADC_ChannelStruct gsl_adc2_channel_rank[gsl_adc_rank_count];

// ADC2 handle
ADC_HandleTypeDef gsl_adc2_hadc = {
    ADC2,
    {
        ADC_CLOCK_SYNC_PCLK_DIV2,
        ADC_RESOLUTION_12B,
        ADC_DATAALIGN_RIGHT,
        ENABLE,
        ADC_EOC_SEQ_CONV,
        DISABLE,
        1,
        DISABLE,
        0,
        ADC_SOFTWARE_START,
        ADC_EXTERNALTRIGCONVEDGE_NONE,
        DISABLE,
        },
    0,
    nullptr,
    HAL_UNLOCKED,
    HAL_ADC_STATE_RESET,
    HAL_ADC_ERROR_NONE};

// initialize ADC2 DMA settings

// ADC2 --> DMA 2 stream 2 or 3 channel 1
#ifndef GSL_ADC2_DMA_STREAM
#define GSL_ADC2_DMA_STREAM DMA2_Stream2
#endif
#define GSL_ADC2_DMA_CHANNEL DMA_CHANNEL_1

DMA_HandleTypeDef gsl_adc2_hdma = {
    GSL_ADC2_DMA_STREAM,
    {
      GSL_ADC2_DMA_CHANNEL,
      DMA_PERIPH_TO_MEMORY,
      DMA_PINC_DISABLE,
      DMA_MINC_ENABLE,
      DMA_PDATAALIGN_HALFWORD,
      DMA_MDATAALIGN_HALFWORD,
      DMA_NORMAL,
      DMA_PRIORITY_VERY_HIGH,
      DMA_FIFOMODE_DISABLE,
      DMA_FIFO_THRESHOLD_FULL,
      DMA_MBURST_SINGLE,
      DMA_PBURST_SINGLE},
    HAL_UNLOCKED,
    HAL_DMA_STATE_RESET,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    HAL_DMA_ERROR_NONE,
    0,
    0};

#endif // #ifdef ADC2

// ************
// *** ADC3 ***
// ************

// initialize ADC3 settings
#ifdef ADC3

// resolution
#ifndef GSL_ADC3_RESOLUTION
#define GSL_ADC3_RESOLUTION GSL_ADC_RESOLUTION_DEFAULT
#endif

// data alignment
#ifndef GSL_ADC3_DATAALIGN
#define GSL_ADC3_DATAALIGN GSL_ADC_DATAALIGN_DEFAULT
#endif

// ADC3 channel pins
const PinEnum gsl_adc3_channel_pin[gsl_adc_channel_count] = {
    kPinA0, kPinA1, kPinA2, kPinA3,
#ifdef GPIOF
    kPinF6, kPinF7, kPinF8,
    kPinF9, kPinF10, kPinF3,
#else
    kPinInvalid, kPinInvalid, kPinInvalid,
    kPinInvalid, kPinInvalid, kPinInvalid,
#endif
    kPinC0, kPinC1, kPinC2, kPinC3,
#ifdef GPIOF
    kPinF4, kPinF5,
#else
    kPinInvalid, kPinInvalid,
#endif
};

// ADC3 regular conversion channels
GSL_ADC_ChannelStruct gsl_adc3_channel_rank[gsl_adc_rank_count];

// ADC3 handle
ADC_HandleTypeDef gsl_adc3_hadc = {
    ADC3,
    {
        ADC_CLOCK_SYNC_PCLK_DIV2,
        ADC_RESOLUTION_12B,
        ADC_DATAALIGN_RIGHT,
        ENABLE,
        ADC_EOC_SEQ_CONV,
        DISABLE,
        1,
        DISABLE,
        0,
        ADC_SOFTWARE_START,
        ADC_EXTERNALTRIGCONVEDGE_NONE,
        DISABLE,
        },
    0,
    nullptr,
    HAL_UNLOCKED,
    HAL_ADC_STATE_RESET,
    HAL_ADC_ERROR_NONE};

// initialize ADC3 DMA settings

// ADC3 --> DMA 2 stream 0 or 1 channel 2
#ifndef GSL_ADC3_DMA_STREAM
#define GSL_ADC3_DMA_STREAM DMA2_Stream1
#endif
#define GSL_ADC3_DMA_CHANNEL DMA_CHANNEL_2

DMA_HandleTypeDef gsl_adc3_hdma = {
    GSL_ADC3_DMA_STREAM,
    {
        GSL_ADC3_DMA_CHANNEL,
        DMA_PERIPH_TO_MEMORY,
        DMA_PINC_DISABLE,
        DMA_MINC_ENABLE,
        DMA_PDATAALIGN_HALFWORD,
        DMA_MDATAALIGN_HALFWORD,
        DMA_NORMAL,
        DMA_PRIORITY_MEDIUM,
        DMA_FIFOMODE_DISABLE,
        DMA_FIFO_THRESHOLD_FULL,
        DMA_MBURST_SINGLE,
        DMA_PBURST_SINGLE},
    HAL_UNLOCKED,
    HAL_DMA_STATE_RESET,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    HAL_DMA_ERROR_NONE,
    0,
    0};

#endif // #ifdef ADC3

// this holds pointers to the various structures we need
struct GSL_ADC_InfoStruct {
  ADC_TypeDef * ADCx;
  ADC_HandleTypeDef * handle;
  const PinEnum * channel_pin;
  GSL_ADC_ChannelStruct * rank_config;
  uint32_t channel_init; // pin set to analog mode
  DMA_HandleTypeDef * hdma;
  void (*ConvCpltCallback)(void);
  void (*ConvHalfCpltCallback)(void);
};

// here we define the info structure which points to all the various other
// structures
GSL_ADC_InfoStruct GSL_ADC_Info[] = {

#ifdef ADC1
    {
        ADC1,
        &gsl_adc1_hadc,
        gsl_adc1_channel_pin,
        gsl_adc1_channel_rank,
        0,
        &gsl_adc1_hdma,
        nullptr,
        nullptr,
    },
#endif // #ifdef ADC1

#ifdef ADC2
    {
        ADC2,
        &gsl_adc2_hadc,
        gsl_adc2_channel_pin,
        gsl_adc2_channel_rank,
        0,
        &gsl_adc2_hdma,
        nullptr,
        nullptr,
    },
#endif // #ifdef ADC2

#ifdef ADC3
    {
        ADC3,
        &gsl_adc3_hadc,
        gsl_adc3_channel_pin,
        gsl_adc3_channel_rank,
        0,
        &gsl_adc3_hdma,
        nullptr,
        nullptr,
    },
#endif // #ifdef ADC3

};

// number of ADC peripherals
const uint32_t GSL_ADC_PeripheralCount =
    sizeof(GSL_ADC_Info) / sizeof(*GSL_ADC_Info);

// bytes of RAM used to store all the handler structs
const uint32_t GSL_ADC_StorageByteCount =
    sizeof(GSL_ADC_Info) +
    GSL_ADC_PeripheralCount * sizeof(ADC_HandleTypeDef) +
    GSL_ADC_PeripheralCount * sizeof(DMA_HandleTypeDef);
