#pragma once

// This provides a interface to configure and use the SPI peripheral.

#include "gsl_includes.h"

// convert an SPI pointer to a 0-based integer
// e.g. SPI1 -> 0
uint32_t GSL_SPI_GetNumber(SPI_TypeDef * SPIx) {
  for (uint32_t i = 0; i < GSL_SPI_PeripheralCount; ++i) {
    if (GSL_SPI_Info[i].SPIx == SPIx) {
      return i;
    }
  }
  HALT("Invalid parameter");
  return -1;
}

// enable the clock
void GSL_SPI_EnableClock(SPI_TypeDef * SPIx) {
  if (false) {
#ifdef SPI1
  } else if (SPIx == SPI1) {
    __HAL_RCC_SPI1_CLK_ENABLE();
#endif
#ifdef SPI2
  } else if (SPIx == SPI2) {
    __HAL_RCC_SPI2_CLK_ENABLE();
#endif
#ifdef SPI3
  } else if (SPIx == SPI3) {
    __HAL_RCC_SPI3_CLK_ENABLE();
#endif
#ifdef SPI4
  } else if (SPIx == SPI4) {
    __HAL_RCC_SPI4_CLK_ENABLE();
#endif
#ifdef SPI5
  } else if (SPIx == SPI5) {
    __HAL_RCC_SPI5_CLK_ENABLE();
#endif
#ifdef SPI6
  } else if (SPIx == SPI6) {
    __HAL_RCC_SPI6_CLK_ENABLE();
#endif
  } else {
    // TODO: make error
    LOG("unknown value");
  }
}

// return pointer to the info struct
GSL_SPI_InfoStruct * GSL_SPI_GetInfo(SPI_TypeDef * SPIx) {
  return &GSL_SPI_Info[GSL_SPI_GetNumber(SPIx)];
}

// return the IRQ for the given USART peripheral
// e.g. SPI1 -> SPI1_IRQn
IRQn_Type GSL_SPI_GetIRQ(SPI_TypeDef * SPIx) {
  if (0) {
#ifdef SPI1
  } else if (SPIx == SPI1) {
    return SPI1_IRQn;
#endif
#ifdef SPI2
  } else if (SPIx == SPI2) {
    return SPI2_IRQn;
#endif
#ifdef SPI3
  } else if (SPIx == SPI3) {
    return SPI3_IRQn;
#endif
#ifdef SPI4
  } else if (SPIx == SPI4) {
    return SPI4_IRQn;
#endif
#ifdef SPI5
  } else if (SPIx == SPI5) {
    return SPI5_IRQn;
#endif
#ifdef SPI6
  } else if (SPIx == SPI6) {
    return SPI6_IRQn;
#endif
  } else {
    HALT("Unknown parameter");
    return NonMaskableInt_IRQn;
  }
}

// set up the callback routine for SPI completion
void GSL_SPI_SetCompleteCallback(
    SPI_TypeDef * SPIx,
    void (*callback_function)(SPI_TypeDef *)) {
  // get the handle
  GSL_SPI_InfoStruct * info = GSL_SPI_GetInfo(SPIx);
  // set the routine
  info->CompleteCallback = callback_function;
}

// set up the callback routine for SPI half completion
void GSL_SPI_SetHalfCompleteCallback(
    SPI_TypeDef * SPIx,
    void (*callback_function)(SPI_TypeDef *)) {
  // get the handle
  GSL_SPI_InfoStruct * info = GSL_SPI_GetInfo(SPIx);
  // set the routine
  info->HalfCompleteCallback = callback_function;
}

void GSL_SPI_CheckAFMode(GSL_SPI_InfoStruct * info,
                         uint32_t & af_mode) {
  if (af_mode == GSL_SPI_DEFAULT_AF_MODE) {
    // if more than one AF mode is possible, show a warning
    if (info->af_mode_count > 1) {
      LOG("\n\nWARNING: There are multiple AF modes for the SPI");
      LOG(GSL_SPI_GetNumber(info->handle->Instance) + 1, " peripheral.  We may");
      LOG("\nnot set the right one automatically.  Defined modes:");
      for (uint8_t i = 0; i < info->af_mode_count; ++i) {
        LOG("\n* AF", info->af_mode[i]);
        if (i == 0) {
          LOG(" (this one will be used)");
        }
      }
      LOG("\nTo hard code a value, set the GSL_SPIx_AF_MODE or pin specific");
      LOG("\ndefine to the appropriate value.");
    }
    af_mode = info->af_mode[0];
  }
}

