#pragma once

// This file implements a GSL_BMP280 object which interfaces with a given
// BMP280 device.

#include "gsl_includes.h"

struct GSL_BMP280 {

  // slave address
  uint8_t i2c_slave_address;

  // I2C peripheral
  I2C_TypeDef * I2Cx;

  // true if bus is initialized
  bool bus_initialized;

  // true if chip is initialized
  bool chip_initialized;

  // bit for measuring in the status register
  static const uint8_t kBmp280StatusMeasuringBit = 0b1000;

  // bit for image data copying in the status register
  static const uint8_t kBmp280StatusImUpdateBit = 0b1;

  // id value of the chip
  static const uint8_t kBmp280IDValue = 0x58;

  // value to write into the reset register to reset
  static const uint8_t kBmp280ResetValue = 0xB6;

  // enum for modes
  enum GSL_BMP280_ModeEnum : uint8_t {
    kBmp280ModeSleep = 0b00,
    kBmp280ModeForced = 0b01,
    kBmp280ModeNormal = 0b11,
  };

  // enum for temperature and pressure oversampling
  enum GSL_BMP280_OversamplingEnum : uint8_t {
    kBmp280OversamplingSkipped = 0b000,
    kBmp280Oversampling1 = 0b001,
    kBmp280Oversampling2 = 0b010,
    kBmp280Oversampling4 = 0b011,
    kBmp280Oversampling8 = 0b100,
    kBmp280Oversampling16 = 0b101,
  };

  // enum for standby settings
  enum GSL_BMP280_StandbyEnum : uint8_t {
    kBmp280Standby0 = 0b000, // 0.5 ms
    kBmp280Standby1 = 0b001, // 62.5 ms
    kBmp280Standby2 = 0b010, // 125 ms
    kBmp280Standby3 = 0b011, // 250 ms
    kBmp280Standby4 = 0b100, // 500 ms
    kBmp280Standby5 = 0b101, // 1000 ms
    kBmp280Standby6 = 0b110, // 2000 ms
    kBmp280Standby7 = 0b111, // 4000 ms
  };

  // enum for IIR filter values
  enum GSL_BMP280_IirFilterEnum : uint8_t {
    kBmp280IirFilterOff = 0b000,
    kBmp280IirFilter2 = 0b001,
    kBmp280IirFilter4 = 0b010,
    kBmp280IirFilter8 = 0b011,
    kBmp280IirFilter16 = 0b100,
  };

  // enum for 3 wire spi enabled
  enum GSL_BMP280_Spi3Wire : uint8_t {
    kBmp280Spi3WireDisabled = 0b0,
    kBmp280Spi3WireEnabled = 0b1,
  };

  // register values
  enum GSL_BMP280_RegisterAddressEnum : uint8_t {
    // ID
    kBmp280RegisterID = 0xD0,
    // Reset
    kBmp280RegisterReset = 0xE0,
    // Status
    kBmp280RegisterStatus = 0xF3,
    // Control measurement
    kBmp280RegisterCtrlMeas = 0xF4,
    // configuration
    kBmp280RegisterConfig = 0xF5,
    // start of pressure data
    kBmp280RegisterPressure = 0xF7,
    // start of temperature data
    kBmp280RegisterTemperature = 0xFA,
    // start of the compensation values
    kBmp280RegisterCompensation = 0x88,
  };

  // struct to hold calibration values
  struct GSL_BMP280_CompensationStruct {
    uint16_t dig_T1;
    int16_t dig_T2;
    int16_t dig_T3;
    uint16_t dig_P1;
    int16_t dig_P2;
    int16_t dig_P3;
    int16_t dig_P4;
    int16_t dig_P5;
    int16_t dig_P6;
    int16_t dig_P7;
    int16_t dig_P8;
    int16_t dig_P9;
    int16_t reserved;
  };

  // calibration values
  GSL_BMP280_CompensationStruct compensation;

  // constructor
  GSL_BMP280(I2C_TypeDef * I2Cx_,
             uint8_t slave_address_) {
    // set everything to 0
    memset(this, 0, sizeof(*this));
    I2Cx = I2Cx_;
    i2c_slave_address = slave_address_;
  }

