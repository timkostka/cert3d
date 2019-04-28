#pragma once

#include "gsl_includes.h"

// convert a SPI pointer to a 0-based integer
// e.g. SPI1 -> 0
uint32_t GSL_SPIM_GetNumber(SPI_TypeDef * SPIx) {
  for (uint32_t i = 0; i < GSL_SPIM_PeripheralCount; ++i) {
    if (GSL_SPIM_Info[i]->SPIx == SPIx) {
      return i;
    }
  }
  HALT("Invalid parameter");
  return -1;
}

// return pointer to the info struct
GSL_SPIM_InfoStruct * GSL_SPIM_GetInfo(SPI_TypeDef * SPIx) {
  return GSL_SPIM_Info[GSL_SPIM_GetNumber(SPIx)];
}

// return true if the buffer is empty
bool GSL_SPIM_IsEmpty(SPI_TypeDef * SPIx) {
  return GSL_SPIM_GetInfo(SPIx)->buffer.GetSizeInUse() == 0;
}

// start a new transfer if data is ready to send
void GSL_SPIM_SendIfPossible(SPI_TypeDef * SPIx) {
  // get the infos
  auto info = GSL_SPIM_GetInfo(SPIx);
  auto spi_info = GSL_SPI_GetInfo(SPIx);
  // to prevent a race condition, disable interrupts briefly
  __disable_irq();
  // if SPIx is free and the buffer isn't frozen, then try to send
  if ((spi_info->handle->State == HAL_SPI_STATE_READY ||
       spi_info->handle->State == HAL_SPI_STATE_BUSY_RX) &&
      info->buffer.GetFrozenBufferSize() == 0) {
    // freeze some portion of the buffer
    info->buffer.Freeze();
    // if frozen region is nonzero, then send it out
    if (info->buffer.GetFrozenBufferSize()) {
      //
      GSL_SPI_ManageCSLow(GSL_SPI_GetInfo(SPIx));
      GSL_SPI_SendMulti_DMA(SPIx,
                             info->buffer.frozen_buffer_start_,
                             info->buffer.GetFrozenBufferSize());
    }
  }
  // re-enable interrupts
  __enable_irq();
}

// transfer complete handler
void GSL_SPIM_TransferComplete(SPI_TypeDef * SPIx) {
  // get the info
  auto info = GSL_SPIM_GetInfo(SPIx);
  // mark bytes as sent
  info->processed_bytes += info->buffer.GetFrozenBufferSize();
  // unfreeze the bytes that were sent, if any
  info->buffer.Thaw();
  // send out more data if possible
  GSL_SPIM_SendIfPossible(SPIx);
}

// initialize
void GSL_SPIM_Initialize(SPI_TypeDef * SPIx) {
  // get the info
  auto info = GSL_SPIM_GetInfo(SPIx);
  // if already active, do nothing
  if (info->active) {
    return;
  }
  // set up the buffer
  info->buffer.SetCapacity(info->capacity);
  // ensure the mode is master
  ASSERT_EQ(GSL_SPI_GetInfo(SPIx)->handle->Init.Mode, SPI_MODE_MASTER);
  // set up transfer complete callback
  GSL_SPI_SetCompleteCallback(
      SPIx,
      GSL_SPIM_TransferComplete);
  // initialize the SPI
  GSL_SPI_Initialize(SPIx);
  // mark it as active
  info->active = true;
}

// send out raw bytes
void GSL_SPIM_Send(SPI_TypeDef * SPIx, uint8_t * data, uint16_t length) {
  // if not sending any data, just return
  if (length == 0) {
    return;
  }
  // get the info
  auto info = GSL_SPIM_GetInfo(SPIx);
  // if not initialized, then initialize
  if (!info->active) {
    GSL_SPIM_Initialize(SPIx);
  }
  // add to the buffer
  if (info->buffer.GetCapacityRemaining() >= length) {
    info->buffer.AddBytes(data, length);
  } else {
    uint16_t size = info->buffer.GetCapacityRemaining();
    info->buffer.AddBytes(data, size);
    // discard the rest, flag something here
    LOG("\nERROR: Bytes were discarded in GSL_SPIM_Send");
    LOG("\n- Sent ", size, " out of ", length, " bytes");
    LOG("\n- Capacity: ", size, "/", info->buffer.capacity_);
  }
  // try to send if possible
  GSL_SPIM_SendIfPossible(SPIx);
}

// send out a null-terminated string
void GSL_SPIM_Send(SPI_TypeDef * SPIx, const char * message) {
  GSL_SPIM_Send(SPIx, (uint8_t *) message, strlen(message));
}
