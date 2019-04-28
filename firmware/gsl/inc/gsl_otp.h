// This file includes functions for accessing the One-Time Programmable (OTP)
// bytes.
//
// There are 16 blocks of 32 bytes.
//
// At startup, all data is 0xFF.
//
// Each block may be written to exactly once.
//
// A block may be locked to prevent further writes.
//
// As the name implies, once written, this data cannot be erased.
// Once a block is locked, the data in that block cannot be written.

#pragma once

#include "gsl_includes.h"

// number of OTP blocks
const uint8_t gsl_otp_block_count = 16;

// number of bytes per block
const uint8_t gsl_otp_bytes_per_block = 32;

// alias to the OTP memory
const uint8_t (*gsl_otp_data)[gsl_otp_bytes_per_block] =
    (uint8_t (*)[gsl_otp_bytes_per_block]) 0x1FFF7800;

// alias to the OTP lock block
const uint8_t * gsl_otp_lock_block = (uint8_t *) 0x1FFF7A00;

// return true if the given block is locked
bool GSL_OTP_IsBlockLocked(uint8_t block_number) {
  ASSERT_LT(block_number, gsl_otp_block_count);
  return gsl_otp_lock_block[block_number] == 0x00;
}

// read a byte from the given block
uint8_t GSL_OTP_Read(uint8_t block_number, uint8_t byte_number) {
  ASSERT_LT(block_number, gsl_otp_block_count);
  ASSERT_LT(byte_number, gsl_otp_bytes_per_block);
  return gsl_otp_data[block_number][byte_number];
}

// read many bytes from the given block and store it
void GSL_OTP_ReadMulti(uint8_t block_number,
                       uint8_t byte_offset,
                       void * destination,
                       uint16_t length) {
  ASSERT_LT(block_number, gsl_otp_block_count);
  ASSERT_LT(byte_offset, gsl_otp_bytes_per_block);
  ASSERT_LE(byte_offset + length, gsl_otp_bytes_per_block);
  for (uint16_t i = 0; i < length; ++i) {
    ((uint8_t *) destination)[i] = gsl_otp_data[block_number][byte_offset + i];
  }
}

// lock the given block
// THIS IS AN IRREVERSIBLE OPERATION
void GSL_OTP_LockBlock(uint8_t block_number) {
  ASSERT_LT(block_number, gsl_otp_block_count);
  ASSERT(!GSL_OTP_IsBlockLocked(block_number));
  HAL_RUN(HAL_FLASH_Unlock());
  HAL_RUN(HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE,
                            (uint32_t)(&gsl_otp_lock_block[block_number]),
                            0x00));
  HAL_RUN(HAL_FLASH_Lock());
}

// write data to the given block
// THIS IS AN IRREVERSIBLE OPERATION
void GSL_OTP_Write(uint8_t block_number,
                   uint8_t byte_number,
                   uint8_t value) {
  ASSERT_LT(block_number, gsl_otp_block_count);
  ASSERT_LT(byte_number, gsl_otp_bytes_per_block);
  ASSERT(!GSL_OTP_IsBlockLocked(block_number));
  HAL_RUN(HAL_FLASH_Unlock());
  uint8_t * ptr = (uint8_t *) &gsl_otp_data[block_number][byte_number];
  HAL_RUN(HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE,
                            (uint32_t) ptr,
                            value));
  HAL_RUN(HAL_FLASH_Lock());
}

// write data to the given block
// THIS IS AN IRREVERSIBLE OPERATION
void GSL_OTP_WriteMulti(uint8_t block_number,
                        uint8_t byte_offset,
                        const void * data,
                        uint16_t length) {
  ASSERT_LT(block_number, gsl_otp_block_count);
  ASSERT_LT(byte_offset, gsl_otp_bytes_per_block);
  ASSERT_LE(byte_offset + length, gsl_otp_bytes_per_block);
  ASSERT(!GSL_OTP_IsBlockLocked(block_number));
  HAL_RUN(HAL_FLASH_Unlock());
  for (uint16_t i = 0; i < length; ++i) {
    uint8_t * ptr = (uint8_t *) &gsl_otp_data[block_number][byte_offset + i];
    HAL_RUN(HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE,
                              (uint32_t) ptr,
                              ((uint8_t *) data)[i]));
  }
  HAL_RUN(HAL_FLASH_Lock());
}
