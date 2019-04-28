#pragma once

// An GSL_AVAL provides a way to filter a quickly changing value.

#include "gsl_includes.h"

struct GSL_AVAL {
  // alpha value
  float alpha;
  // current value
  float value;
  // initializer
  GSL_AVAL(float alpha = 0.0f, float value = 0.0f) :
    alpha(alpha),
    value(value) {
  }
  // set to a value
  GSL_LPFVAL & operator = (float new_value) {
    value = new_value;
    return *this;
  }
  // update with a new value
  float Update(float new_value) {
    // update value
    value += alpha * (new_value - value);
    // if it's currently INF or NAN, just set it to the new value
    if (isinff(value) || isnanf(value)) {
      value = new_value;
    }
    return value;
  }
  // implicit conversion to float
  operator float(void) const {
    return value;
  }
};
