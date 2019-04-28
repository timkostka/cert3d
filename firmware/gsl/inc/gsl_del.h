#pragma once

#include "gsl_includes.h"

// gsl_del.h implements a number of delay and timing functions

// There are several delay functions which will block execution for a given
// number of microsecond, milliseconds, or seconds:
// * GSL_DEL_US()
// * GSL_DEL_MS()
// * GSL_DEL_S()
// For example, to wait 5 milliseconds, call GSL_DEL_MS(5);
//
// There are a few functions which measure the amount of time elapsed between
// events.
// * GSL_DEL_ElapsedUS()
// * GSL_DEL_ElapsedMS()
// * GSL_DEL_ElapsedS()
// Each of these takes up to two time arguments.  If only one time argument is
// given, it returns the time between the argument and the current time.  If no
// arguments are given, it returns the time since reset.  If both arguments are
// given, it returns the time between the two.
//
// To measure the time it takes to do something, you can use the following:
//   uint32_t start_tick = GSL_DEL_Ticks();
//   // (do stuff here)
//   uint32_t elapsed_ms = GSL_DEL_ElapsedMS(start_tick);
//
// Note that the using ticks to measure time will limit the ability to measure
// events which are far apart.  For example, at a clock speed of 180 MHz, the
// tick counter will overflow every 23.8 seconds.  Thus, the result return will
// be incorrect if the event takes longer than this.
//
// To overcome this deficiency, one can also use the GSL_DEL_LongTime type
// which can measure events 136.1 years apart.  However, calling this
// function takes longer than simply using the ticks.
//
// For example, to measure an event which may take longer than 24 seconds, you
// can use the following:
//   GSL_DEL_LongTime start_time = GSL_DEL_GetLongTime();
//   // (do stuff here that may take >24 seconds)
//   uint32_t elapsed_ms = GSL_DEL_ElapsedMS(start_time);
//
// These functions assume (1) the system clock does not change, and (2) the
// system does not go to sleep.

// comparison value
#define GSL_DEL_CompareValue ((uint32_t) 200000000)

// system clock speed
uint32_t gsl_del_system_clock = 0;

// holds a time since reset
// Note that this will overflow after 136.1 years, presumably long enough that
// it won't ever happen.
struct GSL_DEL_LongTime {
  uint64_t ticks;
  // equality operator
  bool operator == (const GSL_DEL_LongTime & that) const {
    return ticks == that.ticks;
  }
  // inequality operator
  bool operator != (const GSL_DEL_LongTime & that) const {
    return !(*this == that);
  }
};

// return the system ticks since reset
// Note: this wraps every ~30s for a clock of 168MHz
inline uint32_t GSL_DEL_Ticks(void) {
  return DWT->CYCCNT;
}

// wait the given amount of time in microseconds
void GSL_DEL_US(uint32_t us) {
  // store the starting tick value
  uint32_t start = GSL_DEL_Ticks();
  uint32_t target = (float) us * HAL_RCC_GetSysClockFreq() / 1e6;
  target += start;
  while (GSL_DEL_Ticks() - target > GSL_DEL_CompareValue) {
  }
}

// wait the given amount of time in milliseconds
void GSL_DEL_MS(uint32_t ms) {
  // store the starting tick value
  uint32_t start = GSL_DEL_Ticks();
  uint32_t target = (float) ms * HAL_RCC_GetSysClockFreq() / 1e3;
  target += start;
  while (GSL_DEL_Ticks() - target > GSL_DEL_CompareValue) {
  }
}

// wait the given amount of time in seconds
void GSL_DEL_S(uint32_t s) {
  while (s--) {
    GSL_DEL_MS(1000);
  }
}

// DEBUG
void LOGLONGINT(uint64_t);

// return the time since reset as a GSL_DEL_LongTime value
// ticks must have been taken in the past 10 seconds
GSL_DEL_LongTime GSL_DEL_GetLongTime(uint32_t ticks = GSL_DEL_Ticks()) {
  // hold the total ticks since reset, approximately
  uint64_t total_ticks =
      (uint64_t) gsl_secnt_seconds * gsl_secnt_ticks_per_update;
  uint32_t offset = ticks - (uint32_t) total_ticks;
  // DEBUG
  //if (offset >= gsl_secnt_ticks_per_update * 3) {
  //  LOG("\nERR:   ticks=", ticks, "   offset=", offset);
  //  ASSERT(offset < gsl_secnt_ticks_per_update * 3);
  //}
  //LOG("\nsec=", gsl_secnt_seconds);
  //LOG("   ticks=", ticks);
  //LOG("   total=");
  //LOGLONGINT(total_ticks);
  //LOG("   delta=", offset);
  total_ticks += offset;
  // hold the result
  GSL_DEL_LongTime result;
  result.ticks = total_ticks;
  //result.seconds = total_ticks / gsl_del_system_clock;
  //result.microseconds = total_ticks % gsl_del_system_clock;
  //result.microseconds /= gsl_del_system_clock / 1.0e6f;
  return result;
}

