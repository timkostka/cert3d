#pragma once

// This file implements a buffer used by the USB to send out data.

#include "c3d_includes.h"

// A C3D_DMA_Monitor provides an interface to monitor data output by a DMA into
// a circular buffer.

template <class T>
struct C3D_DMA_Monitor {

  // buffer capacity
  uint16_t buffer_capacity;

  // point to buffer
  volatile T * buffer;

  // previous NDTR value;
  uint32_t last_NDTR;

  // DMA stream for this monitor
  DMA_Stream_TypeDef * dma_stream;

  // return true if empty
  bool IsEmpty(void) const {
    return dma_stream->NDTR == last_NDTR;
  }

  // clear
  void Clear() {
    last_NDTR = dma_stream->NDTR;
  }

  // return the number of available values
  uint16_t GetAvailable(void) const {
    uint16_t available = last_NDTR;
    available -= dma_stream->NDTR;
    if (available > buffer_capacity) {
      available += buffer_capacity;
      ASSERT_LT(available, buffer_capacity);
    }
    return available;
  }

  // ignore this many values
  void IgnoreMany(uint16_t count) {
    ASSERT_LE(count, GetAvailable());
    if (last_NDTR <= count) {
      last_NDTR += buffer_capacity;
    }
    last_NDTR -= count;
    ASSERT_GT(last_NDTR, 0);
    ASSERT_LE(last_NDTR, buffer_capacity);
  }

  // pop as many values as possible within a contiguous region
  // Note: Because the buffer is circular, two calls to this
  // may be necessary to get all data.
  void PopMany(uint8_t * & buffer_out, uint16_t & length) {
    // next index
    uint16_t start_index = buffer_capacity - last_NDTR;
    uint16_t next_index = buffer_capacity - dma_stream->NDTR;
    if (start_index == next_index) {
      buffer_out = nullptr;
      length = 0;
    } else if (next_index > start_index) {
      uint16_t item_count = next_index - start_index;
      buffer_out = (uint8_t *) &buffer[start_index];
      length = sizeof(T) * item_count;
      last_NDTR -= item_count;
    } else {
      uint16_t item_count = buffer_capacity - start_index;
      buffer_out = (uint8_t *) &buffer[start_index];
      length = sizeof(T) * item_count;
      last_NDTR = buffer_capacity;
    }
  }

  // pop a value
  T Pop(void) {
    ASSERT_NE(dma_stream->NDTR, last_NDTR);
    T value = buffer[buffer_capacity - last_NDTR];
    // move up the pointer by one
    --last_NDTR;
    if (last_NDTR == 0) {
      last_NDTR = buffer_capacity;
    }
    return value;
  }

  // read the next value, but don't remove it
  T Peek(void) {
    ASSERT_NE(dma_stream->NDTR, last_NDTR);
    return buffer[buffer_capacity - last_NDTR];
  }

};
