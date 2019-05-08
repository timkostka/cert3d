#pragma once

// An GSL_PTC2075 object can control a single PTC2075 chip.

#include "gsl_includes.h"

struct GSL_PTC2075 {

  // true if initialized
  bool initialized;

  // I2C bus
  I2C_TypeDef * i2c;

  // slave address (7-bit)
  uint8_t slave_address;

  // registers
  enum RegisterEnum : uint8_t {
    kRegisterConf = 0x01,
    kRegisterTemp = 0x00,
    kRegisterTos = 0x03,
    kRegisterThyst = 0x02,
    kRegisterTidle = 0x04,
  };

  // current value of pointer register
  RegisterEnum pointer;

  // constructor
  GSL_PTC2075(I2C_TypeDef * i2c, uint8_t slave_address) :
      initialized(false),
      i2c(i2c),
      slave_address(slave_address),
      pointer(kRegisterTemp) {
  }

  // set the pointer register if necessary
  void SetPointerRegister(RegisterEnum address, bool force = false) {
    if (!force && pointer == address) {
      return;
    }
    pointer = address;
    uint8_t data;
    GSL_I2C_WriteAddressMulti(i2c, slave_address, address, &data, 0);
  }

  // initialize
  void Initialize(void) {
    if (initialized) {
      return;
    }
    initialized = true;
    GSL_I2C_Initialize(i2c);
    // set pointer register to temp register
    //SetPointerRegister(kRegisterTemp, true);
  }

  // return the temperature in C from the raw counts
  static float ConvertCountsToTempC(uint16_t counts) {
    counts >>= 5;
    float temp_c = 0.125f * counts;
    if (counts & (1 << 10)) {
      temp_c -= 128.0f;
    }
    return temp_c;
  }

  // convert Celsius to Fahrenheit
  static float ConvertTempCToTempF(float temp_c) {
    return (temp_c + 40.0f) * 9.0f / 5.0f - 40.0f;
  }

  // return the temperature reading in C
  float GetTemperatureC(void) {
    uint16_t raw_data;
    SetPointerRegister(kRegisterTemp);
    // transmit and report an error if it occurs
    HAL_RUN(
        HAL_I2C_Master_Receive(
            GSL_I2C_GetInfo(i2c)->handle,
            slave_address << 1,
            (uint8_t *) &raw_data,
            2,
            GSL_I2C_TIMEOUT));
    // reverse bytes
    raw_data = GSL_GEN_SwapEndian(raw_data);
    // zero out lowest 5 bits
    raw_data >>= 5;
    float temp_c = raw_data * 0.125f;
    if (raw_data & (1 << 10)) {
      temp_c -= 256.0f;
    }
    return temp_c;
  }

};
