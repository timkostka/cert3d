#pragma once

// An LED object allows you to interface with a given LED.
//
// Example usage syntax:
//
//   LED statusLED(kPinA0);
//   statusLED.On();
//   statusLED.Off();
//   statusLED.Toggle();

#include "gsl_includes.h"

struct GSL_LED {

  // pin controlling the LED
  const PinEnum pin_;

  // on state
  enum OnStateEnum {
    kOnStateHigh,
    kOnStateLow,
  };
  const OnStateEnum on_state_;

  // if true, pin is initialized and LED is active
  // (if false, pin is not initialized and won't be used)
  bool active;

  // deactivate
  void Deactivate() {
    // if inactive, just return
    if (!active) {
      return;
    }
    active = false;
    // deinitialize the pin
    GSL_PIN_Deinitialize(pin_);
  }

  // activate
  void Activate() {
    // if already active, just return
    if (active) {
      return;
    }
    // if pin isn't real, just return
    if (!GSL_PIN_IsReal(pin_)) {
      return;
    }
    // initialize the pin
    GSL_PIN_Initialize(pin_, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL);
    // set it off
    if (on_state_ == kOnStateHigh) {
      GSL_PIN_SetLow(pin_);
    } else {
      GSL_PIN_SetHigh(pin_);
    }
    active = true;
  }

  // initialize the pin and turn off the LED
  void Initialize() {
    Activate();
    /*
    GSL_PIN_Initialize(pin_, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL);
    if (on_state_ == kOnStateHigh) {
      GSL_PIN_SetLow(pin_);
    } else {
      GSL_PIN_SetHigh(pin_);
    }
    */
  }

  // constructor (default on state high)
  GSL_LED(PinEnum pin)
    : pin_(pin),
      on_state_(kOnStateHigh),
      active(false) {
    Initialize();
  }

  // constructor with on state specified
  GSL_LED(PinEnum pin, OnStateEnum on_state)
    : pin_(pin),
      on_state_(on_state),
      active(false) {
    Initialize();
  }

  // deconstructor
  ~GSL_LED(void) {
    GSL_PIN_Deinitialize(pin_);
  }

  // turn on the LED
  void On(void) {
    if (!active) {
      return;
    }
    if (on_state_ == kOnStateHigh) {
      GSL_PIN_SetHigh(pin_);
    } else {
      GSL_PIN_SetLow(pin_);
    }
  }

  // return true if the LED is on
  bool IsOn(void) {
    return GSL_PIN_GetValue(pin_) == (on_state_ == kOnStateHigh);
  }

  // turn off the LED
  void Off(void) {
    if (!active) {
      return;
    }
    if (on_state_ == kOnStateHigh) {
      GSL_PIN_SetLow(pin_);
    } else {
      GSL_PIN_SetHigh(pin_);
    }
  }

  // set the given state
  // true = on, false = off
  void Set(bool state) {
    if (state) {
      On();
    } else {
      Off();
    }
  }

  // toggle the LED
  void Toggle(void) {
    if (!active) {
      return;
    }
    GSL_PIN_Toggle(pin_);
  }

};

// if defined, we initialize the status and error LEDs here
#ifdef GSL_LED_ERROR_PIN
GSL_LED gsl_led_error(GSL_LED_ERROR_PIN);
#endif

#ifdef GSL_LED_STATUS_PIN
GSL_LED gsl_led_status(GSL_LED_STATUS_PIN);
#endif

// turn on the status LED if it's defined (else do nothing)
void GSL_LED_StatusOn(void) {
#ifdef GSL_LED_STATUS_PIN
  gsl_led_status.On();
#endif
}

// turn off the status LED if it's defined (else do nothing)
void GSL_LED_StatusOff(void) {
#ifdef GSL_LED_STATUS_PIN
  gsl_led_status.Off();
#endif
}

// toggle the status LED if it's defined (else do nothing)
void GSL_LED_StatusToggle(void) {
#ifdef GSL_LED_STATUS_PIN
  gsl_led_status.Toggle();
#endif
}

// turn on the error LED if it's defined (else do nothing)
void GSL_LED_ErrorOn(void) {
#ifdef GSL_LED_ERROR_PIN
  gsl_led_error.On();
#endif
}

// turn off the error LED if it's defined (else do nothing)
void GSL_LED_ErrorOff(void) {
#ifdef GSL_LED_ERROR_PIN
  gsl_led_error.Off();
#endif
}

// toggle the error LED if it's defined (else do nothing)
void GSL_LED_ErrorToggle(void) {
#ifdef GSL_LED_ERROR_PIN
  gsl_led_error.Toggle();
#endif
}
