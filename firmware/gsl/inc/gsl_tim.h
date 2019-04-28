#pragma once

// This file allows one to interface with the TIM peripherals.

#include "gsl_includes.h"

// convert a TIM pointer to a 0-based integer
// e.g. TIM1 -> 0
uint32_t GSL_TIM_GetNumber(TIM_TypeDef * TIMx) {
  for (uint32_t i = 0; i < GSL_TIM_PeripheralCount; ++i) {
    if (GSL_TIM_Info[i].TIMx == TIMx) {
      return i;
    }
  }
  HALT("Invalid parameter");
  return -1;
}

// return trigger connection to trigger the target timer off of the source
uint32_t GSL_TIM_GetInternalTrigger(TIM_TypeDef * target, TIM_TypeDef * source) {
  for (uint16_t i = 0; i < gsl_tim_trigger_count; ++i) {
    auto & trigger = gsl_tim_trigger[i];
    if (target == trigger.target && source == trigger.source) {
      return trigger.trigger;
    }
  }
  HALT("Invalid trigger selection");
}

// return pointer to the info struct
GSL_TIM_InfoStruct * GSL_TIM_GetInfo(TIM_TypeDef * TIMx) {
  return &GSL_TIM_Info[GSL_TIM_GetNumber(TIMx)];
}

// return the alternate function mode for the given timer
uint32_t GSL_TIM_GetAF(TIM_TypeDef * TIMx) {
  if (TIMx == TIM1) {
    return GPIO_AF1_TIM1;
  } else if (TIMx == TIM2) {
    return GPIO_AF1_TIM2;
  } else if (TIMx == TIM3) {
    return GPIO_AF2_TIM3;
  } else if (TIMx == TIM4) {
    return GPIO_AF2_TIM4;
  } else if (TIMx == TIM5) {
    return GPIO_AF2_TIM5;
#ifdef TIM8
  } else if (TIMx == TIM8) {
    return GPIO_AF3_TIM8;
#endif
#ifdef TIM9
  } else if (TIMx == TIM9) {
    return GPIO_AF3_TIM9;
#endif
#ifdef TIM10
  } else if (TIMx == TIM10) {
    return GPIO_AF3_TIM10;
#endif
#ifdef TIM11
  } else if (TIMx == TIM11) {
    return GPIO_AF3_TIM11;
#endif
#ifdef TIM12
  } else if (TIMx == TIM12) {
    return GPIO_AF9_TIM12;
#endif
#ifdef TIM13
  } else if (TIMx == TIM13) {
    return GPIO_AF9_TIM13;
#endif
#ifdef TIM14
  } else if (TIMx == TIM14) {
    return GPIO_AF9_TIM14;
#endif
  } else {
    HALT("Unexpected value");
  }
}

// for a given timer, return the timer clock speed
// TODO: fix this, it's wrong for DIV_1 clock
uint32_t GSL_TIM_GetTickFrequency(TIM_TypeDef * TIMx) {
  // get the system speed
  float system_speed = HAL_RCC_GetSysClockFreq();
  // get the appropriate AHB speed
  float ahb_speed = 0.0f;
  if (TIMx == TIM1
#ifdef TIM8
      || TIMx == TIM8
#endif
#ifdef TIM9
      || TIMx == TIM9
#endif
#ifdef TIM10
      || TIMx == TIM10
#endif
#ifdef TIM11
      || TIMx == TIM11
#endif
  ) {
    ahb_speed = HAL_RCC_GetPCLK2Freq();
  } else {
    ahb_speed = HAL_RCC_GetPCLK1Freq();
  }
  // if ahb speed is the same as the system speed, then that's the timer
  // speed.  else the timer speed is 2x the ahb speed (i think).
  if (ahb_speed == system_speed) {
    return ahb_speed;
  } else {
    return ahb_speed * 2;
  }
}

// set the frequency of a timer
// if allowed_error is nonzero, this will allow some error.  For example, 0.01
// will allow 1% error in either direction
void GSL_TIM_SetFrequency(
    TIM_TypeDef * TIMx,
    float frequency,
    float allowed_error = 0.0f) {
  ASSERT(frequency > 0.0f);
  // get the info
  GSL_TIM_InfoStruct * info = GSL_TIM_GetInfo(TIMx);
  // convert frequency to ticks
  uint32_t tick_frequency = GSL_TIM_GetTickFrequency(TIMx);
  // find the target number of ticks per cycle
  // this is target (period + 1) * (prescaler + 1) * (clock_prescaler) value
  uint32_t target_ticks = tick_frequency / frequency + 0.5f;
  // select clock division if necessary
  if (target_ticks > 0x10000 && (target_ticks % 4) == 0 && false) { // DEBUG
    info->handle->Init.ClockDivision = TIM_CLOCKDIVISION_DIV4;
    target_ticks /= 4;
  }
  // set up period and prescaler
  uint32_t period = target_ticks;
  uint32_t prescaler = 1;
  // go through each factor and adjust if we need to
  {
    uint32_t factor = 10;
    while ((period % factor) == 0 &&
           period > 0x10000 &&
           prescaler * factor <= 0x10000) {
      prescaler *= factor;
      period /= factor;
    }
  }
  for (uint32_t factor = 2; factor < 32; ++factor) {
    while ((period % factor) == 0 &&
           period > 0x10000 &&
           prescaler * factor <= 0x10000) {
      prescaler *= factor;
      period /= factor;
    }
  }
  // check for valid values
  if (period >= 0x10000 || prescaler >= 0x10000) {
    // valid values not found, find approximate ones
    if (allowed_error != 0.0f) {
      prescaler = sqrtf(target_ticks) + 0.5f;
      period = ((float) target_ticks / prescaler + 0.5f);
      float actual_ticks = (float) prescaler * period;
      float error = (actual_ticks / (float) tick_frequency) * frequency - 1.0f;
      if (error < 0) {
        error = -error;
      }
      if (!(error <= allowed_error)) {
        // values not found
        HALT("Invalid values");
      }
    } else {
      // values not found
      HALT("Invalid values");
    }
  }
  // assign values
  info->handle->Init.Period = period - 1;
  info->handle->Init.Prescaler = prescaler - 1;
}