// return the number of microseconds elapsed since the given event
// if no event is specified, then use the time at reset
// (will overflow in ~1.2 hours)
uint32_t GSL_DEL_ElapsedUS(
    GSL_DEL_LongTime event_one = {0},
    GSL_DEL_LongTime event_two = GSL_DEL_GetLongTime()) {
  return (event_two.ticks - event_one.ticks) /
      (gsl_del_system_clock / 1e6f);
}

// return the number of milliseconds elapsed since the given event
// if no event is specified, then use the time at reset
// (will overflow in ~50 days)
uint32_t GSL_DEL_ElapsedMS(
    GSL_DEL_LongTime event_one = {0},
    GSL_DEL_LongTime event_two = GSL_DEL_GetLongTime()) {
  return (event_two.ticks - event_one.ticks) /
      (gsl_del_system_clock / 1e3f);
}

// return the number of seconds elapsed since the given event
// if no event is specified, then use the time at reset
float GSL_DEL_ElapsedSFloat(
    GSL_DEL_LongTime event_one = {0},
    GSL_DEL_LongTime event_two = GSL_DEL_GetLongTime()) {
  return (event_two.ticks - event_one.ticks) /
      (float) gsl_del_system_clock;
}

// return the number of seconds elapsed since the given event
// if no event is specified, then use the time at reset
// (will overflow in ~136 years)
uint32_t GSL_DEL_ElapsedS(
    GSL_DEL_LongTime event_one = {0},
    GSL_DEL_LongTime event_two = GSL_DEL_GetLongTime()) {
  return (event_two.ticks - event_one.ticks) /
      gsl_del_system_clock;
}

// return the number of microseconds elapsed since the given event
// if no event is specified, then use the time at reset
uint32_t GSL_DEL_ElapsedUS(
    uint32_t event_one = 0,
    uint32_t event_two = GSL_DEL_Ticks()) {
  return (event_two - event_one) * 1000000.0f / gsl_del_system_clock;
}

// return the number of milliseconds elapsed since the given event
// if no event is specified, then use the time at reset
uint32_t GSL_DEL_ElapsedMS(
    uint32_t event_one = 0,
    uint32_t event_two = GSL_DEL_Ticks()) {
  return (event_two - event_one) * 1000.0f / gsl_del_system_clock;
}

// return the number of seconds elapsed since the given event
// if no event is specified, then use the time at reset
uint32_t GSL_DEL_ElapsedS(
    uint32_t event_one = 0,
    uint32_t event_two = GSL_DEL_Ticks()) {
  return (event_two - event_one) / gsl_del_system_clock;
}

// return the number of seconds elapsed since the given event
// if no event is specified, then use the time at reset
float GSL_DEL_ElapsedSFloat(
    uint32_t event_one = 0,
    uint32_t event_two = GSL_DEL_Ticks()) {
  return (event_two - event_one) / (float) gsl_del_system_clock;
}

// initialize the debug timer and the second counter
void GSL_DEL_Initialize(void) {
  // get system clock speed
  gsl_del_system_clock = HAL_RCC_GetSysClockFreq();
  // enable the core clock driver
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  // enable the clock counter
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
  // initialize the second counter
  DWT->CYCCNT = 0;
  GSL_SECNT_Initialize();

  // sync the clocks to within a second of each other
  //gsl_secnt_seconds = 0;
}

// dummy object to initialize the counter
//GSL_INITIALIZER GSL_DEL_Initializer(GSL_DEL_Initialize);