// initialize SPI using #defined values
void GSL_SPI_Initialize(SPI_TypeDef * SPIx) {
  // get the handle
  GSL_SPI_InfoStruct * info = GSL_SPI_GetInfo(SPIx);
  // if already initialized, don't do anything
  if (info->handle->State != HAL_SPI_STATE_RESET) {
    return;
  }
  // initialize AF mode
  // initialize pins
  GSL_SPI_CheckAFMode(info, info->pins->sck_af_mode);
  GSL_PIN_InitializeAF(
      info->pins->sck,
      GPIO_MODE_AF_PP,
      GPIO_NOPULL,
      info->pins->sck_af_mode);
  // mosi pin
  // two lines
  // slave, receive only
  // master, send only
  if (info->handle->Init.Direction == SPI_DIRECTION_2LINES ||
      (info->handle->Init.Mode == SPI_MODE_MASTER &&
       info->handle->Init.Direction == SPI_DIRECTION_1LINE) ||
      (info->handle->Init.Mode == SPI_MODE_SLAVE &&
       info->handle->Init.Direction == SPI_DIRECTION_2LINES_RXONLY)) {
    GSL_SPI_CheckAFMode(info, info->pins->mosi_af_mode);
    GSL_PIN_InitializeAF(
        info->pins->mosi,
        GPIO_MODE_AF_PP,
        GPIO_NOPULL,
        info->pins->mosi_af_mode);
  }
  // miso pin
  if (info->handle->Init.Direction == SPI_DIRECTION_2LINES ||
      (info->handle->Init.Mode == SPI_MODE_SLAVE &&
       info->handle->Init.Direction == SPI_DIRECTION_1LINE) ||
      (info->handle->Init.Mode == SPI_MODE_MASTER &&
       info->handle->Init.Direction == SPI_DIRECTION_2LINES_RXONLY)) {
    GSL_SPI_CheckAFMode(info, info->pins->miso_af_mode);
    GSL_PIN_InitializeAF(
        info->pins->miso,
        GPIO_MODE_AF_PP,
        GPIO_NOPULL,
        info->pins->miso_af_mode);
  }
  // nss pin
  if (info->handle->Init.NSS == SPI_NSS_HARD_OUTPUT) {
    GSL_SPI_CheckAFMode(info, info->pins->nss_af_mode);
    GSL_PIN_InitializeAF(
        info->pins->nss,
        GPIO_MODE_AF_PP,
        GPIO_NOPULL,
        info->pins->nss_af_mode);
  } else if (info->handle->Init.NSS == SPI_NSS_HARD_INPUT) {
    GSL_SPI_CheckAFMode(info, info->pins->nss_af_mode);
    GSL_PIN_InitializeAF(
        info->pins->nss,
        GPIO_MODE_AF_PP,
        GPIO_PULLUP,
        info->pins->nss_af_mode);
  } else if (info->handle->Init.NSS == SPI_NSS_SOFT) {
    if (info->pins->nss != kPinNone) {
      GSL_PIN_SetHigh(info->pins->nss);
    }
    GSL_PIN_Initialize(
        info->pins->nss,
        GPIO_MODE_OUTPUT_PP,
        GPIO_NOPULL);
  }
  // initialize CS pins
  for (uint8_t i = 0; i < GSL_SPI_CSPinCount; ++i) {
    if (info->pins->cs[i] != kPinNone) {
      GSL_PIN_Initialize(info->pins->cs[i], GPIO_MODE_OUTPUT_PP);
      GSL_PIN_SetHigh(info->pins->cs[i]);
    }
  }
  // enable the clock
  GSL_SPI_EnableClock(SPIx);
  // enable interrupts
  {
    IRQn_Type irq = GSL_SPI_GetIRQ(SPIx);
    HAL_NVIC_SetPriority(irq, info->priority, 0);
    HAL_NVIC_EnableIRQ(irq);
  }
  // initialize
  HAL_RUN(HAL_SPI_Init(info->handle));
  // link DMA TX
  __HAL_LINKDMA(info->handle, hdmatx, *(info->hdmatx));
  // link DMA RX
  __HAL_LINKDMA(info->handle, hdmarx, *(info->hdmarx));
}

