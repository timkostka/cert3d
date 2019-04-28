#pragma once

#include <stdint.h>
#include <limits.h>

#include "gsl_includes.h"

// typedef to a void function with no arguments
typedef void (*GSL_ERROR_HandlerFunction)();

// if not null, this will be called when we encounter a hard fault
GSL_ERROR_HandlerFunction gsl_error_handler = nullptr;

// this will output the current code position to the log
#define LOG_POSITION \
  LOG("\n", GSL_GEN_GetTimestampSinceReset()); \
  LOG(": In file ", GSL_BaseFilename(__FILE__), " at line ", __LINE__, " in function ", __func__);

// forward defines so blinking the error LED will work
void GSL_PIN_Toggle(PinEnum pin);
void GSL_PIN_SetLow(PinEnum pin);
void GSL_PIN_Initialize(
    PinEnum pin,
    uint32_t gpio_mode,
    uint32_t resistor_mode,
    uint32_t gpio_speed);
void GSL_PIN_Deinitialize(PinEnum pin);
void GSL_DEL_MS(uint32_t delay_ms);

// HALT(message)
// stops the program and outputs debug information

// ASSERT(statement)
// if statement is false, stops the program and outputs debug information

// This provides the HALT("message") macro which stops program execution as
// well as the LOG function implementation
//
// In particular, the following function must be defined somewhere in your
// compilation unit:
//   void LOG(const char * message);

// return the filename without the path
const char * GSL_BaseFilename (const char  * filename) {
  const char * ptr = filename;
  while (*(++filename)) {
    if (*filename == '/' || *filename == '\\') {
      ptr = filename + 1;
    }
  }
  return ptr;
}

// if handler is defined, call it, else loop forever
// at the start, we set the USART interrupt priority to the highest possible
// so that the debug log clears out
#ifdef GSL_LED_ERROR_PIN
#define BLINK_ERROR \
  while (1) { \
    GSL_PIN_Deinitialize(GSL_LED_ERROR_PIN); \
    GSL_PIN_Initialize(GSL_LED_ERROR_PIN, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, GPIO_SPEED_LOW); \
    GSL_PIN_SetLow(GSL_LED_ERROR_PIN); \
    while (1) { \
      for (uint8_t i = 0; i < 6; ++i) { \
        GSL_DEL_MS(100); \
        GSL_PIN_Toggle(GSL_LED_ERROR_PIN); \
      } \
      GSL_DEL_MS(1000); \
      if (gsl_error_handler) { \
        GSL_DEL_MS(900); \
        gsl_error_handler(); \
      } \
      /*asm volatile ("bkpt 0");*/ \
    }; \
  }
#else
#define BLINK_ERROR \
  GSL_DEL_MS(200); \
  if (gsl_error_handler) { \
    gsl_error_handler(); \
  } \
  asm volatile ("bkpt 0"); \
  while (1);
#endif

// run the following statements the first X times it is encountered
#define LIMITED_RUN(times) \
    static uint16_t run_count = 0; \
    if (run_count <= times) { \
      ++run_count; \
    } \
    if (run_count <= times)

// output a message the first X times it is encountered
#define LIMITED_LOG(times, ...) \
{ \
  static uint16_t output_count = 0; \
  if (output_count < times) { \
    LOG(__VA_ARGS__); \
    ++output_count; \
  } \
}

// output a message the first time it is encountered
#define LOG_ONCE(...) LIMITED_LOG(1, ##__VA_ARGS__)

// output the position once
#define LOG_POSITION_ONCE \
    LOG_ONCE("\n", GSL_GEN_GetTimestampSinceReset()); \
    LOG_ONCE(": In file ", GSL_BaseFilename(__FILE__), " at line ", __LINE__, " in function ", __func__);

// log the file, function and line number
#define LOG_LOCATION \
  LOG("\n\nWithin file: ", GSL_BaseFilename(__FILE__)); \
  LOG(  "\n   Function: ", __func__); \
  LOG(  "\n       Line: ", __LINE__);

// stop execution with the following message
#define HALT(...) LOG("\n\n"); LOG(__VA_ARGS__); \
  LOG_LOCATION; \
  LOG("\n\nExecution has been halted."); \
  BLINK_ERROR

// log the duration it took to do something
#define LOG_DURATION(...) { \
  uint32_t start_tick = GSL_DEL_Ticks(); \
  {\
    __VA_ARGS__; \
  } \
  LOG("\nIt took ", GSL_DEL_ElapsedUS(start_tick), " us."); \
}

