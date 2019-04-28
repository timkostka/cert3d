#pragma once

#include "gsl_includes.h"

// lookup table for getting the GPIO port from a PinEnum
constexpr GPIO_TypeDef * const GSL_PIN_PortLookup[] = {
    nullptr,
    nullptr,
    GPIOA, GPIOA, GPIOA, GPIOA, GPIOA, GPIOA, GPIOA, GPIOA,
    GPIOA, GPIOA, GPIOA, GPIOA, GPIOA, GPIOA, GPIOA, GPIOA,
    GPIOB, GPIOB, GPIOB, GPIOB, GPIOB, GPIOB, GPIOB, GPIOB,
    GPIOB, GPIOB, GPIOB, GPIOB, GPIOB, GPIOB, GPIOB, GPIOB,
    GPIOC, GPIOC, GPIOC, GPIOC, GPIOC, GPIOC, GPIOC, GPIOC,
    GPIOC, GPIOC, GPIOC, GPIOC, GPIOC, GPIOC, GPIOC, GPIOC,
#ifdef GPIOD
    GPIOD, GPIOD, GPIOD, GPIOD, GPIOD, GPIOD, GPIOD, GPIOD,
    GPIOD, GPIOD, GPIOD, GPIOD, GPIOD, GPIOD, GPIOD, GPIOD,
#endif
#ifdef GPIOE
    GPIOE, GPIOE, GPIOE, GPIOE, GPIOE, GPIOE, GPIOE, GPIOE,
    GPIOE, GPIOE, GPIOE, GPIOE, GPIOE, GPIOE, GPIOE, GPIOE,
#endif
#ifdef GPIOF
    GPIOF, GPIOF, GPIOF, GPIOF, GPIOF, GPIOF, GPIOF, GPIOF,
    GPIOF, GPIOF, GPIOF, GPIOF, GPIOF, GPIOF, GPIOF, GPIOF,
#endif
#ifdef GPIOG
    GPIOG, GPIOG, GPIOG, GPIOG, GPIOG, GPIOG, GPIOG, GPIOG,
    GPIOG, GPIOG, GPIOG, GPIOG, GPIOG, GPIOG, GPIOG, GPIOG,
#endif
#ifdef GPIOH
    GPIOH, GPIOH, GPIOH, GPIOH, GPIOH, GPIOH, GPIOH, GPIOH,
    GPIOH, GPIOH, GPIOH, GPIOH, GPIOH, GPIOH, GPIOH, GPIOH,
#endif
#ifdef GPIOI
    GPIOI, GPIOI, GPIOI, GPIOI, GPIOI, GPIOI, GPIOI, GPIOI,
    GPIOI, GPIOI, GPIOI, GPIOI, GPIOI, GPIOI, GPIOI, GPIOI,
#endif
#ifdef GPIOJ
    GPIOJ, GPIOJ, GPIOJ, GPIOJ, GPIOJ, GPIOJ, GPIOJ, GPIOJ,
    GPIOJ, GPIOJ, GPIOJ, GPIOJ, GPIOJ, GPIOJ, GPIOJ, GPIOJ,
#endif
#ifdef GPIOK
    GPIOK, GPIOK, GPIOK, GPIOK, GPIOK, GPIOK, GPIOK, GPIOK,
    GPIOK, GPIOK, GPIOK, GPIOK, GPIOK, GPIOK, GPIOK, GPIOK,
#endif
    nullptr
    };