// deinitialize the SPI peripheral
void GSL_SPI_Deinitialize(SPI_TypeDef * SPIx) {
  // get the handle
  GSL_SPI_InfoStruct * info = GSL_SPI_GetInfo(SPIx);
  // if already deinitialized, don't do anything
  if (info->handle->State == HAL_SPI_STATE_RESET) {
    return;
  }
  // deinitialize pins
  // sck pin
  GSL_PIN_Deinitialize(info->pins->sck);
  // mosi pin
  if (info->handle->Init.Direction == SPI_DIRECTION_2LINES ||
      (info->handle->Init.Mode == SPI_MODE_MASTER &&
       info->handle->Init.Direction == SPI_DIRECTION_1LINE) ||
      (info->handle->Init.Mode == SPI_MODE_SLAVE &&
       info->handle->Init.Direction == SPI_DIRECTION_2LINES_RXONLY)) {
    GSL_PIN_Deinitialize(info->pins->mosi);
  }
  // miso pin
  if (info->handle->Init.Direction == SPI_DIRECTION_2LINES ||
      (info->handle->Init.Mode == SPI_MODE_SLAVE &&
       info->handle->Init.Direction == SPI_DIRECTION_1LINE) ||
      (info->handle->Init.Mode == SPI_MODE_MASTER &&
       info->handle->Init.Direction == SPI_DIRECTION_2LINES_RXONLY)) {
    GSL_PIN_Deinitialize(info->pins->miso);
  }
  // nss pin
  if (info->handle->Init.NSS == SPI_NSS_HARD_INPUT ||
      info->handle->Init.NSS == SPI_NSS_HARD_OUTPUT ||
      info->handle->Init.NSS == SPI_NSS_SOFT) {
    GSL_PIN_Deinitialize(info->pins->nss);
  }
  // pins
  for (uint8_t i = 0; i < GSL_SPI_CSPinCount; ++i) {
    if (info->pins->cs[i] != kPinNone) {
      GSL_PIN_Deinitialize(info->pins->cs[i]);
    }
  }
  // disable interrupt
  HAL_NVIC_DisableIRQ(GSL_SPI_GetIRQ(SPIx));
  // deinitialize
  HAL_RUN(HAL_SPI_DeInit(info->handle));
  // if pointers are not null, then deinitialize DMA functions
  // deinitialize DMA TX
  if (info->hdmatx) {
    GSL_DMA_UnreserveStream(info->hdmatx->Instance);
    HAL_RUN(HAL_DMA_DeInit(info->hdmatx));
    IRQn_Type irq = GSL_DMA_GetIRQ(info->hdmatx->Instance);
    HAL_NVIC_DisableIRQ(irq);
    // unreserve this stream
  }
  // deinitialize DMA RX
  if (info->hdmarx) {
    GSL_DMA_UnreserveStream(info->hdmarx->Instance);
    HAL_RUN(HAL_DMA_DeInit(info->hdmarx));
    IRQn_Type irq = GSL_DMA_GetIRQ(info->hdmarx->Instance);
    HAL_NVIC_DisableIRQ(irq);
  }
}

// ensure the given SPI is enabled
// if not, initialize it
/*
void GSL_SPI_CheckEnabled(SPI_HandleTypeDef * hspi) {
  if (hspi->State == HAL_SPI_STATE_RESET) {
    // not initialized, go initialize it
    GSL_SPI_Initialize(hspi->Instance);
  }
}*/

// get the associated AHB clock speed
// SPI2 and SPI3 are APB1, rest are APB2
uint32_t GSL_SPI_GetAHBSpeed(SPI_TypeDef * SPIx) {
  if (0) {
#ifdef SPI1
  } else if (SPIx == SPI1) {
    return HAL_RCC_GetPCLK2Freq();
#endif
#ifdef SPI2
  } else if (SPIx == SPI2) {
    return HAL_RCC_GetPCLK1Freq();
#endif
#ifdef SPI3
  } else if (SPIx == SPI3) {
    return HAL_RCC_GetPCLK1Freq();
#endif
#ifdef SPI4
  } else if (SPIx == SPI4) {
    return HAL_RCC_GetPCLK2Freq();
#endif
#ifdef SPI5
  } else if (SPIx == SPI5) {
    return HAL_RCC_GetPCLK2Freq();
#endif
#ifdef SPI6
  } else if (SPIx == SPI6) {
    return HAL_RCC_GetPCLK2Freq();
#endif
  } else {
    HALT("Invalid parameter");
  }
}

// Set the SPI speed up to the given rate.  Since the SPI clock must
// be a multiple of the system clock, it may not be possible to set
// it to an exact value.
void GSL_SPI_SetMaxSpeed(SPI_TypeDef * SPIx, uint32_t target_speed) {
  // get the handle
  GSL_SPI_InfoStruct * info = GSL_SPI_GetInfo(SPIx);
  // if it's initialized already, uninitialize it
  GSL_SPI_Deinitialize(SPIx);
  // find the maximum and minimum speeds
  uint32_t prescaler = SPI_BAUDRATEPRESCALER_2;
  uint32_t speed = GSL_SPI_GetAHBSpeed(SPIx) / 2;
  while (speed > target_speed && prescaler != SPI_BAUDRATEPRESCALER_256) {
    speed /= 2;
    prescaler += 0x08;
  }
  info->handle->Init.BaudRatePrescaler = prescaler;
}