// return the number of system timer ticks per timer update
uint64_t GSL_TIM_GetTicksPerUpdate(TIM_TypeDef * TIMx) {
  // get the info
  GSL_TIM_InfoStruct * info = GSL_TIM_GetInfo(TIMx);
  uint64_t result = ((uint64_t) info->handle->Init.Period) + 1;
  result *= ((uint64_t) info->handle->Init.Prescaler) + 1;
  switch (info->handle->Init.ClockDivision) {
    case TIM_CLOCKDIVISION_DIV4:
      result *= 4;
      break;
    case TIM_CLOCKDIVISION_DIV2:
      result *= 2;
      break;
    case TIM_CLOCKDIVISION_DIV1:
      break;
    default:
      HALT("Unexpected value");
  }
  result *= HAL_RCC_GetSysClockFreq() / GSL_TIM_GetTickFrequency(TIMx);
  return result;
}

// return the exact frequency of a timer
// details in RM90: 6.2 Clocks
float GSL_TIM_GetFrequency(TIM_TypeDef * TIMx) {
  // get the info
  GSL_TIM_InfoStruct * info = GSL_TIM_GetInfo(TIMx);
  // get the timer tick frequency
  uint32_t tick_frequency = GSL_TIM_GetTickFrequency(TIMx);
  float frequency = tick_frequency;
  // adjust for clock division
  switch (info->handle->Init.ClockDivision) {
  case TIM_CLOCKDIVISION_DIV1:
    break;
  case TIM_CLOCKDIVISION_DIV2:
    frequency /= 2;
    break;
  case TIM_CLOCKDIVISION_DIV4:
    frequency /= 4;
    break;
  default:
    HALT("Invalid parameter");
    break;
  }
  // adjust for period and prescaler
  float factor = info->handle->Init.Period + 1;
  factor *= info->handle->Init.Prescaler + 1;
  frequency /= factor;
  // return the frequency
  return frequency;
}

// initialize and start the timer
/*
void GSL_TIM_Initialize(TIM_TypeDef * TIMx) {

  __HAL_RCC_TIM1_CLK_ENABLE();

  // TODO:
  // TIM1_CH1
  //GSL_PIN_InitializeAF(kPinA8, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_AF1_TIM1);
  // TIM1_ETR
  //GSL_PIN_InitializeAF(kPinE7, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_AF1_TIM1);
  // get the info
  GSL_TIM_InfoStruct * info = GSL_TIM_GetInfo(TIMx);
  TIM_OC_InitTypeDef init;
  init.OCMode = TIM_OCMODE_PWM1;
  init.Pulse = (info->handle->Init.Period) / 2 - 7;
  init.OCPolarity = TIM_OCPOLARITY_LOW;
  init.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  init.OCFastMode = TIM_OCFAST_DISABLE;
  init.OCIdleState = TIM_OCIDLESTATE_RESET;
  init.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  // TODO:
  GSL_ReportError(HAL_TIM_PWM_Init(info->handle));

  // set it up to reset on both edges of TIM1_ETR
  {
    TIM_SlaveConfigTypeDef slave_config = {
        TIM_SLAVEMODE_RESET, // TIM_Slave_Mode
        TIM_TS_ETRF, // TIM_Trigger_Selection
        TIM_TRIGGERPOLARITY_NONINVERTED, // TIM_Trigger_Polarity
        TIM_TRIGGERPRESCALER_DIV1, // TIM_Trigger_Prescaler
        0};
    HAL_TIM_SlaveConfigSynchronization(info->handle, &slave_config);
  }

  //HAL_TIM_IC_ConfigChannel

  // clock source
  {
    TIM_ClockConfigTypeDef clock_source;
    clock_source.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    HAL_TIM_ConfigClockSource(info->handle, &clock_source);
  }

  {
    TIM_MasterConfigTypeDef config;
    config.MasterOutputTrigger = TIM_TRGO_RESET; //TIM_Master_Mode_Selection
    config.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE; //TIM_Master_Slave_Mode
    HAL_TIMEx_MasterConfigSynchronization(info->handle, &config);
  }

  // // Select the TIM4 Input Trigger: TI2FP2
   //TIM_SelectInputTrigger(TIM4, TIM_TS_TI2FP2);
  // // Select the slave Mode: Reset Mode
  // TIM_SelectSlaveMode(TIM4, TIM_SlaveMode_Reset);
  // TIM_SelectMasterSlaveMode(TIM4,TIM_MasterSlaveMode_Enable);

  GSL_ReportError(HAL_TIM_PWM_ConfigChannel(info->handle, &init, TIM_CHANNEL_1));
  GSL_ReportError(HAL_TIM_PWM_Start(info->handle, TIM_CHANNEL_1));

}*/

// if we have callbacks for the given timer, then enable interrupts
// else disable interrupts
void GSL_TIM_CheckInterrupts(TIM_TypeDef * TIMx) {
  // get the info
  GSL_TIM_InfoStruct * info = GSL_TIM_GetInfo(TIMx);
  // check if any callbacks are defined
  // clear the flag if it's set
  __HAL_TIM_CLEAR_IT(info->handle, TIM_IT_UPDATE);
  if (info->UpdateCallback) {
    /// Enable the TIM Update interrupt
    __HAL_TIM_ENABLE_IT(info->handle, TIM_IT_UPDATE);
  } else {
    __HAL_TIM_DISABLE_IT(info->handle, TIM_IT_UPDATE);
  }
}

// set up the update callback function
void GSL_TIM_SetUpdateCallback(
    TIM_TypeDef * TIMx,
    void (*callback_function)(void),
    uint8_t priority = 7) {
  // get the info
  GSL_TIM_InfoStruct * info = GSL_TIM_GetInfo(TIMx);
  // set the callback
  info->UpdateCallback = callback_function;
  // enable or disable interrupts
  GSL_TIM_CheckInterrupts(TIMx);
  // set the priority and enable the IRQ
  HAL_NVIC_SetPriority(info->irq_update, priority, 0);
  if (callback_function) {
    HAL_NVIC_EnableIRQ(info->irq_update);
  } else {
    HAL_NVIC_DisableIRQ(info->irq_update);
  }
}