// lookup table for getting the pin port from a PinEnum
constexpr uint16_t const GSL_PIN_PinLookup[] = {
    0,
    0,
    GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3,
    GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7,
    GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_11,
    GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15,
    GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3,
    GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7,
    GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_11,
    GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15,
    GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3,
    GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7,
    GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_11,
    GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15,
#ifdef GPIOD
    GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3,
    GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7,
    GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_11,
    GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15,
#endif
#ifdef GPIOE
    GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3,
    GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7,
    GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_11,
    GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15,
#endif
#ifdef GPIOF
    GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3,
    GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7,
    GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_11,
    GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15,
#endif
#ifdef GPIOG
    GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3,
    GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7,
    GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_11,
    GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15,
#endif
#ifdef GPIOH
    GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3,
    GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7,
    GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_11,
    GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15,
#endif
#ifdef GPIOI
    GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3,
    GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7,
    GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_11,
    GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15,
#endif
#ifdef GPIOJ
    GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3,
    GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7,
    GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_11,
    GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15,
#endif
#ifdef GPIOK
    GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3,
    GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7,
    GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_11,
    GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15,
#endif
    0};

// reservation pins
uint8_t GSL_PIN_Reserved[(kPinFinal + 7) / 8] = {0};

// return the GPIO port corresponding to the given pin
constexpr GPIO_TypeDef * GSL_PIN_GetPort(PinEnum pin) {
  return GSL_PIN_PortLookup[pin];
}

// return the GPIO pin number corresponding to the given pin
constexpr uint16_t GSL_PIN_GetPin(const PinEnum pin) {
  return GSL_PIN_PinLookup[pin];
}

// convert a GPIO_PIN_X to a 0-based integer value
// e.g. GPIO_PIN_7 -> 7
uint16_t GSL_PIN_PinToNumber(uint16_t mask) {
  uint16_t result = 0;
  while (mask /= 2) {
    ++result;
  }
  return result;
}

// return the number of the given pin
// e.g. kPin13 -> 13
uint16_t GSL_PIN_GetPinNumber(PinEnum pin) {
  return GSL_PIN_PinToNumber(GSL_PIN_GetPin(pin));
}

// list of all GPIO ports
GPIO_TypeDef * const gsl_pin_port[] = {
    GPIOA,
    GPIOB,
    GPIOC,
    GPIOD,
    GPIOE,
#ifdef GPIOF
    GPIOF,
#endif
#ifdef GPIOG
    GPIOG,
#endif
#ifdef GPIOH
    GPIOH,
#endif
#ifdef GPIOI
    GPIOI,
#endif
#ifdef GPIOJ
    GPIOJ,
#endif
#ifdef GPIOK
    GPIOK,
#endif
    nullptr,
};

// return the number of the given port
// e.g. GPIOA -> 0, GPIOB -> 1, etc.
uint16_t GSL_PIN_GetPortNumber(const PinEnum pin) {
  const GPIO_TypeDef * port = GSL_PIN_PortLookup[pin];
  uint16_t i = 0;
  while (gsl_pin_port[i] != port &&
      gsl_pin_port[i] != nullptr) {
    ++i;
  }
  if (gsl_pin_port[i] == nullptr) {
    return -1;
  } else {
    return i;
  }
}

// return the letter of the given pin's port
// e.g. GPIOA -> 'A'
char GSL_PIN_GetPortLetter(PinEnum pin) {
  GPIO_TypeDef * port = GSL_PIN_GetPort(pin);
  if (port == GPIOA) {return 'A';}
  else if (port == GPIOB) {return 'B';}
  else if (port == GPIOC) {return 'C';}
#ifdef GPIOD
  else if (port == GPIOD) {return 'D';}
#endif
#ifdef GPIOE
  else if (port == GPIOE) {return 'E';}
#endif
#ifdef GPIOF
  else if (port == GPIOF) {return 'F';}
#endif
#ifdef GPIOG
  else if (port == GPIOG) {return 'G';}
#endif
#ifdef GPIOH
  else if (port == GPIOH) {return 'H';}
#endif
#ifdef GPIOI
  else if (port == GPIOI) {return 'I';}
#endif
#ifdef GPIOJ
  else if (port == GPIOJ) {return 'J';}
#endif
#ifdef GPIOK
  else if (port == GPIOK) {return 'K';}
#endif
  else {while (1);}
}

