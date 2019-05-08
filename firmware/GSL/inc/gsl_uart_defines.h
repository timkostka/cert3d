#pragma once

// This provides a interface to configure and use the U(S)ART peripherals in
// asynchronous mode.

#include "gsl_includes.h"

// UART pins
// (set to kPinInvalid if not used)
struct GSL_UART_PinsStruct {
  PinEnum tx;
  PinEnum rx;
};

// max timeout in milliseconds used within the HAL SPI functions
#ifndef GSL_UART_TIMEOUT
#define GSL_UART_TIMEOUT 100
#endif

// defaults

// default mode
#ifndef GSL_UART_MODE_DEFAULT
#define GSL_UART_MODE_DEFAULT USART_MODE_TX_RX
#endif

// default baud rate
#ifndef GSL_UART_BAUDRATE_DEFAULT
#define GSL_UART_BAUDRATE_DEFAULT 115200
#endif

// default word length (including parity bit)
#ifndef GSL_UART_WORDLENGTH_DEFAULT
#define GSL_UART_WORDLENGTH_DEFAULT USART_WORDLENGTH_8B
#endif

// default parity type
#ifndef GSL_UART_PARITY_DEFAULT
#define GSL_UART_PARITY_DEFAULT USART_PARITY_NONE
#endif

// default number of stop bits
#ifndef GSL_UART_STOPBITS_DEFAULT
#define GSL_UART_STOPBITS_DEFAULT USART_STOPBITS_1
#endif

// default oversampling
#ifndef GSL_UART_OVERSAMPLING_DEFAULT
#define GSL_UART_OVERSAMPLING_DEFAULT UART_OVERSAMPLING_16
#endif

// default IRQ priority
#ifndef GSL_UART_IRQ_PRIORITY_DEFAULT
#define GSL_UART_IRQ_PRIORITY_DEFAULT 13
#endif

// **************
// *** USART1 ***
// **************

// initialize USART1 settings
#ifdef USART1

// uart mode
#ifndef GSL_USART1_MODE
#define GSL_USART1_MODE GSL_UART_MODE_DEFAULT
#endif

// baud rate
#ifndef GSL_USART1_BAUDRATE
#define GSL_USART1_BAUDRATE GSL_UART_BAUDRATE_DEFAULT
#endif

// data bits (including parity)
#ifndef GSL_USART1_WORDLENGTH
#define GSL_USART1_WORDLENGTH GSL_UART_WORDLENGTH_DEFAULT
#endif

// parity type
#ifndef GSL_USART1_PARITY
#define GSL_USART1_PARITY GSL_UART_PARITY_DEFAULT
#endif

// number of stop bits
#ifndef GSL_USART1_STOPBITS
#define GSL_USART1_STOPBITS GSL_UART_STOPBITS_DEFAULT
#endif

// oversampling
#ifndef GSL_USART1_OVERSAMPLING
#define GSL_USART1_OVERSAMPLING GSL_UART_OVERSAMPLING_DEFAULT
#endif

// irq priority
#ifndef GSL_USART1_IRQ_PRIORITY
#define GSL_USART1_IRQ_PRIORITY GSL_UART_IRQ_PRIORITY_DEFAULT
#endif

// tx pin
// USART1 TX can be PA9 or PB6
#ifndef GSL_USART1_PIN_TX
#define GSL_USART1_PIN_TX kPinA9
#endif

// rx pin
// USART1 RX can be PA10 or PB7
#ifndef GSL_USART1_PIN_RX
#define GSL_USART1_PIN_RX kPinA10
#endif

// initialization routine
UART_HandleTypeDef gsl_usart1_huart = {
    USART1,
    {
        GSL_USART1_BAUDRATE,
        GSL_USART1_WORDLENGTH,
        GSL_USART1_STOPBITS,
        GSL_USART1_PARITY,
        GSL_USART1_MODE,
        UART_HWCONTROL_NONE,
        GSL_USART1_OVERSAMPLING},
    nullptr,
    0,
    0,
    nullptr,
    0,
    0,
    nullptr,
    nullptr,
    HAL_UNLOCKED,
    HAL_UART_STATE_RESET,
    HAL_UART_STATE_RESET,
    HAL_UART_ERROR_NONE};

// pins
GSL_UART_PinsStruct gsl_usart1_pins =  {
    GSL_USART1_PIN_TX,
    GSL_USART1_PIN_RX};

// initialize DMA settings

// USART1 TX --> DMA 2 stream 7 channel 4
#define GSL_USART1_DMA_TX_STREAM DMA2_Stream7
#define GSL_USART1_DMA_TX_CHANNEL DMA_CHANNEL_4

// USART1 RX --> DMA 2 stream 2 or 5 channel 4
#ifndef GSL_USART1_DMA_RX_STREAM
#define GSL_USART1_DMA_RX_STREAM DMA2_Stream2
#endif
#define GSL_USART1_DMA_RX_CHANNEL DMA_CHANNEL_4

