#pragma once

// This provides an easy interface to call an initialization routine when
// global objects are created, after the clock has been set but before
// main() is called.

#include "gsl_includes.h"

struct GSL_INITIALIZER {
  GSL_INITIALIZER(void (*initialize_routine)(void)) {
    initialize_routine();
  }
};