// return a string for the given pin
// e.g. kPinB7 -> "PB7"
char * GSL_PIN_ToString(PinEnum pin) {
  static char buffer[5];
  uint16_t pos = 0;
  buffer[pos++] = 'P';
  buffer[pos++] = GSL_PIN_GetPortLetter(pin);
  uint16_t pin_number = GSL_PIN_GetPinNumber(pin);
  if (pin_number >= 10) {
    buffer[pos++] = '0' + (pin_number / 10);
    pin_number %= 10;
  }
  buffer[pos++] = '0' + pin_number;
  buffer[pos] = '\0';
  return buffer;
}

// return true if the specified pin is a physical pin
// i.e. not kPinNone or kPinInvalid or kPinFinal
bool GSL_PIN_IsReal(PinEnum pin) {
  return pin != kPinNone && pin != kPinInvalid && pin != kPinFinal;
}

// return true if a pin is reserved
bool GSL_PIN_IsReserved(PinEnum pin) {
  if (!GSL_PIN_IsReal(pin)) {
    return false;
  }
  // store the location and bitmask
  uint8_t bitmask = 1 << (pin % 8);
  // check if it's already reserved
  return GSL_PIN_Reserved[pin / 8] & bitmask;
}

// Reserve a pin so that it cannot be reserved again.  If you attempt to
// reserve a pin twice, an error will be thrown.  This can be useful for error
// checking.
void GSL_PIN_Reserve(PinEnum pin) {
  // can't reserve pins that don't exist
  if (pin == kPinInvalid || pin == kPinFinal) {
    return;
  }
  // store the location and bitmask
  uint8_t bitmask = 1 << (pin % 8);
  uint16_t byte_number = pin / 8;
  // check if it's already reserved
  if (GSL_PIN_Reserved[pin / 8] & bitmask) {
    // pin is already reserved!
    LOG("\nPin ", GSL_PIN_ToString(pin), " has been reused!");
    HALT("ERROR");
  }
  // reserve it
  GSL_PIN_Reserved[byte_number] |= bitmask;
}

// Unreserve a pin
void GSL_PIN_Unreserve(PinEnum pin) {
  // can't reserve pins that don't exist
  if (pin == kPinInvalid || pin == kPinFinal) {
    return;
  }
  // store the location and bitmask
  uint8_t bitmask = 1 << (pin % 8);
  uint16_t byte_number = pin / 8;
  // unreserve it (ignore if not reserved
  GSL_PIN_Reserved[byte_number] &= ~bitmask;
}

// for a given pin, enable the corresponding clock
inline void GSL_PIN_EnableClock(PinEnum pin) {
  GPIO_TypeDef * port = GSL_PIN_GetPort(pin);
  if (0) {
  } else if (port == GPIOA) {__HAL_RCC_GPIOA_CLK_ENABLE();
  } else if (port == GPIOB) {__HAL_RCC_GPIOB_CLK_ENABLE();
  } else if (port == GPIOC) {__HAL_RCC_GPIOC_CLK_ENABLE();
#ifdef GPIOD
  } else if (port == GPIOD) {__HAL_RCC_GPIOD_CLK_ENABLE();
#endif
#ifdef GPIOE
  } else if (port == GPIOE) {__HAL_RCC_GPIOE_CLK_ENABLE();
#endif
#ifdef GPIOF
  } else if (port == GPIOF) {__HAL_RCC_GPIOF_CLK_ENABLE();
#endif
#ifdef GPIOG
  } else if (port == GPIOG) {__HAL_RCC_GPIOG_CLK_ENABLE();
#endif
#ifdef GPIOH
  } else if (port == GPIOH) {__HAL_RCC_GPIOH_CLK_ENABLE();
#endif
#ifdef GPIOI
  } else if (port == GPIOI) {__HAL_RCC_GPIOI_CLK_ENABLE();
#endif
#ifdef GPIOJ
  } else if (port == GPIOJ) {__HAL_RCC_GPIOJ_CLK_ENABLE();
#endif
#ifdef GPIOK
  } else if (port == GPIOK) {__HAL_RCC_GPIOK_CLK_ENABLE();
#endif
  } else {
    LOG("\nCannot activate clock of pin ", GSL_PIN_ToString(pin));
    HALT("Invalid parameter");
  }
}

