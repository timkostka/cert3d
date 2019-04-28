#pragma once

// This file implements DMA-based SPI slave receivers.

#include "gsl_includes.h"

// convert an SPI pointer to a 0-based integer
// e.g. SPI1 -> 0
uint32_t GSL_SPIRX_GetNumber(SPI_TypeDef * SPIx) {
  for (uint32_t i = 0; i < GSL_SPIRX_PeripheralCount; ++i) {
    if (GSL_SPIRX_Info[i]->SPIx == SPIx) {
      return i;
    }
  }
  HALT("Invalid parameter");
  return -1;
}

// return pointer to the info struct
GSL_SPIRX_InfoStruct * GSL_SPIRX_GetInfo(SPI_TypeDef * SPIx) {
  return GSL_SPIRX_Info[GSL_SPIRX_GetNumber(SPIx)];
}

// return a pointer to the next stored byte
// note that bytes are stored asynchronously via DMA
uint8_t * GSL_SPIRX_NextStoredByte(SPI_TypeDef * SPIx) {
  // get info
  GSL_SPIRX_InfoStruct * info = GSL_SPIRX_GetInfo(SPIx);
  return info->buffer + info->capacity - info->hdmarx->Instance->NDTR;
}

// return the number of bytes available
uint16_t GSL_SPIRX_Available(SPI_TypeDef * SPIx) {
  // get info
  GSL_SPIRX_InfoStruct * info = GSL_SPIRX_GetInfo(SPIx);
  uint16_t count = info->capacity;
  count += GSL_SPIRX_NextStoredByte(SPIx) - info->next_data;
  count %= info->capacity;
  if (count > info->max_bytes_waiting) {
    info->max_bytes_waiting = count;
  }
  return count;
}

// return true if the buffer is empty
bool GSL_SPIRX_IsEmpty(SPI_TypeDef * SPIx) {
  return GSL_SPIRX_Available(SPIx) == 0;
}

// copy up to the next X bytes to the given buffer
// return the number of bytes transferred
uint16_t GSL_SPIRX_Transfer(
    SPI_TypeDef * SPIx,
    uint8_t * destination,
    uint16_t count) {
  // get info
  GSL_SPIRX_InfoStruct * info = GSL_SPIRX_GetInfo(SPIx);
  uint16_t target = count;
  uint8_t * last_byte = GSL_SPIRX_NextStoredByte(SPIx);
  while (count && info->next_data != last_byte) {
    *destination = *info->next_data;
    ++destination;
    ++info->next_data;
    if (info->next_data == info->buffer + info->capacity) {
      info->next_data = info->buffer;
    }
    --count;
    ++info->bytes_processed;
  }
  return target - count;
}

// pop up to the next X bytes
// return the number of bytes ignored
uint16_t GSL_SPIRX_Ignore(
    SPI_TypeDef * SPIx,
    uint16_t count) {
  // get info
  GSL_SPIRX_InfoStruct * info = GSL_SPIRX_GetInfo(SPIx);
  uint16_t target = count;
  uint8_t * last_byte = GSL_SPIRX_NextStoredByte(SPIx);
  while (count && info->next_data != last_byte) {
    ++info->next_data;
    if (info->next_data == info->buffer + info->capacity) {
      info->next_data = info->buffer;
    }
    --count;
    ++info->bytes_processed;
  }
  return target - count;
}

// set up automatic receiving on the given SPI channel
void GSL_SPIRX_Initialize(SPI_TypeDef * SPIx) {
  // get info
  GSL_SPIRX_InfoStruct * info = GSL_SPIRX_GetInfo(SPIx);
  GSL_SPI_InfoStruct * spi_info = GSL_SPI_GetInfo(SPIx);
  // assign buffer if it's not yet created
  if (info->buffer == nullptr) {
    info->buffer = (uint8_t *) GSL_BUF_Create(info->capacity);
    // initialize the buffer with 0xDEADBEEF
    uint8_t filler[4] = {0xDE, 0xAD, 0xBE, 0xEF};
    for (uint16_t i = 0; i < info->capacity; ++i) {
      info->buffer[i] = filler[i % 4];
    }
  }
  // ensure SPI settings are correct
  // TODO: ensure direction settings are okay
  if (spi_info->handle->Init.Mode != SPI_MODE_SLAVE/* ||
      spi_info->handle->Init.Direction != SPI_DIRECTION_2LINES_RXONLY*/) {
    HALT("Invalid parameters");
  }
  // initialize the SPI
  GSL_SPI_Initialize(SPIx);
  // initialize the info
  if (info->active) {
    HALT("Already initialized");
  }
  // set it to active
  info->active = true;
  info->next_data = info->buffer;
  info->hdmarx = spi_info->hdmarx;
  if (spi_info->handle->Init.Direction == SPI_DIRECTION_2LINES_RXONLY) {
    // start the receiving
    GSL_SPI_ReadMulti_DMA_Circular(SPIx, info->buffer, info->capacity);
  } else if (spi_info->handle->Init.Direction == SPI_DIRECTION_2LINES) {
    // start the receiving and sending
    GSL_SPI_SendReadMulti_DMA_Circular(
        SPIx,
        info->buffer,
        info->buffer,
        info->capacity);
  } else {
    HALT("Unexpected value");
  }
  // disable the DMA stream interrupts
  // (this saves processing time)
  /*
  info->hdmarx->Instance->CR |= DMA_IT_TC | DMA_IT_HT | DMA_IT_TE | DMA_IT_DME;
  info->hdmarx->Instance->CR ^= DMA_IT_TC | DMA_IT_HT | DMA_IT_TE | DMA_IT_DME;
  info->hdmarx->Instance->FCR |= DMA_IT_FE;
  info->hdmarx->Instance->FCR ^= DMA_IT_FE;*/
}

