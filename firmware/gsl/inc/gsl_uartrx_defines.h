#pragma once

// This file implements a DMA-based UART receiver.

#include "gsl_includes.h"

// default values

// default buffer size
#ifndef GSL_UARTRX_BUFFER_SIZE_DEFAULT
#define GSL_UARTRX_BUFFER_SIZE_DEFAULT 256
#endif

// error enum
enum GSL_UARTRX_ErrorCodeEnum {
  kGslUARTRXErrorNone,
  kGslUARTRXErrorOverflow,
};

// information for a given UART RX buffer
struct GSL_UARTRX_InfoStruct {
  // UART instance
  USART_TypeDef * USARTx;
  // active
  bool active;
  // buffer location
  // (volatile since it is written to asynchronously by the DMA)
  volatile uint8_t * buffer;
  // end of buffer location
  volatile uint8_t * buffer_end;
  // buffer capacity
  uint16_t capacity;
  // location of next byte to read
  volatile uint8_t * next_data;
  // pointer to dma rx
  DMA_HandleTypeDef * hdmarx;
  // error codes
  GSL_UARTRX_ErrorCodeEnum last_error;
  // number of bytes processed
  uint32_t processed_bytes;
  // number of received bytes
  // (updated only during certain calls)
  uint32_t received_bytes;
};

// **************
// *** USART1 ***
// **************

#ifdef USART1

#ifndef GSL_USART1RX_BUFFER_SIZE
#define GSL_USART1RX_BUFFER_SIZE GSL_UARTRX_BUFFER_SIZE_DEFAULT
#endif

// buffer info
GSL_UARTRX_InfoStruct gsl_usart1rx_info = {
    USART1,
    false,
    nullptr,
    nullptr,
    GSL_USART1RX_BUFFER_SIZE,
    nullptr,
    nullptr,
    kGslUARTRXErrorNone,
    0,
    0};

#endif // #ifdef USART1

// **************
// *** USART2 ***
// **************

#ifdef USART2

#ifndef GSL_USART2RX_BUFFER_SIZE
#define GSL_USART2RX_BUFFER_SIZE GSL_UARTRX_BUFFER_SIZE_DEFAULT
#endif

// buffer info
GSL_UARTRX_InfoStruct gsl_usart2rx_info = {
    USART2,
    false,
    nullptr,
    nullptr,
    GSL_USART2RX_BUFFER_SIZE,
    nullptr,
    nullptr,
    kGslUARTRXErrorNone,
    0,
    0};

#endif // #ifdef USART2

// **************
// *** USART3 ***
// **************

#ifdef USART3

#ifndef GSL_USART3RX_BUFFER_SIZE
#define GSL_USART3RX_BUFFER_SIZE GSL_UARTRX_BUFFER_SIZE_DEFAULT
#endif

// buffer info
GSL_UARTRX_InfoStruct gsl_usart3rx_info = {
    USART3,
    false,
    nullptr,
    nullptr,
    GSL_USART3RX_BUFFER_SIZE,
    nullptr,
    nullptr,
    kGslUARTRXErrorNone,
    0,
    0};

#endif // #ifdef USART3

// *************
// *** UART4 ***
// *************

#ifdef UART4

#ifndef GSL_UART4RX_BUFFER_SIZE
#define GSL_UART4RX_BUFFER_SIZE GSL_UARTRX_BUFFER_SIZE_DEFAULT
#endif

// buffer info
GSL_UARTRX_InfoStruct gsl_uart4rx_info = {
    UART4,
    false,
    nullptr,
    nullptr,
    GSL_UART4RX_BUFFER_SIZE,
    nullptr,
    nullptr,
    kGslUARTRXErrorNone,
    0,
    0};

#endif // #ifdef UART4

// *************
// *** UART5 ***
// *************

#ifdef UART5

#ifndef GSL_UART5RX_BUFFER_SIZE
#define GSL_UART5RX_BUFFER_SIZE GSL_UARTRX_BUFFER_SIZE_DEFAULT
#endif

// buffer info
GSL_UARTRX_InfoStruct gsl_uart5rx_info = {
    UART5,
    false,
    nullptr,
    nullptr,
    GSL_UART5RX_BUFFER_SIZE,
    nullptr,
    nullptr,
    kGslUARTRXErrorNone,
    0,
    0};

#endif // #ifdef UART5

// **************
// *** USART6 ***
// **************

#ifdef USART6

#ifndef GSL_USART6RX_BUFFER_SIZE
#define GSL_USART6RX_BUFFER_SIZE GSL_UARTRX_BUFFER_SIZE_DEFAULT
#endif

// buffer info
GSL_UARTRX_InfoStruct gsl_usart6rx_info = {
    USART6,
    false,
    nullptr,
    nullptr,
    GSL_USART6RX_BUFFER_SIZE,
    nullptr,
    nullptr,
    kGslUARTRXErrorNone,
    0,
    0};

#endif // #ifdef USART6

// *************
// *** UART7 ***
// *************

#ifdef UART7

#ifndef GSL_UART7RX_BUFFER_SIZE
#define GSL_UART7RX_BUFFER_SIZE GSL_UARTRX_BUFFER_SIZE_DEFAULT
#endif

// buffer info
GSL_UARTRX_InfoStruct gsl_uart7rx_info = {
    UART7,
    false,
    nullptr,
    nullptr,
    GSL_UART7RX_BUFFER_SIZE,
    nullptr,
    nullptr,
    kGslUARTRXErrorNone,
    0,
    0};

#endif // #ifdef UART7

// *************
// *** UART8 ***
// *************

#ifdef UART8

#ifndef GSL_UART8RX_BUFFER_SIZE
#define GSL_UART8RX_BUFFER_SIZE GSL_UARTRX_BUFFER_SIZE_DEFAULT
#endif

// buffer info
GSL_UARTRX_InfoStruct gsl_uart8rx_info = {
    UART8,
    false,
    nullptr,
    nullptr,
    GSL_UART8RX_BUFFER_SIZE,
    nullptr,
    nullptr,
    kGslUARTRXErrorNone,
    0,
    0};

#endif // #ifdef UART8

// end of UART defines

// pointer to all info
GSL_UARTRX_InfoStruct * GSL_UARTRX_Info[] = {
#ifdef USART1
    &gsl_usart1rx_info,
#endif
#ifdef USART2
    &gsl_usart2rx_info,
#endif
#ifdef USART3
    &gsl_usart3rx_info,
#endif
#ifdef UART4
    &gsl_uart4rx_info,
#endif
#ifdef UART5
    &gsl_uart5rx_info,
#endif
#ifdef USART6
    &gsl_usart6rx_info,
#endif
#ifdef UART7
    &gsl_uart7rx_info,
#endif
#ifdef UART8
    &gsl_uart8rx_info,
#endif
    };

// number of peripherals
const uint16_t GSL_UARTRX_PeripheralCount =
    sizeof(GSL_UARTRX_Info) / sizeof(*GSL_UARTRX_Info);
