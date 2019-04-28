#pragma once

#include "gsl_includes.h"

// This includes some defines for converting the __DATE__ macro into other
// versions.

// the current year, as an integer
#define DATE_YEAR ((__DATE__[7] - '0') * 1000 + (__DATE__[8] - '0') * 100 + \
                   (__DATE__[9] - '0') * 10 + (__DATE__[10] - '0'))

// the current month, as an integer, starting with Jan = 1
#define DATE_MONTH (\
      __DATE__ [2] == 'n' ? (__DATE__ [1] == 'a' ? 1 : 6) \
    : __DATE__ [2] == 'b' ? 2 \
    : __DATE__ [2] == 'r' ? (__DATE__ [0] == 'M' ? 3 : 4) \
    : __DATE__ [2] == 'y' ? 5 \
    : __DATE__ [2] == 'l' ? 7 \
    : __DATE__ [2] == 'g' ? 8 \
    : __DATE__ [2] == 'p' ? 9 \
    : __DATE__ [2] == 't' ? 10 \
    : __DATE__ [2] == 'v' ? 11 \
    : 12)

// the current day of the month, as an integer
#define DATE_DAY (((__DATE__[4] == ' ') ? 0 : __DATE__[4] - '0') * 10 + \
                  (__DATE__[5] - '0'))
