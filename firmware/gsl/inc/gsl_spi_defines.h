#pragma once

// This provides a interface to configure and use the SPI peripheral.
//
// Note: the pins offered here (in the code comments) are not comprehensive.
// To find the correct pin and AF for your layout, consult the
// "Alternate Function Mapping" table in the datasheet.
//
// Rather unfortunately, a given SPI peripheral can have up to 3 valid AF
// codes, and the correct one must be set for the chosen SPIx/pin combination
// or it will not work
//
// Based on my research looking at data sheets:
//   SPI1 always uses GPIO_AF5_SPI1
//   SPI2 can be any of: GPIO_AF5_SPI2, GPIO_AF6_SPI2, GPIO_AF7_SPI2
//   SPI3 can be any of: GPIO_AF5_SPI3, GPIO_AF6_SPI3, GPIO_AF7_SPI3
//   SPI4 can be any of: GPIO_AF5_SPI4, GPIO_AF6_SPI4
//   SPI5 can be any of: GPIO_AF5_SPI5, GPIO_AF6_SPI5
//   SPI6 always uses GPIO_AF5_SPI6
// the defaults (used in this library) are marked with an asterisk

#include "gsl_includes.h"

// number of CS pins available in addition to the NSS pin
const uint16_t GSL_SPI_CSPinCount = 4;

// default AF mode
// (If AF modes are not #defined, then they will take this value.  When an AF
// mode is assigned, it will be replaced by a valid (but possibly wrong) AF
// mode and a warning will be issued.)
#define GSL_SPI_DEFAULT_AF_MODE (uint32_t) -1

// SPI pins
// (set to kPinInvalid if not used)
struct GSL_SPI_PinsStruct {
  PinEnum sck;
  uint32_t sck_af_mode;
  PinEnum mosi;
  uint32_t mosi_af_mode;
  PinEnum miso;
  uint32_t miso_af_mode;
  PinEnum nss;
  uint32_t nss_af_mode;
  // 4 additional CS pins (nss pin can be used as a CS as well)
  PinEnum cs[GSL_SPI_CSPinCount];
};

enum GSL_SPI_CSModeEnum {
  // in master mode: initialize, but do not use, the CS pin
  GSL_SPI_CS_MODE_MANUAL,
  // in master mode: automatically pull down the CS pin when in use and pull
  // it back up when done
  GSL_SPI_CS_MODE_MANAGED,
};

// max timeout in milliseconds used within the HAL SPI functions
#ifndef GSL_SPI_TIMEOUT
#define GSL_SPI_TIMEOUT 100
#endif

// defaults

#ifndef GSL_SPI_PRIORITY_DEFAULT
#define GSL_SPI_PRIORITY_DEFAULT 8
#endif

#ifndef GSL_SPI_CS_MODE_DEFAULT
#define GSL_SPI_CS_MODE_DEFAULT GSL_SPI_CS_MODE_MANAGED
#endif

// default mode
#ifndef GSL_SPI_MODE_DEFAULT
#define GSL_SPI_MODE_DEFAULT SPI_MODE_MASTER
#endif

// data size 8 or 16 bit
#ifndef GSL_SPI_DATASIZE_DEFAULT
#define GSL_SPI_DATASIZE_DEFAULT SPI_DATASIZE_8BIT
#endif

// prescaler from AHB clock
#ifndef GSL_SPI_PRESCALER_DEFAULT
#define GSL_SPI_PRESCALER_DEFAULT SPI_BAUDRATEPRESCALER_256
#endif

// bit ordering
#ifndef GSL_SPI_FIRSTBIT_DEFAULT
#define GSL_SPI_FIRSTBIT_DEFAULT SPI_FIRSTBIT_MSB
#endif

// chip select soft (done by software) or hard (done by hardware)
#ifndef GSL_SPI_NSS_DEFAULT
#define GSL_SPI_NSS_DEFAULT SPI_NSS_SOFT
#endif

// direction
#ifndef GSL_SPI_DIRECTION_DEFAULT
#define GSL_SPI_DIRECTION_DEFAULT SPI_DIRECTION_2LINES
#endif

// clock polarity
#ifndef GSL_SPI_POLARITY_DEFAULT
#define GSL_SPI_POLARITY_DEFAULT SPI_POLARITY_LOW
#endif

// data is latched on first or second edge
#ifndef GSL_SPI_PHASE_DEFAULT
#define GSL_SPI_PHASE_DEFAULT SPI_PHASE_1EDGE
#endif

// ************
// *** SPI1 ***
// ************

// initialize SPI1 settings
#ifdef SPI1

// set the mode to master or slave
#ifndef GSL_SPI1_MODE
#define GSL_SPI1_MODE GSL_SPI_MODE_DEFAULT
#endif

// data size 8 or 16 bit
#ifndef GSL_SPI1_DATASIZE
#define GSL_SPI1_DATASIZE GSL_SPI_DATASIZE_DEFAULT
#endif

// prescaler from AHB clock
#ifndef GSL_SPI1_PRESCALER
#define GSL_SPI1_PRESCALER GSL_SPI_PRESCALER_DEFAULT
#endif

// bit ordering
#ifndef GSL_SPI1_FIRSTBIT
#define GSL_SPI1_FIRSTBIT GSL_SPI_FIRSTBIT_DEFAULT
#endif

// chip select soft (done by software) or hard (done by hardware)
#ifndef GSL_SPI1_NSS
#define GSL_SPI1_NSS GSL_SPI_NSS_DEFAULT
#endif

// direction
#ifndef GSL_SPI1_DIRECTION
#define GSL_SPI1_DIRECTION GSL_SPI_DIRECTION_DEFAULT
#endif

// clock polarity
#ifndef GSL_SPI1_POLARITY
#define GSL_SPI1_POLARITY GSL_SPI_POLARITY_DEFAULT
#endif

// data is latched on first or second edge
#ifndef GSL_SPI1_PHASE
#define GSL_SPI1_PHASE GSL_SPI_PHASE_DEFAULT
#endif

// default AF mode
#ifndef GSL_SPI1_DEFAULT_AF_MODE
#define GSL_SPI1_DEFAULT_AF_MODE GSL_SPI_DEFAULT_AF_MODE
#endif

// priority
#ifndef GSL_SPI1_PRIORITY
#define GSL_SPI1_PRIORITY GSL_SPI_PRIORITY_DEFAULT
#endif

// nss pin
// SPI1 NSS pin can be PA4, PA15 (all AF5)
#ifndef GSL_SPI1_PIN_NSS
#define GSL_SPI1_PIN_NSS kPinA4
#endif

