#pragma once

// This file implements the GSL_TIME object, which refers to a time duration.
// The precision is down to microseconds.

#include "gsl_includes.h"

struct GSL_TIME {
  // seconds
  uint32_t seconds;
  // nanoseconds
  uint32_t ns;
  // number of seconds per day
  static const uint32_t kSecondsPerDay = 86400;
  // return the total time in seconds
  float InSeconds(void) const {
    return seconds + ns / 1e9f;
  }
  // return the total time in days
  float InDays(void) const {
    return InSeconds() / kSecondsPerDay;
  }
};
