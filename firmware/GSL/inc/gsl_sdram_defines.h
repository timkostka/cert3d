#pragma once

#include "gsl_includes.h"

// pin structure
// (the variables names correspond to the names given in STM32Cube, except in
// lowercase)
struct GSL_SDRAM_PinsStruct {
  // up to 13 address pins
  PinEnum a[13];
  // up to 16 data pins (32 bits are not implemented here)
  PinEnum d[16];
  // bank address pins
  PinEnum ba[2];
  // 16-bit enable mode
  PinEnum nbl[2];
  PinEnum sdne;
  PinEnum sdcke;
  PinEnum sdncas;
  PinEnum sdclk;
  PinEnum sdnras;
  PinEnum sdnwe;
};

const GSL_SDRAM_PinsStruct gsl_sdram1_pins = {
  {kPinF0, kPinF1, kPinF2, kPinF3, kPinF4, kPinF5, kPinF12, kPinF13,
   kPinF14, kPinF15, kPinG0, kPinG1, kPinG2}, // a
  {kPinD14, kPinD15, kPinD0, kPinD1, kPinE7, kPinE8, kPinE9, kPinE10,
   kPinE11, kPinE12, kPinE13, kPinE14, kPinE15, kPinD8, kPinD9, kPinD10}, // d
  {kPinG4, kPinG5}, // ba
  {kPinE0, kPinE1}, //nbl
  kPinC2, // sdne
  kPinC3, // sdcke
  kPinG15, // sdncas
  kPinG8, // sdclk
  kPinF11, // sdnras
  kPinC0, // sdnwe
};

// handle for sdram
SDRAM_HandleTypeDef gsl_hsdram1 = {
    FMC_Bank5_6,
    {
        FMC_SDRAM_BANK1,
        FMC_SDRAM_COLUMN_BITS_NUM_9,
        FMC_SDRAM_ROW_BITS_NUM_13,
        FMC_SDRAM_MEM_BUS_WIDTH_16,
        FMC_SDRAM_INTERN_BANKS_NUM_4,
        FMC_SDRAM_CAS_LATENCY_3,
        FMC_SDRAM_WRITE_PROTECTION_DISABLE,
        FMC_SDRAM_CLOCK_PERIOD_2,
        FMC_SDRAM_RBURST_DISABLE,
        FMC_SDRAM_RPIPE_DELAY_0},
    HAL_SDRAM_STATE_RESET,
    HAL_UNLOCKED,
    nullptr,
};

// pointer to start of SDRAM memory
uint8_t * const gsl_sdram_ptr = (uint8_t *) ((uint32_t) 0xC0000000);

// SDRAM capacity in bytes
const uint32_t gsl_sdram_capacity = 1024 * 1024 * 32;

// end of SDRAM
uint8_t * const gsl_sdram_end = gsl_sdram_ptr + gsl_sdram_capacity;

// info struct
struct GSL_SDRAM_InfoStruct {
  // handle
  SDRAM_HandleTypeDef * handle;
  // pins
  const GSL_SDRAM_PinsStruct * pins;
};

GSL_SDRAM_InfoStruct gsl_sdram_info[1] = {
    {&gsl_hsdram1,
    &gsl_sdram1_pins}
};
