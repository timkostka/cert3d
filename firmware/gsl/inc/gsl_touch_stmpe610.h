#pragma once

#include "gsl_includes.h"

// This file includes an interface to the STMPE610 resistive touch screen
// controller.
//
// Available here:
// https://www.adafruit.com/products/1571

struct GSL_TOUCH_STMPE610 {
  // mode
  enum InterfaceEnum {
    kInterfaceNone,
    kInterfaceI2C,
    kInterfaceSPI,
  };
  // true when initialized
  bool initialized_;
  // active interface
  InterfaceEnum interface_;
  // interrupt pin (open drain when data is ready)
  PinEnum pinINT_;
  // I2C pins
  I2C_TypeDef * I2Cx_;
  uint8_t i2c_address_;
  // SPI pins
  SPI_TypeDef * SPIx_;
  PinEnum pinCS_;

  // max SPI speed
  static const uint32_t kMaxSPISpeed = 1000000;

  // constructor
  GSL_TOUCH_STMPE610(void) {
    interface_ = kInterfaceNone;
    initialized_ = false;
    pinINT_ = kPinNone;
    I2Cx_ = nullptr;
    i2c_address_ = 0xFF;
    SPIx_ = nullptr;
    pinCS_ = kPinNone;
    touched_ = false;
  };

  // read an address value
  uint8_t ReadAddress(uint8_t address) {
    ASSERT(address < 0x80);
    ASSERT(interface_ == kInterfaceSPI);
    GSL_SPI_SetCSMode(SPIx_, GSL_SPI_CS_MODE_MANUAL);
    GSL_SPI_SetCSPin(SPIx_, pinCS_);
    GSL_SPI_SetCSLow(SPIx_);
    GSL_SPI_Send(SPIx_, address | 0x80);
    uint8_t value = GSL_SPI_Read(SPIx_);
    // send 0 to end read operation
    GSL_SPI_Send(SPIx_, 0);
    GSL_SPI_SetCSHigh(SPIx_);
    return value;
  }

  // read a 16-bit address value
  /*uint16_t ReadAddress16(uint8_t address) {
    ASSERT(address < 0x80);
    ASSERT(interface_ == kInterfaceSPI);
    GSL_SPI_SetCSMode(SPIx_, GSL_SPI_CS_MODE_MANUAL);
    GSL_SPI_SetCSPin(SPIx_, pinCS_);
    GSL_SPI_SetCSLow(SPIx_);
    GSL_SPI_Send(SPIx_, address | 0x80);
    uint8_t value1 = GSL_SPI_Read(SPIx_);
    uint8_t value2 = GSL_SPI_Read(SPIx_);
    // send 0 to end read operation
    GSL_SPI_Send(SPIx_, 0);
    GSL_SPI_SetCSHigh(SPIx_);
    return (((uint16_t) value1) << 8) + value2;
  }*/

  // write an address value
  uint8_t WriteAddress(uint8_t address, uint8_t value) {
    ASSERT(address < 0x80);
    ASSERT(interface_ == kInterfaceSPI);
    GSL_SPI_SetCSMode(SPIx_, GSL_SPI_CS_MODE_MANUAL);
    GSL_SPI_SetCSPin(SPIx_, pinCS_);
    GSL_SPI_SetCSLow(SPIx_);
    GSL_SPI_Send(SPIx_, address);
    GSL_SPI_Send(SPIx_, value);
    GSL_SPI_SetCSHigh(SPIx_);
    return value;
  }

  // verify controller is working correctly
  void Verify(void) {
    uint16_t chip_id = ((uint16_t) ReadAddress(0x00) << 8) + ReadAddress(0x01);
    uint8_t id_ver = ReadAddress(0x02);
    ASSERT(chip_id == 0x0811);
    ASSERT(id_ver == 0x03 || id_ver == 0x01);
  }


  // store the value of the last touch or release
  bool touched_;

  // store the time of the last press or release
  GSL_DEL_LongTime last_event_time_;

  // delay in ms between touch and first possible release and between release
  // and first possible touch
  static const uint32_t kTouchDelayMS = 50;

  // calibration struct
  struct CalibrationDataStruct {
    uint16_t x_raw;
    uint16_t y_raw;
    float x;
    float y;
  };

  float cal[6];

