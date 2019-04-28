#pragma once

// This provides an interface for setting up access to an external SDRAM
// chip via the FMC peripheral.
//
// Note that the FMC peripheral is not available on all devices.  Also note
// that the FSMC and FMC are different peripherals and that the FSMC is not
// capable of interfacing with SDRAM.

#include "gsl_includes.h"

void GSL_SDRAM_Initialize(void) {
  // get the info
  GSL_SDRAM_InfoStruct * info = &(gsl_sdram_info[0]);
  // if already initialized, do nothing
  if (info->handle->State != HAL_SDRAM_STATE_RESET) {
    return;
  }
  // initialize pins
  // address pins (e.g. A[0])
  uint8_t address_pin_count = 11 + info->handle->Init.RowBitsNumber / 4;
  for (uint8_t i = 0; i < address_pin_count; ++i) {
    GSL_PIN_InitializeAF(
        info->pins->a[i],
        GPIO_MODE_AF_PP,
        GPIO_NOPULL,
        GPIO_AF12_FMC);
  }
  // data pins (e.g. D[0])
  for (uint8_t i = 0; i < 8; ++i) {
    GSL_PIN_InitializeAF(
        info->pins->d[i],
        GPIO_MODE_AF_PP,
        GPIO_NOPULL,
        GPIO_AF12_FMC);
  }
  if (info->handle->Init.MemoryDataWidth != FMC_SDRAM_MEM_BUS_WIDTH_8) {
    for (uint8_t i = 8; i < 16; ++i) {
      GSL_PIN_InitializeAF(
          info->pins->d[i],
          GPIO_MODE_AF_PP,
          GPIO_NOPULL,
          GPIO_AF12_FMC);
    }
  }
  if (info->handle->Init.MemoryDataWidth == FMC_SDRAM_MEM_BUS_WIDTH_32) {
    for (uint8_t i = 16; i < 32; ++i) {
      GSL_PIN_InitializeAF(
          info->pins->d[i],
          GPIO_MODE_AF_PP,
          GPIO_NOPULL,
          GPIO_AF12_FMC);
    }
  }
  // BA0
  GSL_PIN_InitializeAF(
      info->pins->ba[0],
      GPIO_MODE_AF_PP,
      GPIO_NOPULL,
      GPIO_AF12_FMC);
  // BA1
  GSL_PIN_InitializeAF(
      info->pins->ba[1],
      GPIO_MODE_AF_PP,
      GPIO_NOPULL,
      GPIO_AF12_FMC);
  // NBL0
  GSL_PIN_InitializeAF(
      info->pins->nbl[0],
      GPIO_MODE_AF_PP,
      GPIO_NOPULL,
      GPIO_AF12_FMC);
  // NBL1
  GSL_PIN_InitializeAF(
      info->pins->nbl[1],
      GPIO_MODE_AF_PP,
      GPIO_NOPULL,
      GPIO_AF12_FMC);
  // SDNE
  GSL_PIN_InitializeAF(
      info->pins->sdne,
      GPIO_MODE_AF_PP,
      GPIO_NOPULL,
      GPIO_AF12_FMC);
  // SDCKE
  GSL_PIN_InitializeAF(
      info->pins->sdcke,
      GPIO_MODE_AF_PP,
      GPIO_NOPULL,
      GPIO_AF12_FMC);
  // SDNCAS
  GSL_PIN_InitializeAF(
      info->pins->sdncas,
      GPIO_MODE_AF_PP,
      GPIO_NOPULL,
      GPIO_AF12_FMC);
  // SDCLK
  GSL_PIN_InitializeAF(
      info->pins->sdclk,
      GPIO_MODE_AF_PP,
      GPIO_NOPULL,
      GPIO_AF12_FMC);
  // SDNRAS
  GSL_PIN_InitializeAF(
      info->pins->sdnras,
      GPIO_MODE_AF_PP,
      GPIO_NOPULL,
      GPIO_AF12_FMC);
  // SDNWE
  GSL_PIN_InitializeAF(
      info->pins->sdnwe,
      GPIO_MODE_AF_PP,
      GPIO_NOPULL,
      GPIO_AF12_FMC);

  // enable the clock
  __HAL_RCC_FMC_CLK_ENABLE();

  // timing information
  FMC_SDRAM_TimingTypeDef SdramTiming;
  SdramTiming.LoadToActiveDelay = 2;
  SdramTiming.ExitSelfRefreshDelay = 7;
  SdramTiming.SelfRefreshTime = 4;
  SdramTiming.RowCycleDelay = 7;
  SdramTiming.WriteRecoveryTime = 2;
  SdramTiming.RPDelay = 2;
  SdramTiming.RCDDelay = 2;
  HAL_SDRAM_Init(info->handle, &SdramTiming);

  // now activate it
  {
    uint32_t target = FMC_SDRAM_CMD_TARGET_BANK1;
    FMC_SDRAM_CommandTypeDef Command;
    // Configure a clock configuration enable command
    Command.CommandMode = FMC_SDRAM_CMD_CLK_ENABLE;
    Command.CommandTarget = target;
    Command.AutoRefreshNumber = 1;
    Command.ModeRegisterDefinition = 0;
    // send the command
    HAL_SDRAM_SendCommand(info->handle, &Command, 0xFFFF);
    // wait a small amount for it to take effect
    GSL_DEL_MS(10);
    // configure a PALL (precharge all) command
    Command.CommandMode = FMC_SDRAM_CMD_PALL;
    Command.CommandTarget = target;
    Command.AutoRefreshNumber = 1;
    Command.ModeRegisterDefinition  = 0;
    // send the command
    HAL_SDRAM_SendCommand(info->handle, &Command, 0xFFFF);
    // Configure a Auto-Refresh command
    Command.CommandMode = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
    Command.CommandTarget = target;
    Command.AutoRefreshNumber = 8;
    Command.ModeRegisterDefinition = 0;
    // send the command
    HAL_SDRAM_SendCommand(info->handle, &Command, 0xFFFF);
    // Configure a load Mode register command
    Command.CommandMode = FMC_SDRAM_CMD_LOAD_MODE;
    Command.CommandTarget = target;
    Command.AutoRefreshNumber = 1;
    // write burst mode disabled
    // CAS latency 3
    // burst length of 1 word (single byte)
    Command.ModeRegisterDefinition = 0b1000110000; //(uint32_t)SDRAM_REG_VALUE;
    // Send the command
    HAL_SDRAM_SendCommand(info->handle, &Command, 0xFFFF);
    // Set the device refresh rate
    HAL_SDRAM_ProgramRefreshRate(info->handle, 680); //SDRAM_REFRESH_COUNT);
    // small delay
    GSL_DEL_MS(10);
  }

  // write a value
  *((__IO uint8_t * ) 0xC0000050) = 0x45;
  // make sure that value is correct
  if (*((__IO uint8_t * ) 0xC0000050) != 0x45) {
    HALT("Bad value (8bit)");
  }

  // write a value
  *((__IO uint16_t * ) 0xC0000050) = 0x1234;
  // make sure that value is correct
  if (*((__IO uint16_t * ) 0xC0000050) != 0x1234) {
    HALT("Bad value (16bit)");
  }

}

