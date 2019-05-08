#pragma once

// An GSL_RTC6715 can control a single RTC6715 chip.  Only SPI mode is
// supported.

#include "gsl_includes.h"

struct GSL_RTC6715 {

  // SCK pin
  PinEnum sck_pin;

  // MOSI pin
  PinEnum mosi_pin;

  // CS pin
  PinEnum cs_pin;

  // register
  enum RegisterEnum : uint8_t {
    kRegisterSynthesizerRegisterA = 0x00,
    kRegisterSynthesizerRegisterB = 0x01,
    kRegisterSynthesizerRegisterC = 0x02,
    kRegisterSynthesizerRegisterD = 0x03,
    kRegisterVCOSwitchCapControlRegister = 0x04,
    kRegisterDFCControlRegister = 0x05,
    kRegister6MAudioDemodulationControlRegister = 0x06,
    kRegister6M5AudioDemodulationControlRegister = 0x07,
    kRegisterReceiverControlRegister1 = 0x08,
    kRegisterReceiverControlRegister2 = 0x09,
    kRegisterPowerDownControlRegister = 0x0A,
    kRegisterStateRegister = 0x0F,
  };

  // max speed for SPI commands
  static const uint32_t max_spi_speed = 10000000;

  // constructor
  GSL_RTC6715(PinEnum sck_pin, PinEnum mosi_pin, PinEnum cs_pin) :
    sck_pin(sck_pin), mosi_pin(mosi_pin), cs_pin(cs_pin) {
  }

  // initialize
  void Initialize(void) {
    GSL_PIN_SetLow(sck_pin);
    GSL_PIN_Initialize(sck_pin, GPIO_MODE_OUTPUT_PP);
    GSL_PIN_SetLow(mosi_pin);
    GSL_PIN_Initialize(mosi_pin, GPIO_MODE_OUTPUT_PP);
    GSL_PIN_SetHigh(cs_pin);
    GSL_PIN_Initialize(cs_pin, GPIO_MODE_OUTPUT_PP);
  }

  // send out one bit on the already initialize SPI line
  void WriteBit(bool value) {
    if (value) {
      GSL_PIN_SetHigh(mosi_pin);
    } else {
      GSL_PIN_SetLow(mosi_pin);
    }
    // wait a bit
    GSL_DEL_US(max_spi_speed / 500000);
    // transition clock to latch value
    GSL_PIN_SetHigh(sck_pin);
    // wait a bit
    GSL_DEL_US(max_spi_speed / 500000);
    // transition block back to low
    GSL_PIN_SetLow(sck_pin);
  }

  // write to a register
  // data is right-aligned
  // i'm not sure if this will work, as we're writing 32 bits instead of 25
  void WriteRegister(RegisterEnum address, uint32_t data) {
    // deinitialize mosi and sck pins
    //GSL_PIN_Deinitialize(info->pins->mosi);
    //GSL_PIN_Deinitialize(info->pins->sck);

    // set data pins low
    GSL_PIN_SetLow(mosi_pin);
    GSL_PIN_SetLow(sck_pin);

    // set CS low
    GSL_PIN_SetLow(cs_pin);

    // wait for CS to take effect
    GSL_DEL_US(max_spi_speed / 500000);

    // send out 4-bit address, LSB first
    {
      uint8_t value = address;
      for (uint16_t i = 0; i < 4; ++i) {
        WriteBit(value % 2);
        value >>= 1;
      }
    }

    // send out R/W bit high to indicate write
    WriteBit(true);

    // send out 20-bit data, lsb first
    {
      for (uint16_t i = 0; i < 20; ++i) {
        WriteBit(data % 2);
        data >>= 1;
      }
    }

    GSL_DEL_US(max_spi_speed / 500000);

    // set CS high
    GSL_PIN_SetHigh(cs_pin);

  }

  // change to the given frequency in MHz
  // note this has a precision of 2 MHz
  void ChangeFrequency(uint16_t frequency_mhz) {
    const uint16_t delta = 479; // MHz
    uint16_t f_lo = frequency_mhz - delta;
    f_lo /= 2;
    uint16_t n = f_lo / 32;
    uint8_t a = f_lo % 32;
    uint32_t data = n;
    data <<= 7;
    data |= a;
    WriteRegister(kRegisterSynthesizerRegisterB, data);
  }

  // power down
  void PowerDown(void) {
    WriteRegister(kRegisterPowerDownControlRegister, 0b11111111111111111111);
  }

  // power up
  void Reset(void) {
    WriteRegister(kRegisterStateRegister, 0b000);
  }

};
