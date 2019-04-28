#pragma once

// This file provides an interface for buttons.

#include "gsl_includes.h"

// maximum number of buttons able to be allocated
#ifndef GSL_BUTTON_MAX_BUTTONS
#define GSL_BUTTON_MAX_BUTTONS 10
#endif

// debounce length in milliseconds
#ifndef GSL_BUTTON_DEBOUNCE_MS
#define GSL_BUTTON_DEBOUNCE_MS 40
#endif

// debounce length in milliseconds
#ifndef GSL_BUTTON_LONG_DEBOUNCE_MS
#define GSL_BUTTON_LONG_DEBOUNCE_MS 750
#endif

// these define when the callback routine will be triggered
enum MonitorEnum : uint8_t {
  // never trigger
  kMonitorNone = 0x00,
  // trigger every time the line goes low
  kMonitorDown = 0x01,
  // trigger every time the line goes high
  kMonitorUp = 0x02,
  // trigger upon a debounced button press
  // this is triggered when the button is held down continuously throughout
  // the debounce period.  cannot trigger more than once without a release event
  kMonitorPress = 0x04,
  // trigger upon a debounced button release
  // this is triggered when the button is held released continuously throughout
  // the debounce period.  cannot trigger more than once without a press event
  kMonitorRelease = 0x08,
  // trigger upon a debounced long button press
  // this is triggered when the button is held down continuously throughout
  // the debounce period.  cannot trigger more than once without a release event
  kMonitorLongPress = 0x10,
};

struct GSL_BUTTON_InfoStruct {
  // pin being monitored
  PinEnum pin;
  // states being flagged
  uint8_t monitor_flag;
  // if true, will be immediately called
  bool immediate;
  // true if button is high when released
  bool released_high;
  // function to call when event is triggered
  void (*callback_function)(MonitorEnum);
  // function to call when event is triggered
  void (*callback_function_2)(void);
  // time between allowable triggers
  uint32_t cooldown_ms;
  // last state change
  GSL_DEL_LongTime last_state_change;
  // true if button was down on the last state
  bool is_down;
  // next possible time it can trigger a press
  GSL_DEL_LongTime next_press_trigger;
  // next possible time it can trigger a release
  //GSL_DEL_LongTime next_release_trigger;
  // time the button was state of the button on the last call
  GSL_DEL_LongTime next_release_trigger;
  // true if a press or release can be triggered if state remains the same
  bool can_trigger;
  // true if a long press or release can be triggered if state remains the same
  bool can_long_trigger;
  // pending flags for calling routines
  uint8_t pending_flag;
};

// struct holding all button info
GSL_BUTTON_InfoStruct gsl_button_info[GSL_BUTTON_MAX_BUTTONS];

// number of buttons allocated
uint16_t gsl_button_count = 0;

// return true if button is down
bool GSL_BUTTON_IsDown(GSL_BUTTON_InfoStruct * button) {
  bool result = GSL_PIN_GetValue(button->pin) != button->released_high;
  return result;
}

// register a new button
void GSL_BUTTON_Register(
    PinEnum pin,
    uint8_t flag,
    void (*callback_function) (MonitorEnum)) {
  // ensure we have space
  if (gsl_button_count == GSL_BUTTON_MAX_BUTTONS) {
    HALT("Too many buttons");
  }
  // find a new button
  GSL_BUTTON_InfoStruct * button = &gsl_button_info[gsl_button_count];
  // initialize the pin
  button->pin = pin;
  GSL_PIN_Initialize(
      button->pin,
      GPIO_MODE_INPUT,
      GPIO_PULLUP);
  // populate it
  button->monitor_flag = flag;
  button->immediate = false;
  button->released_high = true;
  button->callback_function = callback_function;
  button->callback_function_2 = nullptr;
  button->last_state_change = GSL_DEL_GetLongTime();
  button->is_down = GSL_BUTTON_IsDown(button);
  button->can_trigger = false;
  button->can_long_trigger = false;
  button->pending_flag = 0;
  // button is now valid, add it to the list to monitor
  ++gsl_button_count;
}

// register a new button
void GSL_BUTTON_Register(
    PinEnum pin,
    uint8_t flag,
    void (*callback_function) (void)) {
  // ensure we have space
  if (gsl_button_count == GSL_BUTTON_MAX_BUTTONS) {
    HALT("Too many buttons");
  }
  // find a new button
  GSL_BUTTON_InfoStruct * button = &gsl_button_info[gsl_button_count];
  // initialize the pin
  button->pin = pin;
  GSL_PIN_Initialize(
      button->pin,
      GPIO_MODE_INPUT,
      GPIO_PULLUP);
  // populate it
  button->monitor_flag = flag;
  button->immediate = false;
  button->released_high = true;
  button->callback_function = nullptr;
  button->callback_function_2 = callback_function;
  button->last_state_change = GSL_DEL_GetLongTime();
  button->is_down = GSL_BUTTON_IsDown(button);
  button->can_trigger = false;
  button->can_long_trigger = false;
  button->pending_flag = 0;
  // button is now valid, add it to the list to monitor
  ++gsl_button_count;
}

