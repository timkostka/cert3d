#pragma once

// An GSL_COLSIG object defined a color signal.  Most commonly, this is used
// to drive LEDs with a particular color pattern.

#include "gsl_includes.h"

struct GSL_COLSIG {

  // what to do when the end of the signal is reached
  enum EndConditionEnum {
    // unused termination
    kTerminationUnused,
    // repeat the signal indefinitely
    kTerminationRepeat,
    // go to a constant signal
    kTerminationToConstant,
  };

  // termination condition
  EndConditionEnum termination;

  // interpolation type
  enum InterpolationEnum {
    // no interpolation (color remains solid until the next point)
    kInterpolationNone,
    // linear interpolation between points
    kInterpolationLinear,
  };

  // interpolation type
  InterpolationEnum interpolation;

  // data point
  struct ColorSignalPointStruct {
    // color
    GSL_COL_RGB color;
    // duration in ms
    uint16_t duration_ms;
  };

  // list of signal points
  const ColorSignalPointStruct * signal_point;

  // number of signal points
  uint16_t signal_point_count;

};

// define a macro to avoid having to write the same thing 3 times, which may
// lead to errors
#define GSL_COLSIG_DATA_EXPAND(x) x, sizeof(x) / sizeof(*x)

// signal points for triple red blinking signal
const GSL_COLSIG::ColorSignalPointStruct gsl_colsig_triple_red_point[] = {
    {kColorRed, 250},
    {kColorBlack, 250},
    {kColorRed, 250},
    {kColorBlack, 250},
    {kColorRed, 250},
    {kColorBlack, 750}};

// triple red blink
const GSL_COLSIG gsl_colsig_triple_red = {
    GSL_COLSIG::kTerminationRepeat,
    GSL_COLSIG::kInterpolationNone,
    GSL_COLSIG_DATA_EXPAND(gsl_colsig_triple_red_point)};
