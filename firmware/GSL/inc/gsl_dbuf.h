#pragma once

// This file defines a double buffer.

#include "gsl_includes.h"

// size can be assigned once

struct GSL_DBUF {

  // size of each buffer
  uint32_t capacity_;

  // active buffer storage
  uint8_t * active_buffer_;

  // inactive buffer storage
  uint8_t * inactive_buffer_;

  // assign a size to a currently zero-capacity buffer
  void SetCapacity(uint32_t capacity) {
    if (capacity == capacity_) {
      return;
    }
    if (capacity_ != 0) {
      HALT("Cannot change capacity");
    }
    capacity_ = capacity;
    active_buffer_ = (uint8_t *) GSL_BUF_Create(capacity_ * 2);
    inactive_buffer_ = &active_buffer_[capacity_];
  }

  // swap the two buffer
  void Swap(void) {
    uint8_t * temp = active_buffer_;
    active_buffer_ = inactive_buffer_;
    inactive_buffer_ = temp;
  }

  // return the active buffer
  uint8_t * GetActive(void) {
    return active_buffer_;
  }

  // return the inactive buffer
  uint8_t * GetInactive(void) {
    return inactive_buffer_;
  }

  // default constructor (zero size)
  GSL_DBUF(uint32_t capacity = 0) {
    capacity_ = 0;
    active_buffer_ = nullptr;
    inactive_buffer_ = nullptr;
    SetCapacity(capacity);
  }

};