// disable the clock for this timer
void GSL_TIM_DisableClock(TIM_TypeDef * TIMx) {
  if (TIMx == TIM1) {
    __HAL_RCC_TIM1_CLK_DISABLE();
  } else if (TIMx == TIM2) {
    __HAL_RCC_TIM2_CLK_DISABLE();
  } else if (TIMx == TIM3) {
    __HAL_RCC_TIM3_CLK_DISABLE();
  } else if (TIMx == TIM4) {
    __HAL_RCC_TIM4_CLK_DISABLE();
  } else if (TIMx == TIM5) {
    __HAL_RCC_TIM5_CLK_DISABLE();
#ifdef TIM6
  } else if (TIMx == TIM6) {
    __HAL_RCC_TIM6_CLK_DISABLE();
#endif
#ifdef TIM7
  } else if (TIMx == TIM7) {
    __HAL_RCC_TIM7_CLK_DISABLE();
#endif
#ifdef TIM8
  } else if (TIMx == TIM8) {
    __HAL_RCC_TIM8_CLK_DISABLE();
#endif
#ifdef TIM9
  } else if (TIMx == TIM9) {
    __HAL_RCC_TIM9_CLK_DISABLE();
#endif
#ifdef TIM10
  } else if (TIMx == TIM10) {
    __HAL_RCC_TIM10_CLK_DISABLE();
#endif
#ifdef TIM11
  } else if (TIMx == TIM11) {
    __HAL_RCC_TIM11_CLK_DISABLE();
#endif
#ifdef TIM12
  } else if (TIMx == TIM12) {
    __HAL_RCC_TIM12_CLK_DISABLE();
#endif
#ifdef TIM13
  } else if (TIMx == TIM13) {
    __HAL_RCC_TIM13_CLK_DISABLE();
#endif
#ifdef TIM14
  } else if (TIMx == TIM14) {
    __HAL_RCC_TIM14_CLK_DISABLE();
#endif
  } else {
    HALT("Unexpected value");
  }
}

// enable the clock for this timer
void GSL_TIM_EnableClock(TIM_TypeDef * TIMx) {
  if (TIMx == TIM1) {
    __HAL_RCC_TIM1_CLK_ENABLE();
  } else if (TIMx == TIM2) {
    __HAL_RCC_TIM2_CLK_ENABLE();
  } else if (TIMx == TIM3) {
    __HAL_RCC_TIM3_CLK_ENABLE();
  } else if (TIMx == TIM4) {
    __HAL_RCC_TIM4_CLK_ENABLE();
  } else if (TIMx == TIM5) {
    __HAL_RCC_TIM5_CLK_ENABLE();
#ifdef TIM6
  } else if (TIMx == TIM6) {
    __HAL_RCC_TIM6_CLK_ENABLE();
#endif
#ifdef TIM7
  } else if (TIMx == TIM7) {
    __HAL_RCC_TIM7_CLK_ENABLE();
#endif
#ifdef TIM8
  } else if (TIMx == TIM8) {
    __HAL_RCC_TIM8_CLK_ENABLE();
#endif
#ifdef TIM9
  } else if (TIMx == TIM9) {
    __HAL_RCC_TIM9_CLK_ENABLE();
#endif
#ifdef TIM10
  } else if (TIMx == TIM10) {
    __HAL_RCC_TIM10_CLK_ENABLE();
#endif
#ifdef TIM11
  } else if (TIMx == TIM11) {
    __HAL_RCC_TIM11_CLK_ENABLE();
#endif
#ifdef TIM12
  } else if (TIMx == TIM12) {
    __HAL_RCC_TIM12_CLK_ENABLE();
#endif
#ifdef TIM13
  } else if (TIMx == TIM13) {
    __HAL_RCC_TIM13_CLK_ENABLE();
#endif
#ifdef TIM14
  } else if (TIMx == TIM14) {
    __HAL_RCC_TIM14_CLK_ENABLE();
#endif
  } else {
    HALT("Unexpected value");
  }
}

// release the given pin if it's used by the given timer
void GSL_TIM_ReleasePin(TIM_TypeDef * TIMx, PinEnum pin) {
  if (GSL_PIN_IsReserved(pin) &&
      GSL_PIN_IsInAFMode(pin) &&
      GSL_PIN_GetAFMode(pin) == GSL_TIM_GetAF(TIMx)) {
    GSL_PIN_Deinitialize(pin);
  }
}

// deinitialize a timer and release all pins
void GSL_TIM_Deinitialize(TIM_TypeDef * TIMx) {
  // get the info
  GSL_TIM_InfoStruct * info = GSL_TIM_GetInfo(TIMx);
  // if already reset, do nothing
  if (info->handle->State == HAL_TIM_STATE_RESET) {
    return;
  }
  // deinitialize in HAL
  HAL_TIM_Base_DeInit(info->handle);
  // release all pins
  GSL_TIM_ReleasePin(TIMx, info->pins->ch1);
  GSL_TIM_ReleasePin(TIMx, info->pins->ch2);
  GSL_TIM_ReleasePin(TIMx, info->pins->ch3);
  GSL_TIM_ReleasePin(TIMx, info->pins->ch4);
  GSL_TIM_ReleasePin(TIMx, info->pins->etr);
  GSL_TIM_ReleasePin(TIMx, info->pins->ch1n);
  GSL_TIM_ReleasePin(TIMx, info->pins->ch2n);
  GSL_TIM_ReleasePin(TIMx, info->pins->ch3n);
}


// start the timer
void GSL_TIM_Start(TIM_TypeDef * TIMx) {
  // get the info
  GSL_TIM_InfoStruct * info = GSL_TIM_GetInfo(TIMx);
  // enable the clock
  GSL_TIM_EnableClock(TIMx);
  // initialize
  HAL_TIM_Base_Init(info->handle);
  // enable or disable interrupts
  GSL_TIM_CheckInterrupts(TIMx);
  // start the timer
  HAL_TIM_Base_Start(info->handle);
}

