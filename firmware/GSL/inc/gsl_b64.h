#pragma once

// This provides an interface for base 64 encoding.

#include "gsl_includes.h"

// characters to use
const char gsl_b64_chars[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";

// number of characters in encoding
const uint16_t gsl_b64_char_count = 64;

// padding character
const char gsl_b64_padding_char = '=';

// starting character
const char gsl_b64_starting_char = '<';

// ending character
const char gsl_b64_ending_char = '>';

// encode (padded with zeroes), padded at the start and end with the padding
// character, also padded with a null character at the end
void GSL_B64_Encode(
    const uint8_t * data,
    uint16_t length,
    uint8_t * output,
    uint16_t output_capacity,
    bool extra_chars = false) {
  // ensure it fits
  ASSERT_LE((length + 2) / 3 * 4 + 1, output_capacity);
  // add starting character if not null
  if (extra_chars && gsl_b64_starting_char) {
    *output = gsl_b64_starting_char;
    ++output;
  }
  // encode each triplet
  while (length) {
    uint32_t value = 0;
    uint16_t pre_length = length;
    for (uint8_t i = 0; i < 3; ++i) {
      value <<= 8;
      if (length) {
        value += *data;
        ++data;
        --length;
      }
    }
    *output = gsl_b64_chars[(value >> 18) & 63];
    ++output;
    *output = gsl_b64_chars[(value >> 12) & 63];
    ++output;
    if (pre_length > 1) {
      *output = gsl_b64_chars[(value >> 6) & 63];
    } else {
      *output = gsl_b64_padding_char;
    }
    ++output;
    if (pre_length > 2) {
      *output = gsl_b64_chars[value & 63];
    } else {
      *output = gsl_b64_padding_char;
    }
    ++output;
  }
  // add null termination character
  *output = 0;
  ++output;
}

// return the 6-bit value of the given character
uint8_t GSL_B64_Decode(char c) {
  uint8_t value = 0;
  while (value < 63 && gsl_b64_chars[value] != c) {
    ++value;
  }
  ASSERT_EQ(gsl_b64_chars[value], c);
  return value;
}

// decode a null-terminated string and return the number of bytes decoded
uint16_t GSL_B64_Decode(
    const char * text,
    uint8_t * buffer,
    uint16_t buffer_length) {
  uint16_t bytes_decoded = 0;
  // text length should be multiple of 4
  ASSERT_EQ(strlen(text) % 4, 0U);
  // decode each triplet
  while (text[0] != 0) {
    uint32_t value = 0;
    if (text[2] == '=') {
      // one byte
      ASSERT_EQ(strlen(text), 4U);
      ASSERT_EQ(text[3], '=');
      value = GSL_B64_Decode(text[0]);
      value <<= 6;
      value |= GSL_B64_Decode(text[1]);
      ASSERT_EQ(value & 0b1111, 0U);
      value >>= 4;
      buffer[bytes_decoded] = value;
      ++bytes_decoded;
    } else if (text[3] == '=') {
      ASSERT_EQ(strlen(text), 4U);
      // two bytes
      value = GSL_B64_Decode(text[0]);
      value <<= 6;
      value |= GSL_B64_Decode(text[1]);
      value <<= 6;
      value |= GSL_B64_Decode(text[2]);
      ASSERT_EQ(value & 0b11, 0U);
      value >>= 2;
      buffer[bytes_decoded] = value / 256;
      ++bytes_decoded;
      buffer[bytes_decoded] = value % 256;
      ++bytes_decoded;
    } else {
      // three bytes
      value = GSL_B64_Decode(text[0]);
      value <<= 6;
      value |= GSL_B64_Decode(text[1]);
      value <<= 6;
      value |= GSL_B64_Decode(text[2]);
      value <<= 6;
      value |= GSL_B64_Decode(text[3]);
      buffer[bytes_decoded + 2] = value % 256;
      value /= 256;
      buffer[bytes_decoded + 1] = value % 256;
      value /= 256;
      buffer[bytes_decoded + 0] = value % 256;
      value /= 256;
      bytes_decoded += 3;
    }
    text += 4;
  }
  return bytes_decoded;
}

// sanity checks
void GSL_B64_SanityChecks(void) {
  // ensure exact amount of chars in string (plus null terminator)
  ASSERT_EQ(sizeof(gsl_b64_chars), gsl_b64_char_count + 1U);
  ASSERT_EQ(strlen(gsl_b64_chars), gsl_b64_char_count);
  // ensure encoding is correct
  uint8_t buffer[8];
  GSL_B64_Encode((const uint8_t *) "M", 1, (uint8_t *) buffer, 8);
  ASSERT(strcmp((const char *) buffer, "TQ==") == 0);
  GSL_B64_Encode((const uint8_t *) "Ma", 2, (uint8_t *) buffer, 8);
  ASSERT(strcmp((const char *) buffer, "TWE=") == 0);
  GSL_B64_Encode((const uint8_t *) "Man", 3, (uint8_t *) buffer, 8);
  ASSERT(strcmp((const char *) buffer, "TWFu") == 0);
  ASSERT_EQ(GSL_B64_Decode("TQ==", buffer, 8), 1U);
  ASSERT(strncmp((const char *) buffer, "M", 1) == 0);
  ASSERT_EQ(GSL_B64_Decode("TWE=", buffer, 8), 2U);
  ASSERT(strncmp((const char *) buffer, "Ma", 2) == 0);
  ASSERT_EQ(GSL_B64_Decode("TWFu", buffer, 8), 3U);
  ASSERT(strncmp((const char *) buffer, "Man", 3) == 0);
}

// perform sanity checks on object creation
GSL_INITIALIZER GSL_B64_SanityChecker(GSL_B64_SanityChecks);