// fill SDRAM with the given value
void GSL_SDRAM_Fill(uint32_t value = 0) {
  volatile uint32_t * ptr = (uint32_t *) gsl_sdram_ptr;
  uint32_t * end = ((uint32_t *) ptr) + gsl_sdram_capacity / 4;
  while (ptr != end) {
    *ptr = value;
    ++ptr;
  }
}

// run some tests and profile the speed of the SDRAM
void GSL_SDRAM_Profile(void) {
  LOG("\n\nRunning SDRAM profiling tests at ", SystemCoreClock / 1000000, " MHz");
  LOG("\nSDRAM capacity: ", gsl_sdram_capacity / 1024 / 1024, " MB");
  // run 8-bit tests
  {
    volatile uint8_t * ptr = (uint8_t *) gsl_sdram_ptr;
    uint8_t * end = (uint8_t *) ptr + gsl_sdram_capacity;
    uint8_t i = 0;
    uint32_t start_tick = GSL_DEL_Ticks();
    while (ptr != end) {
      *ptr = i;
      i += 67;
      ++ptr;
    }
    uint32_t end_tick = GSL_DEL_Ticks();
    float mb_per_s = (gsl_sdram_capacity / 1024.0f / 1024) /
        (GSL_DEL_GetDifferenceUS(start_tick, end_tick) / 1000000.0f);
    LOG("\nSpeed (8-bit writes): ", GSL_OUT_FixedFloat(mb_per_s, 1), " MB/s");
    // verify 8-bit tests
    i = 0;
    ptr = (uint8_t *) gsl_sdram_ptr;
    while (ptr != end) {
      if (*ptr != i) {
        LOG("\nat address ", GSL_OUT_Hex((uint32_t) ptr));
        LOG("\nvalue = ", i, " <> ", *ptr);
        HALT("Invalid value");
      }
      i += 67;
      ++ptr;
    }
    LOG("\nData verified");
  }
  // run 16-bit tests
  {
    volatile uint16_t * ptr = (uint16_t *) gsl_sdram_ptr;
    volatile uint16_t * end = ptr + gsl_sdram_capacity / 2;
    uint16_t i = 0;
    uint32_t start_tick = GSL_DEL_Ticks();
    while (ptr != end) {
      //TM_SDRAM_Write16(offset, i);
      *ptr = i;
      i += 67;
      ++ptr;
    }
    uint32_t end_tick = GSL_DEL_Ticks();
    float mb_per_s = (gsl_sdram_capacity / 1024.0f / 1024) /
        (GSL_DEL_GetDifferenceUS(start_tick, end_tick) / 1000000.0f);
    LOG("\nSpeed (16-bit writes): ", GSL_OUT_FixedFloat(mb_per_s, 1), " MB/s");
    // verify 16-bit tests
    i = 0;
    ptr = (uint16_t *) gsl_sdram_ptr;
    while (ptr != end) {
      if (*ptr != i) {
        LOG("\nat address ", GSL_OUT_Hex((uint32_t) ptr));
        LOG("\nvalue = ", i, " <> ", *ptr);
        HALT("Invalid value");
      }
      i += 67;
      ++ptr;
    }
    LOG("\nData verified");
  }

  // run 32-bit tests
  {
    volatile uint32_t * ptr = (uint32_t *) gsl_sdram_ptr;
    volatile uint32_t * end = ptr + gsl_sdram_capacity / 4;
    uint32_t i = 0;
    uint32_t start_tick = GSL_DEL_Ticks();
    //memset(gsl_sdram_ptr, 0xDEADBEEF, gsl_sdram_capacity);
    while (ptr != end) {
      *ptr = i;
      i += 67;
      ++ptr;
    }
    uint32_t end_tick = GSL_DEL_Ticks();
    float mb_per_s = (gsl_sdram_capacity / 1024.0f / 1024) /
        (GSL_DEL_GetDifferenceUS(start_tick, end_tick) / 1000000.0f);
    LOG("\nSpeed (32-bit writes): ", GSL_OUT_FixedFloat(mb_per_s, 1), " MB/s");
    // verify 32-bit tests
    i = 0;
    ptr = (uint32_t *) gsl_sdram_ptr;
    while (ptr != end) {
      if (*ptr != i) {
        HALT("Invalid value");
      }
      i += 67;
      ++ptr;
    }
    LOG("\nData verified");
  }
}