// return true if a pin is high
/*inline bool GSL_PIN_IsHigh(PinEnum pin) {
  return GSL_PIN_GetPort(pin)->ODR & GSL_PIN_GetPin(pin);
}*/

// set a digital pin low
inline void GSL_PIN_SetLow(PinEnum pin) {
  //HAL_GPIO_WritePin(GetPort(pin), GetPin(pin), GPIO_PIN_RESET);
  GSL_PIN_GetPort(pin)->BSRR = ((uint32_t) GSL_PIN_GetPin(pin)) << 16;
}

// set a digital pin high
inline void GSL_PIN_SetHigh(PinEnum pin) {
  //HAL_GPIO_WritePin(GetPort(pin), GetPin(pin), GPIO_PIN_SET);
  GSL_PIN_GetPort(pin)->BSRR = GSL_PIN_GetPin(pin);
}

// set a digital pin high or low
inline void GSL_PIN_SetValue(PinEnum pin, bool high) {
  if (high) {
    GSL_PIN_SetHigh(pin);
  } else {
    GSL_PIN_SetLow(pin);
  }
}

// return true if the value of a digital input pin is high
inline bool GSL_PIN_IsOutputStateHigh(PinEnum pin) {
  return GSL_PIN_GetPort(pin)->ODR & GSL_PIN_GetPin(pin);
}

// toggle a digital pin
// since the ODR ^= operator is not atomic, we read it and then
// write to the BSRR register, which is atomic
inline void GSL_PIN_Toggle(PinEnum pin) {
  //HAL_GPIO_TogglePin(GSL_PIN_GetPort(pin), GSL_PIN_GetPin(pin));
  if (GSL_PIN_IsOutputStateHigh(pin)) {
    GSL_PIN_SetLow(pin);
  } else {
    GSL_PIN_SetHigh(pin);
  }
  //GSL_PIN_GetPort(pin)->ODR ^= GSL_PIN_GetPin(pin);
}

// return true if the value of a digital input pin is high
inline bool GSL_PIN_GetValue(PinEnum pin) {
  //return HAL_GPIO_ReadPin(GetPort(pin), GetPin(pin));
  return GSL_PIN_GetPort(pin)->IDR & GSL_PIN_GetPin(pin);
}

// pin state
enum GSL_PIN_PinStateEnum {
  kPinStateUnknown,
  // tied directly to ground
  kPinStateGrounded,
  // pulled to ground with 60kohm or less
  kPinStatePulledDown,
  // floating pin (can be pulled up and down through 60kohm resistor)
  kPinStateFloating,
  // pulled up to VCC with 60 kohm or less
  kPinStatePulledUp,
  // tied directly to VCC
  kPinStateSetHigh,
};

// initialize an input or output pin
// e.g. GSL_PIN_Initialize(kPinB1, GPIO_MODE_INPUT, GPIO_PULLUP);
// e.g. GSL_PIN_Initialize(kPinB1, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL);
void GSL_PIN_Initialize(
    PinEnum pin,
    uint32_t gpio_mode = GPIO_MODE_OUTPUT_PP,
    uint32_t resistor_mode = GPIO_NOPULL,
    uint32_t gpio_speed = GPIO_SPEED_HIGH) {
  // if kPinNone is selected, do nothing
  if (pin == kPinNone) {
    return;
  }
  // reserve the pin
  GSL_PIN_Reserve(pin);
  // enable the clock for this pin
  GSL_PIN_EnableClock(pin);
  // set pin as digital output
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Pin = GSL_PIN_GetPin(pin);
  GPIO_InitStruct.Mode = gpio_mode;
  GPIO_InitStruct.Pull = resistor_mode;
  GPIO_InitStruct.Speed = gpio_speed;
  HAL_GPIO_Init(GSL_PIN_GetPort(pin), &GPIO_InitStruct);
}