// deinitialize the pin if it's reserved and has the given af mode
void GSL_TIM_ReleasePin(PinEnum pin, uint32_t af_mode) {
  if (GSL_PIN_IsReal(pin) &&
      GSL_PIN_IsReserved(pin) &&
      GSL_PIN_GetAFMode(pin) == af_mode) {
    GSL_PIN_Deinitialize(pin);
  }
}

// pause the timer
void GSL_TIM_Pause(TIM_TypeDef * TIMx) {
  // get the info
  GSL_TIM_InfoStruct * info = GSL_TIM_GetInfo(TIMx);
  HAL_RUN(HAL_TIM_Base_Stop(info->handle));
}

// stop the timer
void GSL_TIM_Stop(TIM_TypeDef * TIMx) {
  // get the info
  GSL_TIM_InfoStruct * info = GSL_TIM_GetInfo(TIMx);
  // if it's already reset, we're done here
  if (info->handle->State == HAL_TIM_STATE_RESET) {
    return;
  }
  // release associated pins
  uint32_t af_mode = GSL_TIM_GetAF(TIMx);
  GSL_TIM_ReleasePin(info->pins->ch1, af_mode);
  GSL_TIM_ReleasePin(info->pins->ch2, af_mode);
  GSL_TIM_ReleasePin(info->pins->ch3, af_mode);
  GSL_TIM_ReleasePin(info->pins->ch4, af_mode);
  GSL_TIM_ReleasePin(info->pins->ch1n, af_mode);
  GSL_TIM_ReleasePin(info->pins->ch2n, af_mode);
  GSL_TIM_ReleasePin(info->pins->ch3n, af_mode);
  GSL_TIM_ReleasePin(info->pins->etr, af_mode);
  //HAL_TIM_PWM_DeInit(info->handle);

  // disable all CC outputs
  if (IS_TIM_CCX_INSTANCE(TIMx, TIM_CHANNEL_1)) {
    HAL_RUN(HAL_TIM_PWM_Stop(info->handle, TIM_CHANNEL_1));
  }
  if (IS_TIM_CCX_INSTANCE(TIMx, TIM_CHANNEL_2)) {
    HAL_RUN(HAL_TIM_PWM_Stop(info->handle, TIM_CHANNEL_2));
  }
  if (IS_TIM_CCX_INSTANCE(TIMx, TIM_CHANNEL_3)) {
    HAL_RUN(HAL_TIM_PWM_Stop(info->handle, TIM_CHANNEL_3));
  }
  if (IS_TIM_CCX_INSTANCE(TIMx, TIM_CHANNEL_4)) {
    HAL_RUN(HAL_TIM_PWM_Stop(info->handle, TIM_CHANNEL_4));
  }
  if (IS_TIM_CCXN_INSTANCE(TIMx, TIM_CHANNEL_1)) {
    HAL_RUN(HAL_TIMEx_PWMN_Stop(info->handle, TIM_CHANNEL_1));
  }
  if (IS_TIM_CCXN_INSTANCE(TIMx, TIM_CHANNEL_2)) {
    HAL_RUN(HAL_TIMEx_PWMN_Stop(info->handle, TIM_CHANNEL_2));
  }
  if (IS_TIM_CCXN_INSTANCE(TIMx, TIM_CHANNEL_3)) {
    HAL_RUN(HAL_TIMEx_PWMN_Stop(info->handle, TIM_CHANNEL_3));
  }

  // disable the clock
  HAL_RUN(HAL_TIM_Base_Stop(info->handle));

  // deinitialize
  HAL_RUN(HAL_TIM_Base_DeInit(info->handle));

  TIMx->CNT = 0;

  // DEBUG
  ASSERT((TIMx->CCER & TIM_CCER_CCxE_MASK) == 0);
  ASSERT((TIMx->CCER & TIM_CCER_CCxNE_MASK) == 0);
  ASSERT((TIMx->CR1 & TIM_CR1_CEN) == 0);

}

// get the given pin for the given channel
PinEnum GSL_TIM_GetChannelPin(TIM_TypeDef * TIMx, uint32_t channel) {
  // get the info
  GSL_TIM_InfoStruct * info = GSL_TIM_GetInfo(TIMx);
  switch (channel) {
  case TIM_CHANNEL_1:
    return info->pins->ch1;
  case TIM_CHANNEL_2:
    return info->pins->ch2;
  case TIM_CHANNEL_3:
    return info->pins->ch3;
  case TIM_CHANNEL_4:
    return info->pins->ch4;
  default:
    HALT("Unexpected value");
  }
}

// set up pwm output on the given channel with the given duty cycle
void GSL_TIM_PWM_Setup(
    TIM_TypeDef * TIMx,
    uint32_t channel,
    float duty_cycle = 0.5f,
    uint32_t OCPolarity = TIM_OCPOLARITY_HIGH,
    uint32_t OCNPolarity = TIM_OCNPOLARITY_HIGH) {
  // get the info
  GSL_TIM_InfoStruct * info = GSL_TIM_GetInfo(TIMx);
  // start the clock
  GSL_TIM_EnableClock(TIMx);
  // disable timer (I added this 6/14/2018)
  // otherwise, it would immediately be enabled
  __HAL_TIM_DISABLE(info->handle);
  // initialize
  HAL_TIM_Base_Init(info->handle);
  // initialize
  HAL_TIM_PWM_Init(info->handle);
  // set up channel
  {
    TIM_OC_InitTypeDef sConfig;
    sConfig.OCMode = TIM_OCMODE_PWM1;
    sConfig.OCPolarity = OCPolarity;
    sConfig.OCNPolarity = OCNPolarity;
    sConfig.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    sConfig.OCFastMode = TIM_OCFAST_DISABLE;
    sConfig.Pulse = (info->handle->Init.Period + 1) * duty_cycle + 0.5f;
    HAL_TIM_PWM_ConfigChannel(info->handle, &sConfig, channel);
  }
  // enable this pwm output
  TIM_CCxChannelCmd(TIMx, channel, TIM_CCx_ENABLE);
  // for channels which have it, enable the negative output
  if (IS_TIM_CCXN_INSTANCE(TIMx, channel)) {
    uint32_t tmp = 0U;

    /* Check the parameters */
    assert_param(IS_TIM_CC4_INSTANCE(TIMx));
    assert_param(IS_TIM_COMPLEMENTARY_CHANNELS(channel));

    tmp = TIM_CCER_CC1NE << channel;

    /* Reset the CCxNE Bit */
    TIMx->CCER &= ~tmp;

    /* Set or reset the CCxNE Bit */
    TIMx->CCER |= (uint32_t)(TIM_CCxN_ENABLE << channel);
  }
  // if an advanced timer, enable output
  if (TIMx == TIM1
#ifdef TIM8
      || TIMx == TIM8
#endif
      ) {
    __HAL_TIM_MOE_ENABLE(info->handle);
  }
  // initialize output pin
  GSL_PIN_InitializeAF(
      GSL_TIM_GetChannelPin(TIMx, channel),
      GPIO_MODE_AF_PP,
      GPIO_NOPULL,
      GSL_TIM_GetAF(TIMx));
}