// return the SPI clock speed
uint32_t GSL_SPI_GetSpeed(SPI_TypeDef * SPIx) {
  // get the handle
  GSL_SPI_InfoStruct * info = GSL_SPI_GetInfo(SPIx);
  // get the prescaler
  uint32_t prescaler = info->handle->Init.BaudRatePrescaler / 0x08;
  uint32_t multiple = 2;
  while (prescaler) {
    --prescaler;
    multiple *= 2;
  }
  return GSL_SPI_GetAHBSpeed(SPIx) / multiple;
}

// SPI IRQ handlers
extern "C" {

#ifdef SPI1
void SPI1_IRQHandler(void) {
  HAL_SPI_IRQHandler(&gsl_spi1_hspi);
}
#endif

#ifdef SPI2
void SPI2_IRQHandler(void) {
  HAL_SPI_IRQHandler(&gsl_spi2_hspi);
}
#endif

#ifdef SPI3
void SPI3_IRQHandler(void) {
  HAL_SPI_IRQHandler(&gsl_spi3_hspi);
}
#endif

#ifdef SPI4
void SPI4_IRQHandler(void) {
  HAL_SPI_IRQHandler(&gsl_spi4_hspi);
}
#endif

#ifdef SPI5
void SPI5_IRQHandler(void) {
  HAL_SPI_IRQHandler(&gsl_spi5_hspi);
}
#endif

#ifdef SPI6
void SPI6_IRQHandler(void) {
  HAL_SPI_IRQHandler(&gsl_spi6_hspi);
}
#endif

}

// add and initialize a CS pin for the given peripheral
void GSL_SPI_AddCSPin(SPI_TypeDef * SPIx, PinEnum pin) {
  // get the info
  GSL_SPI_InfoStruct * info = GSL_SPI_GetInfo(SPIx);  // see if the pin already exists somewhere
  bool found = false;
  if (info->pins->nss == pin) {
    found = true;
  }
  for (uint8_t i = 0; i < GSL_SPI_CSPinCount; ++i) {
    if (info->pins->cs[i] == pin) {
      found = true;
    }
  }
  // if not found, then add it
  if (!found) {
    bool added = false;
    if (info->pins->nss == kPinNone) {
      info->pins->nss = pin;
      added = true;
    } else {
      for (uint8_t i = 0; i < GSL_SPI_CSPinCount; ++i) {
        if (info->pins->cs[i] == kPinNone) {
          info->pins->cs[i] = pin;
          added = true;
          break;
        }
      }
    }
    if (!added) {
      HALT("Too many CS pins");
    }
  }
  // if the peripheral is not initialized, don't initialize this pin
  if (info->handle->State == HAL_SPI_STATE_RESET) {
    return;
  }
  // initialize this pin and set it high
  GSL_PIN_Initialize(pin, GPIO_MODE_OUTPUT_PP);
  GSL_PIN_SetHigh(pin);
}

// set the current CS pin used for future routines
void GSL_SPI_SetCSPin(SPI_TypeDef * SPIx, PinEnum pin) {
  // get the info
  GSL_SPI_InfoStruct * info = GSL_SPI_GetInfo(SPIx);
  // TODO:check to see if pin exists
  // set the new pin
  info->current_cs = pin;
}

// report an SPI error and halt the program
/*
void GSL_SPI_ReportError(HAL_StatusTypeDef status) {
  if (status == HAL_OK) {
    return;
  }
  LOG("\nERROR: Encountered SPI error code ", (uint32_t) status);
  HALT("ERROR");
}*/

// wait until both the TX and RX on the given SPI bus is available or
// timeout expires
void GSL_SPI_Wait(SPI_HandleTypeDef *hspi, uint32_t timeout_ms = 1000) {
  // get the start time
  uint32_t start = GSL_DEL_Ticks();
  // initialize if not already done
  if (hspi->State == HAL_SPI_STATE_RESET) {
    GSL_SPI_Initialize(hspi->Instance);
  }
  // wait until it's ready or the timeout occurs
  while (GSL_DEL_ElapsedMS(start) < timeout_ms &&
      hspi->State != HAL_SPI_STATE_READY) {
  }
  if (hspi->State != HAL_SPI_STATE_READY) {
    LOG("\nTimeout on SPI");
    HALT("ERROR");
  }
}

