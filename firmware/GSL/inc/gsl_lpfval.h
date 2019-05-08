#pragma once

// An GSL_LPFVAL provides a way to apply a low pass filter to quickly changing
// values.

#include "gsl_includes.h"

struct GSL_LPFVAL {
  // low pass frequency
  float lpf_frequency;
  // ticks at last update
  uint32_t last_update;
  // current value
  float value;
  // initializer
  GSL_LPFVAL(float lpf_frequency = 0.0f, float value = 0.0f) :
    lpf_frequency(lpf_frequency),
    last_update(GSL_DEL_Ticks()),
    value(value) {
  }
  // set to a value
  GSL_LPFVAL & operator = (float new_value) {
    value = new_value;
    return *this;
  }
  // update with a new value
  float Update(float new_value) {
    if (lpf_frequency == 0.0f) {
      value = new_value;
    } else {
      uint32_t this_update = GSL_DEL_Ticks();
      float duration_s = GSL_DEL_ElapsedSFloat(last_update, this_update);
      last_update = this_update;
      float alpha = 1.0f - powf(2.0, -lpf_frequency * duration_s);
      value += alpha * (new_value - value);
      // change Inf and NaN value to 0.0
      if (!isnormal(value)) {
        value = 0.0f;
      }
    }
    return value;
  }
  // implicit conversion to float
  operator float(void) const {
    return value;
  }
};