// call any pending button callback routines
void GSL_BUTTON_Callback(void) {
  for (uint16_t i = 0; i < gsl_button_count; ++i) {
    // set up a pointer to the current button
    GSL_BUTTON_InfoStruct * button = &gsl_button_info[i];
    uint8_t flag = button->pending_flag;
    if (flag == 0) {
      continue;
    }
    button->pending_flag = 0;
    // handle pending callbacks
    if (flag & kMonitorDown) {
      if (button->callback_function) {
        button->callback_function(kMonitorDown);
      } else {
        button->callback_function_2();
      }
    }
    if (flag & kMonitorPress) {
      if (button->callback_function) {
        button->callback_function(kMonitorPress);
      } else {
        button->callback_function_2();
      }
    }
    if (flag & kMonitorLongPress) {
      if (button->callback_function) {
        button->callback_function(kMonitorLongPress);
      } else {
        button->callback_function_2();
      }
    }
    if (flag & kMonitorUp) {
      if (button->callback_function) {
        button->callback_function(kMonitorUp);
      } else {
        button->callback_function_2();
      }
    }
    if (flag & kMonitorRelease) {
      if (button->callback_function) {
        button->callback_function(kMonitorRelease);
      } else {
        button->callback_function_2();
      }
    }
  }
}

// true if callbacks can be called
// if false, can still monitor but can't callback
bool gsl_button_active = true;

// deactivate button callbacks
void GSL_BUTTON_Deactivate(void) {
  gsl_button_active = false;
}

// activate button callbacks
void GSL_BUTTON_Activate(void) {
  gsl_button_active = true;
}

// process the buttons, set flags to make calls
void GSL_BUTTON_Monitor(void) {
  for (uint16_t i = 0; i < gsl_button_count; ++i) {
    // set up a pointer to the current button
    GSL_BUTTON_InfoStruct * button = &gsl_button_info[i];
    // check for new state
    bool changed_state = button->is_down != GSL_BUTTON_IsDown(button);
    if (changed_state) {
      button->is_down = !button->is_down;
    }
    if (changed_state) {
      // state change, check for up and down triggers
      button->last_state_change = GSL_DEL_GetLongTime();
      button->can_trigger = true;
      button->can_long_trigger = true;
      if (button->is_down) {
        // down trigger
        if (button->monitor_flag & kMonitorDown) {
          button->pending_flag |= kMonitorDown;
        }
      } else {
        // up trigger
        if (button->monitor_flag & kMonitorUp) {
          button->pending_flag |= kMonitorUp;
        }
      }
    } else {
      // check for press or release
      if (button->can_trigger) {
        // find total time
        if (GSL_DEL_ElapsedMS(button->last_state_change) >=
            GSL_BUTTON_DEBOUNCE_MS) {
          // trigger a press or release
          button->can_trigger = false;
          if (button->is_down) {
            if (button->monitor_flag & kMonitorPress) {
              button->pending_flag |= kMonitorPress;
            }
          } else {
            if (button->monitor_flag & kMonitorRelease) {
              button->pending_flag |= kMonitorRelease;
            }
          }
        }
      }
      if (button->can_long_trigger) {
        // find total time
        if (GSL_DEL_ElapsedMS(button->last_state_change) >=
            GSL_BUTTON_LONG_DEBOUNCE_MS) {
          // trigger a press or release
          button->can_long_trigger = false;
          if (button->is_down) {
            if (button->monitor_flag & kMonitorLongPress) {
              button->pending_flag |= kMonitorLongPress;
            }
          }
        }
      }
    }
  }
  // perform callbacks
  if (gsl_button_active) {
    GSL_BUTTON_Callback();
  }

}

// if defined, implement an automatic monitor for the buttons
#ifdef GSL_BUTTON_AUTOMATIC_MONITOR

// monitor period in milliseconds
#ifndef GSL_BUTTON_MONITOR_INTERVAL
#define GSL_BUTTON_MONITOR_INTERVAL 5
#endif

// timer to use for button routines
#ifndef GSL_BUTTON_TIMER
#define GSL_BUTTON_TIMER TIM7
#endif

// set up the timer and interrupt
void GSL_BUTTON_InitializeTimer(void) {
  GSL_TIM_Reserve(GSL_SECNT_TIMER);
  GSL_TIM_SetFrequency(GSL_BUTTON_TIMER, 1000.0f / GSL_BUTTON_MONITOR_INTERVAL);
  GSL_TIM_SetUpdateCallback(GSL_BUTTON_TIMER, GSL_BUTTON_Monitor);
  GSL_TIM_Start(GSL_BUTTON_TIMER);

}

// initialize the button monitor
GSL_INITIALIZER gsl_button_initializer(GSL_BUTTON_InitializeTimer);

#endif
