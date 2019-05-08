#pragma once

// This file defines a bunch of FIR filters

#include "gsl_includes.h"

// this struct describes a given filter
struct GSL_FIR_DecimateF32Struct {
  // number of taps
  uint16_t tap_count;
  // decimation amount;
  uint16_t decimate_count;
  // pointer to coefficients
  float * coefficient;
};

// coefficients for
// 20 ksps readings
// +/-1db response between 0-4kHz
// -50dB max between 5-10kHz
// coefficients from: http://t-filter.engineerjs.com/
const float gsl_fir_one_coefficients[] = {
    -0.0024753116946174157,
    -0.01093854695942997,
    -0.011998627293626357,
    -0.0012172049414123363,
    0.011042307600927913,
    0.005640809620051136,
    -0.011975054034598557,
    -0.012881242538869278,
    0.010449045330178996,
    0.022359189471659918,
    -0.004188356564064448,
    -0.03300327811739778,
    -0.009225415803044078,
    0.04350512027269015,
    0.034416108508927006,
    -0.052373075855077,
    -0.08708424861796121,
    0.0582961349810822,
    0.31170832085399464,
    0.4396183711916292,
    0.31170832085399464,
    0.0582961349810822,
    -0.08708424861796121,
    -0.052373075855077,
    0.034416108508927006,
    0.04350512027269015,
    -0.009225415803044083,
    -0.03300327811739778,
    -0.004188356564064448,
    0.022359189471659918,
    0.010449045330178996,
    -0.012881242538869278,
    -0.011975054034598557,
    0.005640809620051136,
    0.011042307600927913,
    -0.0012172049414123363,
    -0.011998627293626357,
    -0.01093854695942997,
    -0.0024753116946174157};

// 2x decimation filter
GSL_FIR_DecimateF32Struct gsl_fir_one_decimate = {
    sizeof(gsl_fir_one_coefficients) / sizeof(*gsl_fir_one_coefficients),
    2,
    (float *) gsl_fir_one_coefficients};