// forward declaration
void GSL_DEL_US(uint32_t us);

// return the state of the given (uninitialized) pin
GSL_PIN_PinStateEnum GSL_PIN_GetPinState(PinEnum pin) {
  const uint16_t delay_us = 20;
  // see if we can pull it down
  GSL_PIN_Initialize(pin, GPIO_MODE_INPUT, GPIO_PULLDOWN);
  GSL_DEL_US(delay_us);
  bool can_pull_down = !GSL_PIN_GetValue(pin);
  //LOG("\ncan_pull_down = ", (can_pull_down) ? "true" : "false"); // DEBUG
  GSL_PIN_Deinitialize(pin);
  // see if we can pull it up
  GSL_PIN_Initialize(pin, GPIO_MODE_INPUT, GPIO_PULLUP);
  GSL_DEL_US(delay_us);
  bool can_pull_up = GSL_PIN_GetValue(pin);
  //LOG("\ncan_pull_up = ", (can_pull_up) ? "true" : "false"); // DEBUG
  GSL_PIN_Deinitialize(pin);
  // if we can do both, it's floating
  if (can_pull_down && can_pull_up) {
    return kPinStateFloating;
  }
  // we should be able to pull them one way or another
  if (!can_pull_down && !can_pull_up) {
    return kPinStateUnknown;
  }
  // if we can't pull it down, see if we can set it low
  if (!can_pull_down) {
    GSL_PIN_SetLow(pin);
    GSL_PIN_Initialize(pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL);
    GSL_DEL_US(delay_us);
    bool can_set_low = !GSL_PIN_GetValue(pin);
    //LOG("\ncan_set_low = ", (can_set_low) ? "true" : "false"); // DEBUG
    GSL_PIN_Deinitialize(pin);
    if (can_set_low) {
      return kPinStatePulledUp;
    } else {
      return kPinStateSetHigh;
    }
  }
  // if we can't pull it down, see if we can set it low
  if (!can_pull_up) {
    GSL_PIN_SetHigh(pin);
    GSL_PIN_Initialize(pin, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL);
    GSL_DEL_US(delay_us);
    bool can_set_high = GSL_PIN_GetValue(pin);
    //LOG("\ncan_set_high = ", (can_set_high) ? "true" : "false"); // DEBUG
    GSL_PIN_Deinitialize(pin);
    GSL_PIN_SetLow(pin);
    if (can_set_high) {
      return kPinStatePulledDown;
    } else {
      return kPinStateGrounded;
    }
  }
  return kPinStateUnknown;
}

// log the status of all uninitialized pins
void GSL_PIN_LogPinStates(void) {
  // loop through all pins
  PinEnum pin = kPinA0;
  LOG("\n\nState of each pin (floating pins are not output):");
  while (pin != kPinFinal) {
    // enable this clock
    GSL_PIN_EnableClock(pin);
    if (!GSL_PIN_IsReserved(pin)) {
      auto state = GSL_PIN_GetPinState(pin);
      if (state == kPinStateGrounded) {
        LOG("\n- Pin ", GSL_PIN_ToString(pin), " is tied to ground");
      } else if (state == kPinStatePulledDown) {
        LOG("\n- Pin ", GSL_PIN_ToString(pin), " is pulled down");
      } else if (state == kPinStatePulledUp) {
        LOG("\n- Pin ", GSL_PIN_ToString(pin), " is pulled up");
      } else if (state == kPinStateSetHigh) {
        LOG("\n- Pin ", GSL_PIN_ToString(pin), " is tied to VCC");
      } else {
        ASSERT_EQ(state, kPinStateFloating);
      }
    } else {
      LOG("\n- Pin ", GSL_PIN_ToString(pin), " is reserved, unknown state");
    }
    pin = (PinEnum) (((int) pin) + 1);
  }
}