// wait until the TX on the given SPI bus is available or timeout expires
void GSL_SPI_WaitTX(SPI_HandleTypeDef *hspi, uint32_t timeout_ms = 1000) {
  // get the start time
  uint32_t start = GSL_DEL_Ticks();
  // initialize if not already done
  if (hspi->State == HAL_SPI_STATE_RESET) {
    GSL_SPI_Initialize(hspi->Instance);
  }
  // wait until TX is ready or the timeout occurs
  while (GSL_DEL_ElapsedMS(start) < timeout_ms &&
      hspi->State != HAL_SPI_STATE_READY &&
      hspi->State != HAL_SPI_STATE_BUSY_RX) {
  }
  if (hspi->State != HAL_SPI_STATE_READY &&
      hspi->State != HAL_SPI_STATE_BUSY_RX) {
    LOG("\nTimeout on SPI");
    HALT("ERROR");
  }
}

// wait until the RX on the given SPI bus is available or timeout expires
void GSL_SPI_WaitRX(SPI_HandleTypeDef *hspi, uint32_t timeout_ms = 1000) {
  // get the start time
  uint32_t start = GSL_DEL_Ticks();
  // initialize if not already done
  if (hspi->State == HAL_SPI_STATE_RESET) {
    GSL_SPI_Initialize(hspi->Instance);
  }
  // wait until the RX is ready or the timeout occurs
  while (GSL_DEL_ElapsedMS(start) < timeout_ms &&
      hspi->State != HAL_SPI_STATE_READY &&
      hspi->State != HAL_SPI_STATE_BUSY_TX) {
  }
  if (hspi->State != HAL_SPI_STATE_READY &&
      hspi->State != HAL_SPI_STATE_BUSY_TX) {
    LOG("\nTimeout on SPI");
    HALT("ERROR");
  }
}

// wait until the given SPI bus is available or timeout expires
void GSL_SPI_Wait(SPI_TypeDef * SPIx, uint32_t timeout_ms = 1000) {
  GSL_SPI_Wait(GSL_SPI_GetInfo(SPIx)->handle, timeout_ms);
}

// wait until the given SPI bus is available or timeout expires
void GSL_SPI_WaitTX(SPI_TypeDef * SPIx, uint32_t timeout_ms = 1000) {
  GSL_SPI_WaitTX(GSL_SPI_GetInfo(SPIx)->handle, timeout_ms);
}

// wait until the given SPI bus is available or timeout expires
void GSL_SPI_WaitRX(SPI_TypeDef * SPIx, uint32_t timeout_ms = 1000) {
  GSL_SPI_WaitRX(GSL_SPI_GetInfo(SPIx)->handle, timeout_ms);
}

// set the CS pin low
void GSL_SPI_SetCSLow(SPI_TypeDef * SPIx) {
  GSL_SPI_InfoStruct * info = GSL_SPI_GetInfo(SPIx);
  ASSERT(GSL_PIN_IsReal(info->current_cs));
  GSL_PIN_SetLow(info->current_cs);
}

// set the CS pin high
void GSL_SPI_SetCSHigh(SPI_TypeDef * SPIx) {
  GSL_SPI_InfoStruct * info = GSL_SPI_GetInfo(SPIx);
  ASSERT(GSL_PIN_IsReal(info->current_cs));
  GSL_PIN_SetHigh(info->current_cs);
}

// if managed, set CS pin low
void GSL_SPI_ManageCSLow(GSL_SPI_InfoStruct * info) {
  if (info->handle->Init.Mode == SPI_MODE_MASTER &&
      info->cs_mode == GSL_SPI_CS_MODE_MANAGED &&
      info->current_cs != kPinNone) {
    GSL_PIN_SetLow(info->current_cs);
  }
}

// if managed, set CS pin high
void GSL_SPI_ManageCSHigh(GSL_SPI_InfoStruct * info) {
  if (info->handle->Init.Mode == SPI_MODE_MASTER &&
      info->cs_mode == GSL_SPI_CS_MODE_MANAGED &&
      info->current_cs != kPinNone) {
    GSL_PIN_SetHigh(info->current_cs);
  }
}

// set the CS management mode
void GSL_SPI_SetCSMode(SPI_TypeDef *SPIx, GSL_SPI_CSModeEnum mode) {
  GSL_SPI_GetInfo(SPIx)->cs_mode = mode;
}

// error handler
extern "C" {

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi) {
  LOG("\nAn error occurred on SPI ",
      GSL_OUT_Hex((uint32_t) hspi->Instance));
  LOG("\nError code: ", GSL_OUT_Hex(hspi->ErrorCode));
}

