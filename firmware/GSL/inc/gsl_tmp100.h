#pragma once

// This file includes an interface for the TMP100 chips.

#include "gsl_includes.h"

struct GSL_TMP100 {

  // i2c peripheral
  I2C_TypeDef * I2Cx;

  // 7-bit slave address for this particular chip
  uint8_t slave_address;

  // register addresses
  enum RegisterEnum : uint8_t {
    kRegisterTemperature = 0x00,
    kRegisterConfiguration = 0x01,
    kRegisterLowTemperature = 0x02,
    kRegisterHighTemperature = 0x03,
  };

  // initializer
  GSL_TMP100(
      I2C_TypeDef * I2Cx,
      uint8_t slave_address)
      : I2Cx(I2Cx),
      slave_address(slave_address) {
    // ensure valid slave address
    if ((slave_address & 0b11111000) != 0b01001000) {
      HALT("Invalid slave address");
    }
  }

  // write to a register
  void WriteRegister(RegisterEnum memory_address, uint16_t value) {
    uint8_t data[2] = {(uint8_t) (value / 256), (uint8_t) (value % 256)};
    // the config register is 1-byte, the others are 2-byte
    if (memory_address == kRegisterConfiguration) {
      GSL_I2C_WriteAddress(I2Cx, slave_address, memory_address, value % 256);
    } else {
      GSL_I2C_WriteAddressMulti(I2Cx, slave_address, memory_address, data, 2);
    }
  }

  // read from a register
  uint16_t ReadRegister(RegisterEnum memory_address) {
    uint8_t data[2];
    // the config register is 1-byte, the others are 2-byte
    if (memory_address == kRegisterConfiguration) {
      return GSL_I2C_ReadAddress(I2Cx, slave_address, memory_address);
    } else {
      GSL_I2C_ReadAddressMulti(I2Cx, slave_address, memory_address, data, 2);
      return data[1] + (uint16_t) 256 * data[0];
    }
  }

  // return true if present and responding accordingly
  bool IsPresent(void) {
    // see if slave responds
    if (!GSL_I2C_IsSlavePresent(I2Cx, slave_address)) {
      return false;
    }
    return true;
  }


};