// assert two values are equal with some information if the assert fails
#define ASSERT_EQ(one, two) { \
    auto one_value = one; \
    auto two_value = two; \
    if (!(one_value == two_value)) { \
      LOG("\n\nASSERT_EQ FAILED:"); \
      LOG("\n\nStatement: ", #one, " (", one_value, ") == "); \
      LOG(#two, " (", two_value, ")"); \
      LOG_LOCATION; \
      LOG("\n\nExecution has been halted."); \
      BLINK_ERROR; \
    } \
  }

// assert two values are not equal with some information if the assert fails
#define ASSERT_NE(one, two) { \
    auto one_value = one; \
    auto two_value = two; \
    if (!(one_value != two_value)) { \
      LOG("\n\nASSERT_NE FAILED:"); \
      LOG("\n\nStatement: ", #one, " (", one_value, ") != "); \
      LOG(#two, " (", two_value, ")"); \
      LOG_LOCATION; \
      LOG("\n\nExecution has been halted."); \
      BLINK_ERROR; \
    } \
  }

// assert two values are equal with some information if the assert fails
#define ASSERT_GT(one, two) { \
    auto one_value = one; \
    auto two_value = two; \
    if (!(one_value > two_value)) { \
      LOG("\n\nASSERT_GT FAILED:"); \
      LOG("\n\nStatement: ", #one, " (", one_value, ") > "); \
      LOG(#two, " (", two_value, ")"); \
      LOG_LOCATION; \
      LOG("\n\nExecution has been halted."); \
      BLINK_ERROR; \
    } \
  }

// assert two values are equal with some information if the assert fails
#define ASSERT_GE(one, two) { \
    auto one_value = one; \
    auto two_value = two; \
    if (!(one_value >= two_value)) { \
      LOG("\n\nASSERT_GT FAILED:"); \
      LOG("\n\nStatement: ", #one, " (", one_value, ") >= "); \
      LOG(#two, " (", two_value, ")"); \
      LOG_LOCATION; \
      LOG("\n\nExecution has been halted."); \
      BLINK_ERROR; \
    } \
  }

// assert two values are equal with some information if the assert fails
#define ASSERT_LT(one, two) { \
    auto one_value = one; \
    auto two_value = two; \
    if (!(one_value < two_value)) { \
      LOG("\n\nASSERT_LT FAILED:"); \
      LOG("\n\nStatement: ", #one, " (", one_value, ") < "); \
      LOG(#two, " (", two_value, ")"); \
      LOG_LOCATION; \
      LOG("\n\nExecution has been halted."); \
      BLINK_ERROR; \
    } \
  }

// assert two values are equal with some information if the assert fails
#define ASSERT_LE(one, two) { \
    auto one_value = one; \
    auto two_value = two; \
    if (!(one_value <= two_value)) { \
      LOG("\n\nASSERT_LT FAILED:"); \
      LOG("\n\nStatement: ", #one, " (", one_value, ") <= "); \
      LOG(#two, " (", two_value, ")"); \
      LOG_LOCATION; \
      LOG("\n\nExecution has been halted."); \
      BLINK_ERROR; \
    } \
  }

// assert a statement is true, else halt
#define ASSERT(...) if (!(__VA_ARGS__)) { \
    LOG("\n\nASSERT FAILED:"); \
    LOG("\n\nStatement: ", #__VA_ARGS__); \
    LOG_LOCATION; \
    LOG("\n\nExecution has been halted."); \
    BLINK_ERROR; \
  };

// run a function that return a HAL_StatusTypeDef and error out if it doesn't
// return HAL_OK
#define HAL_RUN(...) { \
    HAL_StatusTypeDef result = __VA_ARGS__; \
    if (result != HAL_OK) { \
      LOG("\n\nHAL routine failed:"); \
      LOG("\n\nStatement: ", #__VA_ARGS__); \
      LOG("\n\nReturn code was "); \
      if (result == HAL_ERROR) {LOG("HAL_ERROR");} \
      else if (result == HAL_BUSY) {LOG("HAL_BUSY");} \
      else if (result == HAL_TIMEOUT) {LOG("HAL_TIMEOUT");} \
      else {LOG(result);} \
      LOG(" (", result, ")."); \
      LOG("\nWe were expecting HAL_OK (0)."); \
      LOG_LOCATION; \
      LOG("\n\nExecution has been halted."); \
      BLINK_ERROR;} \
    };

// these must be defined elsewhere
void LOG(const char * message);
//void LOG(uint32_t value);

const char * GSL_OUT_Integer(uint32_t value);

// log a uint64_t
void LOGUINT64(uint64_t value) {
  uint64_t place = 1;
  // find largest place
  while (place < value && place < 1e19) {
    place *= 10;
  }
  if (place > value && place > 1) {
    place /= 10;
  }
  // output each character
  char out[2] = {0, 0};
  while (place) {
    out[0] = '0' + (value / place) % 10;
    LOG(out);
    place /= 10;
  }
}

// log an unsigned integer
void LOG(const uint64_t & in_value) {
  uint64_t value = in_value;
  // if the highest bit in value is set, we're going to assume the integer
  // is signed and output that instead
  if (value & 0x8000000000000000UL) {
    value ^= 0xFFFFFFFFFFFFFFFFUL;
    LOG("-");
  }
  // if it's small enough, call the quicker function
  if (value <= UINT_MAX) {
    LOG(GSL_OUT_Integer(value));
  } else {
    LOGUINT64(value);
  }
}

// log a uint8_t
void LOG(const short int & value) {
  LOG(GSL_OUT_Integer(value));
}

// log a uint8_t
void LOG(const unsigned short int & value) {
  LOG(GSL_OUT_Integer(value));
}

// log an int
void LOG(const int & value) {
  LOG(GSL_OUT_Integer(value));
}

// log an int
void LOG(const unsigned int & value) {
  LOG(GSL_OUT_Integer(value));
}

// log an int
void LOG(const long int & value) {
  LOG(GSL_OUT_Integer(value));
}

// log an int
void LOG(const unsigned long int & value) {
  LOG(GSL_OUT_Integer(value));
}

// log a pointer
/*void LOG(void * ptr) {
  LOG((uint32_t) ptr);
}*/

const char * GSL_OUT_FixedFloat(float value, uint8_t places);

// log a float
void LOG(float value) {
  if (value < 0.0f) {
    LOG("-");
    value *= -1.0f;
  }
  if (value == 0.0f) {
    LOG("0");
  } else if (value < 0.0001f) {
    LOG(GSL_OUT_FixedFloat(value, 6));
  } else if (value < 0.001f) {
    LOG(GSL_OUT_FixedFloat(value, 5));
  } else if (value < 0.01f) {
    LOG(GSL_OUT_FixedFloat(value, 4));
  } else if (value < 0.1f) {
    LOG(GSL_OUT_FixedFloat(value, 3));
  } else if (value < 1.0f) {
    LOG(GSL_OUT_FixedFloat(value, 2));
  } else if (value < 10.0f) {
    LOG(GSL_OUT_FixedFloat(value, 1));
  } else {
    LOG(GSL_OUT_Integer(value + 0.5f));
  }
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

// log seven things
template <class T, class T2, class T3, class T4, class T5, class T6, class T7,
          class T8>
void LOG(T one, T2 two, T3 three, T4 four, T5 five, T6 six, T7 seven,
         T8 eight) {
  LOG(one);
  LOG(two);
  LOG(three);
  LOG(four);
  LOG(five);
  LOG(six);
  LOG(seven);
  LOG(eight);
}

// log seven things
template <class T, class T2, class T3, class T4, class T5, class T6, class T7,
          class T8, class T9>
void LOG(T one, T2 two, T3 three, T4 four, T5 five, T6 six, T7 seven,
         T8 eight, T9 nine) {
  LOG(one);
  LOG(two);
  LOG(three);
  LOG(four);
  LOG(five);
  LOG(six);
  LOG(seven);
  LOG(eight);
  LOG(nine);
}

// report an error if there is one (else do nothing)
/*void GSL_ReportError(HAL_StatusTypeDef status) {
  if (status != HAL_OK) {
    LOG("\nERROR: Received HAL error code ", (uint32_t) status);
    HALT("ERROR");
  }
}*/

extern "C" {

// this is used within the STM32 HAL files
// Called from the assert_param() macro, usually defined in the stm32f*_conf.h
void __attribute__((noreturn)) assert_failed (uint8_t* file, uint32_t line) {
  LOG("\n\nASSERT FAILED:");
  LOG("\n\nFile: ");
  LOG((const char *) file);
  LOG(":");
  LOG(line);
  LOG("\n\nExecution has been halted.");
  BLINK_ERROR;
}

}

#define GSL_TIMEIT(...) { \
  LOG("\n\nPerforming a timing test:"); \
  LOG("\n- Statement: "#__VA_ARGS__); \
  GSL_DELOG_WaitUntilEmpty(); \
  uint32_t ticks_timeit = 0; \
  ticks_timeit -= GSL_DEL_Ticks(); \
  __VA_ARGS__; \
  ticks_timeit += GSL_DEL_Ticks(); \
  LOG("\n- It took ", ticks_timeit, " ticks"); \
  float time_ns = (float) ticks_timeit / HAL_RCC_GetSysClockFreq() * 1e9; \
  if (time_ns < 10000.0f) { \
    LOG(" (", (uint32_t) (time_ns + 0.5f), " ns)"); \
  } else if (time_ns < 10000000.0f) { \
    LOG(" (", (uint32_t) (time_ns / 1e3 + 0.5f), " us)"); \
  } else { \
    LOG(" (", (uint32_t) (time_ns / 1e6 + 0.5f), " ms)"); \
  } \
}
