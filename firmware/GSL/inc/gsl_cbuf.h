#pragma once

#include "gsl_includes.h"

// A GSL_CBUF provides access to a circular buffer.
//
// * Put data in storage
// * Send out the data through a DMA-type function
// * Continue to add data while this is being done
// * When DMA completes, unfreeze that portion and continue
//
// the buffer is circular
// the buffer is broken into three regions
// frozen: Bytes in this will not be altered until Thaw() is called.  This
//         region is contiguous and cannot span the boundary.
// secured: Bytes in this region are correct and will not be altered.
// free: Bytes here are not yet written.

struct GSL_CBUF {

  // total buffer capacity
  uint32_t capacity_;

  // start of the buffer
  uint8_t * buffer_start_;

  // end of the buffer
  uint8_t * buffer_end_;

  // start of the frozen buffer
  uint8_t * frozen_buffer_start_;

  // start of the secured buffer
  uint8_t * secured_buffer_start_;

  // start of the free buffer
  uint8_t * free_buffer_start_;

  // holds the maximum buffer size used at any given time
  // (this helps to size the buffer appropriately)
  uint32_t max_size_used_;

  // set the capacity
  void SetCapacity(uint32_t capacity) {
    // if zero, don't assign any buffer
    if (capacity == 0) {
      return;
    }
    if (capacity == capacity_) {
      return;
    }
    // make sure we don't resize the buffer
    if (capacity_ > 0) {
      HALT("Cannot resize");
    }
    // allocate storage
    capacity_ = capacity;
    buffer_start_ = (uint8_t *) GSL_BUF_Create(capacity);
    buffer_end_ = buffer_start_ + capacity_;
    frozen_buffer_start_ = buffer_start_;
    secured_buffer_start_ = buffer_start_;
    free_buffer_start_ = buffer_start_;
  }

  // constructor
  GSL_CBUF(uint32_t capacity = 0) {
    // initial values
    capacity_ = 0;
    buffer_start_ = nullptr;
    buffer_end_ = nullptr;
    frozen_buffer_start_ = buffer_start_;
    secured_buffer_start_ = buffer_start_;
    free_buffer_start_ = buffer_start_;
    max_size_used_ = 0;
    SetCapacity(capacity);
  }

  // return the size of the frozen buffer
  uint32_t GetFrozenBufferSize(void) {
    return secured_buffer_start_ - frozen_buffer_start_;
  }

  // convert secured region into frozen region
  // (if secured region spans the boundary, this will only convert a portion
  // of it)
  void Freeze(void) {
    // we cannot freeze the buffer twice
    //GSL_GEN_DisableInterrupts();
    if (frozen_buffer_start_ != secured_buffer_start_) {
      LOG("\nWe are");
      if (GSL_GEN_InInterrupt()) {
        LOG(" NOT");
      }
      LOG(" in an interrupt");
      //GSL_GEN_EnableInterrupts();
      HALT("Double freeze");
    }
    // if no secure buffer, don't do anything
    if (secured_buffer_start_ == free_buffer_start_) {
      //GSL_GEN_EnableInterrupts();
      return;
    }
    // if secured buffer spans boundary, just freeze a portion of it
    if (free_buffer_start_ < secured_buffer_start_) {
      secured_buffer_start_ = buffer_end_;
    } else {
      // else secure it all
      secured_buffer_start_ = free_buffer_start_;
    }
    //GSL_GEN_EnableInterrupts();
  }

  // convert up to X bytes in the secured region into frozen region
  // (if secured region spans the boundary, this will only convert a portion
  // of it)
  void Freeze(uint32_t byte_count) {
    // freeze as much as possible
    Freeze();
    // now limit how much is frozen if we have too much
    if (GetFrozenBufferSize() > byte_count) {
      uint32_t overage = GetFrozenBufferSize() - byte_count;
      secured_buffer_start_ -= overage;
    }
  }

  // free the frozen portion of the buffer
  void Thaw(void) {
    //GSL_GEN_DisableInterrupts();
    if (secured_buffer_start_ == buffer_end_) {
      secured_buffer_start_ = buffer_start_;
    }
    frozen_buffer_start_ = secured_buffer_start_;
    //GSL_GEN_EnableInterrupts();
  }

  // return the capacity remaining
  uint32_t GetCapacityRemaining(void) {
    uint32_t capacity = frozen_buffer_start_ - free_buffer_start_;
    if (frozen_buffer_start_ <= free_buffer_start_) {
      capacity += buffer_end_ - buffer_start_;
    }
    return capacity;
  }

  // return the size in use
  // (must be 1 byte free at all times)
  uint32_t GetSizeInUse(void) {
    return (buffer_end_ - buffer_start_) - GetCapacityRemaining();
  }

  // ignore the next X bytes
  // can only be called if frozen buffer is empty
  void IgnoreBytes(uint32_t count) {
    Freeze(count);
    uint32_t ignored_count = GetFrozenBufferSize();
    Thaw();
    if (ignored_count < count) {
      Freeze(ignored_count - count);
      Thaw();
    }
  }

  // attempt to add a given number of bytes to the buffer
  // return true if successful
  bool AddBytes(const uint8_t * data, uint32_t data_size) {

    // if too much data, don't add it
    if (data_size >= GetCapacityRemaining()) {
      return false;
    }

    // check for overflow
    // note that due to our implementation, we must keep capacity remaining
    // nonzero
    uint32_t capacity_remaining = GetCapacityRemaining();

    if (data_size >= capacity_remaining) {
      HALT("Overflow");
    }

    // add data
    uint8_t * ptr = free_buffer_start_;
    while (data_size) {
      *ptr = *data;
      ++ptr;
      ++data;
      if (ptr == buffer_end_) {
        ptr = buffer_start_;
      }
      --data_size;
    }
    free_buffer_start_ = ptr;
    // update max size
    uint32_t size_in_use = GetSizeInUse();
    if (max_size_used_ < size_in_use) {
      max_size_used_ = size_in_use;
    }
    // success!
    return true;
  }

  // add a null terminated string to the buffer, not including the null
  // termination
  void AddString(const char * data) {
    AddBytes((const uint8_t *) data, sizeof(data));
  }

};