// in PWM toggle mode, the duty cycle is 50% and the frequency is half of the
// original frequency.
// channels may be phase shifted (0 = no shift, 0.5 = 180 degree shift).
void GSL_TIM_PWM_SetupToggle(
    TIM_TypeDef * TIMx,
    uint32_t channel,
    float phase = 0.0f) {
  // get the info
  GSL_TIM_InfoStruct * info = GSL_TIM_GetInfo(TIMx);
  // start the clock
  GSL_TIM_EnableClock(TIMx);
  // initialize
  HAL_TIM_Base_Init(info->handle);
  // initialize
  HAL_TIM_PWM_Init(info->handle);
  // set up output pin
  GSL_PIN_InitializeAF(
      GSL_TIM_GetChannelPin(TIMx, channel),
      GPIO_MODE_AF_PP,
      GPIO_NOPULL,
      GSL_TIM_GetAF(TIMx));
  // set up channel
  {
    TIM_OC_InitTypeDef sConfig;
    sConfig.OCMode = TIM_OCMODE_TOGGLE;
    sConfig.OCPolarity = TIM_OCPOLARITY_LOW;
    if (phase >= 0.5f) {
      phase -= 0.5f;
      sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
    }
    sConfig.OCNPolarity = TIM_OCNPOLARITY_LOW;
    sConfig.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    sConfig.OCFastMode = TIM_OCFAST_DISABLE;
    sConfig.Pulse = (info->handle->Init.Period + 1) * phase * 2.0f + 0.5f;
    if (sConfig.Pulse >= info->handle->Init.Period) {
      sConfig.Pulse = info->handle->Init.Period;
      if (sConfig.Pulse) {
        --sConfig.Pulse;
      }
    }
    HAL_TIM_OC_ConfigChannel(info->handle, &sConfig, channel);
  }
  // enable this pwm output
  TIM_CCxChannelCmd(TIMx, channel, TIM_CCx_ENABLE);
  // if an advanced timer, enable output
  if (TIMx == TIM1
#ifdef TIM8
      || TIMx == TIM8
#endif
      ) {
    __HAL_TIM_MOE_ENABLE(info->handle);
  }
}

// set up the timer to send enable as trigger output
void GSL_TIM_EnableTriggerOutput(TIM_TypeDef * TIMx) {
  // get the info
  GSL_TIM_InfoStruct * info = GSL_TIM_GetInfo(TIMx);
  // start clock
  GSL_TIM_EnableClock(TIMx);
  // initialize
  HAL_TIM_Base_Init(info->handle);
  // send enable as trigger output
  TIM_MasterConfigTypeDef config;
  config.MasterOutputTrigger = TIM_TRGO_ENABLE; //TIM_Master_Mode_Selection
  config.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE; //TIM_Master_Slave_Mode
  HAL_TIMEx_MasterConfigSynchronization(info->handle, &config);
}

// set up the timer to send trigger on update
void GSL_TIM_EnableTriggerUpdate(TIM_TypeDef * TIMx) {
  // get the info
  GSL_TIM_InfoStruct * info = GSL_TIM_GetInfo(TIMx);
  // start clock
  GSL_TIM_EnableClock(TIMx);
  // initialize
  HAL_TIM_Base_Init(info->handle);
  // send enable as trigger output
  TIM_MasterConfigTypeDef config;
  config.MasterOutputTrigger = TIM_TRGO_UPDATE; //TIM_Master_Mode_Selection
  config.MasterSlaveMode = TIM_MASTERSLAVEMODE_ENABLE; //TIM_Master_Slave_Mode
  HAL_TIMEx_MasterConfigSynchronization(info->handle, &config);
}

// set up the timer to start on the given trigger
// e.g. GSL_TIM_WaitForTrigger(TIM2, TIM_TS_ITR0);
void GSL_TIM_WaitForTrigger(TIM_TypeDef * TIMx, uint32_t trigger) {
  // get the info
  GSL_TIM_InfoStruct * info = GSL_TIM_GetInfo(TIMx);
  // start clock
  GSL_TIM_EnableClock(TIMx);
  // initialize
  HAL_TIM_Base_Init(info->handle);
  // if we're triggering from ETR, then initialize that pin
  if (trigger == TIM_TS_ETRF) {
    GSL_PIN_InitializeAF(
        info->pins->etr,
        GPIO_MODE_AF_PP,
        GPIO_PULLDOWN,
        GSL_TIM_GetAF(TIMx));
  }
  {
    TIM_SlaveConfigTypeDef slave_config = {
        TIM_SLAVEMODE_TRIGGER, // TIM_Slave_Mode
        trigger, // TIM_Trigger_Selection
        TIM_TRIGGERPOLARITY_NONINVERTED, // TIM_Trigger_Polarity
        TIM_TRIGGERPRESCALER_DIV1, // TIM_Trigger_Prescaler
        0};
    HAL_TIM_SlaveConfigSynchronization(info->handle, &slave_config);
  }
}