// initialize a pin to an alternate function mode
// e.g. GSL_PIN_Initialize(kPinB1, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_AF5_SPI1);
void GSL_PIN_InitializeAF(
    PinEnum pin,
    uint32_t gpio_mode,
    uint32_t resistor_mode,
    uint32_t alternate_function,
    uint32_t gpio_speed = GPIO_SPEED_HIGH) {
  // if kPinNone is selected, do nothing
  if (pin == kPinNone) {
    return;
  }
  // ensure value is correct
  if (gpio_mode != GPIO_MODE_AF_PP && gpio_mode != GPIO_MODE_AF_OD) {
    HALT("Invalid parameter");
  }
  // reserve the pin
  GSL_PIN_Reserve(pin);
  // enable the clock for this pin
  GSL_PIN_EnableClock(pin);
  // set pin as digital output
  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.Pin = GSL_PIN_GetPin(pin);
  GPIO_InitStruct.Mode = gpio_mode;
  GPIO_InitStruct.Pull = resistor_mode;
  GPIO_InitStruct.Speed = gpio_speed;
  GPIO_InitStruct.Alternate = alternate_function;
  HAL_GPIO_Init(GSL_PIN_GetPort(pin), &GPIO_InitStruct);
}

// deinitialize a pin
void GSL_PIN_Deinitialize(PinEnum pin) {
  // if kPinNone is selected, do nothing
  if (pin == kPinNone) {
    return;
  }
  // deinitialize
  HAL_GPIO_DeInit(GSL_PIN_GetPort(pin), GSL_PIN_GetPin(pin));
  // reserve the pin
  GSL_PIN_Unreserve(pin);
}

// return the mode for the given pin
// e.g. GPIO_MODE_AF_PP
uint32_t GSL_PIN_GetMode(PinEnum pin) {
  // get the port and pin
  GPIO_TypeDef * port = GSL_PIN_GetPort(pin);
  uint16_t pin_number = GSL_PIN_GetPinNumber(pin);
  // store the status register
  uint32_t status = 0;
  // add in MODER bits
  status |= (port->MODER >> (2 * pin_number)) & 0b11;
  // unless analog, add 0x10 if open drain
  if (status == 1 || status == 2) {
    if ((port->OTYPER >> pin_number) & 0b1) {
      status += 0x10;
    }
  }
  // return the status
  return status;
}

// return the pull status for the given pin
// e.g. GPIO_NOPULL
uint32_t GSL_PIN_GetPull(PinEnum pin) {
  // get the port and pin
  GPIO_TypeDef * port = GSL_PIN_GetPort(pin);
  uint16_t pin_number = GSL_PIN_GetPinNumber(pin);
  // store the status register
  uint32_t status = 0;
  // add in MODER bits
  status |= (port->PUPDR >> (2 * pin_number)) & 0b11;
  // return the status
  return status;
}

// return the AF mode for the given pin
// e.g. GPIO_NOPULL
uint32_t GSL_PIN_GetAFMode(PinEnum pin) {
  // get the port and pin
  GPIO_TypeDef * port = GSL_PIN_GetPort(pin);
  uint16_t pin_number = GSL_PIN_GetPinNumber(pin);
  return (port->AFR[pin_number / 8] >> (4 * (pin_number % 8))) & 0b1111;
}

// return true if the given pin is in AF mode
bool GSL_PIN_IsInAFMode(PinEnum pin) {
  auto mode = GSL_PIN_GetMode(pin);
  return (mode == GPIO_MODE_AF_PP) || (mode == GPIO_MODE_AF_OD);
}


// forward declaration
const char * GSL_OUT_Hex(uint32_t value, uint8_t length_in_bytes);

