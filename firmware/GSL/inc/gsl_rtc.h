#pragma once

// This file (gsl_rtc.h) has function to interface with the RTC.
//
// Internally, the RTC stores the year with two digits, so we are limited
// to that resolution.  Where it matters, we will assume the year is between
// 1950-2049.

#include "gsl_includes.h"

// handle for the RTC
RTC_HandleTypeDef gsl_rtc_hrtc = {
    RTC,
    {
        RTC_HOURFORMAT_24, // HourFormat (RTC_Hour_Formats)
        31, // AsynchPrediv (0x00 to 0x7F)
        1023, // SynchPrediv (0x00 to 0x7FFFU)
        RTC_OUTPUT_DISABLE, // OutPut (RTC_Output_selection_Definitions)
        RTC_OUTPUT_POLARITY_HIGH, // OutPutPolarity (RTC_Output_Polarity_Definitions)
        RTC_OUTPUT_TYPE_PUSHPULL, // OutPutType (RTC_Output_Type_ALARM_OUT)
    },
    HAL_UNLOCKED,
    HAL_RTC_STATE_RESET};

// data format for RTC readings
struct GSL_RTC_DateTimeStruct {
  // date
  RTC_DateTypeDef date;
  // time
  RTC_TimeTypeDef time;
};

// time difference
struct GSL_RTC_TimeDifferenceStruct {
  // days
  int32_t days;
  // seconds
  int32_t seconds;
  // subseconds
  int32_t milliseconds;
};

// return true if the given year is a leap year
bool GSL_RTC_IsLeapYear(uint16_t year) {
  ASSERT(year >= 1950 && year <= 2049);
  // only years divisible by 4 can be leap years
  if (year % 4 != 0) {
    return false;
  }
  // 2000 is a leap year, 2100 is not
  if ((year % 100 == 0) && (year % 400 != 0)) {
    return false;
  }
  return true;
}

// return the number of days in the given year
uint16_t GSL_RTC_DaysInYear(uint16_t year) {
  ASSERT(year >= 1950 && year <= 2049);
  return (GSL_RTC_IsLeapYear(year)) ? 366 : 365;
}