// TIM IRQ handlers
extern "C" {

void TIM1_BRK_TIM9_IRQHandler(void) {
#ifdef GSL_LOG_IRQ_ONCE
  LOG_ONCE("\nIn TIM1_BRK_TIM9_IRQHandler");
#endif
  HAL_TIM_IRQHandler(&gsl_tim1_htim);
  HAL_TIM_IRQHandler(&gsl_tim9_htim);
}
#ifndef GSL_CUSTOMIRQHANDLER_TIM1_UP_TIM10
void TIM1_UP_TIM10_IRQHandler(void) {
#ifdef GSL_LOG_IRQ_ONCE
  LOG_ONCE("\nIn TIM1_UP_TIM10_IRQHandler");
#endif
  HAL_TIM_IRQHandler(&gsl_tim1_htim);
  HAL_TIM_IRQHandler(&gsl_tim10_htim);
}
#endif
void TIM1_TRG_COM_TIM11_IRQHandler(void) {
#ifdef GSL_LOG_IRQ_ONCE
  LOG_ONCE("\nIn TIM1_TRG_COM_TIM11_IRQHandler");
#endif
  HAL_TIM_IRQHandler(&gsl_tim1_htim);
  HAL_TIM_IRQHandler(&gsl_tim11_htim);
}
void TIM1_CC_IRQHandler(void) {
#ifdef GSL_LOG_IRQ_ONCE
  LOG_ONCE("\nIn TIM1_CC_IRQHandler");
#endif
  HAL_TIM_IRQHandler(&gsl_tim1_htim);
}

#ifndef GSL_CUSTOMIRQHANDLER_TIM2
void TIM2_IRQHandler(void) {
#ifdef GSL_LOG_IRQ_ONCE
  LOG_ONCE("\nIn TIM2_IRQHandler");
#endif
  HAL_TIM_IRQHandler(&gsl_tim2_htim);
}
#endif

void TIM3_IRQHandler(void) {
#ifdef GSL_LOG_IRQ_ONCE
  LOG_ONCE("\nIn TIM3_IRQHandler");
#endif
  HAL_TIM_IRQHandler(&gsl_tim3_htim);
}

void TIM4_IRQHandler(void) {
#ifdef GSL_LOG_IRQ_ONCE
  LOG_ONCE("\nIn TIM4_IRQHandler");
#endif
  HAL_TIM_IRQHandler(&gsl_tim4_htim);
}

void TIM5_IRQHandler(void) {
#ifdef GSL_LOG_IRQ_ONCE
  LOG_ONCE("\nIn TIM5_IRQHandler");
#endif
  HAL_TIM_IRQHandler(&gsl_tim5_htim);
}

#ifdef TIM6
void TIM6_DAC_IRQHandler(void) {
#ifdef GSL_LOG_IRQ_ONCE
  LOG_ONCE("\nIn TIM6_DAC_IRQHandler");
#endif
  HAL_TIM_IRQHandler(&gsl_tim6_htim);
  // TODO: handle DAC underrun error here
}
#endif

#ifdef TIM7
void TIM7_IRQHandler(void) {
#ifdef GSL_LOG_IRQ_ONCE
  LOG_ONCE("\nIn TIM7_IRQHandler");
#endif
  HAL_TIM_IRQHandler(&gsl_tim7_htim);
}
#endif

#ifdef TIM8
void TIM8_BRK_TIM12_IRQHandler(void) {
#ifdef GSL_LOG_IRQ_ONCE
  LOG_ONCE("\nIn TIM8_BRK_TIM12_IRQHandler");
#endif
  HAL_TIM_IRQHandler(&gsl_tim8_htim);
  HAL_TIM_IRQHandler(&gsl_tim12_htim);
}
#endif
#ifdef TIM8
void TIM8_UP_TIM13_IRQHandler(void) {
#ifdef GSL_LOG_IRQ_ONCE
  LOG_ONCE("\nIn TIM8_UP_TIM13_IRQHandler");
#endif
  HAL_TIM_IRQHandler(&gsl_tim8_htim);
  HAL_TIM_IRQHandler(&gsl_tim13_htim);
}
#endif
#ifdef TIM8
void TIM8_TRG_COM_TIM14_IRQHandler(void) {
#ifdef GSL_LOG_IRQ_ONCE
  LOG_ONCE("\nIn TIM8_TRG_COM_TIM14_IRQHandler");
#endif
  HAL_TIM_IRQHandler(&gsl_tim8_htim);
  HAL_TIM_IRQHandler(&gsl_tim14_htim);
}
#endif
#ifdef TIM8
void TIM8_CC_IRQHandler(void) {
#ifdef GSL_LOG_IRQ_ONCE
  LOG_ONCE("\nIn TIM8_CC_IRQHandler");
#endif
  HAL_TIM_IRQHandler(&gsl_tim8_htim);
}
#endif

// update callback
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  // get the info
  GSL_TIM_InfoStruct * info = GSL_TIM_GetInfo(htim->Instance);
  if (info->UpdateCallback) {
    info->UpdateCallback();
  }
}

} // extern "C"

// log the state of each timer
// TIM1: ready, X MHz ticks, period=, X MHz update
void GSL_TIM_LogDescription(void) {
  LOG("\n\nTimers description:");
  LOG("\n- Total timers defined: ", GSL_TIM_PeripheralCount);
  for (uint16_t i = 0; i < GSL_TIM_PeripheralCount; ++i) {
    // get the info
    GSL_TIM_InfoStruct * info = &GSL_TIM_Info[i];
    // if disabled, don't output anything
    if (info->handle->State == HAL_TIM_STATE_RESET) {
      continue;
    }
    LOG("\n- TIM", i + 1, ": ");
    // output state
    switch (info->handle->State) {
    case HAL_TIM_STATE_RESET:
      LOG("disabled");
      break;
    case HAL_TIM_STATE_READY:
      LOG("ready");
      break;
    case HAL_TIM_STATE_BUSY:
      LOG("busy");
      break;
    case HAL_TIM_STATE_TIMEOUT:
      LOG("timeout");
      break;
    case HAL_TIM_STATE_ERROR:
      LOG("error");
      break;
    default:
      LOG("Unknown");
      break;
    }
    LOG(", ",
        (uint32_t) (GSL_TIM_GetTickFrequency(info->handle->Instance) + 0.5f),
        " Hz ticks");
    LOG(", ", info->handle->Init.Period + 1, " tick period");
    LOG(", ",
        (uint32_t) (GSL_TIM_GetFrequency(info->handle->Instance) + 0.5f),
        " Hz update");
  }
}

