#pragma once

// This provides a interface to configure and use the I2C peripherals.

#include "gsl_includes.h"

// I2C pins
struct GSL_I2C_PinsStruct {
  PinEnum scl;
  PinEnum sda;
};

// max timeout in milliseconds used within the HAL I2C functions
#ifndef GSL_I2C_TIMEOUT
#define GSL_I2C_TIMEOUT 100
#endif

// defaults

// default clock speed
#ifndef GSL_I2C_CLOCKSPEED_DEFAULT
#define GSL_I2C_CLOCKSPEED_DEFAULT 400000
#endif

// default slave address
#ifndef GSL_I2C_SLAVEADDRESS_DEFAULT
#define GSL_I2C_SLAVEADDRESS_DEFAULT 0x00
#endif

// ************
// *** I2C1 ***
// ************

// initialize I2C1 settings
#ifdef I2C1

// clock speed
#ifndef GSL_I2C1_CLOCKSPEED
#define GSL_I2C1_CLOCKSPEED GSL_I2C_CLOCKSPEED_DEFAULT
#endif

// slave address
#ifndef GSL_I2C1_SLAVEADDRESS
#define GSL_I2C1_SLAVEADDRESS GSL_I2C_SLAVEADDRESS_DEFAULT
#endif

// scl pin
// I2C1 SCL can be PB6, PB8
#ifndef GSL_I2C1_PIN_SCL
#define GSL_I2C1_PIN_SCL kPinB6
#endif

// sda pin
// I2C1 SDA can be PB7, PB9
#ifndef GSL_I2C1_PIN_SDA
#define GSL_I2C1_PIN_SDA kPinB7
#endif

// initialization routine
I2C_HandleTypeDef gsl_i2c1_hi2c = {
    I2C1,
    {
        GSL_I2C1_CLOCKSPEED,
        I2C_DUTYCYCLE_2,
        GSL_I2C1_SLAVEADDRESS,
        I2C_ADDRESSINGMODE_7BIT,
        I2C_DUALADDRESS_DISABLE,
        0,
        I2C_GENERALCALL_DISABLE,
        I2C_NOSTRETCH_DISABLE},
    nullptr,
    0,
    0,
    0,
    0,
    nullptr,
    nullptr,
    HAL_UNLOCKED,
    HAL_I2C_STATE_RESET,
    HAL_I2C_MODE_NONE,
    HAL_I2C_ERROR_NONE,
    0,
    0,
    0,
    0};

// pins
GSL_I2C_PinsStruct gsl_i2c1_pins = {
    GSL_I2C1_PIN_SCL,
    GSL_I2C1_PIN_SDA};

// initialize DMA settings

// I2C1 TX --> DMA 1 stream 6 or 7 channel 1
#ifndef GSL_I2C1_DMA_TX_STREAM
#define GSL_I2C1_DMA_TX_STREAM DMA1_Stream6
#endif
#define GSL_I2C1_DMA_TX_CHANNEL DMA_CHANNEL_1

// I2C1 RX --> DMA 1 stream 0 or 5 channel 1
#ifndef GSL_I2C1_DMA_RX_STREAM
#define GSL_I2C1_DMA_RX_STREAM DMA1_Stream0
#endif
#define GSL_I2C1_DMA_RX_CHANNEL DMA_CHANNEL_1

