#pragma once

// This file contains routines for converting objects into string format for
// output.

#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "gsl_includes.h"

// format an unsigned integer into hex format with the given number of bytes
// e.g. GSL_OUT_Hex(16, 1) --> "0x10"
// e.g. GSL_OUT_Hex(16, 2) --> "0x0010"
const char * GSL_OUT_Hex(uint32_t value, uint8_t length_in_bytes) {
  ASSERT_LE(length_in_bytes, 4);
	// number of buffers
	// the last X calls to this function are valid
	const uint8_t kBufferCount = 3;
	// the size of each buffer
	const uint32_t kBufferSize = 11;
	// the actual buffers
	static char buffer_array[kBufferCount * kBufferSize];

	// the current buffer
	static uint8_t next_buffer = 0;
	// pointer to the buffer to use for this call
	char * buffer = &buffer_array[next_buffer * kBufferSize];
	++next_buffer;
	next_buffer %= kBufferCount;

	// ensure length is appropriate
	//if (kBufferSize < 3 + 2 * length) {
	  // bad things
		//ERROR("buffer not long enough");
	//}

	// position within the current buffer
	int32_t pos = kBufferSize - 1;
	// null terminate
	buffer[pos] = '\0';

	// go through each character
	while (length_in_bytes-- != 0) {
		for (uint8_t i = 0; i < 2; ++i) {
			uint8_t x = value % 16;
			value /= 16;
			if (x < 10) {
				buffer[--pos] = '0' + x;
			} else {
				buffer[--pos] = 'A' - 10 + x;
			}
		}
	}

	// add the leading "0x" to indicate a hex number
	buffer[--pos] = 'x';
	buffer[--pos] = '0';

	ASSERT_GE(pos, 0);

	// return the expression
	return &buffer[pos];
}

// format an unsigned integer into hex format with the given number of bits
// e.g. GSL_OUT_Binary(1, 1) --> "0b1"
// e.g. GSL_OUT_Binary(127, 8) --> "0b01111111"
const char * GSL_OUT_Binary(uint32_t value, uint8_t length_in_bits) {

  // number of buffers
  // the last X calls to this function are valid
  const uint8_t kBufferCount = 3;
  // the size of each buffer
  const uint32_t kBufferSize = 35 * 3;
  // the actual buffers
  static char buffer_array[kBufferCount][kBufferSize];

  // the current buffer
  static uint8_t next_buffer = 0;
  // pointer to the buffer to use for this call
  char * buffer = buffer_array[next_buffer];
  next_buffer = (next_buffer + 1) % kBufferCount;

  // ensure length is appropriate
  //if (kBufferSize < 3 + 2 * length) {
    // bad things
    //ERROR("buffer not long enough");
  //}

  // position within the current buffer
  int32_t pos = kBufferSize - 1;
  // null terminate
  buffer[pos] = '\0';

  // go through each bit
  while (length_in_bits-- != 0) {
    if (value & 1) {
      buffer[--pos] = '1';
    } else {
      buffer[--pos] = '0';
    }
    value >>= 1;
  }

  // add the leading "0b" to indicate a binary number
  buffer[--pos] = 'b';
  buffer[--pos] = '0';

  // return the expression
  return &buffer[pos];
}

const char * GSL_OUT_Hex(uint8_t value) {
	return GSL_OUT_Hex(value, 1);
}
const char * GSL_OUT_Hex(uint16_t value) {
	return GSL_OUT_Hex(value, 2);
}
const char * GSL_OUT_Hex(uint32_t value) {
	return GSL_OUT_Hex(value, 4);
}

// output a string of memory as binary, starting from MSB
void GSL_OUT_LogBinaryBytes(const uint8_t * ptr, uint16_t length_in_bytes) {
  if (length_in_bytes == 0) {
    return;
  }
  LOG("0b");
  while (length_in_bytes) {
    LOG(&GSL_OUT_Binary(*ptr, 8)[2]);
    ++ptr;
    --length_in_bytes;
  }
}