// do some checks on these functions to ensure everything is working right
void GSL_DEL_SanityCheck(void) {
  // ensure 1000us delay is about right
  {
    uint32_t start = GSL_DEL_Ticks();
    GSL_DEL_US(1000);
    float elapsed_us =
        (GSL_DEL_Ticks() - start) / (float) gsl_del_system_clock * 1e6;
    ASSERT(elapsed_us >= 900);
    ASSERT(elapsed_us <= 1100);
  }
  // ensure 1ms delay is about right
  {
    uint32_t start = GSL_DEL_Ticks();
    GSL_DEL_MS(3);
    float elapsed_ms =
        (GSL_DEL_Ticks() - start) / (float) gsl_del_system_clock * 1e3;
    ASSERT(elapsed_ms >= 2.9f);
    ASSERT(elapsed_ms <= 3.1f);
  }
  // ensure LongTime matches normal ticks to within 4us
  for (uint16_t i = 0; i < 1000; ++i) {
    uint32_t start_sec = gsl_secnt_seconds;
    uint32_t start = GSL_DEL_Ticks();
    auto start_full = GSL_DEL_GetLongTime(start);
    GSL_DEL_MS(1);
    GSL_DEL_US(1000);
    uint32_t end = GSL_DEL_Ticks();
    uint32_t min_ticks = gsl_del_system_clock / 500;
    auto end_full = GSL_DEL_GetLongTime(end);
    uint32_t end_sec = gsl_secnt_seconds;
    float elapsed_us_ticks = GSL_DEL_ElapsedUS(start, end);
    float elapsed_us_full = GSL_DEL_ElapsedUS(start_full, end_full);
    float error = elapsed_us_ticks - elapsed_us_full;
    if (error < 0.0f) {
      error = -error;
    }
    if (error >= 4.0f ||
        end - start < min_ticks ||
        elapsed_us_ticks < 1900) {
      LOG("\nstart = ", start);
      LOG("\nstart = ");
      LOG(start_full.ticks);
      LOG("\nend = ", end);
      LOG("\nend = ");
      LOG(end_full.ticks);
      LOG("\nstart/end seconds: ", start_sec, "/", end_sec);
      LOG("\nus_ticks = ", (uint32_t) elapsed_us_ticks);
      LOG("\nus_full = ", (uint32_t) elapsed_us_full);
      LOG("\nerror = ", GSL_OUT_FixedFloat(error, 1), " us");
    }
    ASSERT(end - start >= min_ticks);
    ASSERT(error < 4.0f);
    ASSERT(elapsed_us_ticks >= 1900);
  }
}

// An GSL_DEL_ShortTimer is used for durations less than the overflow of the
// debug counter (about 30 seconds).
struct GSL_DEL_ShortTimer {
  // ticks at start
  uint32_t start_tick;
  // initializer
  GSL_DEL_ShortTimer(void) : start_tick(GSL_DEL_Ticks()) {
  }
  // elapsed ms
  uint32_t GetElapsedMS(void) const {
    return GSL_DEL_ElapsedMS(start_tick);
  }
  // elapsed seconds as a float
  float GetElapsedSFloat(void) const {
    return GSL_DEL_ElapsedSFloat(start_tick);
  }
  // wait until a given delay is reached
  void WaitUntilMS(uint16_t ms) const {
    while (GetElapsedMS() < ms) {
    }
  }
  // reset the timer to zero
  void Reset() {
    start_tick = GSL_DEL_Ticks();
  }
};

// An GSL_DEL_Timer wraps some GSL_DEL_* functions into a timer object.
struct GSL_DEL_Timer {
  // ticks at start
  GSL_DEL_LongTime start_time;
  // ticks when stopped, if any
  GSL_DEL_LongTime stop_time;
  // if true, timer is running
  bool running;
  // initializer
  GSL_DEL_Timer(void) :
    start_time(GSL_DEL_GetLongTime()),
    stop_time(GSL_DEL_GetLongTime()),
    running(true) {
  }
  // reset the timer to 0 and restart
  void Reset(void) {
    start_time = GSL_DEL_GetLongTime();
    running = true;
  }
  // set the timer to 0 and stop
  void Clear(void) {
    start_time = GSL_DEL_GetLongTime();
    stop_time = start_time;
    running = false;
  }
  // start the timer
  void Start(void) {
    if (running) {
      return;
    }
    GSL_DEL_LongTime this_time = GSL_DEL_GetLongTime();
    start_time.ticks += this_time.ticks - stop_time.ticks;
    running = true;
  }
  // stop the timer
  void Stop(void) {
    if (!running) {
      return;
    }
    running = false;
    stop_time = GSL_DEL_GetLongTime();
  }
  // elapsed ms
  uint32_t GetElapsedMS(void) const {
    if (running) {
      return GSL_DEL_ElapsedMS(start_time);
    } else {
      return GSL_DEL_ElapsedMS(start_time, stop_time);
    }
  }
  // elapsed seconds as a float
  float GetElapsedSFloat(void) const {
    if (running) {
      return GSL_DEL_ElapsedSFloat(start_time);
    } else {
      return GSL_DEL_ElapsedSFloat(start_time, stop_time);
    }
  }
};
