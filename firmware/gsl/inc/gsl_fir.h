#pragma once

// This file contains routines for implemented an FIR filter.

#include <math.h>

#include "arm_math.h"

#include "gsl_includes.h"

// get the next reading (for debugging)
float GSL_FIR_GetReading(void) {
  // sine wave frequency
  const float sine_frequency = 4500.0f;
  // reading frequency
  const float reading_frequency = 20000.0f;
  // hold the angle
  static float omega = 0.0f;
  omega += 2.0f * M_PI * sine_frequency / reading_frequency;
  return cosf(omega);
}

// An GSL_FIR object is used to process data through an FIR filter
struct GSL_FIR {
  // number of taps
  uint16_t tap_count;
  // coefficient values
  float * coefficient;
  // number of channels
  uint16_t channel_count;
  // input sample rate
  uint32_t sample_rate;
  // output data rate
  // (must be divisor of sample rate)
  uint32_t output_rate;
  // processing rate
  // (must be divisor of output rate)
  uint32_t processing_rate;
  // pointer to output data
  float * input_buffer;
  // pointer to output data
  float * output_buffer;
  // ength of state array for each channel
  //uint16_t state_length;
  // pointer to state buffer
  float * state_buffer;
  // pointer to fir decimation structures for each channel
  arm_fir_decimate_instance_f32 * decimate_filter;
  // number of samples per processing block
  uint16_t samples_per_process;
  // number of outputs per processing block
  uint16_t outputs_per_process;
  // pointer to mean value for each channel
  float * mean_buffer;
  // pointer to standard deviation for each channel
  float * stdev_buffer;
  // default constructor
  GSL_FIR(void) {
    tap_count = 0;
    coefficient = nullptr;
    channel_count = 0;
    sample_rate = 0;
    output_rate = 0;
    processing_rate = 0;
    input_buffer = nullptr;
    output_buffer = nullptr;
    //state_length = 0;
    state_buffer = nullptr;
    decimate_filter = nullptr;
    samples_per_process = 0;
    outputs_per_process = 0;
    mean_buffer = nullptr;
    stdev_buffer = nullptr;
  }
  // initialize
  void Initialize(void) {
    // ensure data is valid
    ASSERT_GT(tap_count, 0u);
    ASSERT_GT(channel_count, 0u);
    ASSERT_GT(sample_rate, 0u);
    ASSERT_GT(output_rate, 0u);
    ASSERT_GT(processing_rate, 0u);
    ASSERT(coefficient != nullptr);
    ASSERT(input_buffer == nullptr);
    ASSERT(output_buffer == nullptr);
    ASSERT(state_buffer == nullptr);
    // ensure coefficients add up to 1
    float check = 0.0f;
    float max_coefficient = 1.0f;
    for (uint16_t i = 0; i < tap_count; ++i) {
      check += coefficient[i];
      if (fabs(coefficient[i]) > max_coefficient) {
        max_coefficient = fabs(coefficient[i]);
      }
    }
    // initialize buffers
    samples_per_process = sample_rate / processing_rate;
    input_buffer = (float *) GSL_BUF_Create(
        sizeof(float) * samples_per_process * channel_count);
    outputs_per_process = output_rate / processing_rate;
    output_buffer = (float *) GSL_BUF_Create(
        sizeof(float) * outputs_per_process * channel_count);
    uint16_t state_length = tap_count + samples_per_process - 1;
    state_buffer = (float *) GSL_BUF_Create(
        sizeof(float) * channel_count * state_length);
    // initialize decimation filters
    decimate_filter = (arm_fir_decimate_instance_f32 *) GSL_BUF_Create(
        sizeof(arm_fir_decimate_instance_f32) * channel_count);
    for (uint16_t i = 0; i < channel_count; ++i) {
      auto result = arm_fir_decimate_init_f32(&decimate_filter[i],
                                              tap_count,
                                              sample_rate / output_rate,
                                              coefficient,
                                              &state_buffer[i * state_length],
                                              samples_per_process);
      ASSERT_EQ(result, ARM_MATH_SUCCESS);
    }
    // initialize mean and stdev buffers
    mean_buffer = (float *) GSL_BUF_Create(sizeof(float) * channel_count);
    stdev_buffer = (float *) GSL_BUF_Create(sizeof(float) * channel_count);
  }
  // process the input data and create new output data
  void Process(void) {
    for (uint16_t i = 0; i < channel_count; ++i) {
      arm_fir_decimate_f32(
          &decimate_filter[i],
          &input_buffer[i * samples_per_process],
          &output_buffer[i * outputs_per_process],
          samples_per_process);
      // calculate mean and stdev
      GSL_GEN_CalculateMeanStdev(&output_buffer[i * outputs_per_process],
                                 outputs_per_process,
                                 &mean_buffer[i],
                                 &stdev_buffer[i]);
    }
  }
  // return the attenuation of a sine wave at the given frequency in dB
  // e.g. 0.0 -> no attenuation
  //      1.0 -> amplified by 10x
  //      0.001 -> reduced by 100x
  float GetAttenuation(float frequency) {
    // allowable portion of a wave to include as error
    /*float allowable_wave_portion_error = 0.001f;
    // find the minimum number of waves to simulate to get this error
    uint32_t target_wave_count = 0;
    while (true) {
      ++target_wave_count;
      uint32_t output_count = (float) target_wave_count * output_rate + 0.5f;
      float wave_count = output_count / output_rate
      float error = wave_count - (uint32_t)(wave_count + 0.5f);
      if (fabs(error) <= allowable_wave_portion_error) {
        LOG("\nError is ", GSL_OUT_FixedFloat(error, 6));
        break;
      }
    }*/
    // number of waves to simulate
    // at least 1 full wave
    // at least 5X tap_count
    float waves_to_average = 1.0f;
    waves_to_average += 5 * tap_count * frequency / sample_rate;
    waves_to_average = (uint16_t) (waves_to_average + 0.5f);
    // number of outputs to ignore while waiting for setup
    uint32_t outputs_to_ignore =
        1 + tap_count * output_rate / sample_rate + outputs_per_process;
    // number of outputs to average in order to get the attenuation
    const uint32_t outputs_to_average =
        sample_rate * waves_to_average / frequency + 0.5f;
    // current angle
    float omega = 0.0f;
    // average of the squared value
    float output_squared_average = 0.0f;
    //LOG("\nignoring ", outputs_to_ignore, " outputs, averaging ", outputs_to_average, " outputs");
    uint32_t outputs_left_to_average = outputs_to_average;
    while (outputs_left_to_average) {
      for (uint16_t i = 0; i < samples_per_process; ++i) {
        omega += 2.0 * PI * frequency / sample_rate;
        float value = sinf(omega);
        input_buffer[i] = value;
      }
      // process it
      uint16_t temp = channel_count;
      channel_count = 1;
      Process();
      channel_count = temp;
      // read output values
      for (uint16_t i = 0; i < outputs_per_process; ++i) {
        if (outputs_to_ignore) {
          --outputs_to_ignore;
        } else if (outputs_left_to_average) {
          output_squared_average += output_buffer[i] * output_buffer[i];
          --outputs_left_to_average;
        }
      }
    }
    output_squared_average /= outputs_to_average;
    output_squared_average = sqrtf(output_squared_average) * sqrtf(2.0f);
    if (output_squared_average == 0.0f) {
      return -160.0f;
    } else {
      return 10.0f * log10f(output_squared_average);
    }
  }
};