// transfer complete
void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi) {
  // get the info
  GSL_SPI_InfoStruct * info = GSL_SPI_GetInfo(hspi->Instance);
  // set CS pin high
  GSL_SPI_ManageCSHigh(info);
  // call the callback routine if one is defined
  if (info->CompleteCallback) {
    info->CompleteCallback(hspi->Instance);
  }
}

// receive complete
void HAL_SPI_RxCpltCallback(SPI_HandleTypeDef *hspi) {
  HAL_SPI_TxCpltCallback(hspi);
}

// transfer/receive complete
void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi) {
  HAL_SPI_TxCpltCallback(hspi);
}

void HAL_SPI_TxHalfCpltCallback(SPI_HandleTypeDef *hspi) {
  // get the info
  GSL_SPI_InfoStruct * info = GSL_SPI_GetInfo(hspi->Instance);
  // call the callback routine if one is defined
  if (info->HalfCompleteCallback) {
    info->HalfCompleteCallback(hspi->Instance);
  }
}

void HAL_SPI_RxHalfCpltCallback(SPI_HandleTypeDef *hspi) {
  HAL_SPI_TxHalfCpltCallback(hspi);
}

void HAL_SPI_TxRxHalfCpltCallback(SPI_HandleTypeDef *hspi) {
  HAL_SPI_TxHalfCpltCallback(hspi);
}

}

// send an 8-bit value
void GSL_SPI_Send(
    SPI_TypeDef * SPIx,
    uint8_t value) {
  // get the info
  GSL_SPI_InfoStruct * info = GSL_SPI_GetInfo(SPIx);
  // wait until bus is free
  GSL_SPI_WaitTX(info->handle);
  GSL_SPI_ManageCSLow(info);
  HAL_RUN(HAL_SPI_Transmit(info->handle, &value, 1, GSL_SPI_TIMEOUT));
  GSL_SPI_ManageCSHigh(info);
}

// send a 16-bit value in MSB first mode
void GSL_SPI_Send16(
    SPI_TypeDef * SPIx,
    uint16_t value) {
  // get the info
  GSL_SPI_InfoStruct * info = GSL_SPI_GetInfo(SPIx);
  // wait until bus is free
  GSL_SPI_WaitTX(info->handle);
  GSL_SPI_ManageCSLow(info);
  value = GSL_GEN_SwapEndian(value);
  HAL_RUN(
      HAL_SPI_Transmit(info->handle,
                       (uint8_t *) &value,
                       sizeof(value),
                       GSL_SPI_TIMEOUT));
  GSL_SPI_ManageCSHigh(info);
}

// send many 8-bit values
void GSL_SPI_SendMulti(
    SPI_TypeDef * SPIx,
    const uint8_t * data_out,
    uint16_t count) {
  // get the info
  GSL_SPI_InfoStruct * info = GSL_SPI_GetInfo(SPIx);
  // wait until bus is free
  GSL_SPI_WaitTX(info->handle);
  if (count) {
    GSL_SPI_ManageCSLow(info);
    HAL_RUN(
        HAL_SPI_Transmit(info->handle,
                         (uint8_t *) data_out,
                         count,
                         GSL_SPI_TIMEOUT));
    GSL_SPI_ManageCSHigh(info);
  }
}

// read an 8-bit value
uint8_t GSL_SPI_Read(
    SPI_TypeDef * SPIx) {
  uint8_t value;
  // get the info
  GSL_SPI_InfoStruct * info = GSL_SPI_GetInfo(SPIx);
  // wait until bus is free
  GSL_SPI_WaitRX(info->handle);
  GSL_SPI_ManageCSLow(info);
  HAL_RUN(HAL_SPI_Receive(info->handle, &value, 1, GSL_SPI_TIMEOUT));
  GSL_SPI_ManageCSHigh(info);
  return value;
}

// read many 8-bit values
void GSL_SPI_ReadMulti(
    SPI_TypeDef * SPIx,
    uint8_t * data_in,
    uint16_t count) {
  // get the info
  GSL_SPI_InfoStruct * info = GSL_SPI_GetInfo(SPIx);
  // wait until bus is free
  GSL_SPI_WaitRX(info->handle);
  GSL_SPI_ManageCSLow(info);
  if (count) {
    HAL_RUN(HAL_SPI_Receive(info->handle, data_in, count, GSL_SPI_TIMEOUT));
  }
  GSL_SPI_ManageCSHigh(info);
}

