#pragma once

// This file implements a vector of bits and provides a convenient access to
// each bit.

#include "gsl_includes.h"

struct GSL_BITVECTOR {
  // capacity of vector in bits
  uint32_t bit_capacity;
  // capacity in bytes
  uint32_t byte_capacity;
  // location of buffer
  uint8_t * buffer;
  // set the capacity and buffer
  // (this may only be called if it is not already initialized)
  void SetCapacity(uint32_t bit_capacity_) {
    ASSERT(buffer == nullptr);
    ASSERT_EQ(bit_capacity, 0U);
    if (bit_capacity_ == 0) {
      return;
    }
    bit_capacity = bit_capacity_;
    byte_capacity = (bit_capacity + 8 - 1) / 8;
    buffer = (uint8_t *) GSL_BUF_Create(byte_capacity);
  }
  // initialization
  GSL_BITVECTOR(uint32_t bit_capacity_ = 0) :
      bit_capacity(0),
      byte_capacity(0),
      buffer(nullptr) {
    SetCapacity(bit_capacity_);
  }
  // should never be deinitialized
  ~GSL_BITVECTOR(void) {
    LOG("\nERROR: deinitialized a GSL_BITVECTOR");
  }
  // get multiple bits, MSB first
  uint32_t Get(uint32_t msb, uint32_t lsb) const {
    ASSERT_LT(msb, lsb);
    uint16_t bit_count = lsb - msb + 1;
    ASSERT_LE(bit_count, 32);
    if (false) {
      LOG("\n");
      LOG("\nstart = (", msb / 8, ", ", msb % 8, ")");
      LOG("\nend = (", lsb / 8, ", ", lsb % 8, ")");
      uint16_t byte = msb / 8;
      while (byte != lsb / 8 + 1) {
        LOG("\n", GSL_OUT_Hex(buffer[byte], 1));
        ++byte;
      }
    }

    // store value
    uint32_t value = 0;
    // if high byte and low byte are the same
    if (msb / 8 == lsb / 8) {
      value = buffer[msb / 8];
      value >>= 7 - lsb % 8;
      if (bit_count != 32) {
        value &= 0xFFFF >> (32 - bit_count);
      }
      return value;
    }
    // get bits in highest byte
    uint16_t byte = msb / 8;
    value |= buffer[byte];
    value &= 0xFF >> (msb % 8);
    //LOG("\nfirst: ", GSL_OUT_Hex(value, 2));
    ++byte;
    // store middle bits
    while (byte != lsb / 8) {
      value <<= 8;
      value |= buffer[byte];
      ++byte;
    }
    // get bits in lowest byte
    uint8_t low_bits = buffer[byte];
    low_bits >>= 7 - lsb % 8;
    value <<= 1 + lsb % 8;
    //LOG("\nend1: ", GSL_OUT_Hex(value, 2));
    //LOG("\nend3: ", GSL_OUT_Hex(low_bits, 2));
    value |= low_bits;
    //LOG("\nend: ", GSL_OUT_Hex(value, 2));
    return value;
  }
  // get a bit value
  bool Get(uint32_t i) const {
    ASSERT_LE(i, bit_capacity);
    return buffer[i / 8] & ((uint8_t) 0x80 >> (i % 8));
  }
  // set a bit to 1
  void Set(uint32_t i) {
    ASSERT_LE(i, bit_capacity);
    buffer[i / 8] |= (uint8_t) 0x80 >> (i % 8);
  }
  // set a bit to 0
  void Clear(uint32_t i) {
    ASSERT_LE(i, bit_capacity);
    buffer[i / 8] &= ~((uint8_t) 0x80 >> (i % 8));
  }
  // set or clear a bit
  void Set(uint32_t i, bool value) {
    if (value) {
      Set(i);
    } else {
      Clear(i);
    }
  }
  // clear all bits
  void ClearAll(void) {
    ASSERT(buffer);
    memset(buffer, 0, byte_capacity);
  }
};
