#pragma once

#include <stdint.h>

#include "gsl_includes.h"

// this must be defined elsewhere
void LOG(const char * message);

// log an unsigned integer
void LOG(uint32_t value) {
  LOG(GSL_OUT_Integer(value));
}

// log a signed integer
void LOG(int32_t value) {
  if (value < 0) {
    LOG("-");
    value *= -1;
  }
  LOG((uint32_t) value);
}

// log two things
template <class T, class T2>
void LOG(T one, T2 two) {
  LOG(one);
  LOG(two);
}

// log three things
template <class T, class T2, class T3>
void LOG(T one, T2 two, T3 three) {
  LOG(one);
  LOG(two);
  LOG(three);
}

// log four things
template <class T, class T2, class T3, class T4>
void LOG(T one, T2 two, T3 three, T4 four) {
  LOG(one);
  LOG(two);
  LOG(three);
  LOG(four);
}

// log five things
template <class T, class T2, class T3, class T4, class T5>
void LOG(T one, T2 two, T3 three, T4 four, T5 five) {
  LOG(one);
  LOG(two);
  LOG(three);
  LOG(four);
  LOG(five);
}

// log six things
template <class T, class T2, class T3, class T4, class T5, class T6>
void LOG(T one, T2 two, T3 three, T4 four, T5 five, T6 six) {
  LOG(one);
  LOG(two);
  LOG(three);
  LOG(four);
  LOG(five);
  LOG(six);
}

// log seven things
template <class T, class T2, class T3, class T4, class T5, class T6, class T7>
void LOG(T one, T2 two, T3 three, T4 four, T5 five, T6 six, T7 seven) {
  LOG(one);
  LOG(two);
  LOG(three);
  LOG(four);
  LOG(five);
  LOG(six);
  LOG(seven);
}