// log the status of the GPIO pins
// GPIO Pin description:
// GPIOA: clock disabled
// GPIOB: clock enabled
// Pin    Mode               Pull       Speed
// PB15   Input Push-pull    None       High
void GSL_PIN_LogInformation(void) {
  // loop through each pin
  for (PinEnum pin = kPinA0; pin != kPinFinal; pin = (PinEnum) (pin + 1)) {
    // if pin 0, check for clock enabled
    char string[2] = {0};
    string[0] = 'A' + GSL_PIN_GetPortNumber(pin);
    if (GSL_PIN_GetPin(pin) == GPIO_PIN_0) {
      LOG("\n- GPIO", string, ": ");
      if (((RCC->AHB1ENR >> GSL_PIN_GetPortNumber(pin)) & 0b1) == 0) {
        // skip this port
        pin = (PinEnum) (pin + 15);
        LOG("disabled");
        continue;
      }
      LOG("clock enabled");
    }
    // output the pin name (e.g. PA0)
    LOG("\n  - P", string, GSL_PIN_GetPinNumber(pin));
    if (GSL_PIN_GetPinNumber(pin) < 10) {
    }
    LOG(": ");
    // output the mode
    const uint32_t mode = GSL_PIN_GetMode(pin);
    switch (mode) {
    case GPIO_MODE_INPUT:
      LOG("Input");
      break;
    case GPIO_MODE_OUTPUT_PP:
      LOG("Output, push-pull");
      break;
    case GPIO_MODE_OUTPUT_OD:
      LOG("Output, open-drain");
      break;
    case GPIO_MODE_AF_PP:
      LOG("Alternate function ", GSL_PIN_GetAFMode(pin), ", push-pull");
      break;
    case GPIO_MODE_AF_OD:
      LOG("Alternate function ", GSL_PIN_GetAFMode(pin), ", open-drain");
      break;
    case GPIO_MODE_ANALOG:
      LOG("Analog");
      break;
    default:
      LOG("Unknown mode (", GSL_OUT_Hex(mode, 4), ")");
      break;
    }
    // output the pullup or pulldown
    LOG(", ");
    const uint32_t pull = GSL_PIN_GetPull(pin);
    switch (pull) {
    case GPIO_NOPULL:
      LOG("no pull");
      break;
    case GPIO_PULLUP:
      LOG("pull up");
      break;
    case GPIO_PULLDOWN:
      LOG("pull up");
      break;
    default:
      LOG("unknown pull (", GSL_OUT_Hex(pull, 4), ")");
      break;
    }
    // if output mode, output the current state
    if (mode == GPIO_MODE_OUTPUT_PP || mode == GPIO_MODE_OUTPUT_OD) {
      bool output_value = GSL_PIN_IsOutputStateHigh(pin);
      if (output_value) {
        LOG(", output high");
      } else {
        LOG(", output low");
      }
      bool input_value = GSL_PIN_GetValue(pin);
      if (output_value != input_value) {
        if (input_value) {
          LOG(", input high");
        } else {
          LOG(", input low");
        }
      }
    }
    // if reserved, mark it
    if (GSL_PIN_IsReserved(pin)) {
      LOG(", reserved");
    }
  }
}

// reserve the SWD pins
void GSL_PIN_ReserveSWDPins(void) {

  GSL_PIN_InitializeAF(kPinA13, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_AF0_SWJ);
  GSL_PIN_InitializeAF(kPinA14, GPIO_MODE_AF_PP, GPIO_NOPULL, GPIO_AF0_SWJ);

  //GSL_PIN_Reserve(kPinA13);
  //GSL_PIN_Reserve(kPinA14);

  // deinitialize JTAG pins not also used by SWD
  GSL_PIN_Initialize(kPinA15, GPIO_MODE_INPUT, GPIO_NOPULL);
  GSL_PIN_Initialize(kPinB3, GPIO_MODE_INPUT, GPIO_NOPULL);
  GSL_PIN_Initialize(kPinB4, GPIO_MODE_INPUT, GPIO_NOPULL);
  GSL_PIN_Unreserve(kPinA15);
  GSL_PIN_Unreserve(kPinB3);
  GSL_PIN_Unreserve(kPinB4);
}

// initialize SWD pins during startup
GSL_INITIALIZER GSL_PIN_Initializer(GSL_PIN_ReserveSWDPins);
