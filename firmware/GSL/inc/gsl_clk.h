#pragma once

// This file contains interfaces for adjusting the clock settings and related
// functions.
//
// Under normal circumstances, it is expected that clock initialization will be
// done in the "_initialize_hardware.c" file.  Functions in this file do not
// need to be used at all.
//
// Note that changing the clock is a signficant change in that the clock
// controls the timing of pretty much everything the chip is doing, so changing
// it will affect pretty much everything you're doing.

#include "gsl_includes.h"

// number of settings that can be pushed at once
const uint8_t gsl_clk_max_pushed_settings = 1;

// saved settings structure
struct GSL_CLK_ClockSettingsStruct {
  // clock settings
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  // flash latency
  uint32_t pFLatency;
  // oscillator settings
  RCC_OscInitTypeDef RCC_OscInitStruct;
  // peripheral clock config
  RCC_PeriphCLKInitTypeDef PeriphClkInit;
};

// saved settings
GSL_CLK_ClockSettingsStruct gsl_clk_saved_settings[gsl_clk_max_pushed_settings];

// clock settings
// push level (0 = nothing stored)
uint8_t gsl_clk_push_level = 0;

// set main clock to the HSI.

// get the clock settings
GSL_CLK_ClockSettingsStruct GSL_CLK_GetSettings(void) {
  GSL_CLK_ClockSettingsStruct setting;
  HAL_RCC_GetClockConfig(&setting.RCC_ClkInitStruct, &setting.pFLatency);
  HAL_RCC_GetOscConfig(&setting.RCC_OscInitStruct);
  HAL_RCCEx_GetPeriphCLKConfig(&setting.PeriphClkInit);
  return setting;
}

// set the PLL settings and enable it
void GSL_CLK_SetPLL(uint32_t source,
                    uint8_t M,
                    uint8_t N,
                    uint8_t P,
                    uint8_t Q) {
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_RCC_GetOscConfig(&RCC_OscInitStruct);
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_NONE;
  RCC_OscInitStruct.PLL.PLLSource = source;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLM = M;
  RCC_OscInitStruct.PLL.PLLN = N;
  RCC_OscInitStruct.PLL.PLLP = P;
  RCC_OscInitStruct.PLL.PLLQ = Q;
  HAL_RUN(HAL_RCC_OscConfig(&RCC_OscInitStruct));
}

// disable the PLL
void GSL_CLK_DisablePLL(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_RCC_GetOscConfig(&RCC_OscInitStruct);
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_NONE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_OFF;
  HAL_RUN(HAL_RCC_OscConfig(&RCC_OscInitStruct));
}

// enable the PLL with the stored settings
void GSL_CLK_EnablePLL(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct;
  HAL_RCC_GetOscConfig(&RCC_OscInitStruct);
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_NONE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  HAL_RUN(HAL_RCC_OscConfig(&RCC_OscInitStruct));
}

// save clock settings
void GSL_CLK_PushSettings(void) {
  ASSERT(gsl_clk_push_level < gsl_clk_max_pushed_settings);
  // alias the settings we are storing
  GSL_CLK_ClockSettingsStruct & setting =
      gsl_clk_saved_settings[gsl_clk_push_level];
  ++gsl_clk_push_level;
  // save the current settings
  setting = GSL_CLK_GetSettings();
}

// switch to safe HSI settings
// system clock = HSI
// PLL is disabled
// flash latency is at maximum
// AHB1/AHB2 prescalers are 4
void GSL_CLK_SwitchToSafeHSI(void) {
  // read current settings
  auto setting = GSL_CLK_GetSettings();
  // enable HSI if not already enabled
  if (setting.RCC_OscInitStruct.HSIState != RCC_HSI_ON) {
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    HAL_RUN(HAL_RCC_OscConfig(&RCC_OscInitStruct));
  }
  // disable PLL if enabled
  if (setting.RCC_OscInitStruct.PLL.PLLState != RCC_PLL_OFF) {
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_NONE;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_OFF;
    HAL_RUN(HAL_RCC_OscConfig(&RCC_OscInitStruct));
  }
  // switch to HSI, no PLL for main clock
  if (setting.RCC_ClkInitStruct.SYSCLKSource != RCC_SYSCLKSOURCE_HSI) {
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK |
        RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    // use slow dividers
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;
    // use the slowest flash latency
    uint32_t fLatency = FLASH_LATENCY_7;
#ifdef FLASH_LATENCY_15
    fLatency = FLASH_LATENCY_15;
#endif
    // set it
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, fLatency);
  }
}

// retrieve clock settings
void GSL_CLK_PopSettings(void) {
  ASSERT(gsl_clk_push_level > 0);
  --gsl_clk_push_level;
  // alias the settings we want to switch to
  GSL_CLK_ClockSettingsStruct & new_setting =
      gsl_clk_saved_settings[gsl_clk_push_level];
  // switch to safe settings
  GSL_CLK_SwitchToSafeHSI();
  // get the current settings
  auto old_setting = GSL_CLK_GetSettings();
  // turn on HSE if it needs to be enabled
  if (new_setting.RCC_OscInitStruct.HSEState != RCC_HSE_OFF &&
      new_setting.RCC_OscInitStruct.HSEState != old_setting.RCC_OscInitStruct.HSEState) {
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = new_setting.RCC_OscInitStruct.HSEState;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    HAL_RUN(HAL_RCC_OscConfig(&RCC_OscInitStruct));
  }
  // now enable PLL if it needs enabled
  if (new_setting.RCC_OscInitStruct.PLL.PLLState == RCC_PLL_ON) {
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_NONE;
    RCC_OscInitStruct.PLL = new_setting.RCC_OscInitStruct.PLL;
    HAL_RUN(HAL_RCC_OscConfig(&RCC_OscInitStruct));
  }
  // now switch the clock configuration
  HAL_RUN(HAL_RCC_ClockConfig(&new_setting.RCC_ClkInitStruct,
                              new_setting.pFLatency));
  // set up peripheral clocks
  HAL_RUN(HAL_RCCEx_PeriphCLKConfig(&new_setting.PeriphClkInit));
  // set up oscillators
  HAL_RUN(HAL_RCC_OscConfig(&new_setting.RCC_OscInitStruct));
}

// estimate the LSI frequency based on the system clock