// output a string of memory as hex
void GSL_OUT_LogHex(const uint8_t * ptr, uint16_t length) {
  if (length == 0) {
    return;
  }
  LOG("0x");
  while (length) {
    LOG(&GSL_OUT_Hex(*ptr)[2]);
    ++ptr;
    --length;
  }
}

// output a float with the given number of decimal places
// e.g. GSL_OUT_FixedFloat(1.0f, 2) --> "0.00"
const char * GSL_OUT_FixedFloat(float value, uint8_t places) {

	// number of buffers
	// the last X calls to this function are valid
	const uint8_t kBufferCount = 3;
	// the size of each buffer
	const uint32_t kBufferSize = 32;
	// the actual buffers
	static char buffer_array[kBufferCount][kBufferSize];

	// the current buffer
	static uint8_t next_buffer = 0;
	// pointer to the buffer to use for this call
	char * buffer = buffer_array[next_buffer];
	next_buffer = (next_buffer + 1) % kBufferCount;

	// position within the current buffer
	int32_t pos = kBufferSize - 1;

	// detect NaN value
	if (isnan(value)) {
		strcpy(buffer, "NaN");
		return buffer;
	}

	// detect +Inf and -Inf values
	if (isinf(value)) {
		if (value < 0) {
			strcpy(buffer, "-Inf");
		} else {
			strcpy(buffer, "+Inf");
		}
		return buffer;
	}

	// if negative, put a negative sign in front
	bool negative = value < 0;
	if (negative) {
		value *= -1.0f;
	}

	// round to the given number of decimal places and turn into an integer
	for (uint8_t places2 = places; places2 != 0; --places2) {
	  value *= 10.0f;
	}
	uint32_t int_value = value + 0.5f;

	// we write the digits from right to left
	bool all_zero = true;

	// null terminate
	buffer[pos] = '\0';
	--pos;
	if (places > 0) {
		for (uint32_t i = 0; i < places; ++i) {
			buffer[pos] = '0' + (char)(int_value % 10);
			if (int_value % 10) {
			  all_zero = false;
			}
			int_value /= 10;
			--pos;
		}
		buffer[pos] = '.';
		--pos;
	}
	if (int_value == 0) {
		buffer[pos] = '0';
		--pos;
	}
	while (int_value > 0) {
		buffer[pos] = '0' + (int_value % 10);
    if (int_value % 10) {
      all_zero = false;
    }
		int_value /= 10;
		--pos;
	}

	// add negative sign
	if (negative && !all_zero) {
		buffer[pos] = '-';
		--pos;
	}

	// return pointer to null-terminated string
	return &buffer[pos + 1];

}

