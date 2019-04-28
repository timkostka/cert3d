#pragma once

#include "gsl_includes.h"

// An GSL_FIFO object holds a first in, first out buffer of objects.  This uses a
// circular buffer.

template <class T>
struct GSL_FIFO {
  // object capacity
  uint16_t capacity;
  // objects stored
  uint16_t length;
  // object buffer
  T * buffer;
  // index of first item
  uint16_t first_index;
  // set capacity
  // (may only be called once)
  void SetCapacity(uint16_t target_capacity) {
    ASSERT_EQ(capacity, 0U);
    if (target_capacity == 0) {
      return;
    }
    capacity = target_capacity;
    buffer = (T *) GSL_BUF_Create(sizeof(T) * target_capacity);
  }
  // initializer
  GSL_FIFO(uint16_t target_capacity = 0)
    : capacity(0), length(0), buffer(nullptr), first_index(0) {
    SetCapacity(target_capacity);
  }
  // return True if full
  bool IsFull(void) const {
    return length == capacity;
  }
  // return True if empty
  bool IsEmpty(void) const {
    return length == 0;
  }
  // return a point to item X
  T * Item(uint16_t index) {
    ASSERT_LT(index, length);
    index += first_index;
    index %= capacity;
    return &buffer[index];
  }
  // return a pointer to the first item
  T * Next(void) {
    ASSERT_GT(length, 0);
    return &buffer[first_index];
  }
  // push an item
  void Push(const T & item) {
    ASSERT_LT(length, capacity);
    uint16_t index = (first_index + length) % capacity;
    buffer[index] = item;
    ++length;
  }
  // pop an item
  void Pop(void) {
    ASSERT_GT(length, 0);
    --length;
    ++first_index;
    first_index %= capacity;
  }
  // return the number of bytes used by this object
  uint16_t GetFootprintBytes(void) const {
    return sizeof(*this) + capacity * sizeof(T);
  }
};