// send and receive one 8-bit value
uint8_t GSL_SPI_SendRead(
    SPI_TypeDef * SPIx,
    uint8_t value_out) {
  uint8_t data_in;
  // get the info
  GSL_SPI_InfoStruct * info = GSL_SPI_GetInfo(SPIx);
  // wait until bus is free
  GSL_SPI_Wait(info->handle);
  GSL_SPI_ManageCSLow(info);
  HAL_RUN(
      HAL_SPI_TransmitReceive(
          info->handle,
          &value_out,
          &data_in,
          1,
          GSL_SPI_TIMEOUT));
  GSL_SPI_ManageCSHigh(info);
  return data_in;
}

// send and receive many 8-bit values
void GSL_SPI_SendReadMulti(
    SPI_TypeDef * SPIx,
    uint8_t * data_out,
    uint8_t * data_in,
    uint16_t count) {
  // get the info
  GSL_SPI_InfoStruct * info = GSL_SPI_GetInfo(SPIx);
  // wait until bus is free
  GSL_SPI_Wait(info->handle);
  GSL_SPI_ManageCSLow(info);
  if (count) {
    HAL_RUN(
        HAL_SPI_TransmitReceive(
            info->handle,
            data_out,
            data_in,
            count,
            GSL_SPI_TIMEOUT));
  }
  GSL_SPI_ManageCSHigh(info);
}

// start send via interrupt
void GSL_SPI_SendMulti_IT(
    SPI_TypeDef * SPIx,
    uint8_t * data_out,
    uint16_t count) {
  SPI_HandleTypeDef * hspi = GSL_SPI_GetInfo(SPIx)->handle;
  // wait for SPI to be ready
  GSL_SPI_WaitTX(hspi);
  // start asynchronous transfer
  HAL_RUN(HAL_SPI_Transmit_IT(hspi, data_out, count));
}

// start receive via interrupt
void GSL_SPI_ReadMulti_IT(
    SPI_TypeDef * SPIx,
    uint8_t * data_in,
    uint16_t count) {
  SPI_HandleTypeDef * hspi = GSL_SPI_GetInfo(SPIx)->handle;
  // wait for SPI to be ready
  GSL_SPI_WaitRX(hspi);
  // start asynchronous transfer
  HAL_RUN(HAL_SPI_Receive_IT(hspi, data_in, count));
}

// start receive via interrupt
void GSL_SPI_SendReadMulti_IT(
    SPI_TypeDef * SPIx,
    uint8_t * data_out,
    uint8_t * data_in,
    uint16_t count) {
  SPI_HandleTypeDef * hspi = GSL_SPI_GetInfo(SPIx)->handle;
  // wait for SPI to be ready
  GSL_SPI_Wait(hspi);
  // start asynchronous transfer
  HAL_RUN(HAL_SPI_TransmitReceive_IT(hspi, data_out, data_in, count));
}

// start send via DMA
void GSL_SPI_SendMulti_DMA(
    SPI_TypeDef * SPIx,
    uint8_t * data_out,
    uint16_t count) {
  GSL_SPI_InfoStruct * info = GSL_SPI_GetInfo(SPIx);
  SPI_HandleTypeDef * hspi = info->handle;
  // wait for SPI to be ready
  GSL_SPI_WaitTX(hspi);
  // set one-shot mode
  hspi->hdmatx->Init.Mode = DMA_NORMAL;
  // initialize dma
  GSL_DMA_Initialize(hspi->hdmatx, info->priority);
  // start transfer via DMA
  // TODO: why is this needed?  seems like a hold-over from UART
  bool busy_reading = hspi->State == HAL_SPI_STATE_BUSY_RX;
  if (busy_reading) {
    hspi->State = HAL_SPI_STATE_READY;
  }
  GSL_SPI_ManageCSLow(info);
  HAL_RUN(HAL_SPI_Transmit_DMA(hspi, data_out, count));
  if (busy_reading) {
    hspi->State = HAL_SPI_STATE_BUSY_TX_RX;
  }
}

// start send via circular DMA
void GSL_SPI_SendMulti_DMA_Circular(
    SPI_TypeDef * SPIx,
    uint8_t * data_out,
    uint16_t count) {
  GSL_SPI_InfoStruct * info = GSL_SPI_GetInfo(SPIx);
  SPI_HandleTypeDef * hspi = info->handle;
  // wait for SPI to be ready
  GSL_SPI_WaitTX(hspi);
  // set circular mode
  hspi->hdmatx->Init.Mode = DMA_CIRCULAR;
  // initialize dma
  GSL_DMA_Initialize(hspi->hdmatx, info->priority);
  // disable the IRQ
  // DEBUG
  //HAL_NVIC_DisableIRQ(GSL_DMA_GetIRQ(hspi->hdmatx->Instance));
  // start transfer via DMA
  bool busy_reading = hspi->State == HAL_SPI_STATE_BUSY_RX;
  if (busy_reading) {
    hspi->State = HAL_SPI_STATE_READY;
  }
  HAL_RUN(HAL_SPI_Transmit_DMA(hspi, data_out, count));
  if (busy_reading) {
    hspi->State = HAL_SPI_STATE_BUSY_TX_RX;
  }
}