// cs management mode
#ifndef GSL_SPI1_CS_MODE
#define GSL_SPI1_CS_MODE GSL_SPI_CS_MODE_DEFAULT
#endif

// first CS pin
#ifndef GSL_SPI1_PIN_CS1
#define GSL_SPI1_PIN_CS1 kPinNone
#endif

// second CS pin
#ifndef GSL_SPI1_PIN_CS2
#define GSL_SPI1_PIN_CS2 kPinNone
#endif

// third CS pin
#ifndef GSL_SPI1_PIN_CS3
#define GSL_SPI1_PIN_CS3 kPinNone
#endif

// fourth CS pin
#ifndef GSL_SPI1_PIN_CS4
#define GSL_SPI1_PIN_CS4 kPinNone
#endif

// sck pin
// SPI1 SCK pin can be PA5, PB3 (all AF5)
#ifndef GSL_SPI1_PIN_SCK
#define GSL_SPI1_PIN_SCK kPinA5
#endif

//  miso pin
// SPI1 MISO pin can be PA6, PB4 (all AF5)
#ifndef GSL_SPI1_PIN_MISO
#define GSL_SPI1_PIN_MISO kPinA6
#endif

// mosi pin
// SPI1 MOSI pin can be PA7, PB5 (all AF5)
#ifndef GSL_SPI1_PIN_MOSI
#define GSL_SPI1_PIN_MOSI kPinA7
#endif

// possible alternate function modes for SPI1
uint8_t gsl_spi1_afmode[] = {
#ifdef GPIO_AF5_SPI1
    GPIO_AF5_SPI1,
#endif
#ifdef GPIO_AF6_SPI1
    GPIO_AF6_SPI1,
#endif
#ifdef GPIO_AF7_SPI1
    GPIO_AF7_SPI1,
#endif
};

// total number of AF modes for SPI1
uint8_t gsl_spi1_afmode_count =
    sizeof(gsl_spi1_afmode) / sizeof(*gsl_spi1_afmode);

// SPI1 handle
SPI_HandleTypeDef gsl_spi1_hspi = {
    SPI1,
    {
        GSL_SPI1_MODE,
        GSL_SPI1_DIRECTION,
        GSL_SPI1_DATASIZE,
        GSL_SPI1_POLARITY,
        GSL_SPI1_PHASE,
        GSL_SPI1_NSS,
        GSL_SPI1_PRESCALER,
        GSL_SPI1_FIRSTBIT,
        SPI_TIMODE_DISABLE,
        SPI_CRCCALCULATION_DISABLE,
        7},
    nullptr,
    0,
    0,
    nullptr,
    0,
    0,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    HAL_UNLOCKED,
    HAL_SPI_STATE_RESET,
    HAL_SPI_ERROR_NONE};

// SPI1 pin settings
GSL_SPI_PinsStruct gsl_spi1_pins =  {
    GSL_SPI1_PIN_SCK,
#ifdef GSL_SPI1_PIN_SCK_AF_MODE
    GSL_SPI1_PIN_SCK_AF_MODE,
#else
    GSL_SPI1_DEFAULT_AF_MODE,
#endif
    GSL_SPI1_PIN_MOSI,
#ifdef GSL_SPI1_PIN_MOSI_AF_MODE
    GSL_SPI1_PIN_MOSI_AF_MODE,
#else
    GSL_SPI1_DEFAULT_AF_MODE,
#endif
    GSL_SPI1_PIN_MISO,
#ifdef GSL_SPI1_PIN_MISO_AF_MODE
    GSL_SPI1_PIN_MISO_AF_MODE,
#else
    GSL_SPI1_DEFAULT_AF_MODE,
#endif
    GSL_SPI1_PIN_NSS,
#ifdef GSL_SPI1_PIN_NSS_AF_MODE
    GSL_SPI1_PIN_NSS_AF_MODE,
#else
    GSL_SPI1_DEFAULT_AF_MODE,
#endif
    {GSL_SPI1_PIN_CS1, GSL_SPI1_PIN_CS2, GSL_SPI1_PIN_CS3, GSL_SPI1_PIN_CS4}};

// initialize SPI1 DMA settings

// SPI1 TX --> DMA 2 stream 3 or 5 channel 3
#ifndef GSL_SPI1_DMA_TX_STREAM
#define GSL_SPI1_DMA_TX_STREAM DMA2_Stream3
#endif
#define GSL_SPI1_DMA_TX_CHANNEL DMA_CHANNEL_3

// ensure valid values
/*static_assert(GSL_SPI1_DMA_TX_STREAM == DMA2_Stream3 ||
              GSL_SPI1_DMA_TX_STREAM == DMA2_Stream5, "Invalid DMA values");*/

// SPI1 RX --> DMA 2 stream 0 or 2 channel 3
#ifndef GSL_SPI1_DMA_RX_STREAM
#define GSL_SPI1_DMA_RX_STREAM DMA2_Stream0
#endif
#define GSL_SPI1_DMA_RX_CHANNEL DMA_CHANNEL_3

