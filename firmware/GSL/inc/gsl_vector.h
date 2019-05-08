#pragma once

// An GSL_VECTOR is a generic vector used for storing values.  Static memory
// allocation is done using the GSL_BUF and is done up to once per object.
// Memory cannot be de-allocated.
//
// Example usage:
// GSL_VECTOR<uint16_t> this_vector;
// this_vector.SetCapacity(128);
// this_vector[127] = 67;

#include "gsl_includes.h"

template <class T>
struct GSL_VECTOR {
  // length of this vector
  uint16_t length;
  // buffer
  T * buffer;
  // set the capacity
  // may only be done if length=0
  void SetCapacity(uint16_t new_length) {
    // can only be called if current length is zero
    ASSERT_EQ(length, 0);
    // if we're not doing anything, just return
    if (new_length == 0) {
      return;
    }
    // set the length
    length = new_length;
    buffer = (T *) GSL_BUF_Create(sizeof(T) * length);
  }
  // initializer
  GSL_VECTOR(uint16_t new_length = 0) :
      length(0),
      buffer(nullptr) {
    SetCapacity(new_length);
  }
  // const member access
  const T & operator[] (uint16_t index) const {
    ASSERT_LE(index, length);
    return buffer[index];
  }
  // member access
  T & operator[] (uint16_t index) {
    ASSERT_LE(index, length);
    return buffer[index];
  }
  // set all values to 0
  void Clear(void) {
    memset(buffer, 0, sizeof(T) * length);
  }
};