  // find values to best fit the calibration data
  bool Calibrate(CalibrationDataStruct * data, uint16_t data_count) {
    ASSERT(data_count >= 3);
    // allowable error
    const float kAllowableError = 0.1f;

    const bool kAlignToAxes = true;
    // x = b0 + b1 * x_raw + b2 * y_raw;
    // y = b3 + b4 * x_raw + b5 * y_raw;
    // solve for some constants
    float x_mean = 0.0f;
    float y_mean = 0.0f;
    float xraw_mean = 0.0f;
    float yraw_mean = 0.0f;
    float xraw_yraw_mean = 0.0f;
    float xraw2_mean = 0.0f;
    float yraw2_mean = 0.0f;
    float x_xraw_mean = 0.0f;
    float y_xraw_mean = 0.0f;
    float x_yraw_mean = 0.0f;
    float y_yraw_mean = 0.0f;
    for (uint16_t i = 0; i < data_count; ++i) {
      x_mean += data[i].x / data_count;
      xraw_mean += (float) data[i].x_raw / data_count;
      xraw2_mean += ((float) data[i].x_raw * data[i].x_raw) / data_count;
      x_xraw_mean += (data[i].x * data[i].x_raw) / data_count;
      x_yraw_mean += (data[i].x * data[i].y_raw) / data_count;
      y_mean += data[i].y / data_count;
      yraw_mean += (float) data[i].y_raw / data_count;
      yraw2_mean += ((float) data[i].y_raw * data[i].y_raw) / data_count;
      y_xraw_mean += (data[i].y * data[i].x_raw) / data_count;
      y_yraw_mean += (data[i].y * data[i].y_raw) / data_count;
      xraw_yraw_mean += (float) data[i].x_raw * data[i].y_raw / data_count;
    }
    // we need to solve for cal[1] and cal[2] using the equations
    // a * cal[1] + b * cal[2] == e
    // c * cal[1] + d * cal[2] == f
    {
      float a = xraw_mean * xraw_mean - xraw2_mean;
      float b = xraw_mean * yraw_mean - xraw_yraw_mean;
      float e = x_mean * xraw_mean - x_xraw_mean;
      float c = yraw_mean * xraw_mean - xraw_yraw_mean;
      float d = yraw_mean * yraw_mean - yraw2_mean;
      float f = x_mean * yraw_mean - x_yraw_mean;
      float det = a * d - b * c;
      if (det == 0.0f) {
        return false;
      }
      cal[1] = (d * e - b * f) / det;
      cal[2] = (-c * e + a * f) / det;
    }
    // solve for c3
    cal[0] = x_mean - cal[1] * xraw_mean - cal[2] * yraw_mean;
    // we need to solve for cal[4] and cal[5] using the equations
    // a * cal[1] + b * cal[2] == e
    // c * cal[1] + d * cal[2] == f
    {
      float a = xraw_mean * xraw_mean - xraw2_mean;
      float b = xraw_mean * yraw_mean - xraw_yraw_mean;
      float e = y_mean * xraw_mean - y_xraw_mean;
      float c = yraw_mean * xraw_mean - xraw_yraw_mean;
      float d = yraw_mean * yraw_mean - yraw2_mean;
      float f = y_mean * yraw_mean - y_yraw_mean;
      float det = a * d - b * c;
      if (det == 0.0f) {
        return false;
      }
      cal[4] = (d * e - b * f) / det;
      cal[5] = (-c * e + a * f) / det;
    }
    // solve for c3
    cal[3] = y_mean - cal[4] * xraw_mean - cal[5] * yraw_mean;

    // DEBUG
    {
      float err = 0.0f;
      float den = 0.0f;
      for (uint16_t i = 0; i < data_count; ++i) {
        float x = cal[0] + cal[1] * data[i].x_raw + cal[2] * data[i].y_raw;
        float y = cal[3] + cal[4] * data[i].x_raw + cal[5] * data[i].y_raw;
        err += powf(x - data[i].x, 2.0f) + powf(y - data[i].y, 2.0f);
        den += powf(data[i].x, 2.0f) + powf(data[i].y, 2.0f);
        //LOG("\npoint ", i, " (", (uint16_t) data[i].x, ", ");
        //LOG((uint16_t) data[i].y, ") --> (", GSL_OUT_FixedFloat(x, 2), ", ");
        //LOG(GSL_OUT_FixedFloat(y, 2), ")");
      }
      err = sqrtf(err / den) * sqrtf(2.0f);
      //LOG("\nerr = ", GSL_OUT_FixedFloat(err, 5));
      if (!kAlignToAxes) {
        return err <= kAllowableError;
      }
    }
    // now we need to determine rotation and either set cal[1] = cal[5] = 0
    // or cal[2] = cal[4] = 0
    if (fabs(cal[1]) + fabs(cal[5]) > fabs(cal[2]) + fabs(cal[4])) {
      cal[2] = 0.0f;
      cal[1] = (x_mean * xraw_mean - x_xraw_mean) /
          (xraw_mean * xraw_mean - xraw2_mean);
      cal[0] = x_mean - cal[1] * xraw_mean - cal[2] * yraw_mean;

      cal[4] = 0.0f;
      cal[5] = (y_mean * yraw_mean - y_yraw_mean) /
          (yraw_mean * yraw_mean - yraw2_mean);
      cal[3] = y_mean - cal[4] * xraw_mean - cal[5] * yraw_mean;
    } else {
      cal[1] = 0.0f;
      cal[2] = (x_mean * yraw_mean - x_yraw_mean) /
          (yraw_mean * yraw_mean - yraw2_mean);
      cal[0] = x_mean - cal[1] * xraw_mean - cal[2] * yraw_mean;

      cal[5] = 0.0f;
      cal[4] = (y_mean * xraw_mean - y_xraw_mean) /
          (xraw_mean * xraw_mean - xraw2_mean);
      cal[3] = y_mean - cal[4] * xraw_mean - cal[5] * yraw_mean;
    }
    // DEBUG
    {
      float err = 0.0f;
      float den = 0.0f;
      for (uint16_t i = 0; i < data_count; ++i) {
        float x = cal[0] + cal[1] * data[i].x_raw + cal[2] * data[i].y_raw;
        float y = cal[3] + cal[4] * data[i].x_raw + cal[5] * data[i].y_raw;
        err += powf(x - data[i].x, 2.0f) + powf(y - data[i].y, 2.0f);
        den += powf(data[i].x, 2.0f) + powf(data[i].y, 2.0f);
        //LOG("\npoint ", i, " (", (uint16_t) data[i].x, ", ");
        //LOG((uint16_t) data[i].y, ") --> (", GSL_OUT_FixedFloat(x, 2), ", ");
        //LOG(GSL_OUT_FixedFloat(y, 2), ")");
      }
      err = sqrtf(err / den) * sqrtf(2.0f);
      //LOG("\nerr = ", GSL_OUT_FixedFloat(err, 5));
      // return true if error meets our success criterion
      return err <= kAllowableError;
    }
  }