// return the number of days in the given month
uint16_t GSL_RTC_DaysInMonth(uint8_t month, uint8_t year) {
  static const uint8_t days_in_month[] =
    {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  ASSERT(month < sizeof(days_in_month) / sizeof(*days_in_month));
  // for February, adjust for leap year
  if (month == RTC_MONTH_FEBRUARY) {
    if (GSL_RTC_IsLeapYear(year)) {
      return 29;
    } else {
      return 28;
    }
  } else {
    return days_in_month[month];
  }
}

// return the day of the year
// 0 = January 1
// 364 (or 365) = December 31
uint16_t GSL_RTC_DayOfTheYear(const RTC_DateTypeDef & date) {
  uint16_t result = 0;
  // add days for months before the given month
  for (uint8_t i = 1; i < date.Month; ++i) {
    result += GSL_RTC_DaysInMonth(i, date.Year);
  }
  // add day of the month
  result += date.Date - 1;
  return result;
}

// return the number of days between the two events
int32_t GSL_RTC_DaysUntil(const RTC_DateTypeDef & begin,
                          const RTC_DateTypeDef & end) {
  int32_t result = 0;
  ASSERT(false);
  return result;
}

bool operator==(const RTC_DateTypeDef & one,
                const RTC_DateTypeDef & two) {
  return (one.Date == two.Date) &&
      (one.Month == two.Month) &&
      (one.Year == two.Year);
}

bool operator==(const RTC_TimeTypeDef & one,
                const RTC_TimeTypeDef & two) {
  return (one.Hours == two.Hours) &&
      (one.Minutes == two.Minutes) &&
      (one.Seconds == two.Seconds) &&
      (one.SubSeconds == two.SubSeconds);
}

// compare two GSL_RTC_DateTimeStruct
bool operator==(const GSL_RTC_DateTimeStruct & one,
                const GSL_RTC_DateTimeStruct & two) {
  return (one.date == two.date) && (one.time == two.time);
}

// return the milliseconds of the given time
uint16_t GSL_RTC_GetMilliseconds(const RTC_TimeTypeDef & time) {
  return 1000.0f * (time.SecondFraction - time.SubSeconds) /
      (time.SecondFraction + 1) + 0.5f;
}

// return the actual year from the two-digit year
uint16_t GSL_RTC_GetYear(uint8_t year) {
  ASSERT(year <= 99);
  return (year < 50) ? 2000 + year : 1900 + year;
}

// compare two RTC_TimeTypeDef
bool operator<(const RTC_DateTypeDef & one,
               const RTC_DateTypeDef & two) {
  return (GSL_RTC_GetYear(one.Year) < GSL_RTC_GetYear(two.Year)) ||
      ((GSL_RTC_GetYear(one.Year) == GSL_RTC_GetYear(two.Year)) &&
      ((one.Month < two.Month) ||
          ((one.Month == two.Month) && (one.Date < two.Date))));
}

// compare two RTC_TimeTypeDef
bool operator<(const RTC_TimeTypeDef & one,
               const RTC_TimeTypeDef & two) {
  return (one.Hours < two.Hours) ||
      ((one.Hours == two.Hours) && ((one.Minutes < two.Minutes) ||
      ((one.Minutes == two.Minutes) && ((one.Seconds < two.Seconds) ||
      ((one.Seconds == two.Seconds) && (one.SubSeconds > two.SubSeconds))))));
}

// compare two GSL_RTC_DateTimeStruct
bool operator<(const GSL_RTC_DateTimeStruct & one,
               const GSL_RTC_DateTimeStruct & two) {
  return (one.date < two.date) ||
      ((one.date == two.date) && (one.time < two.time));
}

// normalize a time difference
// 0 <= seconds < 86400
// 0 <= milliseconds < 1000
void GSL_RTC_Normalize(GSL_RTC_TimeDifferenceStruct & time_difference) {
  // make milliseconds positive
  while (time_difference.milliseconds < 0) {
    time_difference.milliseconds += 1000;
    time_difference.seconds -= 1;
  }
  // make seconds positive
  while (time_difference.seconds < 0) {
    time_difference.seconds += 86400;
    time_difference.days -= 1;
  }
  // ensure 0 <= milliseconds < 1000 (milliseconds in a second)
  time_difference.seconds += time_difference.milliseconds / 1000;
  time_difference.milliseconds %= 1000;
  // ensure 0 <= seconds < 86400 (seconds in a day)
  time_difference.days += time_difference.seconds / 86400;
  time_difference.seconds %= 86400;
}

// convert a time different into seconds in floating point format
// note that some precision may be lost
float GSL_RTC_ToSeconds(const GSL_RTC_TimeDifferenceStruct & time_difference) {
  float result = 0;
  // add in days
  result += 86400 * time_difference.days;
  result += time_difference.seconds;
  result += time_difference.milliseconds / 1000.0f;
  return result;
}

// return the second of the day for the given time (ignoring milliseconds)
uint16_t GSL_RTC_SecondOfTheDay(const RTC_TimeTypeDef & time) {
  return (uint16_t) time.Hours * 3600 +
      (uint16_t) time.Minutes * 60 +
      (uint16_t) time.Seconds;
}

// subtract two datetimes and return a timer difference
GSL_RTC_TimeDifferenceStruct operator-(const GSL_RTC_DateTimeStruct & end,
                                       const GSL_RTC_DateTimeStruct & begin) {
  // store the result
  GSL_RTC_TimeDifferenceStruct result = {0, 0, 0};
  // ensure end > begin, else recall the routine
  if (end < begin) {
    result = begin - end;
    result.days *= -1;
    result.seconds *= -1;
    result.milliseconds *= -1;
    GSL_RTC_Normalize(result);
    return result;
  }
  // adjust days to January 1 of each year
  result.days -= GSL_RTC_DayOfTheYear(begin.date);
  result.days += GSL_RTC_DayOfTheYear(end.date);
  // adjust for the years between the events
  ASSERT(GSL_RTC_GetYear(begin.date.Year) <= GSL_RTC_GetYear(end.date.Year));
  for (uint16_t year = GSL_RTC_GetYear(begin.date.Year);
      year < GSL_RTC_GetYear(end.date.Year);
      ++year) {
    result.days += GSL_RTC_DaysInYear(year);
  }
  // adjust for the seconds
  result.seconds -= GSL_RTC_SecondOfTheDay(begin.time);
  result.seconds += GSL_RTC_SecondOfTheDay(end.time);
  // adjust for milliseconds
  result.milliseconds -= GSL_RTC_GetMilliseconds(begin.time);
  result.milliseconds += GSL_RTC_GetMilliseconds(end.time);
  // now adjust so that seconds and milliseconds are positive
  GSL_RTC_Normalize(result);
  return result;
}

// start the RTC using the internal LSI clock (not that accurate)
void GSL_RTC_Start(void) {
  // if already started, don't do it again
  if (gsl_rtc_hrtc.State != HAL_RTC_STATE_RESET) {
    return;
  }
  // turn on the LSI oscillator
  {
    RCC_OscInitTypeDef osc_config;
    osc_config.OscillatorType = RCC_OSCILLATORTYPE_LSI;
    osc_config.LSIState = RCC_LSI_ON;
    // don't change the PLL
    osc_config.PLL.PLLState = RCC_PLL_NONE;
    HAL_RUN(HAL_RCC_OscConfig(&osc_config));
  }
  // tell RTC to use the LSI clock
  {
    RCC_PeriphCLKInitTypeDef clock_init;
    clock_init.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    clock_init.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
    HAL_RUN(HAL_RCCEx_PeriphCLKConfig(&clock_init));
  }
  // enable access to backup domain, which includes RTC registers
  HAL_PWR_EnableBkUpAccess();
  // enable the RTC clock
  __HAL_RCC_RTC_ENABLE();
  // turn on the RTC
  HAL_RUN(HAL_RTC_Init(&gsl_rtc_hrtc));
}

// stop the RTC and disable all associated clocks
void GSL_RTC_Stop(void) {
  // if not started, we're done
  if (gsl_rtc_hrtc.State == HAL_RTC_STATE_RESET) {
    return;
  }
  // deinitialize
  HAL_RUN(HAL_RTC_DeInit(&gsl_rtc_hrtc));
  // disable the RTC clock
  __HAL_RCC_RTC_DISABLE();
  // turn off the LSI oscillator
  {
    RCC_OscInitTypeDef osc_config;
    osc_config.OscillatorType = RCC_OSCILLATORTYPE_LSI;
    osc_config.LSIState = RCC_LSI_OFF;
    // don't change the PLL
    osc_config.PLL.PLLState = RCC_PLL_NONE;
    HAL_RUN(HAL_RCC_OscConfig(&osc_config));
  }
  // disable access to backup domain, which includes RTC registers
  HAL_PWR_EnableBkUpAccess();
}

// get the current date and time
GSL_RTC_DateTimeStruct GSL_RTC_GetDateTime(void) {
  // turn on the RTC if it's not already done
  GSL_RTC_Start();
  // hold the result
  GSL_RTC_DateTimeStruct result;
  // wait for shadow registers to be updated
  __HAL_RTC_WRITEPROTECTION_DISABLE(&gsl_rtc_hrtc);
  HAL_RTC_WaitForSynchro(&gsl_rtc_hrtc);
  __HAL_RTC_WRITEPROTECTION_ENABLE(&gsl_rtc_hrtc);
  // get the time
  HAL_RTC_GetTime(&gsl_rtc_hrtc, &result.time, RTC_FORMAT_BIN);
  // get the date
  HAL_RTC_GetDate(&gsl_rtc_hrtc, &result.date, RTC_FORMAT_BIN);
  // return our result
  return result;
}

// log the time (but not the date) to the debug log
// format: "HH:MM:SS.MMM"
// HH will be between 00 and 23
void GSL_RTC_LogTime(
    const RTC_TimeTypeDef & time = GSL_RTC_GetDateTime().time) {
  if (time.Hours < 10) {
    LOG("0");
  }
  LOG(time.Hours, ":");
  if (time.Minutes < 10) {
    LOG("0");
  }
  LOG(time.Minutes, ":");
  if (time.Seconds < 10) {
    LOG("0");
  }
  LOG(time.Seconds, ".");
  // convert SubSeconds to milliseconds
  uint16_t milliseconds = GSL_RTC_GetMilliseconds(time);
  // clip it to 999
  if (milliseconds > 999) {
    milliseconds = 999;
  }
  if (milliseconds < 100) {
    LOG("0");
  }
  if (milliseconds < 10) {
    LOG("0");
  }
  LOG(milliseconds);
}

// use the RTC to delay by the given amount of milliseconds
void GSL_RTC_DelayMS(uint32_t millis) {
  const auto start_time = GSL_RTC_GetDateTime();
  // loop until the amount of time has elapsed
  float delay_s = millis / 1000.0f;
  while (GSL_RTC_ToSeconds(GSL_RTC_GetDateTime() - start_time) < delay_s) {
  }
}

// attempt to find the LSI speed using the HSI oscillator
// HSI is +/- 1% accurate @ 25 C
// HSI is +/- 4% accurate from -10 to +85 C
// HSI is -8%/+4.5% accurate from -40 to +105 C
float GSL_RTC_EstimateLSISpeed(uint32_t delay_ms = 100) {
  HALT("TODO");
}

// use the LSI to figure approximate the system clock speed
// in my testing, the estimate was off by 8-10%
// the LSI is not terribly accurate or precise
// For the STM32F411, the LSI can range from 17-47 kHz.  This translates to
// an error of -30.2% to +92.7% over the temperature range -40 to 105 C.
uint32_t GSL_RTC_EstimateSystemClock(uint32_t delay_ms = 100) {
  ASSERT(delay_ms > 0);
  auto start = GSL_RTC_GetDateTime();
  uint32_t start_ticks = GSL_DEL_Ticks();
  GSL_RTC_DelayMS(delay_ms);
  auto end = GSL_RTC_GetDateTime();
  uint32_t end_ticks = GSL_DEL_Ticks();
  uint32_t elapsed_ticks = end_ticks - start_ticks;
  float elapsed_seconds = GSL_RTC_ToSeconds(end - start);
  float hse_estimate = elapsed_ticks / elapsed_seconds;
  return hse_estimate + 0.5f;
}