// output a number into the given number of characters
const char * GSL_OUT_FormattedFloat(float value,
                                    uint16_t width,
                                    uint16_t max_digits_after_decimal = 1000) {

  // number of buffers
  // the last X calls to this function are valid
  const uint8_t kBufferCount = 2;
  // the size of each buffer
  const uint32_t kBufferSize = 12;
  // the actual buffers
  static char buffer_array[kBufferCount][kBufferSize];

  // the current buffer
  static uint8_t next_buffer = 0;
  // pointer to the buffer to use for this call
  char * buffer = buffer_array[next_buffer];
  next_buffer = (next_buffer + 1) % kBufferCount;

  // ensure we can fit this
  ASSERT_LT(width, kBufferSize);

  // initialize string
  buffer[0] = 0;

  // get integer digits of the number
  ASSERT_GT(width, 0U);
  if (width < 2) {
    max_digits_after_decimal = 0;
  } else if (max_digits_after_decimal + 2 > width) {
    max_digits_after_decimal = width - 2;
  }

  // since we're using an int32_t to hold digits, we can only have so many
  // digits without overflowing
  ASSERT_LE(max_digits_after_decimal, 9);

  // convert to floating point and check width
  uint16_t places = max_digits_after_decimal;
  const char * text = nullptr;
  while (true) {
    text = GSL_OUT_FixedFloat(value, places);
    if (places == 0 || strlen(text) <= width) {
      break;
    }
    --places;
  }

  // pad buffer with spaces
  while (strlen(text) + strlen(buffer) < width) {
    strcpy(&buffer[strlen(buffer)], " ");
  }

  // copy string
  strcpy(&buffer[strlen(buffer)], text);

  return buffer;
  /*

  // test for negative value
  bool negative = value < 0.0f;
  if (negative) {
    value = -value;
  }

  // get digits
  uint16_t digits =
      floorf(value * powf(10.0f, max_digits_after_decimal) + 0.5f);

  // get digits in the integer portion
  uint16_t integer_digits = digits;
  for (uint16_t i = 0; i < max_digits_after_decimal; ++i) {
    integer_digits /= 10;
  }

  // ensure acceptable precision for floating point
  int32_t digits =

  LOG("\ndigits=", digits);

  // handle 0 differently
  if (digits == 0) {
    if (width == 2) {
      strcpy(&buffer[strlen(buffer)], " 0");
    } else {
      strcpy(&buffer[strlen(buffer)], "0");
      if (width >= 3) {
        strcpy(&buffer[strlen(buffer)], ".");
        for (uint16_t i = 2; i < width; ++i) {
          strcpy(&buffer[strlen(buffer)], "0");
        }
      }
    }
    return buffer;
  }

  // check negative values
  if (digits < 0) {
    strcpy(&buffer[strlen(buffer)], "-");
    const char * text = GSL_OUT_FormattedFloat(-value,
                                               width - 1,
                                               max_digits_after_decimal);
    strcpy(&buffer[strlen(buffer)], text);
    return buffer;
  }

  // get width of integer portion
  uint16_t integer_digits = digits;
  uint32_t reversed_fraction_digits = 0;
  for (uint16_t i = 0; i < max_digits_after_decimal; ++i) {
    reversed_fraction_digits *= 10;
    reversed_fraction_digits += integer_digits % 10;
    integer_digits /= 10;
  }

  const char * integers = GSL_OUT_Integer(integer_digits);
  int32_t width_left = width - strlen(integers);

  if (width_left <= 1) {
    if (width_left == 1) {
      strcpy(&buffer[strlen(buffer)], " ");
    }
    strcpy(&buffer[strlen(buffer)], integers);
    return buffer;
  }

  strcpy(&buffer[strlen(buffer)], integers);
  strcpy(&buffer[strlen(buffer)], ".");
  --width_left;
  for (uint16_t i = 0; i < width_left; ++i) {
    strcpy(&buffer[strlen(buffer)], " ");
    buffer[strlen(buffer) - 1] = '0' + (reversed_fraction_digits % 10);
    reversed_fraction_digits /= 10;
  }
  return buffer;
  */

}

// convert a signed number into a string
const char * GSL_OUT_SignedInteger(int32_t value) {

  // number of buffers
  // the last X calls to this function are valid
  const uint8_t kBufferCount = 3;
  // the size of each buffer
  const uint32_t kBufferSize = 12;
  // the actual buffers
  static char buffer_array[kBufferCount][kBufferSize];

  // the current buffer
  static uint8_t next_buffer = 0;

  // pointer to the buffer to use for this call
  char * buffer = buffer_array[next_buffer];
  next_buffer = (next_buffer + 1) % kBufferCount;

  if (value < 0) {
    buffer[0] = '-';
    strcpy(&buffer[1], GSL_OUT_Integer(-value));
    return buffer;
  } else {
    return GSL_OUT_Integer(value);
  }

}

