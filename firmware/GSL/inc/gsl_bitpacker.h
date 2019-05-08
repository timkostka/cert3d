#pragma once

// An GSL_BITPACKER class allows a straightforward interface for packing values
// of arbitrary bit width into a byte-aligned structure.

#include "gsl_includes.h"

struct GSL_BITPACKER {

  // location of the buffer start
  uint8_t * buffer_;

  // size of the buffer in bytes
  uint32_t capacity_;

  // number of bytes fully packed
  uint32_t bytes_packed_;

  // number of bits packed into the last unfinished byte
  uint8_t bits_packed_;

  enum ErrorEnum {
    kErrorNone = 0,
    kErrorOverflow,
  };

  // error code
  ErrorEnum error_;

  // initialization
  GSL_BITPACKER(uint8_t * buffer, uint32_t capacity_in_bytes) {
    buffer_ = buffer;
    capacity_ = capacity_in_bytes;
    bytes_packed_ = 0;
    bits_packed_ = 0;
    // set buffer to zero
    for (uint32_t i = 0; i < capacity_; ++i) {
      buffer_[i] = 0;
    }
    error_ = kErrorNone;
  }

  // return true if the buffer is full
  bool IsFull(void) {
    return bytes_packed_ == capacity_;
  }

  // pack a single bit, 1 if true, 0 if false
  void PackBit(bool value) {
    // if the last byte is already partially filled, no need to do an overflow
    // check, just add it
    if (bits_packed_) {
      buffer_[bytes_packed_] <<= 1;
      if (value) {
        buffer_[bytes_packed_] |= 0b1;
      }
      ++bits_packed_;
      if (bits_packed_ == 8) {
        ++bytes_packed_;
        bits_packed_ = 0;
      }
    } else {
      // check for overflow
      if (IsFull()) {
        error_ = kErrorOverflow;
        return;
      }
      buffer_[bytes_packed_] <<= 1;
      if (value) {
        buffer_[bytes_packed_] |= 0b1;
      }
      ++bits_packed_;
    }
  }

  // pack the value
  void Pack8(uint8_t value) {
    for (uint32_t i = 0; i < 8; ++i) {
      PackBit(value & 0x80);
      value <<= 1;
    }
  }

  // pack the value
  void Pack16(uint16_t value) {
    for (uint32_t i = 0; i < 16; ++i) {
      PackBit(value & 0x8000);
      value <<= 1;
    }
  }

  // pack the value
  void Pack32(uint32_t value) {
    for (uint32_t i = 0; i < 32; ++i) {
      PackBit(value & 0x80000000);
      value <<= 1;
    }
  }

  // pack the LSBs of the value
  void Pack(uint32_t value, uint8_t bits) {
    value <<= (32 - bits);
    for (uint32_t i = 0; i < bits; ++i) {
      PackBit(value & 0x80000000);
      value <<= 1;
    }
  }

  // clear everything
  void Clear() {
    bytes_packed_ = 0;
    bits_packed_ = 0;
    // set buffer to zero
    for (uint32_t i = 0; i < capacity_; ++i) {
      buffer_[i] = 0;
    }
  }

};
