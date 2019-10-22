#pragma once

// This file implements a buffer used by the USB to send out data.

#include "c3d_includes.h"

// A C3D_DMA_Monitor provides an interface to monitor data output by a DMA into
// a circular buffer.

struct C3D_SignalDmaMonitor {
  // pointer to start of the buffer
  volatile uint16_t * buffer;
  // capacity of buffer
  uint16_t capacity;
  // middle count
  uint16_t middle_count;
  // tail of stuff to send
  uint16_t tail_index;
  // middle of stuff to send
  uint16_t middle_index;
  // head index
  uint16_t head_index;
  // DMA stream for this monitor
  DMA_Stream_TypeDef * dma_stream;
  // get index of the head
  uint16_t GetHeadIndex(void) const {
    return capacity - dma_stream->NDTR;
  }
  // update the head index
  void Update(void) {
    head_index = GetHeadIndex();
  }
  // clear all values
  void Clear(void) {
    middle_index = head_index;
    tail_index = head_index;
  }
  // pop a value
  uint16_t Pop(void) {
    // ensure something is there
    ASSERT_NE(tail_index, GetHeadIndex());
    // read value
    uint16_t value = buffer[tail_index];
    // increment index
    ++tail_index;
    tail_index %= capacity;
    // return value
    return value;
  }
  // read the next value, but don't remove it
  uint16_t Peek(void) {
    return buffer[tail_index];
  }
};