// setup for the USART1 DMA TX stream
DMA_HandleTypeDef gsl_usart1_hdmatx = {
    GSL_USART1_DMA_TX_STREAM,
    {
        GSL_USART1_DMA_TX_CHANNEL,
        DMA_MEMORY_TO_PERIPH,
        DMA_PINC_DISABLE,
        DMA_MINC_ENABLE,
        DMA_PDATAALIGN_BYTE,
        DMA_MDATAALIGN_BYTE,
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

// setup for the USART1 DMA RX stream
DMA_HandleTypeDef gsl_usart1_hdmarx = {
    GSL_USART1_DMA_RX_STREAM,
    {
        GSL_USART1_DMA_RX_CHANNEL,
        DMA_PERIPH_TO_MEMORY,
        DMA_PINC_DISABLE,
        DMA_MINC_ENABLE,
        DMA_PDATAALIGN_BYTE,
        DMA_MDATAALIGN_BYTE,
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

#endif // #ifdef USART1

// **************
// *** USART2 ***
// **************

// initialize USART2 settings
#ifdef USART2

// uart mode
#ifndef GSL_USART2_MODE
#define GSL_USART2_MODE GSL_UART_MODE_DEFAULT
#endif

// baud rate
#ifndef GSL_USART2_BAUDRATE
#define GSL_USART2_BAUDRATE GSL_UART_BAUDRATE_DEFAULT
#endif

// data bits (including parity)
#ifndef GSL_USART2_WORDLENGTH
#define GSL_USART2_WORDLENGTH GSL_UART_WORDLENGTH_DEFAULT
#endif

// parity type
#ifndef GSL_USART2_PARITY
#define GSL_USART2_PARITY GSL_UART_PARITY_DEFAULT
#endif

// number of stop bits
#ifndef GSL_USART2_STOPBITS
#define GSL_USART2_STOPBITS GSL_UART_STOPBITS_DEFAULT
#endif

// oversampling
#ifndef GSL_USART2_OVERSAMPLING
#define GSL_USART2_OVERSAMPLING GSL_UART_OVERSAMPLING_DEFAULT
#endif

// irq priority
#ifndef GSL_USART2_IRQ_PRIORITY
#define GSL_USART2_IRQ_PRIORITY GSL_UART_IRQ_PRIORITY_DEFAULT
#endif

// tx pin
// USART2 TX can be PA2, PD5
#ifndef GSL_USART2_PIN_TX
#define GSL_USART2_PIN_TX kPinA2
#endif

// rx pin
// USART2 RX can be PA3, PD6
#ifndef GSL_USART2_PIN_RX
#define GSL_USART2_PIN_RX kPinA3
#endif

// initialization routine
UART_HandleTypeDef gsl_usart2_huart = {
    USART2,
    {
        GSL_USART2_BAUDRATE,
        GSL_USART2_WORDLENGTH,
        GSL_USART2_STOPBITS,
        GSL_USART2_PARITY,
        GSL_USART2_MODE,
        UART_HWCONTROL_NONE,
        GSL_USART2_OVERSAMPLING},
    nullptr,
    0,
    0,
    nullptr,
    0,
    0,
    nullptr,
    nullptr,
    HAL_UNLOCKED,
    HAL_UART_STATE_RESET,
    HAL_UART_STATE_RESET,
    HAL_UART_ERROR_NONE};

// pins
GSL_UART_PinsStruct gsl_usart2_pins =  {
    GSL_USART2_PIN_TX,
    GSL_USART2_PIN_RX};

// initialize DMA settings

// USART2 TX --> DMA 1 stream 6 channel 4
#define GSL_USART2_DMA_TX_STREAM DMA1_Stream6
#define GSL_USART2_DMA_TX_CHANNEL DMA_CHANNEL_4

// USART2 RX --> DMA 1 stream 5 channel 4
#define GSL_USART2_DMA_RX_STREAM DMA1_Stream5
#define GSL_USART2_DMA_RX_CHANNEL DMA_CHANNEL_4

// setup for the USART2 DMA TX stream
DMA_HandleTypeDef gsl_usart2_hdmatx = {
    GSL_USART2_DMA_TX_STREAM,
    {
        GSL_USART2_DMA_TX_CHANNEL,
        DMA_MEMORY_TO_PERIPH,
        DMA_PINC_DISABLE,
        DMA_MINC_ENABLE,
        DMA_PDATAALIGN_BYTE,
        DMA_MDATAALIGN_BYTE,
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

// setup for the USART2 DMA RX stream
DMA_HandleTypeDef gsl_usart2_hdmarx = {
    GSL_USART2_DMA_RX_STREAM,
    {
        GSL_USART2_DMA_RX_CHANNEL,
        DMA_PERIPH_TO_MEMORY,
        DMA_PINC_DISABLE,
        DMA_MINC_ENABLE,
        DMA_PDATAALIGN_BYTE,
        DMA_MDATAALIGN_BYTE,
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

#endif // #ifdef USART2

// **************
// *** USART3 ***
// **************

// initialize USART3 settings
#ifdef USART3

// uart mode
#ifndef GSL_USART3_MODE
#define GSL_USART3_MODE GSL_UART_MODE_DEFAULT
#endif

// baud rate
#ifndef GSL_USART3_BAUDRATE
#define GSL_USART3_BAUDRATE GSL_UART_BAUDRATE_DEFAULT
#endif

// data bits (including parity)
#ifndef GSL_USART3_WORDLENGTH
#define GSL_USART3_WORDLENGTH GSL_UART_WORDLENGTH_DEFAULT
#endif

// parity type
#ifndef GSL_USART3_PARITY
#define GSL_USART3_PARITY GSL_UART_PARITY_DEFAULT
#endif

// number of stop bits
#ifndef GSL_USART3_STOPBITS
#define GSL_USART3_STOPBITS GSL_UART_STOPBITS_DEFAULT
#endif

// oversampling
#ifndef GSL_USART3_OVERSAMPLING
#define GSL_USART3_OVERSAMPLING GSL_UART_OVERSAMPLING_DEFAULT
#endif

// irq priority
#ifndef GSL_USART3_IRQ_PRIORITY
#define GSL_USART3_IRQ_PRIORITY GSL_UART_IRQ_PRIORITY_DEFAULT
#endif

// tx pin
// USART3 TX can be PB10, PC10, PD8
#ifndef GSL_USART3_PIN_TX
#define GSL_USART3_PIN_TX kPinB10
#endif

// rx pin
// USART3 TX can be PB11, PC11, PD9
#ifndef GSL_USART3_PIN_RX
#define GSL_USART3_PIN_RX kPinB11
#endif

// initialization routine
UART_HandleTypeDef gsl_usart3_huart = {
    USART3,
    {
        GSL_USART3_BAUDRATE,
        GSL_USART3_WORDLENGTH,
        GSL_USART3_STOPBITS,
        GSL_USART3_PARITY,
        GSL_USART3_MODE,
        UART_HWCONTROL_NONE,
        GSL_USART3_OVERSAMPLING},
    nullptr,
    0,
    0,
    nullptr,
    0,
    0,
    nullptr,
    nullptr,
    HAL_UNLOCKED,
    HAL_UART_STATE_RESET,
    HAL_UART_STATE_RESET,
    HAL_UART_ERROR_NONE};

// pins
GSL_UART_PinsStruct gsl_usart3_pins =  {
    GSL_USART3_PIN_TX,
    GSL_USART3_PIN_RX};

// initialize DMA settings

// USART3 TX --> DMA 1 (stream 3 channel 4) or (stream 4 channel 7)
#ifndef GSL_USART3_DMA_TX_STREAM
#define GSL_USART3_DMA_TX_STREAM DMA1_Stream3
#define GSL_USART3_DMA_TX_CHANNEL DMA_CHANNEL_4
#endif

// USART3 RX --> DMA 1 stream 1 channel 4
#define GSL_USART3_DMA_RX_STREAM DMA1_Stream1
#define GSL_USART3_DMA_RX_CHANNEL DMA_CHANNEL_4

// setup for the USART3 DMA TX stream
DMA_HandleTypeDef gsl_usart3_hdmatx = {
    GSL_USART3_DMA_TX_STREAM,
    {
        GSL_USART3_DMA_TX_CHANNEL,
        DMA_MEMORY_TO_PERIPH,
        DMA_PINC_DISABLE,
        DMA_MINC_ENABLE,
        DMA_PDATAALIGN_BYTE,
        DMA_MDATAALIGN_BYTE,
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

// setup for the USART3 DMA RX stream
DMA_HandleTypeDef gsl_usart3_hdmarx = {
    GSL_USART3_DMA_RX_STREAM,
    {
        GSL_USART3_DMA_RX_CHANNEL,
        DMA_PERIPH_TO_MEMORY,
        DMA_PINC_DISABLE,
        DMA_MINC_ENABLE,
        DMA_PDATAALIGN_BYTE,
        DMA_MDATAALIGN_BYTE,
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

#endif // #ifdef USART3

// **************
// *** UART4 ***
// **************

// initialize UART4 settings
#ifdef UART4

// uart mode
#ifndef GSL_UART4_MODE
#define GSL_UART4_MODE GSL_UART_MODE_DEFAULT
#endif

// baud rate
#ifndef GSL_UART4_BAUDRATE
#define GSL_UART4_BAUDRATE GSL_UART_BAUDRATE_DEFAULT
#endif

// data bits (including parity)
#ifndef GSL_UART4_WORDLENGTH
#define GSL_UART4_WORDLENGTH GSL_UART_WORDLENGTH_DEFAULT
#endif

// parity type
#ifndef GSL_UART4_PARITY
#define GSL_UART4_PARITY GSL_UART_PARITY_DEFAULT
#endif

// number of stop bits
#ifndef GSL_UART4_STOPBITS
#define GSL_UART4_STOPBITS GSL_UART_STOPBITS_DEFAULT
#endif

// oversampling
#ifndef GSL_UART4_OVERSAMPLING
#define GSL_UART4_OVERSAMPLING GSL_UART_OVERSAMPLING_DEFAULT
#endif

// irq priority
#ifndef GSL_UART4_IRQ_PRIORITY
#define GSL_UART4_IRQ_PRIORITY GSL_UART_IRQ_PRIORITY_DEFAULT
#endif

// tx pin
// UART4 TX can be PA0, PC10
#ifndef GSL_UART4_PIN_TX
#define GSL_UART4_PIN_TX kPinA0
#endif

// rx pin
// UART4 RX can be PA1, PC11
#ifndef GSL_UART4_PIN_RX
#define GSL_UART4_PIN_RX kPinA1
#endif

// initialization routine
UART_HandleTypeDef gsl_uart4_huart = {
    UART4,
    {
        GSL_UART4_BAUDRATE,
        GSL_UART4_WORDLENGTH,
        GSL_UART4_STOPBITS,
        GSL_UART4_PARITY,
        GSL_UART4_MODE,
        UART_HWCONTROL_NONE,
        GSL_UART4_OVERSAMPLING},
    nullptr,
    0,
    0,
    nullptr,
    0,
    0,
    nullptr,
    nullptr,
    HAL_UNLOCKED,
    HAL_UART_STATE_RESET,
    HAL_UART_STATE_RESET,
    HAL_UART_ERROR_NONE};

// pins
GSL_UART_PinsStruct gsl_uart4_pins =  {
    GSL_UART4_PIN_TX,
    GSL_UART4_PIN_RX};

// initialize DMA settings

// UART4 TX --> DMA 1 stream 4 channel 4
#define GSL_UART4_DMA_TX_STREAM DMA1_Stream4
#define GSL_UART4_DMA_TX_CHANNEL DMA_CHANNEL_4

// UART4 RX --> DMA 1 stream 2 channel 4
#define GSL_UART4_DMA_RX_STREAM DMA1_Stream2
#define GSL_UART4_DMA_RX_CHANNEL DMA_CHANNEL_4

// setup for the UART4 DMA TX stream
DMA_HandleTypeDef gsl_uart4_hdmatx = {
    GSL_UART4_DMA_TX_STREAM,
    {
        GSL_UART4_DMA_TX_CHANNEL,
        DMA_MEMORY_TO_PERIPH,
        DMA_PINC_DISABLE,
        DMA_MINC_ENABLE,
        DMA_PDATAALIGN_BYTE,
        DMA_MDATAALIGN_BYTE,
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

// setup for the UART4 DMA RX stream
DMA_HandleTypeDef gsl_uart4_hdmarx = {
    GSL_UART4_DMA_RX_STREAM,
    {
        GSL_UART4_DMA_RX_CHANNEL,
        DMA_PERIPH_TO_MEMORY,
        DMA_PINC_DISABLE,
        DMA_MINC_ENABLE,
        DMA_PDATAALIGN_BYTE,
        DMA_MDATAALIGN_BYTE,
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

#endif // #ifdef UART4

// **************
// *** UART5 ***
// **************

// initialize UART5 settings
#ifdef UART5

// uart mode
#ifndef GSL_UART5_MODE
#define GSL_UART5_MODE GSL_UART_MODE_DEFAULT
#endif

// baud rate
#ifndef GSL_UART5_BAUDRATE
#define GSL_UART5_BAUDRATE GSL_UART_BAUDRATE_DEFAULT
#endif

// data bits (including parity)
#ifndef GSL_UART5_WORDLENGTH
#define GSL_UART5_WORDLENGTH GSL_UART_WORDLENGTH_DEFAULT
#endif

// parity type
#ifndef GSL_UART5_PARITY
#define GSL_UART5_PARITY GSL_UART_PARITY_DEFAULT
#endif

// number of stop bits
#ifndef GSL_UART5_STOPBITS
#define GSL_UART5_STOPBITS GSL_UART_STOPBITS_DEFAULT
#endif

// oversampling
#ifndef GSL_UART5_OVERSAMPLING
#define GSL_UART5_OVERSAMPLING GSL_UART_OVERSAMPLING_DEFAULT
#endif

// irq priority
#ifndef GSL_UART5_IRQ_PRIORITY
#define GSL_UART5_IRQ_PRIORITY GSL_UART_IRQ_PRIORITY_DEFAULT
#endif

// tx pin
// UART5 TX can be PC12
#define GSL_UART5_PIN_TX kPinC12

// rx pin
// UART5 TX can be PD2
#define GSL_UART5_PIN_RX kPinD2

// initialization routine
UART_HandleTypeDef gsl_uart5_huart = {
    UART5,
    {
        GSL_UART5_BAUDRATE,
        GSL_UART5_WORDLENGTH,
        GSL_UART5_STOPBITS,
        GSL_UART5_PARITY,
        GSL_UART5_MODE,
        UART_HWCONTROL_NONE,
        GSL_UART5_OVERSAMPLING},
    nullptr,
    0,
    0,
    nullptr,
    0,
    0,
    nullptr,
    nullptr,
    HAL_UNLOCKED,
    HAL_UART_STATE_RESET,
    HAL_UART_STATE_RESET,
    HAL_UART_ERROR_NONE};

// pins
GSL_UART_PinsStruct gsl_uart5_pins =  {
    GSL_UART5_PIN_TX,
    GSL_UART5_PIN_RX};

// initialize DMA settings

// UART5 TX --> DMA 1 stream 7 channel 4
#define GSL_UART5_DMA_TX_STREAM DMA1_Stream7
#define GSL_UART5_DMA_TX_CHANNEL DMA_CHANNEL_4

// UART5 RX --> DMA 1 stream 0 channel 4
#define GSL_UART5_DMA_RX_STREAM DMA1_Stream0
#define GSL_UART5_DMA_RX_CHANNEL DMA_CHANNEL_4

// setup for the UART5 DMA TX stream
DMA_HandleTypeDef gsl_uart5_hdmatx = {
    GSL_UART5_DMA_TX_STREAM,
    {
        GSL_UART5_DMA_TX_CHANNEL,
        DMA_MEMORY_TO_PERIPH,
        DMA_PINC_DISABLE,
        DMA_MINC_ENABLE,
        DMA_PDATAALIGN_BYTE,
        DMA_MDATAALIGN_BYTE,
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

// setup for the UART5 DMA RX stream
DMA_HandleTypeDef gsl_uart5_hdmarx = {
    GSL_UART5_DMA_RX_STREAM,
    {
        GSL_UART5_DMA_RX_CHANNEL,
        DMA_PERIPH_TO_MEMORY,
        DMA_PINC_DISABLE,
        DMA_MINC_ENABLE,
        DMA_PDATAALIGN_BYTE,
        DMA_MDATAALIGN_BYTE,
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

#endif // #ifdef UART5

// **************
// *** USART6 ***
// **************

// initialize USART6 settings
#ifdef USART6

// uart mode
#ifndef GSL_USART6_MODE
#define GSL_USART6_MODE GSL_UART_MODE_DEFAULT
#endif

// baud rate
#ifndef GSL_USART6_BAUDRATE
#define GSL_USART6_BAUDRATE GSL_UART_BAUDRATE_DEFAULT
#endif

// data bits (including parity)
#ifndef GSL_USART6_WORDLENGTH
#define GSL_USART6_WORDLENGTH GSL_UART_WORDLENGTH_DEFAULT
#endif

// parity type
#ifndef GSL_USART6_PARITY
#define GSL_USART6_PARITY GSL_UART_PARITY_DEFAULT
#endif

// number of stop bits
#ifndef GSL_USART6_STOPBITS
#define GSL_USART6_STOPBITS GSL_UART_STOPBITS_DEFAULT
#endif

// oversampling
#ifndef GSL_USART6_OVERSAMPLING
#define GSL_USART6_OVERSAMPLING GSL_UART_OVERSAMPLING_DEFAULT
#endif

// irq priority
#ifndef GSL_USART6_IRQ_PRIORITY
#define GSL_USART6_IRQ_PRIORITY GSL_UART_IRQ_PRIORITY_DEFAULT
#endif

// tx pin
// USART6 TX can be PC6, PG14
#ifndef GSL_USART6_PIN_TX
#define GSL_USART6_PIN_TX kPinC6
#endif

// rx pin
// USART6 RX can be PC7, PG9
#ifndef GSL_USART6_PIN_RX
#define GSL_USART6_PIN_RX kPinC7
#endif

// initialization routine
UART_HandleTypeDef gsl_usart6_huart = {
    USART6,
    {
        GSL_USART6_BAUDRATE,
        GSL_USART6_WORDLENGTH,
        GSL_USART6_STOPBITS,
        GSL_USART6_PARITY,
        GSL_USART6_MODE,
        UART_HWCONTROL_NONE,
        GSL_USART6_OVERSAMPLING},
    nullptr,
    0,
    0,
    nullptr,
    0,
    0,
    nullptr,
    nullptr,
    HAL_UNLOCKED,
    HAL_UART_STATE_RESET,
    HAL_UART_STATE_RESET,
    HAL_UART_ERROR_NONE};

// pins
GSL_UART_PinsStruct gsl_usart6_pins =  {
    GSL_USART6_PIN_TX,
    GSL_USART6_PIN_RX};

// initialize DMA settings

// USART6 TX --> DMA 2 stream 6 or 7 channel 5
#ifndef GSL_USART6_DMA_TX_STREAM
#define GSL_USART6_DMA_TX_STREAM DMA2_Stream6
#endif
#define GSL_USART6_DMA_TX_CHANNEL DMA_CHANNEL_5

// USART6 RX --> DMA 2 stream 1 or 2 channel 5
#ifndef GSL_USART6_DMA_RX_STREAM
#define GSL_USART6_DMA_RX_STREAM DMA2_Stream1
#endif
#define GSL_USART6_DMA_RX_CHANNEL DMA_CHANNEL_5

// setup for the USART6 DMA TX stream
DMA_HandleTypeDef gsl_usart6_hdmatx = {
    GSL_USART6_DMA_TX_STREAM,
    {
        GSL_USART6_DMA_TX_CHANNEL,
        DMA_MEMORY_TO_PERIPH,
        DMA_PINC_DISABLE,
        DMA_MINC_ENABLE,
        DMA_PDATAALIGN_BYTE,
        DMA_MDATAALIGN_BYTE,
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

// setup for the USART6 DMA RX stream
DMA_HandleTypeDef gsl_usart6_hdmarx = {
    GSL_USART6_DMA_RX_STREAM,
    {
        GSL_USART6_DMA_RX_CHANNEL,
        DMA_PERIPH_TO_MEMORY,
        DMA_PINC_DISABLE,
        DMA_MINC_ENABLE,
        DMA_PDATAALIGN_BYTE,
        DMA_MDATAALIGN_BYTE,
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

#endif // #ifdef USART6

// *************
// *** UART7 ***
// *************

// initialize UART7 settings
#ifdef UART7

// uart mode
#ifndef GSL_UART7_MODE
#define GSL_UART7_MODE GSL_UART_MODE_DEFAULT
#endif

// baud rate
#ifndef GSL_UART7_BAUDRATE
#define GSL_UART7_BAUDRATE GSL_UART_BAUDRATE_DEFAULT
#endif

// data bits (including parity)
#ifndef GSL_UART7_WORDLENGTH
#define GSL_UART7_WORDLENGTH GSL_UART_WORDLENGTH_DEFAULT
#endif

// parity type
#ifndef GSL_UART7_PARITY
#define GSL_UART7_PARITY GSL_UART_PARITY_DEFAULT
#endif

// number of stop bits
#ifndef GSL_UART7_STOPBITS
#define GSL_UART7_STOPBITS GSL_UART_STOPBITS_DEFAULT
#endif

// oversampling
#ifndef GSL_UART7_OVERSAMPLING
#define GSL_UART7_OVERSAMPLING GSL_UART_OVERSAMPLING_DEFAULT
#endif

// irq priority
#ifndef GSL_UART7_IRQ_PRIORITY
#define GSL_UART7_IRQ_PRIORITY GSL_UART_IRQ_PRIORITY_DEFAULT
#endif

// tx pin
// UART7 TX can be PE8, PF7
#ifndef GSL_UART7_PIN_TX
#define GSL_UART7_PIN_TX kPinE8
#endif

// rx pin
// UART7 RX can be PE7, PF6
#ifndef GSL_UART7_PIN_RX
#define GSL_UART7_PIN_RX kPinE7
#endif

// initialization routine
UART_HandleTypeDef gsl_uart7_huart = {
    UART7,
    {
        GSL_UART7_BAUDRATE,
        GSL_UART7_WORDLENGTH,
        GSL_UART7_STOPBITS,
        GSL_UART7_PARITY,
        GSL_UART7_MODE,
        UART_HWCONTROL_NONE,
        GSL_UART7_OVERSAMPLING},
    nullptr,
    0,
    0,
    nullptr,
    0,
    0,
    nullptr,
    nullptr,
    HAL_UNLOCKED,
    HAL_UART_STATE_RESET,
    HAL_UART_STATE_RESET,
    HAL_UART_ERROR_NONE};

// pins
GSL_UART_PinsStruct gsl_uart7_pins =  {
    GSL_UART7_PIN_TX,
    GSL_UART7_PIN_RX};

// initialize DMA settings

// UART7 TX --> DMA 1 stream 1 channel 5
#define GSL_UART7_DMA_TX_STREAM DMA1_Stream1
#define GSL_UART7_DMA_TX_CHANNEL DMA_CHANNEL_5

// UART7 RX --> DMA 1 stream 3 channel 5
#define GSL_UART7_DMA_RX_STREAM DMA1_Stream3
#define GSL_UART7_DMA_RX_CHANNEL DMA_CHANNEL_5

// setup for the UART7 DMA TX stream
DMA_HandleTypeDef gsl_uart7_hdmatx = {
    GSL_UART7_DMA_TX_STREAM,
    {
        GSL_UART7_DMA_TX_CHANNEL,
        DMA_MEMORY_TO_PERIPH,
        DMA_PINC_DISABLE,
        DMA_MINC_ENABLE,
        DMA_PDATAALIGN_BYTE,
        DMA_MDATAALIGN_BYTE,
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

// setup for the UART7 DMA RX stream
DMA_HandleTypeDef gsl_uart7_hdmarx = {
    GSL_UART7_DMA_RX_STREAM,
    {
        GSL_UART7_DMA_RX_CHANNEL,
        DMA_PERIPH_TO_MEMORY,
        DMA_PINC_DISABLE,
        DMA_MINC_ENABLE,
        DMA_PDATAALIGN_BYTE,
        DMA_MDATAALIGN_BYTE,
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

#endif // #ifdef UART7

// *************
// *** UART8 ***
// *************

// initialize UART8 settings
#ifdef UART8

// uart mode
#ifndef GSL_UART8_MODE
#define GSL_UART8_MODE GSL_UART_MODE_DEFAULT
#endif

// baud rate
#ifndef GSL_UART8_BAUDRATE
#define GSL_UART8_BAUDRATE GSL_UART_BAUDRATE_DEFAULT
#endif

// data bits (including parity)
#ifndef GSL_UART8_WORDLENGTH
#define GSL_UART8_WORDLENGTH GSL_UART_WORDLENGTH_DEFAULT
#endif

// parity type
#ifndef GSL_UART8_PARITY
#define GSL_UART8_PARITY GSL_UART_PARITY_DEFAULT
#endif

// number of stop bits
#ifndef GSL_UART8_STOPBITS
#define GSL_UART8_STOPBITS GSL_UART_STOPBITS_DEFAULT
#endif

// oversampling
#ifndef GSL_UART8_OVERSAMPLING
#define GSL_UART8_OVERSAMPLING GSL_UART_OVERSAMPLING_DEFAULT
#endif

// irq priority
#ifndef GSL_UART8_IRQ_PRIORITY
#define GSL_UART8_IRQ_PRIORITY GSL_UART_IRQ_PRIORITY_DEFAULT
#endif

// tx pin
// UART8 TX can be PE1
#define GSL_UART8_PIN_TX kPinE1

// rx pin
// UART8 TX can be PE0
#define GSL_UART8_PIN_RX kPinE0

// initialization routine
UART_HandleTypeDef gsl_uart8_huart = {
    UART8,
    {
        GSL_UART8_BAUDRATE,
        GSL_UART8_WORDLENGTH,
        GSL_UART8_STOPBITS,
        GSL_UART8_PARITY,
        GSL_UART8_MODE,
        UART_HWCONTROL_NONE,
        GSL_UART8_OVERSAMPLING},
    nullptr,
    0,
    0,
    nullptr,
    0,
    0,
    nullptr,
    nullptr,
    HAL_UNLOCKED,
    HAL_UART_STATE_RESET,
    HAL_UART_STATE_RESET,
    HAL_UART_ERROR_NONE};

// pins
GSL_UART_PinsStruct gsl_uart8_pins =  {
    GSL_UART8_PIN_TX,
    GSL_UART8_PIN_RX};

// initialize DMA settings

// UART8 TX --> DMA 1 stream 0 channel 5
#define GSL_UART8_DMA_TX_STREAM DMA1_Stream0
#define GSL_UART8_DMA_TX_CHANNEL DMA_CHANNEL_5

// UART8 RX --> DMA 1 stream 6 channel 5
#define GSL_UART8_DMA_RX_STREAM DMA1_Stream6
#define GSL_UART8_DMA_RX_CHANNEL DMA_CHANNEL_5

// setup for the UART8 DMA TX stream
DMA_HandleTypeDef gsl_uart8_hdmatx = {
    GSL_UART8_DMA_TX_STREAM,
    {
        GSL_UART8_DMA_TX_CHANNEL,
        DMA_MEMORY_TO_PERIPH,
        DMA_PINC_DISABLE,
        DMA_MINC_ENABLE,
        DMA_PDATAALIGN_BYTE,
        DMA_MDATAALIGN_BYTE,
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

// setup for the UART8 DMA RX stream
DMA_HandleTypeDef gsl_uart8_hdmarx = {
    GSL_UART8_DMA_RX_STREAM,
    {
        GSL_UART8_DMA_RX_CHANNEL,
        DMA_PERIPH_TO_MEMORY,
        DMA_PINC_DISABLE,
        DMA_MINC_ENABLE,
        DMA_PDATAALIGN_BYTE,
        DMA_MDATAALIGN_BYTE,
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

#endif // #ifdef UART8

// this holds pointers to the various structures we need
struct GSL_UART_InfoStruct {
  USART_TypeDef * USARTx;
  UART_HandleTypeDef* handle;
  GSL_UART_PinsStruct* pins;
  uint8_t af_mode;
  uint8_t irq_priority;
  DMA_HandleTypeDef* hdmatx;
  DMA_HandleTypeDef* hdmarx;
  void (*SendCompleteCallback)(USART_TypeDef * USARTx);
  void (*SendHalfCompleteCallback)(USART_TypeDef * USARTx);
  void (*ReadCompleteCallback)(USART_TypeDef * USARTx);
  void (*ReadHalfCompleteCallback)(USART_TypeDef * USARTx);
};

// here we define the info structure which points to all the various other
// structures
GSL_UART_InfoStruct GSL_UART_Info[] = {

#ifdef USART1
    {
      USART1,
      &gsl_usart1_huart,
      &gsl_usart1_pins,
      GPIO_AF7_USART1,
      GSL_USART1_IRQ_PRIORITY,
      &gsl_usart1_hdmatx,
      &gsl_usart1_hdmarx,
      nullptr,
      nullptr,
      nullptr,
      nullptr,
    },
#endif // #ifdef USART1

#ifdef USART2
    {
      USART2,
      &gsl_usart2_huart,
      &gsl_usart2_pins,
      GPIO_AF7_USART2,
      GSL_USART2_IRQ_PRIORITY,
      &gsl_usart2_hdmatx,
      &gsl_usart2_hdmarx,
      nullptr,
      nullptr,
      nullptr,
      nullptr,
    },
#endif // #ifdef USART2

#ifdef USART3
    {
      USART3,
      &gsl_usart3_huart,
      &gsl_usart3_pins,
      GPIO_AF7_USART3,
      GSL_USART3_IRQ_PRIORITY,
      &gsl_usart3_hdmatx,
      &gsl_usart3_hdmarx,
      nullptr,
      nullptr,
      nullptr,
      nullptr,
    },
#endif // #ifdef USART3

#ifdef UART4
    {
      UART4,
      &gsl_uart4_huart,
      &gsl_uart4_pins,
      GPIO_AF8_UART4,
      GSL_UART4_IRQ_PRIORITY,
      &gsl_uart4_hdmatx,
      &gsl_uart4_hdmarx,
      nullptr,
      nullptr,
      nullptr,
      nullptr,
    },
#endif // #ifdef UART4

#ifdef UART5
    {
      UART5,
      &gsl_uart5_huart,
      &gsl_uart5_pins,
      GPIO_AF8_UART5,
      GSL_UART5_IRQ_PRIORITY,
      &gsl_uart5_hdmatx,
      &gsl_uart5_hdmarx,
      nullptr,
      nullptr,
      nullptr,
      nullptr,
    },
#endif // #ifdef UART5

#ifdef USART6
    {
      USART6,
      &gsl_usart6_huart,
      &gsl_usart6_pins,
      GPIO_AF8_USART6,
      GSL_USART6_IRQ_PRIORITY,
      &gsl_usart6_hdmatx,
      &gsl_usart6_hdmarx,
      nullptr,
      nullptr,
      nullptr,
      nullptr,
    },
#endif // #ifdef USART6

#ifdef UART7
    {
      UART7,
      &gsl_uart7_huart,
      &gsl_uart7_pins,
      GPIO_AF8_UART7,
      GSL_UART7_IRQ_PRIORITY,
      &gsl_uart7_hdmatx,
      &gsl_uart7_hdmarx,
      nullptr,
      nullptr,
      nullptr,
      nullptr,
    },
#endif // #ifdef UART7

#ifdef UART8
    {
      UART8,
      &gsl_uart8_huart,
      &gsl_uart8_pins,
      GPIO_AF8_UART8,
      GSL_UART8_IRQ_PRIORITY,
      &gsl_uart8_hdmatx,
      &gsl_uart8_hdmarx,
      nullptr,
      nullptr,
      nullptr,
      nullptr,
    },
#endif // #ifdef UART8

};

// number of UART peripherals
const uint32_t GSL_UART_PeripheralCount =
    sizeof(GSL_UART_Info) / sizeof(*GSL_UART_Info);

// bytes of RAM used to store all the handler structs
const uint32_t GSL_UART_StorageByteCount =
    sizeof(GSL_UART_Info) +
    GSL_UART_PeripheralCount * sizeof(UART_HandleTypeDef) +
    GSL_UART_PeripheralCount * sizeof(GSL_UART_PinsStruct) +
    2 * GSL_UART_PeripheralCount * sizeof(DMA_HandleTypeDef);