// forward declaration
uint32_t GSL_DEL_Ticks(void);

// some profiling stuff
void GSL_TIM_Profile(void) {
  // profile GSL_TIM_GetNumber
  uint32_t ticks = 0 - GSL_DEL_Ticks();
  for (uint16_t i = 0; i < GSL_TIM_PeripheralCount; ++i) {
    GSL_TIM_GetNumber(GSL_TIM_Info[i].TIMx);
  }
  ticks += GSL_DEL_Ticks();
  float average = (float) ticks / GSL_TIM_PeripheralCount;
  LOG("\nGSL_TIM_GetNumber: ", GSL_OUT_FixedFloat(average, 1));
  LOG(" ticks average for ", GSL_TIM_PeripheralCount, " timers");
}

// set up the timer to call the given function after a delay
void GSL_TIM_FutureCallback(TIM_TypeDef * TIMx,
                            void (*callback)(void),
                            float delay_s,
                            uint8_t priority = 7) {
  // set frequency (allow 5% error)
  GSL_TIM_SetFrequency(TIMx, 1.0f / delay_s, 0.05f);
  // enable update trigger
  GSL_TIM_SetUpdateCallback(TIMx, callback, priority);
  GSL_TIM_EnableTriggerUpdate(TIMx);
  // set one-shot mode
  TIMx->CR1 &= ~TIM_CR1_OPM;
  TIMx->CR1 |= TIM_OPMODE_SINGLE;
  // start it
  GSL_TIM_Start(TIMx);
}

// reserve a timer
void GSL_TIM_Reserve(TIM_TypeDef * TIMx) {
  auto info = GSL_TIM_GetInfo(TIMx);
  if (info->locked) {
    HALT("\nTimer TIM", GSL_TIM_GetNumber(TIMx) + 1, " already reserved.")
  }
  info->locked = true;
}

// unreserve a timer
void GSL_TIM_Unreserve(TIM_TypeDef * TIMx) {
  auto info = GSL_TIM_GetInfo(TIMx);
  info->locked = false;
}

// return the DMA ID for the given timer capture compare channel
uint16_t GSL_TIM_GetDMACCID(uint32_t channel) {
  if (channel == TIM_CHANNEL_1) {
    return TIM_DMA_ID_CC1;
  } else if (channel == TIM_CHANNEL_2) {
    return TIM_DMA_ID_CC2;
  } else if (channel == TIM_CHANNEL_3) {
    return TIM_DMA_ID_CC3;
  } else if (channel == TIM_CHANNEL_4) {
    return TIM_DMA_ID_CC4;
  }
  HALT("Unexpected value")
}

// output a timer register
void GSL_TIM_OutputRegister(const char * name, uint32_t value, uint32_t mask, uint32_t reset_value) {
  ASSERT_NE(mask, 0u);
  // if value is same as reset value, just return
  if ((value & mask) == (reset_value & mask)) {
    return;
  }
  // else output the information
  LOG(" ", name, "=");
  // get number of bits and offset of mask
  uint8_t bit_offset = 0;
  uint8_t bit_count = 0;
  {
    uint32_t temp_mask = mask;
    while ((temp_mask & 1) == 0) {
      temp_mask /= 2;
      ++bit_offset;
    }
    while ((temp_mask & 1) == 1) {
      temp_mask /= 2;
      ++bit_count;
    }
    ASSERT_EQ(temp_mask, 0u);
  }
  // output register value
  // output in binary unless 16 or 32 bits, in which case we output in decimal
  if (bit_count == 16 || bit_count == 32) {
    LOG((value & mask) >> bit_offset);
  } else {
    for (uint16_t i = 0; i < bit_count; ++i) {
      bool bit_set = value & (1 << (bit_offset + bit_count - 1 - i));
      LOG((bit_set) ? "1" : "0");
    }
  }
}

#ifndef STRINGIFY
#define STRINGIFY(x) #x
#endif
#ifndef TOSTRING
#define TOSTRING(x) STRINGIFY(x)
#endif