  // read a register
  uint8_t ReadRegister(GSL_BMP280_RegisterAddressEnum memory_address) {
    return GSL_I2C_ReadAddress(I2Cx, i2c_slave_address, memory_address);
  }

  // read several registers
  void ReadRegisterMulti(GSL_BMP280_RegisterAddressEnum memory_address,
                         uint8_t * data,
                         uint8_t length) {
    GSL_I2C_ReadAddressMulti(I2Cx,
                             i2c_slave_address,
                             memory_address,
                             data,
                             length);
  }

  // write to a register
  void WriteRegister(GSL_BMP280_RegisterAddressEnum memory_address,
                     uint8_t value) {
    GSL_I2C_WriteAddress(I2Cx, i2c_slave_address, memory_address, value);
  }

  // read compensation values if not already done
  void ReadCompensationValues(void) {
    // if we've already done this, just return
    if (compensation.dig_T1 != 0) {
      return;
    }
    // else read them all
    GSL_I2C_ReadAddressMulti(I2Cx,
                             i2c_slave_address,
                             kBmp280RegisterCompensation,
                             (uint8_t *) &compensation,
                             sizeof(compensation));
    // now convert
    /*
    compensation.dig_T1 = GSL_GEN_SwapEndian(compensation.dig_T1);
    compensation.dig_T2 = GSL_GEN_SwapEndian(compensation.dig_T2);
    compensation.dig_T3 = GSL_GEN_SwapEndian(compensation.dig_T3);
    compensation.dig_P1 = GSL_GEN_SwapEndian(compensation.dig_P1);
    compensation.dig_P2 = GSL_GEN_SwapEndian(compensation.dig_P2);
    compensation.dig_P3 = GSL_GEN_SwapEndian(compensation.dig_P3);
    compensation.dig_P4 = GSL_GEN_SwapEndian(compensation.dig_P4);
    compensation.dig_P5 = GSL_GEN_SwapEndian(compensation.dig_P5);
    compensation.dig_P6 = GSL_GEN_SwapEndian(compensation.dig_P6);
    compensation.dig_P7 = GSL_GEN_SwapEndian(compensation.dig_P7);
    compensation.dig_P8 = GSL_GEN_SwapEndian(compensation.dig_P8);
    compensation.dig_P9 = GSL_GEN_SwapEndian(compensation.dig_P9);
    */
  }

  // convert pressure into altitude in meters above sea level
  // taken from http://keisan.casio.com/exec/system/1224585971
  float PressureToAltitude(float pressure_pa,
                           float temperature_c,
                           float mean_sea_level_pressure = 101325.0f) {
    float altitude_m = powf(mean_sea_level_pressure / pressure_pa,
                          1.0f / 5.257f);
    altitude_m -= 1.0f;
    altitude_m *= temperature_c + 273.15;
    altitude_m /= 0.0065;
    return altitude_m;
  }

  // global variable used in the pressure compensation
  float t_fine;

  // this is adapted from the datasheet
  // Returns temperature in DegC, double precision. Output value of "51.23"
  // equals 51.23 DegC.
  // t_fine carries fine temperature as global value
  float bmp280_compensate_T_float(int32_t adc_T) {
    // convert values to float
    float dig_T1_f = (float) compensation.dig_T1;
    float dig_T2_f = (float) compensation.dig_T2;
    float dig_T3_f = (float) compensation.dig_T3;
    float adc_T_f = (float) adc_T;
    float var1, var2, T;
    var1 = (adc_T_f / 16384.0f - dig_T1_f / 1024.0f) * dig_T2_f;
    var2 = ((adc_T_f / 131072.0f - dig_T1_f / 8192.0f) *
    (adc_T_f / 131072.0f - dig_T1_f / 8192.0f)) * dig_T3_f;
    t_fine = var1 + var2;
    T = (var1 + var2) / 5120.0f;
    return T;
  }

