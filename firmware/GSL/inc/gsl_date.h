#pragma once

// This file implements the GSL_DATE object, which contains dates

#include "gsl_includes.h"

#include "gsl_date_defines.h"

struct GSL_DATE {

  // year
  uint16_t year;

  // month (0 = January)
  uint8_t month;

  // day of the month (0 = first day)
  uint8_t day;

  // number of days per week
  static const uint8_t kDaysPerWeek = 7;

  // number of months
  static const uint8_t kMonthsPerYear = 12;

  // constants for days of the week
  static const char * kDayName[kDaysPerWeek];

  // constants for months
  static const char * kMonthName[kMonthsPerYear];

  // number of days in each month (28 for February)
  static const uint8_t kDaysInMonth[];

  // return true if the given year is a leap year
  static bool IsLeapYear(uint16_t year) {
    // years not divisible by 4 are not leap years
    if (year % 4 != 0) {
      return false;
    }
    // years divisible by 100 are leap years, unless they are also
    // divisible by 400
    if (year % 100 == 0 && year % 400 != 0) {
      return false;
    }
    // it's leap year
    return true;
  }

  // return true if this year is a leap year
  bool IsLeapYear(void) const {
    return IsLeapYear(year);
  }

  // return the number of days in the given month
  uint16_t GetDaysInMonth(void) const {
    ASSERT_LE(month, kMonthsPerYear);
    if (month == 1 && IsLeapYear()) {
      return kDaysInMonth[month] + 1;
    } else {
      return kDaysInMonth[month];
    }
  }

  // return true if the date is valid
  // (for example, February 30th, 2000 is not a valid date, though it may work
  // for some cases)
  bool IsValid(void) const {
    return month < 12 &&
        day < GetDaysInMonth();
  }

  // constructor
  // default = January 1, 2000
  GSL_DATE(uint16_t year_ = 2000, uint16_t month_ = 1, uint16_t day_ = 1) :
    year(year_), month(month_ - 1), day(day_ - 1) {
  }

  // assignment operator
  GSL_DATE & operator= (const GSL_DATE & that) {
    memcpy(this, &that, sizeof(*this));
    return *this;
  }

  // return the number of days in the given year
  static uint16_t GetDaysInYear(uint16_t year) {
    if (IsLeapYear(year)) {
      return 366;
    } else {
      return 365;
    }
  }

  // return the number of days since the start of the year
  // Jan 1 = 0
  uint16_t GetDayOfYear(void) const {
    uint16_t day_of_year = day;
    uint16_t i = month;
    while (i > 0) {
      --i;
      // add number of days in the previous month
      day_of_year += kDaysInMonth[i];
      // add 1 day for February on leap years
      if (i == 1 && IsLeapYear()) {
        ++day_of_year;
      }
    }
    return day_of_year;
  }

  // return the number of days from the first date until the second date
  static int32_t GetDaysBetween(const GSL_DATE & start_date,
                                 const GSL_DATE & end_date) {
    int32_t days = 0;
    days += end_date.GetDayOfYear();
    days -= start_date.GetDayOfYear();
    if (start_date.year < end_date.year) {
      for (uint16_t i = start_date.year; i < end_date.year; ++i) {
        days += GetDaysInYear(i);
      }
    } else {
      for (uint16_t i = end_date.year; i < start_date.year; ++i) {
        days -= GetDaysInYear(i);
      }
    }
    return days;
  }

  // return the number of days from this day and the input date
  // if the input is tomorrow, this will return 1
  // if the input is yesterday, this will return -1
  int32_t GetDaysUntil(const GSL_DATE & target_date) const {
    return GetDaysBetween(*this, target_date);
  }

  // return the day of the week
  // January 1, 1970 was a Thursday
  // 0 = Sunday, 6 = Saturday
  uint16_t GetDayOfWeek(void) const {
    // reference date for day of the week
    static const GSL_DATE gsl_date_reference(1970, 1, 1);
    // reference date for day of the week
    static const uint16_t gsl_date_reference_day_of_week = 4;
    int32_t days = gsl_date_reference.GetDaysUntil(*this);
    return (days + gsl_date_reference_day_of_week - 1) % kDaysPerWeek + 1;
  }

