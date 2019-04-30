#pragma once

// The EndStopStruct provides an interface to simulate a single endstop.
// Pins are configured in open-drain mode and are active low.  In other words,
// the line drives low when the endstop is closed.

#include "c3d_includes.h"

struct C3D_EndStopStruct {
  // pin connected to this end stop
  PinEnum pin;
  // true if switch normally high
  bool normally_high;
  // constructor
  C3D_EndStopStruct(PinEnum pin, bool normally_high=true) :
      pin(pin), normally_high(normally_high) {
    // initialize pin in open drain mode
    auto pull = (normally_high) ? GPIO_PULLUP : GPIO_PULLDOWN;
    GSL_PIN_Initialize(pin, GPIO_MODE_OUTPUT_PP, pull);
    Off();
  }
  // turn on to simulate a triggered end stop
  void Off(void) {
    GSL_PIN_SetHigh(pin);
  }
  // turn off
  void On(void) {
    GSL_PIN_SetLow(pin);
  }
};