  // this is adapted from the datasheet
  // Returns pressure in Pa as float.
  // Output value of "96386.2" equals 96386.2 Pa = 963.862 hPa
  float bmp280_compensate_P_float(int32_t adc_P) {
    float dig_P1_f = (float) compensation.dig_P1;
    float dig_P2_f = (float) compensation.dig_P2;
    float dig_P3_f = (float) compensation.dig_P3;
    float dig_P4_f = (float) compensation.dig_P4;
    float dig_P5_f = (float) compensation.dig_P5;
    float dig_P6_f = (float) compensation.dig_P6;
    float dig_P7_f = (float) compensation.dig_P7;
    float dig_P8_f = (float) compensation.dig_P8;
    float dig_P9_f = (float) compensation.dig_P9;
    float adc_P_f = (float) adc_P;
    float var1, var2, p;
    var1 = t_fine / 2.0f - 64000.0f;
    var2 = var1 * var1 * dig_P6_f / 32768.0f;
    var2 = var2 + var1 * dig_P5_f * 2.0f;
    var2 = var2 / 4.0f + dig_P4_f * 65536.0f;
    var1 = (dig_P3_f * var1 * var1 / 524288.0f + dig_P2_f * var1) / 524288.0f;
    var1 = (1.0 + var1 / 32768.0f) * dig_P1_f;
    if (var1 == 0.0f) {
      return 0.0f; // avoid exception caused by division by zero
    }
    p = 1048576.0f - adc_P_f;
    p = (p - var2 / 4096.0f) * 6250.0f / var1;
    var1 = dig_P9_f * p * p / 2147483648.0f;
    var2 = p * dig_P8_f / 32768.0f;
    p = p + (var1 + var2 + dig_P7_f) / 16.0f;
    return p;
  }

  // reset the chip
  // must wait 2ms for boot
  void Reset(void) {
    WriteRegister(kBmp280RegisterReset, kBmp280ResetValue);
    GSL_DEL_MS(2);
  }

  // set the mode
  // in forced mode, this is also how we start a conversion
  void SetMode(GSL_BMP280_OversamplingEnum temperature_oversampling,
               GSL_BMP280_OversamplingEnum pressure_oversampling,
               GSL_BMP280_ModeEnum mode) {
    uint8_t value = temperature_oversampling;
    value <<= 3;
    value += pressure_oversampling;
    value <<= 2;
    value += mode;
    WriteRegister(kBmp280RegisterCtrlMeas, value);
  }

  // set the config
  void SetConfig(GSL_BMP280_StandbyEnum standby,
                 GSL_BMP280_IirFilterEnum iir_filter,
                 GSL_BMP280_Spi3Wire spi_3_wire) {
    uint8_t value = standby;
    value <<= 3;
    value += iir_filter;
    value <<= 2;
    value += spi_3_wire;
    WriteRegister(kBmp280RegisterConfig, value);
  }

  // return true if a reading is ongoing
  bool IsConversionRunning(void) {
    return ReadRegister(kBmp280RegisterStatus) & kBmp280StatusMeasuringBit;
  }

  // convert 3 bytes to an int32_t value
  int32_t MemoryToCounts(uint8_t * data) {
    int32_t value = 0;
    value += data[0];
    value <<= 8;
    value += data[1];
    value <<= 8;
    value += data[2];
    value >>= 4;
    return value;
/*
    int32_t value = 0;
    value += data[0];
    value <<= 8;
    value += data[1];
    value <<= 4;
    value += data[2] >> 4;
    return value;*/
  }

  // read out pressure and temperature
  void ReadValues(float & pressure_pa,
                  float & temperature_c) {
    // read compensation values
    ReadCompensationValues();
    uint8_t data[6];
    ReadRegisterMulti(kBmp280RegisterPressure, data, sizeof(data));
    int32_t pressure = MemoryToCounts(data);
    int32_t temperature = MemoryToCounts(&data[3]);
    // convert temperature before pressure since it's used in the pressure
    // conversion
    temperature_c = bmp280_compensate_T_float(temperature);
    pressure_pa = bmp280_compensate_P_float(pressure);
  }

};