// start receive via DMA
void GSL_SPI_ReadMulti_DMA(
    SPI_TypeDef * SPIx,
    uint8_t * data_in,
    uint16_t count) {
  GSL_SPI_InfoStruct * info = GSL_SPI_GetInfo(SPIx);
  SPI_HandleTypeDef * hspi = info->handle;
  // wait for SPI to be ready
  GSL_SPI_WaitRX(hspi);
  // set normal mode
  hspi->hdmarx->Init.Mode = DMA_NORMAL;
  // initialize dma
  GSL_DMA_Initialize(hspi->hdmarx, info->priority);
  // start transfer via DMA
  HAL_RUN(HAL_SPI_Receive_DMA(hspi, data_in, count));
}

// start receive via DMA in circular mode
void GSL_SPI_ReadMulti_DMA_Circular(
    SPI_TypeDef * SPIx,
    uint8_t * data_in,
    uint16_t count) {
  GSL_SPI_InfoStruct * info = GSL_SPI_GetInfo(SPIx);
  SPI_HandleTypeDef * hspi = info->handle;
  // wait for SPI to be ready
  GSL_SPI_WaitRX(hspi);
  // set circular mode
  hspi->hdmarx->Init.Mode = DMA_CIRCULAR;
  // initialize dma
  GSL_DMA_Initialize(hspi->hdmarx, info->priority);
  // start transfer via DMA
  HAL_RUN(HAL_SPI_Receive_DMA(hspi, data_in, count));
}

// start receive via DMA
void GSL_SPI_SendReadMulti_DMA(
    SPI_TypeDef * SPIx,
    uint8_t * data_out,
    uint8_t * data_in,
    uint16_t count) {
  GSL_SPI_InfoStruct * info = GSL_SPI_GetInfo(SPIx);
  SPI_HandleTypeDef * hspi = info->handle;
  // wait for SPI to be ready
  GSL_SPI_Wait(hspi);
  // set normal mode
  hspi->hdmatx->Init.Mode = DMA_NORMAL;
  hspi->hdmarx->Init.Mode = DMA_NORMAL;
  // initialize DMA
  GSL_DMA_Initialize(hspi->hdmatx, info->priority);
  GSL_DMA_Initialize(hspi->hdmarx, info->priority);
  // start transfer via DMA
  HAL_RUN(HAL_SPI_TransmitReceive_DMA(hspi, data_out, data_in, count));
}

// start receive via DMA circular mode
void GSL_SPI_SendReadMulti_DMA_Circular(
    SPI_TypeDef * SPIx,
    uint8_t * data_out,
    uint8_t * data_in,
    uint16_t count) {
  GSL_SPI_InfoStruct * info = GSL_SPI_GetInfo(SPIx);
  SPI_HandleTypeDef * hspi = info->handle;
  // wait for SPI to be ready
  GSL_SPI_Wait(hspi);
  // set circular mode
  hspi->hdmatx->Init.Mode = DMA_CIRCULAR;
  hspi->hdmarx->Init.Mode = DMA_CIRCULAR;
  // initialize DMA
  GSL_DMA_Initialize(hspi->hdmatx, info->priority);
  GSL_DMA_Initialize(hspi->hdmarx, info->priority);
  // start transfer via DMA
  HAL_RUN(HAL_SPI_TransmitReceive_DMA(hspi, data_out, data_in, count));
}

// given a peripheral and pin, return the corresponding AF mode
// Note: this comes from the "Alternate function mapping" table in the
// datasheet.
//
// e.g. (SPI3, kPinC10) -> GPIO_AF6_SPI3
// e.g. (SPI3, kPinD4) -> GPIO_AF5_SPI3
//
// Pins for a given SPIx may have different alternate functions.
/*
uint8_t GSL_SPI_GetAlternateFunctionMode(SPI_TypeDef * SPIx, PinEnum pin) {
  HALT("Incomplete");
  // TODO?
  if (0) {
  } else if (SPIx == SPI1) {
    // all SPI1 pins are AF5
    return GPIO_AF5_SPI1;
  } else {
    HALT("Invalid parameter");
    return 0;
  }
}*/

// return the actual clock speed in Hz
//void GSL_SPI_GetClock(void) {
  // TODO
//}
