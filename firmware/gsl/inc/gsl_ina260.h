#pragma once

#include "gsl_includes.h"

struct GSL_INA260 {

  // i2c peripheral
  I2C_TypeDef * I2Cx;

  // 7-bit slave address
  uint8_t slave_address;

  // register addresses
  enum RegisterEnum : uint8_t {
    kRegisterConfiguration = 0x00,
    kRegisterCurrent = 0x01,
    kRegisterBusVoltage = 0x02,
    kRegisterPower = 0x03,
    kRegisterMaskEnable = 0x06,
    kRegisterAlertLimit = 0x07,
    kRegisterManufacturerID = 0xFE,
    kRegisterDieID = 0xFF,
  };

  // manufacturer ID
  static const uint16_t kValueManufacturerID = 0x5449;

  // die ID
  static const uint16_t kValueDieID = 0x2270;

  // initializer
  GSL_INA260(
      I2C_TypeDef * I2Cx,
      uint8_t slave_address)
      : I2Cx(I2Cx),
      slave_address(slave_address) {
    // ensure valid slave address
    if ((slave_address & 0b11110000) != 0b01000000) {
      HALT("Invalid slave address");
    }
  }

  // write to a register
  void WriteRegister(RegisterEnum memory_address, uint16_t value) {
    uint8_t data[2] = {(uint8_t) (value / 256), (uint8_t) (value % 256)};
    GSL_I2C_WriteAddressMulti(I2Cx, slave_address, memory_address, data, 2);
  }

  // read from a register
  uint16_t ReadRegister(RegisterEnum memory_address) {
    uint8_t data[2];
    GSL_I2C_ReadAddressMulti(I2Cx, slave_address, memory_address, data, 2);
    return data[1] + (uint16_t) 256 * data[0];
  }

  // return true if present and responding accordingly
  bool IsPresent(void) {
    // see if slave responds
    if (!GSL_I2C_IsSlavePresent(I2Cx, slave_address)) {
      return false;
    }
    // check for correct manufacturer ID
    {
      uint16_t value = ReadRegister(kRegisterManufacturerID);
      if (value != kValueManufacturerID) {
        return false;
      }
    }
    // check for correct die ID
    {
      uint16_t value = ReadRegister(kRegisterDieID);
      if (value != kValueDieID) {
        return false;
      }
    }
    return true;
  }

  // return the bus voltage in volts
  float GetVoltage(void) {
    return 1.25e-3f * ReadRegister(kRegisterBusVoltage);
  }

  // return the current in amps
  float GetCurrent(void) {
    uint16_t value = ReadRegister(kRegisterCurrent);
    float current = 1.25e-3f * (value & 0x7FFF);
    if (value & 0x8000) {
      current *= -1.0f;
    }
    return current;
  }

  // return the power in watts
  float GetPower(void) {
    return 10.0e-3f * ReadRegister(kRegisterPower);
  }

};