DMA_HandleTypeDef gsl_spi1_hdmatx = {
    GSL_SPI1_DMA_TX_STREAM,
    {
        GSL_SPI1_DMA_TX_CHANNEL,
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

DMA_HandleTypeDef gsl_spi1_hdmarx = {
    GSL_SPI1_DMA_RX_STREAM,
    {
        GSL_SPI1_DMA_RX_CHANNEL,
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

#endif // #ifdef SPI1

// ************
// *** SPI2 ***
// ************

// initialize SPI2 settings
#ifdef SPI2

// set the mode to master or slave
#ifndef GSL_SPI2_MODE
#define GSL_SPI2_MODE GSL_SPI_MODE_DEFAULT
#endif

// data size 8 or 16 bit
#ifndef GSL_SPI2_DATASIZE
#define GSL_SPI2_DATASIZE GSL_SPI_DATASIZE_DEFAULT
#endif

// prescaler from AHB clock
#ifndef GSL_SPI2_PRESCALER
#define GSL_SPI2_PRESCALER GSL_SPI_PRESCALER_DEFAULT
#endif

// bit ordering
#ifndef GSL_SPI2_FIRSTBIT
#define GSL_SPI2_FIRSTBIT GSL_SPI_FIRSTBIT_DEFAULT
#endif

// chip select soft (done by software) or hard (done by hardware)
#ifndef GSL_SPI2_NSS
#define GSL_SPI2_NSS GSL_SPI_NSS_DEFAULT
#endif

// direction
#ifndef GSL_SPI2_DIRECTION
#define GSL_SPI2_DIRECTION GSL_SPI_DIRECTION_DEFAULT
#endif

// clock polarity
#ifndef GSL_SPI2_POLARITY
#define GSL_SPI2_POLARITY GSL_SPI_POLARITY_DEFAULT
#endif

// data is latched on first or second edge
#ifndef GSL_SPI2_PHASE
#define GSL_SPI2_PHASE GSL_SPI_PHASE_DEFAULT
#endif

// default AF mode
#ifndef GSL_SPI2_DEFAULT_AF_MODE
#define GSL_SPI2_DEFAULT_AF_MODE GSL_SPI_DEFAULT_AF_MODE
#endif

// priority
#ifndef GSL_SPI2_PRIORITY
#define GSL_SPI2_PRIORITY GSL_SPI_PRIORITY_DEFAULT
#endif

// nss pin
// SPI2 NSS pin can be PB9, PB12, PI0 (all AF5)
#ifndef GSL_SPI2_PIN_NSS
#define GSL_SPI2_PIN_NSS kPinB12
#endif

// cs management mode
#ifndef GSL_SPI2_CS_MODE
#define GSL_SPI2_CS_MODE GSL_SPI_CS_MODE_DEFAULT
#endif

// first CS pin
#ifndef GSL_SPI2_PIN_CS1
#define GSL_SPI2_PIN_CS1 kPinNone
#endif

// second CS pin
#ifndef GSL_SPI2_PIN_CS2
#define GSL_SPI2_PIN_CS2 kPinNone
#endif

// third CS pin
#ifndef GSL_SPI2_PIN_CS3
#define GSL_SPI2_PIN_CS3 kPinNone
#endif

// fourth CS pin
#ifndef GSL_SPI2_PIN_CS4
#define GSL_SPI2_PIN_CS4 kPinNone
#endif

// sck pin
// SPI2 SCK pin can be PB10, PB13, PD3, PI1 (all AF5)
#ifndef GSL_SPI2_PIN_SCK
#define GSL_SPI2_PIN_SCK kPinB13
#endif

// miso pin
// SPI2 MISO pin can be PB14, PC2, PI2 (all AF5)
#ifndef GSL_SPI2_PIN_MISO
#define GSL_SPI2_PIN_MISO kPinB14
#endif

// mosi pin
// SPI2 MOSI pin can be PB15, PC3, PI3 (all AF5)
#ifndef GSL_SPI2_PIN_MOSI
#define GSL_SPI2_PIN_MOSI kPinB15
#endif

// possible alternate function modes for SPI2
uint8_t gsl_spi2_afmode[] = {
#ifdef GPIO_AF5_SPI2
    GPIO_AF5_SPI2,
#endif
#ifdef GPIO_AF6_SPI2
    GPIO_AF6_SPI2,
#endif
#ifdef GPIO_AF7_SPI2
    GPIO_AF7_SPI2,
#endif
};

// total number of AF modes for SPI2
uint8_t gsl_spi2_afmode_count =
    sizeof(gsl_spi2_afmode) / sizeof(*gsl_spi2_afmode);

// SPI2 handle
SPI_HandleTypeDef gsl_spi2_hspi = {
    SPI2,
    {
        GSL_SPI2_MODE,
        GSL_SPI2_DIRECTION,
        GSL_SPI2_DATASIZE,
        GSL_SPI2_POLARITY,
        GSL_SPI2_PHASE,
        GSL_SPI2_NSS,
        GSL_SPI2_PRESCALER,
        GSL_SPI2_FIRSTBIT,
        SPI_TIMODE_DISABLE,
        SPI_CRCCALCULATION_DISABLE,
        7},
    nullptr,
    0,
    0,
    nullptr,
    0,
    0,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    HAL_UNLOCKED,
    HAL_SPI_STATE_RESET,
    HAL_SPI_ERROR_NONE};

// SPI2 pin settings
GSL_SPI_PinsStruct gsl_spi2_pins =  {
    GSL_SPI2_PIN_SCK,
#ifdef GSL_SPI2_PIN_SCK_AF_MODE
    GSL_SPI2_PIN_SCK_AF_MODE,
#else
    GSL_SPI2_DEFAULT_AF_MODE,
#endif
    GSL_SPI2_PIN_MOSI,
#ifdef GSL_SPI2_PIN_MOSI_AF_MODE
    GSL_SPI2_PIN_MOSI_AF_MODE,
#else
    GSL_SPI2_DEFAULT_AF_MODE,
#endif
    GSL_SPI2_PIN_MISO,
#ifdef GSL_SPI2_PIN_MISO_AF_MODE
    GSL_SPI2_PIN_MISO_AF_MODE,
#else
    GSL_SPI2_DEFAULT_AF_MODE,
#endif
    GSL_SPI2_PIN_NSS,
#ifdef GSL_SPI2_PIN_NSS_AF_MODE
    GSL_SPI2_PIN_NSS_AF_MODE,
#else
    GSL_SPI2_DEFAULT_AF_MODE,
#endif
    {GSL_SPI2_PIN_CS1, GSL_SPI2_PIN_CS2, GSL_SPI2_PIN_CS3, GSL_SPI2_PIN_CS4}};

// initialize SPI2 DMA settings

// SPI2 TX --> DMA 1 stream 4 channel 0
#define GSL_SPI2_DMA_TX_STREAM DMA1_Stream4
#define GSL_SPI2_DMA_TX_CHANNEL DMA_CHANNEL_0

// SPI2 RX --> DMA 1 stream 3 channel 0
#define GSL_SPI2_DMA_RX_STREAM DMA1_Stream3
#define GSL_SPI2_DMA_RX_CHANNEL DMA_CHANNEL_0

DMA_HandleTypeDef gsl_spi2_hdmatx = {
    GSL_SPI2_DMA_TX_STREAM,
    {
        GSL_SPI2_DMA_TX_CHANNEL,
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

DMA_HandleTypeDef gsl_spi2_hdmarx = {
    GSL_SPI2_DMA_RX_STREAM,
    {
        GSL_SPI2_DMA_RX_CHANNEL,
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

#endif // #ifdef SPI2

// ************
// *** SPI3 ***
// ************

// initialize SPI3 settings
#ifdef SPI3

// set the mode to master or slave
#ifndef GSL_SPI3_MODE
#define GSL_SPI3_MODE GSL_SPI_MODE_DEFAULT
#endif

// data size 8 or 16 bit
#ifndef GSL_SPI3_DATASIZE
#define GSL_SPI3_DATASIZE GSL_SPI_DATASIZE_DEFAULT
#endif

// prescaler from AHB clock
#ifndef GSL_SPI3_PRESCALER
#define GSL_SPI3_PRESCALER GSL_SPI_PRESCALER_DEFAULT
#endif

// bit ordering
#ifndef GSL_SPI3_FIRSTBIT
#define GSL_SPI3_FIRSTBIT GSL_SPI_FIRSTBIT_DEFAULT
#endif

// chip select soft (done by software) or hard (done by hardware)
#ifndef GSL_SPI3_NSS
#define GSL_SPI3_NSS GSL_SPI_NSS_DEFAULT
#endif

// direction
#ifndef GSL_SPI3_DIRECTION
#define GSL_SPI3_DIRECTION GSL_SPI_DIRECTION_DEFAULT
#endif

// clock polarity
#ifndef GSL_SPI3_POLARITY
#define GSL_SPI3_POLARITY GSL_SPI_POLARITY_DEFAULT
#endif

// data is latched on first or second edge
#ifndef GSL_SPI3_PHASE
#define GSL_SPI3_PHASE GSL_SPI_PHASE_DEFAULT
#endif

// default AF mode
#ifndef GSL_SPI3_DEFAULT_AF_MODE
#define GSL_SPI3_DEFAULT_AF_MODE GSL_SPI_DEFAULT_AF_MODE
#endif

// priority
#ifndef GSL_SPI3_PRIORITY
#define GSL_SPI3_PRIORITY GSL_SPI_PRIORITY_DEFAULT
#endif

// nss pin
// SPI3 NSS pin can be PA4, PA15 (all AF6)
#ifndef GSL_SPI3_PIN_NSS
#define GSL_SPI3_PIN_NSS kPinA4
#endif

// cs management mode
#ifndef GSL_SPI3_CS_MODE
#define GSL_SPI3_CS_MODE GSL_SPI_CS_MODE_DEFAULT
#endif

// first CS pin
#ifndef GSL_SPI3_PIN_CS1
#define GSL_SPI3_PIN_CS1 kPinNone
#endif

// second CS pin
#ifndef GSL_SPI3_PIN_CS2
#define GSL_SPI3_PIN_CS2 kPinNone
#endif

// third CS pin
#ifndef GSL_SPI3_PIN_CS3
#define GSL_SPI3_PIN_CS3 kPinNone
#endif

// fourth CS pin
#ifndef GSL_SPI3_PIN_CS4
#define GSL_SPI3_PIN_CS4 kPinNone
#endif

// sck pin
// SPI3 SCK pin can be PB3, PC10 (all AF6)
#ifndef GSL_SPI3_PIN_SCK
#define GSL_SPI3_PIN_SCK kPinB3
#endif

//  miso pin
// SPI3 MISO pin can be PB4, PC11 (all AF6)
#ifndef GSL_SPI3_PIN_MISO
#define GSL_SPI3_PIN_MISO kPinB4
#endif

// mosi pin
// SPI3 MOSI pin can be PB5, PC12 (AF6), or PD6 (AF5)
#ifndef GSL_SPI3_PIN_MOSI
#define GSL_SPI3_PIN_MOSI kPinB5
#endif

// possible alternate function modes for SPI3
uint8_t gsl_spi3_afmode[] = {
#ifdef GPIO_AF5_SPI3
    GPIO_AF5_SPI3,
#endif
#ifdef GPIO_AF6_SPI3
    GPIO_AF6_SPI3,
#endif
#ifdef GPIO_AF7_SPI3
    GPIO_AF7_SPI3,
#endif
};

// total number of AF modes for SPI3
uint8_t gsl_spi3_afmode_count =
    sizeof(gsl_spi3_afmode) / sizeof(*gsl_spi3_afmode);

// SPI3 handle
SPI_HandleTypeDef gsl_spi3_hspi = {
    SPI3,
    {
        GSL_SPI3_MODE,
        GSL_SPI3_DIRECTION,
        GSL_SPI3_DATASIZE,
        GSL_SPI3_POLARITY,
        GSL_SPI3_PHASE,
        GSL_SPI3_NSS,
        GSL_SPI3_PRESCALER,
        GSL_SPI3_FIRSTBIT,
        SPI_TIMODE_DISABLE,
        SPI_CRCCALCULATION_DISABLE,
        7},
    nullptr,
    0,
    0,
    nullptr,
    0,
    0,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    HAL_UNLOCKED,
    HAL_SPI_STATE_RESET,
    HAL_SPI_ERROR_NONE};

// SPI3 pin settings
GSL_SPI_PinsStruct gsl_spi3_pins =  {
    GSL_SPI3_PIN_SCK,
#ifdef GSL_SPI3_PIN_SCK_AF_MODE
    GSL_SPI3_PIN_SCK_AF_MODE,
#else
    GSL_SPI3_DEFAULT_AF_MODE,
#endif
    GSL_SPI3_PIN_MOSI,
#ifdef GSL_SPI3_PIN_MOSI_AF_MODE
    GSL_SPI3_PIN_MOSI_AF_MODE,
#else
    GSL_SPI3_DEFAULT_AF_MODE,
#endif
    GSL_SPI3_PIN_MISO,
#ifdef GSL_SPI3_PIN_MISO_AF_MODE
    GSL_SPI3_PIN_MISO_AF_MODE,
#else
    GSL_SPI3_DEFAULT_AF_MODE,
#endif
    GSL_SPI3_PIN_NSS,
#ifdef GSL_SPI3_PIN_NSS_AF_MODE
    GSL_SPI3_PIN_NSS_AF_MODE,
#else
    GSL_SPI3_DEFAULT_AF_MODE,
#endif
    {GSL_SPI3_PIN_CS1, GSL_SPI3_PIN_CS2, GSL_SPI3_PIN_CS3, GSL_SPI3_PIN_CS4}};

// initialize SPI3 DMA settings

// SPI3 TX --> DMA 1 stream 5 or 7 channel 0
#ifndef GSL_SPI3_DMA_TX_STREAM
#define GSL_SPI3_DMA_TX_STREAM DMA1_Stream5
#endif
#define GSL_SPI3_DMA_TX_CHANNEL DMA_CHANNEL_0

// SPI3 RX --> DMA 1 stream 0 or 2 channel 0
#ifndef GSL_SPI3_DMA_RX_STREAM
#define GSL_SPI3_DMA_RX_STREAM DMA1_Stream0
#endif
#define GSL_SPI3_DMA_RX_CHANNEL DMA_CHANNEL_0

DMA_HandleTypeDef gsl_spi3_hdmatx = {
    GSL_SPI3_DMA_TX_STREAM,
    {
        GSL_SPI3_DMA_TX_CHANNEL,
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

DMA_HandleTypeDef gsl_spi3_hdmarx = {
    GSL_SPI3_DMA_RX_STREAM,
    {
        GSL_SPI3_DMA_RX_CHANNEL,
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

#endif // #ifdef SPI3

// ************
// *** SPI4 ***
// ************

// initialize SPI4 settings
#ifdef SPI4

// set the mode to master or slave
#ifndef GSL_SPI4_MODE
#define GSL_SPI4_MODE GSL_SPI_MODE_DEFAULT
#endif

// data size 8 or 16 bit
#ifndef GSL_SPI4_DATASIZE
#define GSL_SPI4_DATASIZE GSL_SPI_DATASIZE_DEFAULT
#endif

// prescaler from AHB clock
#ifndef GSL_SPI4_PRESCALER
#define GSL_SPI4_PRESCALER GSL_SPI_PRESCALER_DEFAULT
#endif

// bit ordering
#ifndef GSL_SPI4_FIRSTBIT
#define GSL_SPI4_FIRSTBIT GSL_SPI_FIRSTBIT_DEFAULT
#endif

// chip select soft (done by software) or hard (done by hardware)
#ifndef GSL_SPI4_NSS
#define GSL_SPI4_NSS GSL_SPI_NSS_DEFAULT
#endif

// direction
#ifndef GSL_SPI4_DIRECTION
#define GSL_SPI4_DIRECTION GSL_SPI_DIRECTION_DEFAULT
#endif

// clock polarity
#ifndef GSL_SPI4_POLARITY
#define GSL_SPI4_POLARITY GSL_SPI_POLARITY_DEFAULT
#endif

// data is latched on first or second edge
#ifndef GSL_SPI4_PHASE
#define GSL_SPI4_PHASE GSL_SPI_PHASE_DEFAULT
#endif

// default AF mode
#ifndef GSL_SPI4_DEFAULT_AF_MODE
#define GSL_SPI4_DEFAULT_AF_MODE GSL_SPI_DEFAULT_AF_MODE
#endif

// priority
#ifndef GSL_SPI4_PRIORITY
#define GSL_SPI4_PRIORITY GSL_SPI_PRIORITY_DEFAULT
#endif

// nss pin
// SPI4 NSS pin can be PE4, PE11 (all AF5)
#ifndef GSL_SPI4_PIN_NSS
#define GSL_SPI4_PIN_NSS kPinE4
#endif

// cs management mode
#ifndef GSL_SPI4_CS_MODE
#define GSL_SPI4_CS_MODE GSL_SPI_CS_MODE_DEFAULT
#endif

// first CS pin
#ifndef GSL_SPI4_PIN_CS1
#define GSL_SPI4_PIN_CS1 kPinNone
#endif

// second CS pin
#ifndef GSL_SPI4_PIN_CS2
#define GSL_SPI4_PIN_CS2 kPinNone
#endif

// third CS pin
#ifndef GSL_SPI4_PIN_CS3
#define GSL_SPI4_PIN_CS3 kPinNone
#endif

// fourth CS pin
#ifndef GSL_SPI4_PIN_CS4
#define GSL_SPI4_PIN_CS4 kPinNone
#endif

// sck pin
// SPI4 SCK pin can be PE2, PE12 (all AF5)
#ifndef GSL_SPI4_PIN_SCK
#define GSL_SPI4_PIN_SCK kPinE2
#endif

//  miso pin
// SPI4 MISO pin can be PE5, PE13 (all AF5)
#ifndef GSL_SPI4_PIN_MISO
#define GSL_SPI4_PIN_MISO kPinE5
#endif

// mosi pin
// SPI4 MOSI pin can be PE6, PE14 (all AF5)
#ifndef GSL_SPI4_PIN_MOSI
#define GSL_SPI4_PIN_MOSI kPinE6
#endif

// possible alternate function modes for SPI4
uint8_t gsl_spi4_afmode[] = {
#ifdef GPIO_AF5_SPI4
    GPIO_AF5_SPI4,
#endif
#ifdef GPIO_AF6_SPI4
    GPIO_AF6_SPI4,
#endif
#ifdef GPIO_AF7_SPI4
    GPIO_AF7_SPI4,
#endif
};

// total number of AF modes for SPI4
uint8_t gsl_spi4_afmode_count =
    sizeof(gsl_spi4_afmode) / sizeof(*gsl_spi4_afmode);

// SPI4 handle
SPI_HandleTypeDef gsl_spi4_hspi = {
    SPI4,
    {
        GSL_SPI4_MODE,
        GSL_SPI4_DIRECTION,
        GSL_SPI4_DATASIZE,
        GSL_SPI4_POLARITY,
        GSL_SPI4_PHASE,
        GSL_SPI4_NSS,
        GSL_SPI4_PRESCALER,
        GSL_SPI4_FIRSTBIT,
        SPI_TIMODE_DISABLE,
        SPI_CRCCALCULATION_DISABLE,
        7},
    nullptr,
    0,
    0,
    nullptr,
    0,
    0,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    HAL_UNLOCKED,
    HAL_SPI_STATE_RESET,
    HAL_SPI_ERROR_NONE};

// SPI4 pin settings
GSL_SPI_PinsStruct gsl_spi4_pins =  {
    GSL_SPI4_PIN_SCK,
#ifdef GSL_SPI4_PIN_SCK_AF_MODE
    GSL_SPI4_PIN_SCK_AF_MODE,
#else
    GSL_SPI4_DEFAULT_AF_MODE,
#endif
    GSL_SPI4_PIN_MOSI,
#ifdef GSL_SPI4_PIN_MOSI_AF_MODE
    GSL_SPI4_PIN_MOSI_AF_MODE,
#else
    GSL_SPI4_DEFAULT_AF_MODE,
#endif
    GSL_SPI4_PIN_MISO,
#ifdef GSL_SPI4_PIN_MISO_AF_MODE
    GSL_SPI4_PIN_MISO_AF_MODE,
#else
    GSL_SPI4_DEFAULT_AF_MODE,
#endif
    GSL_SPI4_PIN_NSS,
#ifdef GSL_SPI4_PIN_NSS_AF_MODE
    GSL_SPI4_PIN_NSS_AF_MODE,
#else
    GSL_SPI4_DEFAULT_AF_MODE,
#endif
    {GSL_SPI4_PIN_CS1, GSL_SPI4_PIN_CS2, GSL_SPI4_PIN_CS3, GSL_SPI4_PIN_CS4}};

// initialize SPI4 DMA settings

// SPI4 TX --> DMA 2 (stream 1 channel 4) or (stream 4 channel 5)
#ifndef GSL_SPI4_DMA_TX_STREAM
#define GSL_SPI4_DMA_TX_STREAM DMA2_Stream1
#define GSL_SPI4_DMA_TX_CHANNEL DMA_CHANNEL_4
#endif

// SPI4 RX --> DMA 2 (stream 0 channel 4) or (stream 3 channel 5)
#ifndef GSL_SPI4_DMA_RX_STREAM
#define GSL_SPI4_DMA_RX_STREAM DMA2_Stream0
#define GSL_SPI4_DMA_RX_CHANNEL DMA_CHANNEL_4
#endif

DMA_HandleTypeDef gsl_spi4_hdmatx = {
    GSL_SPI4_DMA_TX_STREAM,
    {
        GSL_SPI4_DMA_TX_CHANNEL,
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

DMA_HandleTypeDef gsl_spi4_hdmarx = {
    GSL_SPI4_DMA_RX_STREAM,
    {
        GSL_SPI4_DMA_RX_CHANNEL,
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


#endif // #ifdef SPI4

// ************
// *** SPI5 ***
// ************

// initialize SPI5 settings
#ifdef SPI5

// set the mode to master or slave
#ifndef GSL_SPI5_MODE
#define GSL_SPI5_MODE GSL_SPI_MODE_DEFAULT
#endif

// data size 8 or 16 bit
#ifndef GSL_SPI5_DATASIZE
#define GSL_SPI5_DATASIZE GSL_SPI_DATASIZE_DEFAULT
#endif

// prescaler from AHB clock
#ifndef GSL_SPI5_PRESCALER
#define GSL_SPI5_PRESCALER GSL_SPI_PRESCALER_DEFAULT
#endif

// bit ordering
#ifndef GSL_SPI5_FIRSTBIT
#define GSL_SPI5_FIRSTBIT GSL_SPI_FIRSTBIT_DEFAULT
#endif

// chip select soft (done by software) or hard (done by hardware)
#ifndef GSL_SPI5_NSS
#define GSL_SPI5_NSS GSL_SPI_NSS_DEFAULT
#endif

// direction
#ifndef GSL_SPI5_DIRECTION
#define GSL_SPI5_DIRECTION GSL_SPI_DIRECTION_DEFAULT
#endif

// clock polarity
#ifndef GSL_SPI5_POLARITY
#define GSL_SPI5_POLARITY GSL_SPI_POLARITY_DEFAULT
#endif

// data is latched on first or second edge
#ifndef GSL_SPI5_PHASE
#define GSL_SPI5_PHASE GSL_SPI_PHASE_DEFAULT
#endif

// default AF mode
#ifndef GSL_SPI5_DEFAULT_AF_MODE
#define GSL_SPI5_DEFAULT_AF_MODE GSL_SPI_DEFAULT_AF_MODE
#endif

// priority
#ifndef GSL_SPI5_PRIORITY
#define GSL_SPI5_PRIORITY GSL_SPI_PRIORITY_DEFAULT
#endif

// nss pin
// SPI5 NSS pin can be PF6, PH5 (AF5), or PB1, PE4, PE11 (AF6)
#ifndef GSL_SPI5_PIN_NSS
#define GSL_SPI5_PIN_NSS kPinB1
#endif

// cs management mode
#ifndef GSL_SPI5_CS_MODE
#define GSL_SPI5_CS_MODE GSL_SPI_CS_MODE_DEFAULT
#endif

// first CS pin
#ifndef GSL_SPI5_PIN_CS1
#define GSL_SPI5_PIN_CS1 kPinNone
#endif

// second CS pin
#ifndef GSL_SPI5_PIN_CS2
#define GSL_SPI5_PIN_CS2 kPinNone
#endif

// third CS pin
#ifndef GSL_SPI5_PIN_CS3
#define GSL_SPI5_PIN_CS3 kPinNone
#endif

// fourth CS pin
#ifndef GSL_SPI5_PIN_CS4
#define GSL_SPI5_PIN_CS4 kPinNone
#endif

// sck pin
// SPI5 SCK pin can be PF7, PH6 (AF5), or PB0, PE2, PE12 (AF6)
#ifndef GSL_SPI5_PIN_SCK
#define GSL_SPI5_PIN_SCK kPinB0
#endif

//  miso pin
// SPI5 MISO pin can be PF8, PH7 (AF5), or PA12, PE5, PE13 (AF6)
#ifndef GSL_SPI5_PIN_MISO
#define GSL_SPI5_PIN_MISO kPinA12
#endif

// mosi pin
// SPI5 MOSI pin can be PF9, PF11 (AF5), or PA10, PB8, PE6, PE14 (AF6)
#ifndef GSL_SPI5_PIN_MOSI
#define GSL_SPI5_PIN_MOSI kPinA10
#endif

// possible alternate function modes for SPI5
uint8_t gsl_spi5_afmode[] = {
#ifdef GPIO_AF5_SPI5
    GPIO_AF5_SPI5,
#endif
#ifdef GPIO_AF6_SPI5
    GPIO_AF6_SPI5,
#endif
#ifdef GPIO_AF7_SPI5
    GPIO_AF7_SPI5,
#endif
};

// total number of AF modes for SPI5
uint8_t gsl_spi5_afmode_count =
    sizeof(gsl_spi5_afmode) / sizeof(*gsl_spi5_afmode);

// SPI5 handle
SPI_HandleTypeDef gsl_spi5_hspi = {
    SPI5,
    {
        GSL_SPI5_MODE,
        GSL_SPI5_DIRECTION,
        GSL_SPI5_DATASIZE,
        GSL_SPI5_POLARITY,
        GSL_SPI5_PHASE,
        GSL_SPI5_NSS,
        GSL_SPI5_PRESCALER,
        GSL_SPI5_FIRSTBIT,
        SPI_TIMODE_DISABLE,
        SPI_CRCCALCULATION_DISABLE,
        7},
    nullptr,
    0,
    0,
    nullptr,
    0,
    0,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    HAL_UNLOCKED,
    HAL_SPI_STATE_RESET,
    HAL_SPI_ERROR_NONE};

// SPI5 pin settings
GSL_SPI_PinsStruct gsl_spi5_pins =  {
    GSL_SPI5_PIN_SCK,
#ifdef GSL_SPI5_PIN_SCK_AF_MODE
    GSL_SPI5_PIN_SCK_AF_MODE,
#else
    GSL_SPI5_DEFAULT_AF_MODE,
#endif
    GSL_SPI5_PIN_MOSI,
#ifdef GSL_SPI5_PIN_MOSI_AF_MODE
    GSL_SPI5_PIN_MOSI_AF_MODE,
#else
    GSL_SPI5_DEFAULT_AF_MODE,
#endif
    GSL_SPI5_PIN_MISO,
#ifdef GSL_SPI5_PIN_MISO_AF_MODE
    GSL_SPI5_PIN_MISO_AF_MODE,
#else
    GSL_SPI5_DEFAULT_AF_MODE,
#endif
    GSL_SPI5_PIN_NSS,
#ifdef GSL_SPI5_PIN_NSS_AF_MODE
    GSL_SPI5_PIN_NSS_AF_MODE,
#else
    GSL_SPI5_DEFAULT_AF_MODE,
#endif
    {GSL_SPI5_PIN_CS1, GSL_SPI5_PIN_CS2, GSL_SPI5_PIN_CS3, GSL_SPI5_PIN_CS4}};

// initialize SPI5 DMA settings

// SPI5 TX --> DMA 2 (stream 4 channel 2) or (stream 6 channel 7)
#ifndef GSL_SPI5_DMA_TX_STREAM
#define GSL_SPI5_DMA_TX_STREAM DMA2_Stream4
#define GSL_SPI5_DMA_TX_CHANNEL DMA_CHANNEL_2
#endif

// SPI5 RX --> DMA 2 (stream 3 channel 2) or (stream 5 channel 7)
#ifndef GSL_SPI5_DMA_RX_STREAM
#define GSL_SPI5_DMA_RX_STREAM DMA2_Stream3
#define GSL_SPI5_DMA_RX_CHANNEL DMA_CHANNEL_2
#endif

DMA_HandleTypeDef gsl_spi5_hdmatx = {
    GSL_SPI5_DMA_TX_STREAM,
    {
        GSL_SPI5_DMA_TX_CHANNEL,
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

DMA_HandleTypeDef gsl_spi5_hdmarx = {
    GSL_SPI5_DMA_RX_STREAM,
    {
        GSL_SPI5_DMA_RX_CHANNEL,
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

#endif // #ifdef SPI5

// ************
// *** SPI6 ***
// ************

// initialize SPI6 settings
#ifdef SPI6

// set the mode to master or slave
#ifndef GSL_SPI6_MODE
#define GSL_SPI6_MODE GSL_SPI_MODE_DEFAULT
#endif

// data size 8 or 16 bit
#ifndef GSL_SPI6_DATASIZE
#define GSL_SPI6_DATASIZE GSL_SPI_DATASIZE_DEFAULT
#endif

// prescaler from AHB clock
#ifndef GSL_SPI6_PRESCALER
#define GSL_SPI6_PRESCALER GSL_SPI_PRESCALER_DEFAULT
#endif

// bit ordering
#ifndef GSL_SPI6_FIRSTBIT
#define GSL_SPI6_FIRSTBIT GSL_SPI_FIRSTBIT_DEFAULT
#endif

// chip select soft (done by software) or hard (done by hardware)
#ifndef GSL_SPI6_NSS
#define GSL_SPI6_NSS GSL_SPI_NSS_DEFAULT
#endif

// direction
#ifndef GSL_SPI6_DIRECTION
#define GSL_SPI6_DIRECTION GSL_SPI_DIRECTION_DEFAULT
#endif

// clock polarity
#ifndef GSL_SPI6_POLARITY
#define GSL_SPI6_POLARITY GSL_SPI_POLARITY_DEFAULT
#endif

// data is latched on first or second edge
#ifndef GSL_SPI6_PHASE
#define GSL_SPI6_PHASE GSL_SPI_PHASE_DEFAULT
#endif

// default AF mode
#ifndef GSL_SPI6_DEFAULT_AF_MODE
#define GSL_SPI6_DEFAULT_AF_MODE GSL_SPI_DEFAULT_AF_MODE
#endif

// priority
#ifndef GSL_SPI6_PRIORITY
#define GSL_SPI6_PRIORITY GSL_SPI_PRIORITY_DEFAULT
#endif

// nss pin
// SPI6 NSS pin can be PG8 (all AF5)
#ifndef GSL_SPI6_PIN_NSS
#define GSL_SPI6_PIN_NSS kPinG8
#endif

// cs management mode
#ifndef GSL_SPI6_CS_MODE
#define GSL_SPI6_CS_MODE GSL_SPI_CS_MODE_DEFAULT
#endif

// first CS pin
#ifndef GSL_SPI6_PIN_CS1
#define GSL_SPI6_PIN_CS1 kPinNone
#endif

// second CS pin
#ifndef GSL_SPI6_PIN_CS2
#define GSL_SPI6_PIN_CS2 kPinNone
#endif

// third CS pin
#ifndef GSL_SPI6_PIN_CS3
#define GSL_SPI6_PIN_CS3 kPinNone
#endif

// fourth CS pin
#ifndef GSL_SPI6_PIN_CS4
#define GSL_SPI6_PIN_CS4 kPinNone
#endif

// sck pin
// SPI6 SCK pin can be PG13 (all AF5)
#ifndef GSL_SPI6_PIN_SCK
#define GSL_SPI6_PIN_SCK kPinG13
#endif

//  miso pin
// SPI6 MISO pin can be PG12 (all AF5)
#ifndef GSL_SPI6_PIN_MISO
#define GSL_SPI6_PIN_MISO kPinG12
#endif

// mosi pin
// SPI6 MOSI pin can be PG14 (all AF5)
#ifndef GSL_SPI6_PIN_MOSI
#define GSL_SPI6_PIN_MOSI kPinG14
#endif

// possible alternate function modes for SPI6
uint8_t gsl_spi6_afmode[] = {
#ifdef GPIO_AF5_SPI6
    GPIO_AF5_SPI6,
#endif
#ifdef GPIO_AF6_SPI6
    GPIO_AF6_SPI6,
#endif
#ifdef GPIO_AF7_SPI6
    GPIO_AF7_SPI6,
#endif
};

// total number of AF modes for SPI6
uint8_t gsl_spi6_afmode_count =
    sizeof(gsl_spi6_afmode) / sizeof(*gsl_spi6_afmode);

// SPI6 handle
SPI_HandleTypeDef gsl_spi6_hspi = {
    SPI6,
    {
        GSL_SPI6_MODE,
        GSL_SPI6_DIRECTION,
        GSL_SPI6_DATASIZE,
        GSL_SPI6_POLARITY,
        GSL_SPI6_PHASE,
        GSL_SPI6_NSS,
        GSL_SPI6_PRESCALER,
        GSL_SPI6_FIRSTBIT,
        SPI_TIMODE_DISABLE,
        SPI_CRCCALCULATION_DISABLE,
        7},
    nullptr,
    0,
    0,
    nullptr,
    0,
    0,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    HAL_UNLOCKED,
    HAL_SPI_STATE_RESET,
    HAL_SPI_ERROR_NONE};

// SPI6 pin settings
GSL_SPI_PinsStruct gsl_spi6_pins =  {
    GSL_SPI6_PIN_SCK,
#ifdef GSL_SPI6_PIN_SCK_AF_MODE
    GSL_SPI6_PIN_SCK_AF_MODE,
#else
    GSL_SPI6_DEFAULT_AF_MODE,
#endif
    GSL_SPI6_PIN_MOSI,
#ifdef GSL_SPI6_PIN_MOSI_AF_MODE
    GSL_SPI6_PIN_MOSI_AF_MODE,
#else
    GSL_SPI6_DEFAULT_AF_MODE,
#endif
    GSL_SPI6_PIN_MISO,
#ifdef GSL_SPI6_PIN_MISO_AF_MODE
    GSL_SPI6_PIN_MISO_AF_MODE,
#else
    GSL_SPI6_DEFAULT_AF_MODE,
#endif
    GSL_SPI6_PIN_NSS,
#ifdef GSL_SPI6_PIN_NSS_AF_MODE
    GSL_SPI6_PIN_NSS_AF_MODE,
#else
    GSL_SPI6_DEFAULT_AF_MODE,
#endif
    {GSL_SPI6_PIN_CS1, GSL_SPI6_PIN_CS2, GSL_SPI6_PIN_CS3, GSL_SPI6_PIN_CS4}};

// initialize SPI6 DMA settings

// SPI6 TX --> DMA 2 stream 5 channel 1
#define GSL_SPI6_DMA_TX_STREAM DMA2_Stream5
#define GSL_SPI6_DMA_TX_CHANNEL DMA_CHANNEL_1

// SPI6 RX --> DMA 2 stream 6 channel 1
#define GSL_SPI6_DMA_RX_STREAM DMA2_Stream6
#define GSL_SPI6_DMA_RX_CHANNEL DMA_CHANNEL_1

DMA_HandleTypeDef gsl_spi6_hdmatx = {
    GSL_SPI6_DMA_TX_STREAM,
    {
        GSL_SPI6_DMA_TX_CHANNEL,
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

DMA_HandleTypeDef gsl_spi6_hdmarx = {
    GSL_SPI6_DMA_RX_STREAM,
    {
        GSL_SPI6_DMA_RX_CHANNEL,
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

#endif // #ifdef SPI6

// this holds pointers to the various structures we need
struct GSL_SPI_InfoStruct {
  SPI_TypeDef * SPIx;
  SPI_HandleTypeDef * handle;
  GSL_SPI_PinsStruct * pins;
  uint8_t * af_mode;
  uint8_t af_mode_count;
  DMA_HandleTypeDef * hdmatx;
  DMA_HandleTypeDef * hdmarx;
  void (*CompleteCallback)(SPI_TypeDef *);
  void (*HalfCompleteCallback)(SPI_TypeDef *);
  GSL_SPI_CSModeEnum cs_mode;
  PinEnum current_cs;
  uint8_t priority;
};

// here we define the info structure which points to all the various other
// structures
GSL_SPI_InfoStruct GSL_SPI_Info[] = {

#ifdef SPI1
    {
        SPI1,
        &gsl_spi1_hspi,
        &gsl_spi1_pins,
        gsl_spi1_afmode,
        gsl_spi1_afmode_count,
        &gsl_spi1_hdmatx,
        &gsl_spi1_hdmarx,
        nullptr,
        nullptr,
        GSL_SPI1_CS_MODE,
        gsl_spi1_pins.nss,
        GSL_SPI1_PRIORITY,
    },
#endif // #ifdef SPI1

#ifdef SPI2
    {
        SPI2,
        &gsl_spi2_hspi,
        &gsl_spi2_pins,
        gsl_spi2_afmode,
        gsl_spi2_afmode_count,
        &gsl_spi2_hdmatx,
        &gsl_spi2_hdmarx,
        nullptr,
        nullptr,
        GSL_SPI2_CS_MODE,
        gsl_spi2_pins.nss,
        GSL_SPI2_PRIORITY,
    },
#endif // #ifdef SPI2

#ifdef SPI3
    {
        SPI3,
        &gsl_spi3_hspi,
        &gsl_spi3_pins,
        gsl_spi3_afmode,
        gsl_spi3_afmode_count,
        &gsl_spi3_hdmatx,
        &gsl_spi3_hdmarx,
        nullptr,
        nullptr,
        GSL_SPI3_CS_MODE,
        gsl_spi3_pins.nss,
        GSL_SPI3_PRIORITY,
    },
#endif // #ifdef SPI3

#ifdef SPI4
    {
        SPI4,
        &gsl_spi4_hspi,
        &gsl_spi4_pins,
        gsl_spi4_afmode,
        gsl_spi4_afmode_count,
        &gsl_spi4_hdmatx,
        &gsl_spi4_hdmarx,
        nullptr,
        nullptr,
        GSL_SPI4_CS_MODE,
        gsl_spi4_pins.nss,
        GSL_SPI4_PRIORITY,
    },
#endif // #ifdef SPI4

#ifdef SPI5
    {
        SPI5,
        &gsl_spi5_hspi,
        &gsl_spi5_pins,
        gsl_spi5_afmode,
        gsl_spi5_afmode_count,
        &gsl_spi5_hdmatx,
        &gsl_spi5_hdmarx,
        nullptr,
        nullptr,
        GSL_SPI5_CS_MODE,
        gsl_spi5_pins.nss,
        GSL_SPI5_PRIORITY,
    },
#endif // #ifdef SPI5

#ifdef SPI6
    {
        SPI6,
        &gsl_spi6_hspi,
        &gsl_spi6_pins,
        gsl_spi6_afmode,
        gsl_spi6_afmode_count,
        &gsl_spi6_hdmatx,
        &gsl_spi6_hdmarx,
        nullptr,
        nullptr,
        GSL_SPI6_CS_MODE,
        gsl_spi6_pins.nss,
        GSL_SPI6_PRIORITY,
    },
#endif // #ifdef SPI6

};

// number of SPI peripherals
const uint16_t GSL_SPI_PeripheralCount =
    sizeof(GSL_SPI_Info) / sizeof(*GSL_SPI_Info);

// bytes of RAM used to store all the handler structs
const uint16_t GSL_SPI_StorageByteCount =
    sizeof(GSL_SPI_Info) +
    GSL_SPI_PeripheralCount * sizeof(SPI_HandleTypeDef) +
    GSL_SPI_PeripheralCount * sizeof(GSL_SPI_PinsStruct) +
    2 * GSL_SPI_PeripheralCount * sizeof(DMA_HandleTypeDef);
