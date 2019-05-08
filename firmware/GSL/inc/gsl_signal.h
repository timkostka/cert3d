#pragma once

#include "gsl_includes.h"

// This file implements an GSL_SIGNAL object, which defines a time dependent
// signal.

enum GSL_SIGNAL_SignalPointEnum {
  kSignalUnused,
  kSignalConstant,
  kSignalLinear,
  kSignalFinished,
};

struct GSL_SIGNAL_SignalPoint {
  // type of data point
  GSL_SIGNAL_SignalPointEnum type;
  // starting value
  float value;
  // duration for this data point in seconds
  float duration;
};

typedef GSL_SIGNAL_SignalPoint GSL_SIGNAL[];

GSL_SIGNAL error_signal = {
    {kSignalConstant, 1.0f, 0.5f},
    {kSignalConstant, 0.0f, 0.5f},
    {kSignalFinished, 0.0f, 0.0f}};