// helper macro for outputting register values
#define GSL_TIM_OUTPUT_REGISTER_VALUE(reg, name) \
    GSL_TIM_OutputRegister(TOSTRING(name), TIMx->reg, TIM_##reg##_##name##_Msk, reset_value);


// output detailed information on the state of each timer
/*
TIM1: clock enabled
  * CR1: CKD=00, ARPE=1, CMS=00, DIR=0, OPM=0, URS=0, UDIS0, CEN=1
  * CR2:
  * SMCR:
  * DIER:
  * SR:
  * EGR:
  * CCMR1:
  * CCMR2:
  * CCER:
  *
 */
void GSL_TIM_LogDetailedInformation(void) {
  LOG("\n\nTimers detailed description:");
  LOG("\n- Total timers defined: ", GSL_TIM_PeripheralCount);
  for (uint16_t i = 0; i < GSL_TIM_PeripheralCount; ++i) {
    // get the timer
    auto & info = GSL_TIM_Info[i];
    TIM_TypeDef * TIMx = info.TIMx;
    bool enabled = READ_BIT(*info.ahb_en_reg, info.ahb_en_mask);
    // see if the clock is enabled
    if (!enabled) {
      continue;
    }
    LOG("\nTIM", i + 1, ", clock enabled");
    // CR register
    uint32_t reset_value = 0;
    if (TIMx->CR1 != reset_value) {
      LOG("\n- CR1:");
      GSL_TIM_OUTPUT_REGISTER_VALUE(CR1, CKD);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CR1, ARPE);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CR1, CMS);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CR1, DIR);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CR1, OPM);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CR1, URS);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CR1, UDIS);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CR1, CEN);
    }
    if (TIMx->CR2 != reset_value) {
      LOG("\n- CR2:");
      GSL_TIM_OUTPUT_REGISTER_VALUE(CR2, TI1S);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CR2, MMS);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CR2, CCDS);
    }
    if (TIMx->SMCR != reset_value) {
      LOG("\n- SCMR:");
      GSL_TIM_OUTPUT_REGISTER_VALUE(SMCR, ETP);
      GSL_TIM_OUTPUT_REGISTER_VALUE(SMCR, ECE);
      GSL_TIM_OUTPUT_REGISTER_VALUE(SMCR, ETPS);
      GSL_TIM_OUTPUT_REGISTER_VALUE(SMCR, ETF);
      GSL_TIM_OUTPUT_REGISTER_VALUE(SMCR, MSM);
      GSL_TIM_OUTPUT_REGISTER_VALUE(SMCR, TS);
      GSL_TIM_OUTPUT_REGISTER_VALUE(SMCR, SMS);
    }
    if (TIMx->DIER != reset_value) {
      LOG("\n- DIER:");
      GSL_TIM_OUTPUT_REGISTER_VALUE(DIER, TDE);
      GSL_TIM_OUTPUT_REGISTER_VALUE(DIER, CC4DE);
      GSL_TIM_OUTPUT_REGISTER_VALUE(DIER, CC3DE);
      GSL_TIM_OUTPUT_REGISTER_VALUE(DIER, CC2DE);
      GSL_TIM_OUTPUT_REGISTER_VALUE(DIER, CC1DE);
      GSL_TIM_OUTPUT_REGISTER_VALUE(DIER, UDE);
      GSL_TIM_OUTPUT_REGISTER_VALUE(DIER, TIE);
      GSL_TIM_OUTPUT_REGISTER_VALUE(DIER, CC4IE);
      GSL_TIM_OUTPUT_REGISTER_VALUE(DIER, CC3IE);
      GSL_TIM_OUTPUT_REGISTER_VALUE(DIER, CC2IE);
      GSL_TIM_OUTPUT_REGISTER_VALUE(DIER, CC1IE);
      GSL_TIM_OUTPUT_REGISTER_VALUE(DIER, UIE);
    }
    if (TIMx->SR != reset_value) {
      LOG("\n- SR:");
      GSL_TIM_OUTPUT_REGISTER_VALUE(SR, CC4OF);
      GSL_TIM_OUTPUT_REGISTER_VALUE(SR, CC3OF);
      GSL_TIM_OUTPUT_REGISTER_VALUE(SR, CC2OF);
      GSL_TIM_OUTPUT_REGISTER_VALUE(SR, CC1OF);
      GSL_TIM_OUTPUT_REGISTER_VALUE(SR, TIF);
      GSL_TIM_OUTPUT_REGISTER_VALUE(SR, CC4IF);
      GSL_TIM_OUTPUT_REGISTER_VALUE(SR, CC3IF);
      GSL_TIM_OUTPUT_REGISTER_VALUE(SR, CC2IF);
      GSL_TIM_OUTPUT_REGISTER_VALUE(SR, CC1IF);
      GSL_TIM_OUTPUT_REGISTER_VALUE(SR, UIF);
    }
    if (TIMx->EGR != reset_value) {
      LOG("\n- EGR:");
      GSL_TIM_OUTPUT_REGISTER_VALUE(EGR, TG);
      GSL_TIM_OUTPUT_REGISTER_VALUE(EGR, CC4G);
      GSL_TIM_OUTPUT_REGISTER_VALUE(EGR, CC3G);
      GSL_TIM_OUTPUT_REGISTER_VALUE(EGR, CC2G);
      GSL_TIM_OUTPUT_REGISTER_VALUE(EGR, CC1G);
      GSL_TIM_OUTPUT_REGISTER_VALUE(EGR, UG);
    }
    if (TIMx->CCMR1 != reset_value) {
      LOG("\n- CCMR1:");
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCMR1, OC2CE);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCMR1, OC2M);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCMR1, OC2PE);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCMR1, OC2FE);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCMR1, IC2F);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCMR1, IC2PSC);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCMR1, CC2S);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCMR1, OC1CE);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCMR1, OC1M);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCMR1, OC1PE);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCMR1, OC1FE);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCMR1, IC1F);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCMR1, IC1PSC);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCMR1, CC1S);
    }
    if (TIMx->CCMR2 != reset_value) {
      LOG("\n- CCMR2:");
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCMR2, OC4CE);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCMR2, OC4M);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCMR2, OC4PE);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCMR2, OC4FE);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCMR2, IC4F);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCMR2, IC4PSC);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCMR2, CC4S);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCMR2, OC3CE);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCMR2, OC3M);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCMR2, OC3PE);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCMR2, OC3FE);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCMR2, IC3F);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCMR2, IC3PSC);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCMR2, CC3S);
    }
    if (TIMx->CCER != reset_value) {
      LOG("\n- CCER:");
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCER, CC4NP);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCER, CC4P);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCER, CC4E);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCER, CC3NP);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCER, CC3P);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCER, CC3E);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCER, CC2NP);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCER, CC2P);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCER, CC2E);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCER, CC1NP);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCER, CC1P);
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCER, CC1E);
    }
    if (TIMx->CNT != reset_value) {
      LOG("\n- CNT:");
      GSL_TIM_OUTPUT_REGISTER_VALUE(CNT, CNT);
    }
    if (TIMx->PSC != reset_value) {
      LOG("\n- PSC:");
      GSL_TIM_OUTPUT_REGISTER_VALUE(PSC, PSC);
    }
    if (TIMx->ARR != reset_value) {
      LOG("\n- ARR:");
      GSL_TIM_OUTPUT_REGISTER_VALUE(ARR, ARR);
    }
    if (TIMx->CCR1 != reset_value) {
      LOG("\n- CCR1:");
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCR1, CCR1);
    }
    if (TIMx->CCR2 != reset_value) {
      LOG("\n- CCR2:");
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCR2, CCR2);
    }
    if (TIMx->CCR3 != reset_value) {
      LOG("\n- CCR3:");
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCR3, CCR3);
    }
    if (TIMx->CCR4 != reset_value) {
      LOG("\n- CCR4:");
      GSL_TIM_OUTPUT_REGISTER_VALUE(CCR4, CCR4);
    }
    if (TIMx->DCR != reset_value) {
      LOG("\n- DCR:");
      GSL_TIM_OUTPUT_REGISTER_VALUE(DCR, DBL);
      GSL_TIM_OUTPUT_REGISTER_VALUE(DCR, DBA);
    }
  }
}
