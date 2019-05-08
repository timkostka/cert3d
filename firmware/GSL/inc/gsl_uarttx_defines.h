#pragma once

// This file implements a DMA-based asynchronous UART sender.

#include "gsl_includes.h"

// default values

// default buffer size
#ifndef GSL_UARTTX_BUFFER_SIZE_DEFAULT
#define GSL_UARTTX_BUFFER_SIZE_DEFAULT 256
#endif

// information for a given UART TX buffer
struct GSL_UARTTX_InfoStruct {
  // UART instance
  USART_TypeDef * USARTx;
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

// **************
// *** USART1 ***
// **************

#ifdef USART1

#ifndef GSL_USART1TX_BUFFER_SIZE
#define GSL_USART1TX_BUFFER_SIZE GSL_UARTTX_BUFFER_SIZE_DEFAULT
#endif

// buffer info
GSL_UARTTX_InfoStruct gsl_usart1tx_info = {
    USART1,
    false,
    GSL_USART1TX_BUFFER_SIZE,
    GSL_CBUF(0),
    nullptr,
    0};

#endif // #ifdef USART1

// **************
// *** USART2 ***
// **************

#ifdef USART2

#ifndef GSL_USART2TX_BUFFER_SIZE
#define GSL_USART2TX_BUFFER_SIZE GSL_UARTTX_BUFFER_SIZE_DEFAULT
#endif

// buffer info
GSL_UARTTX_InfoStruct gsl_usart2tx_info = {
    USART2,
    false,
    GSL_USART2TX_BUFFER_SIZE,
    GSL_CBUF(0),
    nullptr,
    0};

#endif // #ifdef USART2

// **************
// *** USART3 ***
// **************

#ifdef USART3

#ifndef GSL_USART3TX_BUFFER_SIZE
#define GSL_USART3TX_BUFFER_SIZE GSL_UARTTX_BUFFER_SIZE_DEFAULT
#endif

// buffer info
GSL_UARTTX_InfoStruct gsl_usart3tx_info = {
    USART3,
    false,
    GSL_USART3TX_BUFFER_SIZE,
    GSL_CBUF(0),
    nullptr,
    0};

#endif // #ifdef USART3

// *************
// *** UART4 ***
// *************

#ifdef UART4

#ifndef GSL_UART4TX_BUFFER_SIZE
#define GSL_UART4TX_BUFFER_SIZE GSL_UARTTX_BUFFER_SIZE_DEFAULT
#endif

// buffer info
GSL_UARTTX_InfoStruct gsl_uart4tx_info = {
    UART4,
    false,
    GSL_UART4TX_BUFFER_SIZE,
    GSL_CBUF(0),
    nullptr,
    0};

#endif // #ifdef UART4

// *************
// *** UART5 ***
// *************

#ifdef UART5

#ifndef GSL_UART5TX_BUFFER_SIZE
#define GSL_UART5TX_BUFFER_SIZE GSL_UARTTX_BUFFER_SIZE_DEFAULT
#endif

// buffer info
GSL_UARTTX_InfoStruct gsl_uart5tx_info = {
    UART5,
    false,
    GSL_UART5TX_BUFFER_SIZE,
    GSL_CBUF(0),
    nullptr,
    0};

#endif // #ifdef UART5

// **************
// *** USART6 ***
// **************

#ifdef USART6

#ifndef GSL_USART6TX_BUFFER_SIZE
#define GSL_USART6TX_BUFFER_SIZE GSL_UARTTX_BUFFER_SIZE_DEFAULT
#endif

// buffer info
GSL_UARTTX_InfoStruct gsl_usart6tx_info = {
    USART6,
    false,
    GSL_USART6TX_BUFFER_SIZE,
    GSL_CBUF(0),
    nullptr,
    0};

#endif // #ifdef USART6

// *************
// *** UART7 ***
// *************

#ifdef UART7

#ifndef GSL_UART7TX_BUFFER_SIZE
#define GSL_UART7TX_BUFFER_SIZE GSL_UARTTX_BUFFER_SIZE_DEFAULT
#endif

// buffer info
GSL_UARTTX_InfoStruct gsl_uart7tx_info = {
    UART7,
    false,
    GSL_UART7TX_BUFFER_SIZE,
    GSL_CBUF(0),
    nullptr,
    0};

#endif // #ifdef UART7

// *************
// *** UART8 ***
// *************

#ifdef UART8

#ifndef GSL_UART8TX_BUFFER_SIZE
#define GSL_UART8TX_BUFFER_SIZE GSL_UARTTX_BUFFER_SIZE_DEFAULT
#endif

// buffer info
GSL_UARTTX_InfoStruct gsl_uart8tx_info = {
    UART8,
    false,
    GSL_UART8TX_BUFFER_SIZE,
    GSL_CBUF(0),
    nullptr,
    0};

#endif // #ifdef UART8

// end of UART defines

// pointer to all info
GSL_UARTTX_InfoStruct * GSL_UARTTX_Info[] = {
#ifdef USART1
    &gsl_usart1tx_info,
#endif
#ifdef USART2
    &gsl_usart2tx_info,
#endif
#ifdef USART3
    &gsl_usart3tx_info,
#endif
#ifdef UART4
    &gsl_uart4tx_info,
#endif
#ifdef UART5
    &gsl_uart5tx_info,
#endif
#ifdef USART6
    &gsl_usart6tx_info,
#endif
#ifdef UART7
    &gsl_uart7tx_info,
#endif
#ifdef UART8
    &gsl_uart8tx_info,
#endif
    };

// number of peripherals
const uint16_t GSL_UARTTX_PeripheralCount =
    sizeof(GSL_UARTTX_Info) / sizeof(*GSL_UARTTX_Info);
