#pragma once

// The EndStopStruct provides an interface to simulate a single endstop.
// Pins are configured in open-drain mode and are active low.  In other words,
// the line drives low when the endstop is closed.

#include "c3d_includes.h"

class EndStopStruct {
  // pin connected to this end stop
  PinEnum pin;
  // constructor
  EndStopStruct(PinEnum pin) :
      pin(pin) {
    // initialize pin in open drain mode
    GSL_PIN_Initialize(pin, GPIO_MODE_OUTPUT_OD, GPIO_NOPULL);
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