  // convert the date to a string and return a pointer
  // the pointer will remain valid until the next call to this function
  // e.g. January 1, 2000
  const char * ToString(bool weekday = false) const {
    // buffer for holding the string
    static char buffer[32] = {0};
    // zero the string
    buffer[0] = 0;
    if (weekday) {
      strcpy(&buffer[strlen(buffer)], kDayName[GetDayOfWeek()]);
      strcpy(&buffer[strlen(buffer)], ", ");
    }
    ASSERT_LE(month, sizeof(kMonthName));
    strcpy(&buffer[strlen(buffer)], kMonthName[month]);
    strcpy(&buffer[strlen(buffer)], " ");
    strcpy(&buffer[strlen(buffer)], GSL_OUT_Integer(day + 1));
    strcpy(&buffer[strlen(buffer)], ", ");
    strcpy(&buffer[strlen(buffer)], GSL_OUT_Integer(year));
    return buffer;
  }

  // go back X days in the past
  void MoveDaysBack(uint32_t day_count) {
    // go back each month until we can just move back days in the current month
    while (day_count > day) {
      day_count -= day;
      day -= day;
      if (month == 0) {
        month += kMonthsPerYear;
        ASSERT_GT(year, 0);
        --year;
      }
      --month;
      day = GetDaysInMonth();
      --day_count;
      --day;
    }
    // move back in the current month X days
    day -= day_count;
  }

  // advance X days
  void MoveDaysAhead(uint32_t day_count) {
    day += day_count;
    while (day >= GetDaysInMonth()) {
      day -= GetDaysInMonth();
      ++month;
      if (month >= kMonthsPerYear) {
        month -= kMonthsPerYear;
        ++year;
      }
    }
  }

  // return the next day
  GSL_DATE NextDay(void) const {
    GSL_DATE tomorrow = *this;
    tomorrow.MoveDaysAhead(1);
    return tomorrow;
  }

};

// run some sanity checks
void GSL_DATE_VerificationChecks(void) {
  // 1600 is a leap year
  ASSERT(GSL_DATE::IsLeapYear(1600));
  // 1700 is not a leap year
  ASSERT(!GSL_DATE::IsLeapYear(1700));
  // 1601 is not a leap year
  ASSERT(!GSL_DATE::IsLeapYear(1601));
  // 1604 is a leap year
  ASSERT(GSL_DATE::IsLeapYear(1604));
  // 1/1/1970 is a Thursday
  ASSERT_EQ(GSL_DATE(1970, 1, 1).GetDayOfWeek(), 4);
  // 12619 days between 1/1/1970 and 1/2/1970
  ASSERT_EQ(GSL_DATE(1970, 1, 1).GetDaysUntil(GSL_DATE(1970, 1, 2)), 1);
  // 17274 days between 1/1/1970 and 4/18/2017
  ASSERT_EQ(GSL_DATE(1970, 1, 1).GetDaysUntil(GSL_DATE(2017, 4, 18)), 17274);
  // 4/18/2017 is a Tuesday
  ASSERT_EQ(GSL_DATE(2017, 4, 18).GetDayOfWeek(), 2);

  // move dates
  GSL_DATE then(2017, 4, 18);
  GSL_DATE now = then;
  for (int32_t i = 0; i <= 17274; ++i) {
    ASSERT_EQ(now.GetDaysUntil(then), i);
    ASSERT_EQ(then.GetDaysUntil(now), -i);
    now.MoveDaysBack(1);
  }
}

// more extensive sanity checks
void GSL_DATE_ExtendedVerificationChecks(void) {
}

// call sanity checks during initialization
// takes 242ms on a 100MHz processor
//GSL_INITIALIZER gsl_date_initializer(GSL_DATE_VerificationChecks);

// constants for days of the week string
const char * GSL_DATE::kDayName[GSL_DATE::kDaysPerWeek] = {
    "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday,", "Friday",
    "Saturday"};

// constants for the names of the month
const char * GSL_DATE::kMonthName[GSL_DATE::kMonthsPerYear] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"};

// number of days in each month (28 for February)
const uint8_t GSL_DATE::kDaysInMonth[GSL_DATE::kMonthsPerYear] =
    {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

// compilation date
const GSL_DATE
    gsl_date_compilation(DATE_YEAR, DATE_MONTH, DATE_DAY);