// setup for the I2C1 DMA TX stream
DMA_HandleTypeDef gsl_i2c1_hdmatx = {
    GSL_I2C1_DMA_TX_STREAM,
    {
        GSL_I2C1_DMA_TX_CHANNEL,
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

// setup for the I2C1 DMA RX stream
DMA_HandleTypeDef gsl_i2c1_hdmarx = {
    GSL_I2C1_DMA_RX_STREAM,
    {
        GSL_I2C1_DMA_RX_CHANNEL,
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

#endif // #ifdef I2C1

// ************
// *** I2C2 ***
// ************

// initialize I2C2 settings
#ifdef I2C2

// clock speed
#ifndef GSL_I2C2_CLOCKSPEED
#define GSL_I2C2_CLOCKSPEED GSL_I2C_CLOCKSPEED_DEFAULT
#endif

// slave address
#ifndef GSL_I2C2_SLAVEADDRESS
#define GSL_I2C2_SLAVEADDRESS GSL_I2C_SLAVEADDRESS_DEFAULT
#endif

// scl pin
// I2C2 SCL can be PB10, PF1, PH4
#ifndef GSL_I2C2_PIN_SCL
#define GSL_I2C2_PIN_SCL kPinB10
#endif

// sda pin
// I2C2 SDA can be PB11, PF0, PH5
#ifndef GSL_I2C2_PIN_SDA
#define GSL_I2C2_PIN_SDA kPinB11
#endif

// initialization routine
I2C_HandleTypeDef gsl_i2c2_hi2c = {
    I2C2,
    {
        GSL_I2C2_CLOCKSPEED,
        I2C_DUTYCYCLE_2,
        GSL_I2C2_SLAVEADDRESS,
        I2C_ADDRESSINGMODE_7BIT,
        I2C_DUALADDRESS_DISABLE,
        0,
        I2C_GENERALCALL_DISABLE,
        I2C_NOSTRETCH_DISABLE},
    nullptr,
    0,
    0,
    0,
    0,
    nullptr,
    nullptr,
    HAL_UNLOCKED,
    HAL_I2C_STATE_RESET,
    HAL_I2C_MODE_NONE,
    HAL_I2C_ERROR_NONE,
    0,
    0,
    0,
    0};

// pins
GSL_I2C_PinsStruct gsl_i2c2_pins = {
    GSL_I2C2_PIN_SCL,
    GSL_I2C2_PIN_SDA};

// initialize DMA settings

// I2C2 TX --> DMA 1 stream 7 channel 7
#define GSL_I2C2_DMA_TX_STREAM DMA1_Stream7
#define GSL_I2C2_DMA_TX_CHANNEL DMA_CHANNEL_7

// I2C2 RX --> DMA 1 stream 2 or 3 channel 7
#ifndef GSL_I2C2_DMA_RX_STREAM
#define GSL_I2C2_DMA_RX_STREAM DMA1_Stream2
#endif
#define GSL_I2C2_DMA_RX_CHANNEL DMA_CHANNEL_7

// setup for the I2C2 DMA TX stream
DMA_HandleTypeDef gsl_i2c2_hdmatx = {
    GSL_I2C2_DMA_TX_STREAM,
    {
        GSL_I2C2_DMA_TX_CHANNEL,
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

// setup for the I2C2 DMA RX stream
DMA_HandleTypeDef gsl_i2c2_hdmarx = {
    GSL_I2C2_DMA_RX_STREAM,
    {
        GSL_I2C2_DMA_RX_CHANNEL,
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

#endif // #ifdef I2C2

// ************
// *** I2C3 ***
// ************

// initialize I2C3 settings
#ifdef I2C3

// clock speed
#ifndef GSL_I2C3_CLOCKSPEED
#define GSL_I2C3_CLOCKSPEED GSL_I2C_CLOCKSPEED_DEFAULT
#endif

// slave address
#ifndef GSL_I2C3_SLAVEADDRESS
#define GSL_I2C3_SLAVEADDRESS GSL_I2C_SLAVEADDRESS_DEFAULT
#endif

// scl pin
// I2C3 SCL can be PA8, PH7
#ifndef GSL_I2C3_PIN_SCL
#define GSL_I2C3_PIN_SCL kPinA8
#endif

// sda pin
// I2C3 SDA can be PC9, PH8
#ifndef GSL_I2C3_PIN_SDA
#define GSL_I2C3_PIN_SDA kPinC9
#endif

// initialization routine
I2C_HandleTypeDef gsl_i2c3_hi2c = {
    I2C3,
    {
        GSL_I2C3_CLOCKSPEED,
        I2C_DUTYCYCLE_2,
        GSL_I2C3_SLAVEADDRESS,
        I2C_ADDRESSINGMODE_7BIT,
        I2C_DUALADDRESS_DISABLE,
        0,
        I2C_GENERALCALL_DISABLE,
        I2C_NOSTRETCH_DISABLE},
    nullptr,
    0,
    0,
    0,
    0,
    nullptr,
    nullptr,
    HAL_UNLOCKED,
    HAL_I2C_STATE_RESET,
    HAL_I2C_MODE_NONE,
    HAL_I2C_ERROR_NONE,
    0,
    0,
    0,
    0};

// pins
GSL_I2C_PinsStruct gsl_i2c3_pins = {
    GSL_I2C3_PIN_SCL,
    GSL_I2C3_PIN_SDA};

// initialize DMA settings

// I2C3 TX --> DMA 1 stream 4 channel 3
#define GSL_I2C3_DMA_TX_STREAM DMA1_Stream4
#define GSL_I2C3_DMA_TX_CHANNEL DMA_CHANNEL_3

// I2C3 RX --> DMA 1 stream 2 channel 3
#define GSL_I2C3_DMA_RX_STREAM DMA1_Stream2
#define GSL_I2C3_DMA_RX_CHANNEL DMA_CHANNEL_3

// setup for the I2C3 DMA TX stream
DMA_HandleTypeDef gsl_i2c3_hdmatx = {
    GSL_I2C3_DMA_TX_STREAM,
    {
        GSL_I2C3_DMA_TX_CHANNEL,
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

// setup for the I2C3 DMA RX stream
DMA_HandleTypeDef gsl_i2c3_hdmarx = {
    GSL_I2C3_DMA_RX_STREAM,
    {
        GSL_I2C3_DMA_RX_CHANNEL,
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

#endif // #ifdef I2C3

// this holds pointers to the various structures we need
struct GSL_I2C_InfoStruct {
  I2C_TypeDef * I2Cx;
  I2C_HandleTypeDef* handle;
  GSL_I2C_PinsStruct* pins;
  uint8_t af_mode;
  DMA_HandleTypeDef* hdmatx;
  DMA_HandleTypeDef* hdmarx;
  void (*MasterTxCpltCallback)(void);
  void (*MasterRxCpltCallback)(void);
  void (*SlaveTxCpltCallback)(void);
  void (*SlaveRxCpltCallback)(void);
  void (*MemTxCpltCallback)(void);
  void (*MemRxCpltCallback)(void);
};

// here we define the info structure which points to all the various other
// structures
GSL_I2C_InfoStruct GSL_I2C_Info[] = {

#ifdef I2C1
    {
      I2C1,
      &gsl_i2c1_hi2c,
      &gsl_i2c1_pins,
      GPIO_AF4_I2C1,
      &gsl_i2c1_hdmatx,
      &gsl_i2c1_hdmarx,
      nullptr,
      nullptr,
      nullptr,
      nullptr,
      nullptr,
      nullptr,
    },
#endif // #ifdef I2C1

#ifdef I2C2
    {
      I2C2,
      &gsl_i2c2_hi2c,
      &gsl_i2c2_pins,
      GPIO_AF4_I2C2,
      &gsl_i2c2_hdmatx,
      &gsl_i2c2_hdmarx,
      nullptr,
      nullptr,
      nullptr,
      nullptr,
      nullptr,
      nullptr,
    },
#endif // #ifdef I2C2

#ifdef I2C3
    {
      I2C3,
      &gsl_i2c3_hi2c,
      &gsl_i2c3_pins,
      GPIO_AF4_I2C3,
      &gsl_i2c3_hdmatx,
      &gsl_i2c3_hdmarx,
      nullptr,
      nullptr,
      nullptr,
      nullptr,
      nullptr,
      nullptr,
    },
#endif // #ifdef I2C3

};

// number of I2C peripherals
const uint32_t GSL_I2C_PeripheralCount =
    sizeof(GSL_I2C_Info) / sizeof(*GSL_I2C_Info);

// bytes of RAM used to store all the handler structs
const uint32_t GSL_I2C_StorageByteCount =
    sizeof(GSL_I2C_Info) +
    GSL_I2C_PeripheralCount * sizeof(I2C_HandleTypeDef) +
    GSL_I2C_PeripheralCount * sizeof(GSL_I2C_PinsStruct) +
    2 * GSL_I2C_PeripheralCount * sizeof(DMA_HandleTypeDef);
