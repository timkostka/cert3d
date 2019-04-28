#pragma once

// includes various CRC calculation routines

#include "gsl_includes.h"

// lookup table for 8-bit CCITT CRC
uint8_t gsl_crc8_ccitt_lookup[256];

// calculates the crc with the given data and returns the result
// x^8 + x^2 + x + 1 (0x07)
uint8_t GSL_CRC_CalculateCRC8CCITT_Bitwise(const uint8_t * data, uint32_t length) {
  // reset the value
  uint8_t crc = 0;
  while (length) {
    // update the crc with this value
    crc ^= *data;
    for(uint8_t i = 0; i < 8; ++i) {
      if (crc & 0x80) {
          crc <<= 1;
          crc ^= 0x07;
      } else {
          crc <<= 1;
      }
    }
    // increment pointer
    ++data;
    --length;
  }
  return crc;
}

// calculates the crc with the given data and returns the result
uint8_t GSL_CRC_CalculateCRC8CCITT(const uint8_t * data, uint32_t length) {
  // reset the value
  uint8_t crc = 0;
  while (length) {
    // update the crc with this value
    crc ^= *data;
    crc = gsl_crc8_ccitt_lookup[crc];
    // increment pointer
    ++data;
    --length;
  }
  return crc;
}

// lookup table for 16-bit CCITT CRC
const uint16_t gsl_crc16_ccitt_lookup[256] = {
    0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
    0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
    0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
    0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
    0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
    0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
    0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
    0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
    0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
    0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
    0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
    0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
    0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
    0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
    0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
    0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
    0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
    0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
    0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
    0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
    0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
    0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
    0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
    0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
    0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
    0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
    0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
    0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
    0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
    0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
    0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
    0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0};

// calculates the crc with the given data and return the result
uint16_t GSL_CRC_CalculateCRC16CCITT(
    const uint8_t * data,
    uint32_t length,
    uint16_t initial_value = 0x0000) {
  // reset the value
  uint16_t crc = initial_value;
  while (length) {
    uint8_t value = (*data) ^ (crc >> 8);
    crc = gsl_crc16_ccitt_lookup[value] ^ (crc << 8);
    // increment pointer
    ++data;
    --length;
  }
  return crc;
}

// calculates the crc with the given data and return the result
// TODO: check this with known values
// algorithm taken from
// https://www.ccsinfo.com/forum/viewtopic.php?t=24977
uint16_t GSL_CRC_CalculateCRC16CCITT_OLD(
    const uint8_t * data,
    uint32_t length) {
  // reset the value
  uint16_t crc = 0;
  uint16_t x = 0;
  while (length) {
    // black magic
    x = (crc >> 8) ^ *data;
    x ^= x >> 4;
    crc <<= 8;
    crc ^= x;
    crc ^= x << 5;
    crc ^= x << 12;
    // increment pointer
    ++data;
    --length;
  }
  return crc;
}

// initialize the lookup table
void GSL_CRC_Initialize(void) {
  for (uint16_t i = 0; i < 256; ++i) {
    uint8_t data = i;
    gsl_crc8_ccitt_lookup[i] = GSL_CRC_CalculateCRC8CCITT_Bitwise(&data, 1);
  }
}

// dummy object to initialize lookup table on global object creation
// takes 300us on a 100MHz processor
GSL_INITIALIZER mdm_crc_initializer(GSL_CRC_Initialize);