// convert an unsigned number into a string
const char * GSL_OUT_Integer(uint32_t value) {

  // number of buffers
  // the last X calls to this function are valid
  const uint8_t kBufferCount = 3;
  // the size of each buffer
  const uint32_t kBufferSize = 12;
  // the actual buffers
  static char buffer_array[kBufferCount][kBufferSize];

  // the current buffer
  static uint8_t next_buffer = 0;
  // pointer to the buffer to use for this call
  char * buffer = buffer_array[next_buffer];
  next_buffer = (next_buffer + 1) % kBufferCount;

  // position within the current buffer
  int32_t pos = kBufferSize - 1;

  buffer[pos] = 0;
  --pos;

  // special case for zero
  if (value == 0) {
    buffer[pos] = '0';
    return &buffer[pos];
  }

  // fill in the digits
  while (value > 0) {
    buffer[pos] = '0' + (value % 10);
    value /= 10;
    --pos;
  }

  // return the completed string
  ++pos;
  return &buffer[pos];

}

// convert the number of bytes into a human readable form
// 1kB = 1024 bytes
// rollover occurs at 1000 bytes (e.g. 999 bytes, 0.976 kB)
const char * GSL_OUT_StorageBytes(uint32_t bytes) {

  // prefixes
  const char prefix[] = " kMGTPEZY";

  // number of buffers
  // the last X calls to this function are valid
  const uint8_t kBufferCount = 3;
  // the size of each buffer
  const uint32_t kBufferSize = 16;
  // the actual buffers
  static char buffer_array[kBufferCount][kBufferSize];

  // the current buffer
  static uint8_t next_buffer = 0;
  // pointer to the buffer to use for this call
  char * buffer = buffer_array[next_buffer];

  // find the appropriate prefix
  float bytes_float = bytes;
  uint32_t prefix_index = 0;
  while (bytes_float >= 1000.0f) {
    ++prefix_index;
    bytes_float /= 1024.0f;
  }

  // format it into a value (three significant figures)
  // 100
  // 99.9
  // 9.87
  if (bytes_float == 0.0f) {
    strcpy(buffer, "0");
  } else if (bytes_float >= 100) {
    strcpy(buffer, GSL_OUT_Integer(bytes_float));
  } else if (bytes_float >= 10) {
    strcpy(buffer, GSL_OUT_FixedFloat(bytes_float, 1));
  } else {
    strcpy(buffer, GSL_OUT_FixedFloat(bytes_float, 2));
  }

  /*float bytes_float = bytes;
  uint32_t prefix_index = 0;
  uint32_t last_bytes = bytes * 1024;
  while (bytes > 999) {
    ++prefix_index;
    last_bytes = bytes * 1000 / 1024;
    bytes /= 1024;
  }

  // format it into a value (three significant figures)
  // 100
  // 99.9
  // 9.87
  if (bytes == 0) {
    strcpy_stm32(buffer, "0");
  } else if (bytes >= 100) {
    strcpy_stm32(buffer, OutputFormInteger(bytes));
  } else if (bytes >= 10) {
    strcpy_stm32(buffer, OutputFormFixedFloat(last_bytes / 1000.0f, 1));
  } else {
    strcpy_stm32(buffer, OutputFormFixedFloat(last_bytes / 1000.0f, 2));
  }*/

  // add unit prefix
  uint32_t position = strlen(buffer);
  buffer[position++] = ' ';
  if (prefix_index > 0) {
    buffer[position++] = prefix[prefix_index];
  }
  buffer[position++] = 'B';
  buffer[position++] = '\0';

  // return pointer to null-terminated string
  return buffer;

}

// output text and pad it with spaces to fit the desired width
void GSL_OUT_LogPaddedText(
    const char * text,
    uint16_t width,
    bool pad_on_left) {
  uint16_t spaces = (strlen(text) < width) ? width - strlen(text) : 0;
  if (pad_on_left) {
    for (uint16_t i = 0; i < spaces; ++i) {
      LOG(" ");
    }
  }
  LOG(text);
  if (!pad_on_left) {
    for (uint16_t i = 0; i < spaces; ++i) {
      LOG(" ");
    }
  }
}
