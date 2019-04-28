#pragma once

// This file implements a counter which increments once per second.

#include "gsl_includes.h"

// define the timer to use
#ifndef GSL_SECNT_TIMER
#define GSL_SECNT_TIMER TIM5
#endif

// priority
#ifndef GSL_SECNT_PRIORITY
#define GSL_SECNT_PRIORITY 13
#endif

// total seconds since reset
uint32_t gsl_secnt_seconds = 0;

// ticks per timer update
uint32_t gsl_secnt_ticks_per_update = 0;

// callback routine
void GSL_SECNT_Callback(void) {
  ++gsl_secnt_seconds;
}

// initialize the second counter
// (this is called in GSL_DEL initialization)
void GSL_SECNT_Initialize(void) {
  GSL_TIM_Reserve(GSL_SECNT_TIMER);
  // set up the timer frequency
  GSL_TIM_SetFrequency(GSL_SECNT_TIMER, 1.0f, 0.01f);
  // get exact tick frequency
  gsl_secnt_ticks_per_update =
      GSL_TIM_GetTicksPerUpdate(GSL_SECNT_TIMER);
  // make sure it's exactly one second
  //if (GSL_TIM_GetFrequency(GSL_SECNT_TIMER) != 1.0f) {
  //  HALT("Unexpected clock");
  //}
  // set the callback routine (low priority)
  GSL_TIM_SetUpdateCallback(GSL_SECNT_TIMER,
                            GSL_SECNT_Callback,
                            GSL_SECNT_PRIORITY);
  // start the timer
  DWT->CYCCNT = 0;
  GSL_TIM_Start(GSL_SECNT_TIMER);
}