// unit tests and profiles the various routines
void GSL_CRC_Profile(void) {

  const uint32_t kDataLength = 250 * 16;

  GSL_BUF_TempArray<uint8_t> data(kDataLength + 2);
  //uint8_t * data = (uint8_t *) GSL_BUF_CreateTemporary(kDataLength + 2);

  //uint8_t data[kDataLength + 2];
  LOG("\n\n\nRunning CRC profile routines with ",
      SystemCoreClock / 1000000, " MHz clock");
  // initialize the data
  for (uint32_t i = 0; i < kDataLength; ++i) {
    data[i] = i % 256;
  }
  // do the calculation
  {
    uint32_t start_tick = GSL_DEL_Ticks();
    data[kDataLength] = GSL_CRC_CalculateCRC8CCITT_Bitwise(data, kDataLength);
    uint32_t end_tick = GSL_DEL_Ticks();
    // calculate again and make sure the result is zero
    if (GSL_CRC_CalculateCRC8CCITT(data, kDataLength + 1) != 0) {
      HALT("Invalid data");
    }
    // print out profile information
    {
      float clocks_per_byte = 1.0f * (end_tick - start_tick) / kDataLength;
      float mbyte = SystemCoreClock / clocks_per_byte / 1000000;
      LOG("\nGSL_CRC_CalculateCRC8CCITT_Bitwise: ");
      LOG(GSL_OUT_FixedFloat(mbyte, 1), " MBps (");
      LOG(GSL_OUT_FixedFloat(clocks_per_byte, 2), " clocks/byte)");
    }
  }
  // do the calculation
  {
    uint32_t start_tick = GSL_DEL_Ticks();
    data[kDataLength] = GSL_CRC_CalculateCRC8CCITT(data, kDataLength);
    uint32_t end_tick = GSL_DEL_Ticks();
    // calculate again and make sure the result is zero
    if (GSL_CRC_CalculateCRC8CCITT(data, kDataLength + 1) != 0) {
      HALT("Invalid data");
    }
    // print out profile information
    {
      float clocks_per_byte = 1.0f * (end_tick - start_tick) / kDataLength;
      float mbyte = SystemCoreClock / clocks_per_byte / 1000000;
      LOG("\nGSL_CRC_CalculateCRC8CCITT: ");
      LOG(GSL_OUT_FixedFloat(mbyte, 1), " MBps (");
      LOG(GSL_OUT_FixedFloat(clocks_per_byte, 2), " clocks/byte)");
    }
  }
  // test
  {
    uint8_t data2[10];
    for (uint8_t i = 0; i < 10; ++i) {
      data2[i] = 0x30 + i;
    }
    uint16_t crc = GSL_CRC_CalculateCRC16CCITT(data2, 10, 0xFFFF);
    LOG("\nCRC = ", GSL_OUT_Hex(crc));
    if (crc != 0x7D61) {
      HALT("Invalid data");
    }
    uint16_t crc2 = GSL_CRC_CalculateCRC16CCITT(data2, 10);
    LOG("\nCRC2 = ", GSL_OUT_Hex(crc2));
  }
  // do the calculation
  {
    uint32_t start_tick = GSL_DEL_Ticks();
    uint16_t crc = GSL_CRC_CalculateCRC16CCITT(data, kDataLength);
    uint32_t end_tick = GSL_DEL_Ticks();
    data[kDataLength] = crc >> 8;
    data[kDataLength + 1] = crc;
    // calculate again and make sure the result is zero
    if (GSL_CRC_CalculateCRC16CCITT(data, kDataLength + 2) != 0) {
      HALT("Invalid data");
    }
    // print out profile information
    {
      float clocks_per_byte = 1.0f * (end_tick - start_tick) / kDataLength;
      float mbyte = SystemCoreClock / clocks_per_byte / 1000000;
      LOG("\nGSL_CRC_CalculateCRC16CCITT: ");
      LOG(GSL_OUT_FixedFloat(mbyte, 1), " MBps (");
      LOG(GSL_OUT_FixedFloat(clocks_per_byte, 2), " clocks/byte)");
    }
  }
}

// calculate the CRC using the built-in formula
// length is the length in 32-bit words
uint32_t GSL_CRC_Calculate(const uint32_t * data, uint32_t length) {
  // enable the clock
  __HAL_RCC_CRC_CLK_ENABLE();
  // reset the value
  CRC->CR |= CRC_CR_RESET;
  const uint32_t * data_end = data + length;
  // calculate each word
  while (data != data_end) {
    CRC->DR = *data;
    ++data;
  }
  uint32_t crc = CRC->DR;
  // disable the clock
  __HAL_RCC_CRC_CLK_DISABLE();
  // return the value
  return crc;
}
