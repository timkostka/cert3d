#pragma once

// This file implements DMA-based SPI slave receivers.

#include "gsl_includes.h"

// when sending out data, number of dummy bytes before the actual data
const uint16_t gsl_spirx_dummy_bytes = 2;

// default values

// default buffer size
#ifndef GSL_SPIRX_BUFFER_SIZE_DEFAULT
#define GSL_SPIRX_BUFFER_SIZE_DEFAULT 256
#endif

// error enum
enum GSL_SPIRX_ErrorCodeEnum {
  kGslSPIRXErrorNone,
  kGslSPIRXErrorOverflow,
};

// information for a given SPI RX buffer
struct GSL_SPIRX_InfoStruct {
  // SPI instance
  SPI_TypeDef * SPIx;
  // active
  bool active;
  // buffer location
  uint8_t * buffer;
  // buffer capacity
  uint16_t capacity;
  // location of next byte to read
  uint8_t * next_data;
  // pointer to dma rx
  DMA_HandleTypeDef * hdmarx;
  // pointer to dma tx, if any
  DMA_HandleTypeDef * hdmatx;
  // error codes
  GSL_SPIRX_ErrorCodeEnum last_error;
  // number of bytes processed
  uint32_t bytes_processed;
  // max bytes waiting
  uint16_t max_bytes_waiting;
};

// ************
// *** SPI1 ***
// ************

#ifdef SPI1

#ifndef GSL_SPI1RX_BUFFER_SIZE
#define GSL_SPI1RX_BUFFER_SIZE GSL_SPIRX_BUFFER_SIZE_DEFAULT
#endif

// buffer info
GSL_SPIRX_InfoStruct gsl_spi1rx_info = {
    SPI1,
    false,
    nullptr,
    GSL_SPI1RX_BUFFER_SIZE,
    nullptr,
    nullptr,
    nullptr,
    kGslSPIRXErrorNone,
    0,
    0};

#endif // #ifdef SPI1

// ************
// *** SPI2 ***
// ************

#ifdef SPI2

#ifndef GSL_SPI2RX_BUFFER_SIZE
#define GSL_SPI2RX_BUFFER_SIZE GSL_SPIRX_BUFFER_SIZE_DEFAULT
#endif

// buffer info
GSL_SPIRX_InfoStruct gsl_spi2rx_info = {
    SPI2,
    false,
    nullptr,
    GSL_SPI2RX_BUFFER_SIZE,
    nullptr,
    nullptr,
    nullptr,
    kGslSPIRXErrorNone,
    0,
    0};

#endif // #ifdef SPI2

// ************
// *** SPI3 ***
// ************

#ifdef SPI3

#ifndef GSL_SPI3RX_BUFFER_SIZE
#define GSL_SPI3RX_BUFFER_SIZE GSL_SPIRX_BUFFER_SIZE_DEFAULT
#endif

// buffer info
GSL_SPIRX_InfoStruct gsl_spi3rx_info = {
    SPI3,
    false,
    nullptr,
    GSL_SPI3RX_BUFFER_SIZE,
    nullptr,
    nullptr,
    nullptr,
    kGslSPIRXErrorNone,
    0,
    0};

#endif // #ifdef SPI3

// ************
// *** SPI4 ***
// ************

#ifdef SPI4

#ifndef GSL_SPI4RX_BUFFER_SIZE
#define GSL_SPI4RX_BUFFER_SIZE GSL_SPIRX_BUFFER_SIZE_DEFAULT
#endif

// buffer info
GSL_SPIRX_InfoStruct gsl_spi4rx_info = {
    SPI4,
    false,
    nullptr,
    GSL_SPI4RX_BUFFER_SIZE,
    nullptr,
    nullptr,
    nullptr,
    kGslSPIRXErrorNone,
    0,
    0};

#endif // #ifdef SPI4

// ************
// *** SPI5 ***
// ************

#ifdef SPI5

#ifndef GSL_SPI5RX_BUFFER_SIZE
#define GSL_SPI5RX_BUFFER_SIZE GSL_SPIRX_BUFFER_SIZE_DEFAULT
#endif

// buffer info
GSL_SPIRX_InfoStruct gsl_spi5rx_info = {
    SPI5,
    false,
    nullptr,
    GSL_SPI5RX_BUFFER_SIZE,
    nullptr,
    nullptr,
    nullptr,
    kGslSPIRXErrorNone,
    0,
    0};

#endif // #ifdef SPI5

// ************
// *** SPI6 ***
// ************

#ifdef SPI6

#ifndef GSL_SPI6RX_BUFFER_SIZE
#define GSL_SPI6RX_BUFFER_SIZE GSL_SPIRX_BUFFER_SIZE_DEFAULT
#endif

// buffer info
GSL_SPIRX_InfoStruct gsl_spi6rx_info = {
    SPI6,
    false,
    nullptr,
    GSL_SPI6RX_BUFFER_SIZE,
    nullptr,
    nullptr,
    nullptr,
    kGslSPIRXErrorNone,
    0,
    0};

#endif // #ifdef SPI6

// end of SPI defines

// pointer to all info
GSL_SPIRX_InfoStruct * GSL_SPIRX_Info[] = {
#ifdef SPI1
    &gsl_spi1rx_info,
#endif
#ifdef SPI2
    &gsl_spi2rx_info,
#endif
#ifdef SPI3
    &gsl_spi3rx_info,
#endif
#ifdef SPI4
    &gsl_spi4rx_info,
#endif
#ifdef SPI5
    &gsl_spi5rx_info,
#endif
#ifdef SPI6
    &gsl_spi6rx_info,
#endif
    };

// number of peripherals
const uint16_t GSL_SPIRX_PeripheralCount =
    sizeof(GSL_SPIRX_Info) / sizeof(*GSL_SPIRX_Info);