  // initialize
  void Initialize(void) {
    if (initialized_) {
      return;
    }
    initialized_= true;
    // only SPI interface currently implemented
    ASSERT(interface_ == kInterfaceSPI);
    ASSERT(SPIx_ != nullptr);
    ASSERT(GSL_PIN_IsReal(pinCS_));
    ASSERT(GSL_PIN_IsReal(pinINT_));
    // initialize pins
    GSL_PIN_Initialize(pinINT_, GPIO_MODE_INPUT, GPIO_PULLUP);
    GSL_SPI_SetMaxSpeed(SPIx_, kMaxSPISpeed);
    GSL_SPI_AddCSPin(SPIx_, pinCS_);
    GSL_SPI_SetCSMode(SPIx_, GSL_SPI_CS_MODE_MANUAL);
    GSL_SPI_SetCSPin(SPIx_, pinCS_);
    GSL_SPI_Initialize(SPIx_);
    // the following initialization is based on the Adafruit library
    // reset the controller and wait for it to reset
    WriteAddress(0x03, 2);
    GSL_DEL_MS(10);
    // verify controller is responding
    Verify();
    // (i'm not sure why they do this)
    for (uint8_t i = 0; i < 65; ++i) {
      ReadAddress(i);
    }
    // enable clock
    WriteAddress(0x04, 0x0); // turn on clocks!
    WriteAddress(0x40, 0b1 | (0b001 << 1) | (0b000 << 4)); // XY and enable!
    //Serial.println(readRegister8(STMPE_TSC_CTRL), HEX);
    WriteAddress(0x0A, 0x01);
    WriteAddress(0x20, 0 | (0x6 << 4)); // 96 clocks per conversion
    WriteAddress(0x21, 0x00);
    // 4 sample average, 1ms touch detect delay, 5ms settle time
    WriteAddress(0x41, (0b10 << 6) | (0b100 << 3) | (0b100 << 0));
    // 4 sample average, 500us detect delay, 100us settle time
    WriteAddress(0x41, (0b10 << 6) | (0b011 << 3) | (0b001 << 0));
    // set z axis resolution
    WriteAddress(0x56, 0x4);
    // trigger interrupt when FIFO reaches 1 value
    WriteAddress(0x4A, 1);
    WriteAddress(0x4B, 0x01);
    WriteAddress(0x4B, 0);    // unreset
    WriteAddress(0x58, 0x01);
    WriteAddress(0x0B, 0xFF); // reset all ints
    WriteAddress(0x09, 0x04 | 0x01);

    last_event_time_.ticks = 0;
    touched_ = false;

    // reset calibration so that value matches raw values
    for (uint16_t i = 0; i < 6; ++i) {
      cal[i] = 0.0f;
    }
    cal[1] = 1.0f;
    cal[5] = 1.0f;
  }