// return the given byte number without removing it from the buffer
uint8_t GSL_SPIRX_Peek(SPI_TypeDef * SPIx, uint16_t offset = 0) {
  // if nothing available, return 0
  if (GSL_SPIRX_Available(SPIx) <= offset) {
    return 0;
  }
  // get info
  GSL_SPIRX_InfoStruct * info = GSL_SPIRX_GetInfo(SPIx);
  // find pointer to next data
  uint8_t * ptr = info->next_data;
  ptr += offset;
  uint8_t * end_ptr = info->buffer + info->capacity;
  if (ptr >= end_ptr) {
    ptr -= info->capacity;
  }
  return *ptr;
}

// peek a value of the given type
template <class T>
void GSL_SPIRX_PeekTo(SPI_TypeDef * SPIx, T * objectPtr, uint16_t offset = 0) {
  uint8_t * data = (uint8_t *) objectPtr;
  for (uint16_t i = 0; i < sizeof(T); ++i) {
    data[i] = GSL_SPIRX_Peek(SPIx, offset + i);
  }
}

// pop and return a uint8_t value
uint8_t GSL_SPIRX_Pop(SPI_TypeDef * SPIx) {
  uint8_t value = 0;
  uint16_t count = GSL_SPIRX_Transfer(SPIx, &value, 1);
  ASSERT(count == 1U);
  return value;
}

// pop a value of the given type
template <class T>
void GSL_SPIRX_PopTo(SPI_TypeDef * SPIx, T * objectPtr) {
  GSL_SPIRX_Transfer(SPIx, (uint8_t *) objectPtr, sizeof(*objectPtr));
}

// empty the buffer and return the number of bytes ignored
uint16_t GSL_SPIRX_Purge(SPI_TypeDef * SPIx) {
  // get info
  GSL_SPIRX_InfoStruct * info = GSL_SPIRX_GetInfo(SPIx);
  // set the new pointer
  uint8_t * new_ptr = GSL_SPIRX_NextStoredByte(SPIx);
  // calculate the number of bytes ignored
  uint16_t bytes_ignored =
      (info->capacity + (new_ptr - info->next_data)) % info->capacity;
  // set the new data pointer
  info->bytes_processed += bytes_ignored;
  info->next_data = new_ptr;
  return bytes_ignored;
}

// send the following info out
// (there will be a one byte delay until the info is sent out)
void GSL_SPIRX_Send(
    SPI_TypeDef * SPIx,
    const uint8_t * data,
    uint16_t size,
    uint16_t offset = gsl_spirx_dummy_bytes) {
  // ensure transmission DMA is set
  ASSERT(gsl_spirx_dummy_bytes > 0);
  // get info
  GSL_SPIRX_InfoStruct * info = GSL_SPIRX_GetInfo(SPIx);
  // get location of next byte to be sent
  uint8_t * new_ptr = GSL_SPIRX_NextStoredByte(SPIx);
  uint32_t required =
      size + 1 + offset + GSL_SPIRX_Available(SPIx);
  // ensure size is okay
  if (required > info->capacity) {
    HALT("Insufficient buffer size");
  }
  uint8_t * end_ptr = info->buffer + info->capacity;
  // put the new info into place
  for (uint16_t i = 1; i < offset; ++i) {
    ++new_ptr;
    if (new_ptr == end_ptr) {
      new_ptr = info->buffer;
    }
  }
  // put the data into memory
  while (size > 0) {
    ++new_ptr;
    if (new_ptr == end_ptr) {
      new_ptr = info->buffer;
    }
    *new_ptr = *data;
    ++data;
    --size;
  }
}
