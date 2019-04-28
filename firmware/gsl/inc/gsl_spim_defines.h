#pragma once

// This file implements a DMA-based asynchronous SPI master.

#include "gsl_includes.h"

// default values

// default buffer size
#ifndef GSL_SPIM_BUFFER_SIZE_DEFAULT
#define GSL_SPIM_BUFFER_SIZE_DEFAULT 256
#endif

// information for a given SPIM buffer
struct GSL_SPIM_InfoStruct {
  // SPI instance
  SPI_TypeDef * SPIx;
  // active
  bool active;
  // buffer capacity
  uint32_t capacity;
  // buffer for outgoing data
  GSL_CBUF buffer;
  // pointer to dma tx
  DMA_HandleTypeDef * hdmatx;
  // total number of bytes processed
  uint32_t processed_bytes;
};

// ************
// *** SPI1 ***
// ************

#ifdef SPI1

#ifndef GSL_SPI1M_BUFFER_SIZE
#define GSL_SPI1M_BUFFER_SIZE GSL_SPIM_BUFFER_SIZE_DEFAULT
#endif

// buffer info
GSL_SPIM_InfoStruct gsl_spi1m_info = {
    SPI1,
    false,
    GSL_SPI1M_BUFFER_SIZE,
    GSL_CBUF(0),
    nullptr,
    0};

#endif // #ifdef SPI1

// ************
// *** SPI2 ***
// ************

#ifdef SPI2

#ifndef GSL_SPI2M_BUFFER_SIZE
#define GSL_SPI2M_BUFFER_SIZE GSL_SPIM_BUFFER_SIZE_DEFAULT
#endif

// buffer info
GSL_SPIM_InfoStruct gsl_spi2m_info = {
    SPI2,
    false,
    GSL_SPI2M_BUFFER_SIZE,
    GSL_CBUF(0),
    nullptr,
    0};

#endif // #ifdef SPI2

// ************
// *** SPI3 ***
// ************

#ifdef SPI3

#ifndef GSL_SPI3M_BUFFER_SIZE
#define GSL_SPI3M_BUFFER_SIZE GSL_SPIM_BUFFER_SIZE_DEFAULT
#endif

// buffer info
GSL_SPIM_InfoStruct gsl_spi3m_info = {
    SPI3,
    false,
    GSL_SPI3M_BUFFER_SIZE,
    GSL_CBUF(0),
    nullptr,
    0};

#endif // #ifdef SPI3

// ************
// *** SPI4 ***
// ************

#ifdef SPI4

#ifndef GSL_SPI4M_BUFFER_SIZE
#define GSL_SPI4M_BUFFER_SIZE GSL_SPIM_BUFFER_SIZE_DEFAULT
#endif

// buffer info
GSL_SPIM_InfoStruct gsl_spi4m_info = {
    SPI4,
    false,
    GSL_SPI4M_BUFFER_SIZE,
    GSL_CBUF(0),
    nullptr,
    0};

#endif // #ifdef SPI4

// ************
// *** SPI5 ***
// ************

#ifdef SPI5

#ifndef GSL_SPI5M_BUFFER_SIZE
#define GSL_SPI5M_BUFFER_SIZE GSL_SPIM_BUFFER_SIZE_DEFAULT
#endif

// buffer info
GSL_SPIM_InfoStruct gsl_spi5m_info = {
    SPI5,
    false,
    GSL_SPI5M_BUFFER_SIZE,
    GSL_CBUF(0),
    nullptr,
    0};

#endif // #ifdef SPI5

// *************
// *** SPI6 ***
// *************

#ifdef SPI6

#ifndef GSL_SPI6M_BUFFER_SIZE
#define GSL_SPI6M_BUFFER_SIZE GSL_SPIM_BUFFER_SIZE_DEFAULT
#endif

// buffer info
GSL_SPIM_InfoStruct gsl_spi6m_info = {
    SPI6,
    false,
    GSL_SPI6M_BUFFER_SIZE,
    GSL_CBUF(0),
    nullptr,
    0};

#endif // #ifdef SPI6

// end of SPI defines

// pointer to all info
GSL_SPIM_InfoStruct * GSL_SPIM_Info[] = {
#ifdef SPI1
    &gsl_spi1m_info,
#endif
#ifdef SPI2
    &gsl_spi2m_info,
#endif
#ifdef SPI3
    &gsl_spi3m_info,
#endif
#ifdef SPI4
    &gsl_spi4m_info,
#endif
#ifdef SPI5
    &gsl_spi5m_info,
#endif
#ifdef SPI6
    &gsl_spi6m_info,
#endif
    };

// number of peripherals
const uint16_t GSL_SPIM_PeripheralCount =
    sizeof(GSL_SPIM_Info) / sizeof(*GSL_SPIM_Info);
