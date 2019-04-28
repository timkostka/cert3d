#pragma once

// This file contains routines for assigning memory buffers.  Memory here
// is permanently assigned.  It cannot later be freed.

#include "gsl_includes.h"

// total capacity we can assign
#ifndef GSL_BUF_CAPACITY
#define GSL_BUF_CAPACITY 32768 * 2
#endif

// storage space for all buffers
const uint32_t gsl_buffer_capacity = GSL_BUF_CAPACITY;

// amount of space currently assigned
uint32_t gsl_buffer_assigned = 0;

// buffer
uint8_t gsl_buffer_storage[gsl_buffer_capacity];

// size of temporary buffer
uint32_t gsl_buffer_temporary_assigned = 0;

// return the total amount free
uint32_t GSL_BUF_Available(void) {
  uint32_t free_space = gsl_buffer_capacity;
  free_space -= gsl_buffer_assigned;
  free_space -= gsl_buffer_temporary_assigned;
  return free_space;
}

// free the temporary buffer
void GSL_BUF_FreeTemporary() {
  gsl_buffer_temporary_assigned = 0;
}

// get a pointer to a temporary buffer of X bytes
// This buffer will remain valid until a call to GSL_BUF_FreeTemporary().
// Only one temporary buffer can be created at a time
void * GSL_BUF_CreateTemporary(uint32_t capacity, bool word_aligned = true) {
  // make sure we don't already have a temporary buffer
  if (gsl_buffer_temporary_assigned) {
    HALT("ERROR: Temporary buffer already active.  Cannot create more than\n"
        "one at a time.");
  }
  // if we want it word aligned, then make the capacity a multiple of 4
  if (word_aligned) {
    while (capacity % 4) {
      ++capacity;
    }
  }
  // save number of bytes assigned
  gsl_buffer_temporary_assigned = capacity;
  // check for overcapacity
  if (capacity > GSL_BUF_Available()) {
    LOG("\n\nERROR: We're out of buffer storage.");
    LOG("\nIncrease GSL_BUF_CAPACITY from ", GSL_BUF_CAPACITY,
        " to at least ",
        gsl_buffer_assigned + gsl_buffer_temporary_assigned + capacity,
        " and recompile.");
    HALT("Out of buffer storage");
  }
  // create pointer to the end of the data
  uint8_t * ptr = gsl_buffer_storage + gsl_buffer_capacity;
  // return a pointer to the temporary buffer
  ptr -= capacity;
  return ptr;
}

// create a new buffer
void * GSL_BUF_Create(uint32_t capacity, bool word_aligned = true) {
  // check for overstorage
  uint8_t * next_ptr = gsl_buffer_storage + gsl_buffer_assigned;
  // byte-align if necessary
  if (word_aligned) {
    if (((uint32_t) next_ptr) % 4) {
      uint32_t to_advance = 4 - (((uint32_t) next_ptr) % 4);
      GSL_BUF_Create(to_advance, false);
      next_ptr = gsl_buffer_storage + gsl_buffer_assigned;
    }
  }
  // check for overcapacity
  if (capacity > GSL_BUF_Available()) {
    LOG("\n\nERROR: We're out of buffer storage.");
    LOG("\nIncrease GSL_BUF_CAPACITY from ", GSL_BUF_CAPACITY,
        " to at least ",
        gsl_buffer_assigned + gsl_buffer_temporary_assigned + capacity,
        " and recompile.");
    HALT("Out of buffer storage");
  }
  // assign it
  gsl_buffer_assigned += capacity;
  // zero out the buffer
  memset(next_ptr, 0, capacity);
  // return pointer to the buffer start
  return next_ptr;
}

template <class T>
struct GSL_BUF_TempArray {
  // buffer location
  T * buffer;
  // initializer
  GSL_BUF_TempArray(uint16_t size) {
    buffer = (T *) GSL_BUF_CreateTemporary(sizeof(T) * size);
  }
  // deinitializer
  ~GSL_BUF_TempArray(void) {
    GSL_BUF_FreeTemporary();
  }
  // implicit conversion to pointer
  operator T *() {
    return buffer;
  }
  // implicit conversion to const pointer
  operator const T *() const {
    return buffer;
  }
};