  // return true if a touch is ready
  bool TouchReady(void) {
    return ReadAddress(0x40) & 0x80;
    //return !GSL_PIN_GetValue(pinINT_);
  }

  // return true if the touch buffer is empty
  bool IsBufferEmpty(void) {
    return ReadAddress(0x4B) & 0x20;
  }

  // if a touch is ready, read it and return true
  // else return false
  //bool GetRawTouch(uint16_t * x, uint16_t * y, uint8_t * z) {
  bool GetRawTouch(uint16_t * x, uint16_t * y) {
    if (IsBufferEmpty()) {
      return false;
    }
    uint8_t data[3];
    for (uint8_t i = 0; i < 3; ++i) {
      data[i] = ReadAddress(0x57);
    }
    // get x value
    if (x) {
      *x = data[0];
      *x <<= 4;
      *x |= data[1] >> 4;
    }
    // get y value
    if (y) {
      *y = data[1] & 0b1111;
      *y <<= 8;
      *y |= data[2];
    }
    //if (z) {
    //  *z = data[3];
    //}
    // reset the interrupt if buffer is empty
    if (IsBufferEmpty()) {
      WriteAddress(0x0B, 0xFF);
    }
    return true;
  }

  // discard all touch data
  void Purge(void) {
    // reset FIFO
    WriteAddress(0x4B, 0b1);
    // pull FIFO out of reset
    WriteAddress(0x4B, 0b0);
  }

  // if a new touch is detected, return true and store the touch coordinates
  bool GetTouch(float * x, float * y) {
    // store the time
    GSL_DEL_LongTime this_time = GSL_DEL_GetLongTime();
    // last time this function was called
    static GSL_DEL_LongTime last_call = {0};
    bool can_change_state = (last_call != last_event_time_) &&
        (GSL_DEL_ElapsedMS(last_event_time_, this_time) >= kTouchDelayMS);
    last_call = this_time;
    // if it's too early to register a release, or if we haven't called this
    // function since that event, just purge and return
    if (touched_ && !can_change_state) {
      // if a touch is registered, reset the timer
      if (!IsBufferEmpty()) {
        Purge();
        last_event_time_ = this_time;
      }
      return false;
    }
    // if it's too early to register a touch, or if we haven't called this
    // function since that event, just purge and return
    if (!touched_ && !can_change_state) {
      Purge();
      return false;
    }
    ASSERT(can_change_state);
    // if the state has not changed, just return
    if (touched_) {
      if (IsBufferEmpty()) {
        // set status to released
        touched_ = false;
        ASSERT(GSL_DEL_ElapsedMS(last_event_time_, this_time) >= kTouchDelayMS);
        last_event_time_ = this_time;
      } else {
        Purge();
      }
      return false;
    }
    // if no touches present, just return
    if (IsBufferEmpty()) {
      return false;
    }
    // read last value in buffer
    uint16_t xr, yr;
    ASSERT(GetRawTouch(&xr, &yr));
    while (GetRawTouch(&xr, &yr)) {
    }
    // apply calibration values
    *x = cal[0] + cal[1] * xr + cal[2] * yr;
    *y = cal[3] + cal[4] * xr + cal[5] * yr;
    // set status to touched
    touched_ = true;
    // reset timer
    ASSERT(GSL_DEL_ElapsedMS(last_event_time_, this_time) >= kTouchDelayMS);
    last_event_time_ = this_time;
    return true;

  }

  // return true if a touch is not detected
  // we take care here not to handle the case where
  bool IsReleased(void) {
    // if not touched, just return
    if (!touched_) {
      return false;
    }
    // else we process the buffer to possibly register a release
    GetTouch(nullptr, nullptr);
    // return true if not touched
    return !touched_;
  }

};
