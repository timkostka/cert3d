#pragma once

#include "gsl_includes.h"

struct GSL_INA226 {

  // i2c peripheral
  I2C_TypeDef * I2Cx;

  // 7-bit slave address
  uint8_t slave_address;

  // register addresses
  enum RegisterEnum : uint8_t {
    kRegisterConfiguration = 0x00,
    kRegisterShuntVoltage = 0x01,
    kRegisterBusVoltage = 0x02,
    kRegisterPower = 0x03,
    kRegisterCurrent = 0x04,
    kRegisterCalibration = 0x05,
    kRegisterMaskEnable = 0x06,
    kRegisterAlertLimit = 0x07,
    kRegisterManufacturerID = 0xFE,
    kRegisterDieID = 0xFF,
  };

  // manufacturer ID
  static const uint16_t kValueManufacturerID = 0x5449;

  // die ID
  static const uint16_t kValueDieID = 0x2260;

  // volts per count for bus voltage reading
  static const float kBusVoltsPerCount;

  // volts per count for shunt voltage reading
  static const float kShuntVoltsPerCount;

  // resistor value
  float resistor;

  // initializer
  GSL_INA226(
      I2C_TypeDef * I2Cx,
      uint8_t slave_address)
      : I2Cx(I2Cx),
      slave_address(slave_address) {
    // ensure valid slave address
    if ((slave_address & 0b11110000) != 0b01000000) {
      HALT("Invalid slave address");
    }
    resistor = 0.0f;
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

  // return the bus voltage in counts
  int16_t GetBusVoltageCounts(void) {
    return ReadRegister(kRegisterBusVoltage);
  }

  // return the bus voltage in volts
  float GetBusVoltage(void) {
    return kBusVoltsPerCount * GetBusVoltageCounts();
  }

  // return the shunt voltage drop in counts
  int16_t GetShuntVoltageCounts(void) {
    return ReadRegister(kRegisterShuntVoltage);
  }

  // return the shunt voltage drop in volts
  float GetShuntVoltage(void) {
    return kShuntVoltsPerCount * GetShuntVoltageCounts();
  }

  // return the current in amps
  /*
  float GetCurrent(void) {
    if (resistor == 0.0f) {
      return 0.0f;
    }
    return GetShuntVoltage() / resistor;
  }

  // sets the value of the shunt resistor
  void SetResistorValue(float resistance) {
    resistor = resistance;
  }

  // return the power in watts
  // resistor must be set first
  float GetPower(void) {
    return GetVoltage() * GetCurrent();
  }
  */

};


// volts per count for bus voltage reading
const float GSL_INA226::kBusVoltsPerCount = 1.25e-3f;

// volts per count for shunt voltage reading
const float GSL_INA226::kShuntVoltsPerCount = 2.5e-6f;
