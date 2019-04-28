#pragma once

#include "c3d_includes.h"

// number of chunks
const uint16_t c3d_chunk_count = 32;

// length of each chunk in bytes
const uint16_t c3d_chunk_capacity = 2048;

// A C3D_ChunkBuffer is a staging buffer which holds data meant to be sent out
// via USB.
struct C3D_ChunkBuffer {

  // location of each chunk
  uint8_t * chunk[c3d_chunk_count];

  // stored length of each chunk
  uint16_t chunk_length[c3d_chunk_count];

  // active chunk
  uint16_t active_chunk;

  // first chunk with data in it
  uint16_t first_chunk;

  // true if a chunk is frozen
  bool frozen;

  // constructor
  C3D_ChunkBuffer() {
    // initialize all to nullptr
    for (uint16_t i = 0; i < c3d_chunk_count; ++i) {
      chunk[i] = nullptr;
      chunk_length[i] = 0;
    }
    active_chunk = 0;
    first_chunk = 0;
    frozen = false;
  }

  // initialize the chunk buffers
  void Initialize(void) {
    for (uint16_t i = 0; i < c3d_chunk_count; ++i) {
      // create word-aligned buffers
      chunk[i] = (uint8_t *) GSL_BUF_Create(c3d_chunk_capacity, true);
      chunk_length[i] = 0;
    }
    active_chunk = 0;
    first_chunk = 0;
    frozen = false;
  }

  // return available length in current buffer
  uint16_t GetAvailable(void) const {
    ASSERT_GE(c3d_chunk_capacity, chunk_length[active_chunk]);
    return c3d_chunk_capacity - chunk_length[active_chunk];
  }

  // increment the active chunk, return true if successful
  bool AdvanceChunk(void) {
    // must have data in order to advance
    if (chunk_length[active_chunk] == 0) {
      return false;
    }
    // get next chunk index
    uint16_t next_index = (active_chunk + 1) % c3d_chunk_count;
    // cannot increment into first index
    if (next_index == first_chunk) {
      return false;
    }
    // advance and report success
    active_chunk = next_index;
    return true;
  }

  // store data
  void StageData(void const * buffer, uint16_t length) {
    const uint8_t * ptr = (const uint8_t *) buffer;
    while (length) {
      // get available data in active buffer
      uint16_t this_length = GetAvailable();
      // if we need to store more than is available, increase chunk after we store
      const bool roll_buffer = this_length < length;
      if (this_length > length) {
        this_length = length;
      }
      // put this much into the active buffer
      memcpy(&chunk[active_chunk][chunk_length[active_chunk]],
             ptr,
             this_length);
      chunk_length[active_chunk] += this_length;
      ptr += this_length;
      length -= this_length;
      if (roll_buffer) {
        if (!AdvanceChunk()) {
          if (length > 0) {
            LOG_ONCE("\nERROR: buffer overflow");
          }
          return;
        }
      }
    }
  }

  // store a single variable
  template <class T>
  void StageVariable(const T & variable) {
    StageData(&variable, sizeof(variable));
  }

  // freeze the first buffer if it has data, return true if successful
  bool Freeze(void) {
    if (frozen) {
      HALT("\nERROR: chunk is already frozen");
    }
    if (first_chunk == active_chunk) {
      if (!AdvanceChunk()) {
        return false;
      }
    }
    ASSERT_NE(first_chunk, active_chunk);
    frozen = true;
    return true;
  }

  // thaw the frozen buffer
  void Thaw() {
    ASSERT(frozen);
    frozen = false;
    chunk_length[first_chunk] = 0;
    first_chunk = (first_chunk + 1) % c3d_chunk_count;
  }

  // get total size stored
  uint16_t GetStoredBytes(void) const {
    uint16_t length = 0;
    for (uint16_t i = 0; i < c3d_chunk_count; ++i) {
      length += chunk_length[i];
    }
    return length;
  }

};
