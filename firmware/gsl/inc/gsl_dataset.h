#pragma once

// And GSL_DATASET object holds a series of values.

#include "gsl_includes.h"

template <class T>
struct GSL_DATASET {
  // capacity
  uint16_t capacity;
  // length of current set
  uint16_t length;
  // index of first value
  uint16_t first_index;
  // index of next value
  uint16_t next_index;
  // buffer
  T * buffer;
  // set capacity
  void SetCapacity(uint16_t target_capacity) {
    // ensure we don't assign memory more than once
    if (target_capacity == capacity) {
      return;
    }
    ASSERT_EQ(capacity, 0);
    capacity = target_capacity;
    buffer = (T *) GSL_BUF_Create(capacity * sizeof(T));
  }
  // default initializer
  GSL_DATASET (uint16_t capacity = 0) :
      capacity(0), length(0), first_index(0), next_index(0), buffer(nullptr) {
    SetCapacity(capacity);
  }
  // return the i'th value
  float Value(uint16_t index) const {
    ASSERT_LT(index, length);
    return buffer[(first_index + index) % capacity];
  }
  // return true if the history is full
  bool IsFull(void) const {
    return length == capacity;
  }
  // return the maximum value
  float Maximum(void) const {
    if (length == 0) {
      return NAN;
    }
    float value = Value(0);
    for (uint16_t i = 1; i < length; ++i) {
      float this_value = Value(i);
      if (this_value > value) {
        value = this_value;
      }
    }
    return value;
  }
  // return the minimum value
  float Minimum(void) const {
    if (length == 0) {
      return NAN;
    }
    float value = Value(0);
    for (uint16_t i = 1; i < length; ++i) {
      float this_value = Value(i);
      if (this_value < value) {
        value = this_value;
      }
    }
    return value;
  }
  // return the range (max - min)
  float Range(void) const {
    return Maximum() - Minimum();
  }
  // return the average
  float Average(void) const {
    if (length == 0) {
      return NAN;
    }
    ASSERT_GT(length, 0);
    float total = 0.0f;
    for (uint16_t i = 0; i < length; ++i) {
      total += Value(i);
    }
    return total / length;
  }
  // return the standard deviation
  float StDev(void) const {
    if (length <= 1) {
      return NAN;
    }
    float mean = Average();
    float stdev = 0.0f;
    for (uint16_t i = 0; i < length; ++i) {
      float value = Value(i) - mean;
      stdev += value * value;
    }
    return sqrtf(stdev / length);
  }
  // return the correlation coefficient
  // x = index
  // y = value
  float Correlation(void) const {
    if (length <= 1) {
      return NAN;
    }
    // if all the same value, return 0.0
    float first_value = Value(0);
    // get the covariance (E(xy) - E(x)E(y))
    float covariance = 0.0f;
    float y_mean = Average();
    float y_stdev = StDev();
    float x_mean = (length - 1) / 2.0f;
    float x_stdev = 0.0f;
    bool all_same_value = true;
    for (uint16_t i = 0; i < length; ++i) {
      float value = i - x_mean;
      if (value != first_value) {
        all_same_value = false;
      }
      x_stdev += value * value;
      covariance += i * Value(i);
    }
    if (all_same_value) {
      return 0.0f;
    }
    x_stdev /= length;
    x_stdev = sqrt(x_stdev);
    covariance /= length;
    covariance -= x_mean * y_mean;
    if (x_stdev * y_stdev == 0) {
      covariance = NAN;
    } else {
      covariance /= x_stdev * y_stdev;
    }
    return covariance;
  }
  // remove and return the last value
  T Pop(void) {
    ASSERT_GT(length, 0);
    T value = buffer[first_index];
    ++first_index;
    first_index %= capacity;
    --length;
    return value;
  }
  // return the last value
  T Last(void) const {
    ASSERT_GT(length, 0);
    return Value(length - 1);
  }
  // add a value to the set
  void Add(const T & value) {
    ASSERT_GT(capacity, 0);
    if (length == capacity) {
      ASSERT_EQ(first_index, next_index);
      Pop();
    }
    buffer[next_index] = value;
    ++next_index;
    next_index %= capacity;
    ++length;
  }
  // clear all values
  void Clear(void) {
    length = 0;
    first_index = 0;
    next_index = 0;
  }
  // clear all except the most recent X values
  void ClearMost(uint16_t keep) {
    while (length > keep) {
      Pop();
    }
  }
  // return the most common value and the number of instances
  void GetMode(float & mode, uint16_t & count) const {
    if (length == 0) {
      mode = NAN;
      count = 0;
      return;
    }
    // count the number of instances of this value
    count = 0;
    float this_value = Minimum();
    float next_value;
    while (true) {
      next_value = this_value;
      uint16_t this_count = 0;
      for (uint16_t i = 0; i < length; ++i) {
        float value = Value(i);
        if (value == this_value) {
          ++this_count;
        }
        if (value > this_value && (next_value == this_value ||
            value < next_value)) {
          next_value = value;
        }
      }
      if (this_count > count) {
        mode = this_value;
        count = this_count;
      }
      if (next_value == this_value) {
        break;
      }
      this_value = next_value;
    }
  }
};
