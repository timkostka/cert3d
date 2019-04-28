#pragma once

// This file provides an interface for external interrupts.

#include "gsl_includes.h"

struct GSL_EXTI_InfoStruct {
  // true when initialized
  bool active = false;
  // pin being monitored
  PinEnum pin;
  // states being flagged
  uint32_t mode;
  // function to call when event is triggered
  void (*callback_function)(void);
};

// hold all 16 possible interrupts
GSL_EXTI_InfoStruct gsl_exti_info[16];

// return the interrupt corresponding to the given pin
IRQn_Type GSL_EXTI_GetIRQ(PinEnum pin) {
  switch (GSL_PIN_GetPin(pin)) {
  case GPIO_PIN_0:
    return EXTI0_IRQn;
  case GPIO_PIN_1:
    return EXTI1_IRQn;
  case GPIO_PIN_2:
    return EXTI2_IRQn;
  case GPIO_PIN_3:
    return EXTI3_IRQn;
  case GPIO_PIN_4:
    return EXTI4_IRQn;
  case GPIO_PIN_5:
  case GPIO_PIN_6:
  case GPIO_PIN_7:
  case GPIO_PIN_8:
  case GPIO_PIN_9:
    return EXTI9_5_IRQn;
  case GPIO_PIN_10:
  case GPIO_PIN_11:
  case GPIO_PIN_12:
  case GPIO_PIN_13:
  case GPIO_PIN_14:
  case GPIO_PIN_15:
    return EXTI15_10_IRQn;
  default:
    HALT("ERROR");
  }
}

// attach a new interrupt
// e.g. GSL_EXTI_Attach(kPinA0, GPIO_MODE_IT_RISING, FunctionToCall);
void GSL_EXTI_Attach(
    PinEnum pin,
    uint32_t mode,
    void (*callback_function) (void),
    uint8_t priority = 8) {
  // get this exti info struct
  GSL_EXTI_InfoStruct * info = &gsl_exti_info[GSL_PIN_GetPinNumber(pin)];
  // if already active, we have a problem
  if (info->active) {
    // for a given pin number, cannot have two ports active at once
    // e.g. can't have PA0 and PB0 both be active
    HALT("Interrupt overload");
  }
  // set up the structure
  info->pin = pin;
  info->mode = mode;
  info->callback_function = callback_function;
  // initialize it
  GSL_PIN_Initialize(info->pin, info->mode, GPIO_NOPULL);
  // enable the interrupt
  IRQn_Type irq;
  irq = GSL_EXTI_GetIRQ(info->pin);
  info->active = true;
  HAL_NVIC_SetPriority(irq, priority, 0);
  HAL_NVIC_EnableIRQ(irq);
}

// detach the new interrupt
// e.g. GSL_EXTI_Attach(kPinA0, GPIO_MODE_IT_RISING, FunctionToCall);
void GSL_EXTI_Detach(
    PinEnum pin) {
  // get this exti info struct
  GSL_EXTI_InfoStruct * info = &gsl_exti_info[GSL_PIN_GetPinNumber(pin)];
  // if not already active, we're done
  if (!info->active) {
    return;
  }
  // deinitialize the pin
  GSL_PIN_Deinitialize(info->pin);
  // disable the interrupt
  IRQn_Type irq;
  irq = GSL_EXTI_GetIRQ(info->pin);
  HAL_NVIC_DisableIRQ(irq);
  info->active = false;
}

extern "C" {

// this gets called from HAL_GPIO_EXTI_IRQHandler() as appropriate
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  // call the appropriate routine
  uint16_t pin_number = GSL_PIN_PinToNumber(GPIO_Pin);
  if (!gsl_exti_info[pin_number].active) {
    LOG("Unexpected interrupt");
  } else {
    gsl_exti_info[pin_number].callback_function();
  }
}

void EXTI0_IRQHandler(void) {
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}
void EXTI1_IRQHandler(void) {
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}
void EXTI2_IRQHandler(void) {
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
}
void EXTI3_IRQHandler(void) {
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
}
void EXTI4_IRQHandler(void) {
  HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
}
void EXTI9_5_IRQHandler(void) {
  if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_5) != RESET) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_5);
  }
  if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_6) != RESET) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_6);
  }
  if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_7) != RESET) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_7);
  }
  if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_8) != RESET) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_8);
  }
  if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_9) != RESET) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_9);
  }
}
void EXTI15_10_IRQHandler(void) {
  if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_10) != RESET) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_10);
  }
  if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_11) != RESET) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_11);
  }
  if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_12) != RESET) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_12);
  }
  if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_13) != RESET) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
  }
  if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_14) != RESET) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_14);
  }
  if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_15) != RESET) {
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_15);
  }
}

}
