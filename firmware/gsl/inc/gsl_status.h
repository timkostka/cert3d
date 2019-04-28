#pragma once

// This file contains routines for easily manipulating the ERR and STS pins

#include "gsl_includes.h"

// signal enum
enum kStatusEnum {
  kStatusOff,
  kStatusOn,
  kStatusFastBlink,
  kStatusSlowBlink,
  kStatusBlinkOnce,
  kStatusBlinkTwice,
  kStatusBlinkThrice,
};

// current status
__IO kStatusEnum gsl_status_state = kStatusOff;

// error state
__IO kStatusEnum gsl_error_state = kStatusOff;

// callback period in ms
const uint16_t gsl_status_callback_period = 100;

#ifndef GSL_STATUS_TIMER
#define GSL_STATUS_TIMER TIM13
#endif

// callback routine
void GSL_STATUS_Callback(void) {
  const uint8_t off_cycles = 7;
  // process the status LED
  {
    static uint8_t status_phase = 0;
    ++status_phase;
    switch (gsl_status_state) {
    case kStatusOff:
      GSL_LED_StatusOff();
      break;
    case kStatusOn:
      GSL_LED_StatusOn();
      break;
    case kStatusFastBlink:
      GSL_LED_StatusToggle();
      break;
    case kStatusSlowBlink:
      status_phase %= 5;
      if (status_phase == 0) {
        GSL_LED_StatusToggle();
      }
      break;
    case kStatusBlinkOnce:
      status_phase %= off_cycles + 1;
      if (status_phase >= off_cycles &&
          (status_phase - off_cycles) % 2 == 0) {
        GSL_LED_StatusOn();
      } else {
        GSL_LED_StatusOff();
      }
      break;
    case kStatusBlinkTwice:
      status_phase %= off_cycles + 3;
      if (status_phase >= off_cycles &&
          (status_phase - off_cycles) % 2 == 0) {
        GSL_LED_StatusOn();
      } else {
        GSL_LED_StatusOff();
      }
      break;
    case kStatusBlinkThrice:
      status_phase %= off_cycles + 5;
      if (status_phase >= off_cycles &&
          (status_phase - off_cycles) % 2 == 0) {
        GSL_LED_StatusOn();
      } else {
        GSL_LED_StatusOff();
      }
      break;
    }
  }
  // process the error LED
  {
    static uint8_t error_phase = 0;
    ++error_phase;
    switch (gsl_error_state) {
    case kStatusOff:
      GSL_LED_ErrorOff();
      break;
    case kStatusOn:
      GSL_LED_ErrorOn();
      break;
    case kStatusFastBlink:
      GSL_LED_ErrorToggle();
      break;
    case kStatusSlowBlink:
      error_phase %= 5;
      if (error_phase == 0) {
        GSL_LED_ErrorToggle();
      }
      break;
    case kStatusBlinkOnce:
      error_phase %= off_cycles + 1;
      if (error_phase >= off_cycles &&
          (error_phase - off_cycles) % 2 == 0) {
        GSL_LED_ErrorOn();
      } else {
        GSL_LED_ErrorOff();
      }
      break;
    case kStatusBlinkTwice:
      error_phase %= off_cycles + 3;
      if (error_phase >= off_cycles &&
          (error_phase - off_cycles) % 2 == 0) {
        GSL_LED_ErrorOn();
      } else {
        GSL_LED_ErrorOff();
      }
      break;
    case kStatusBlinkThrice:
      error_phase %= off_cycles + 5;
      if (error_phase >= off_cycles &&
          (error_phase - off_cycles) % 2 == 0) {
        GSL_LED_ErrorOn();
      } else {
        GSL_LED_ErrorOff();
      }
      break;
    }
  }
}

// initialize
void GSL_STATUS_Initialize(void) {
  GSL_TIM_SetFrequency(
      GSL_STATUS_TIMER,
      1000.0f / gsl_status_callback_period);
  // set low priority callback routine
  GSL_TIM_SetUpdateCallback(GSL_STATUS_TIMER, GSL_STATUS_Callback, 11);
  // start the timer
  GSL_TIM_Start(GSL_STATUS_TIMER);
}

// automatically initialize
GSL_INITIALIZER gsl_status_initializer(GSL_STATUS_Initialize);
