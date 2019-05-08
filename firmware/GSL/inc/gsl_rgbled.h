#pragma once

// This file implements the GSL_RGBLED object which controls a single RGB LED.
// Each channel must be able to be controlled by pulling a pin high or low.
// Therefore, both common-cathode and common-anode types are supported.

#include "gsl_includes.h"

struct GSL_RGBLED {

  // individual red, green, and blue LEDs
  GSL_LED led[3];

  // true for signal
  //bool use_signal;

  // target signal
  //GSL_COLSIG signal;

  // start LongTime of target signal
  //GSL_DEL_LongTime signal_start;

  // target color
  GSL_COL_RGB color;
  // current color (updated every iteration)
  float color_tally[3];

  // ticks at last update
  uint32_t last_update;

  // update the color
  // If this routine is called frequently and at regular intervals, the color
  // of the LED will approach the color specified by turning each LED on/off.
  // for example, this can be called in a timer update routine at 200 Hz
  void Update(void) {
    // get ticks since last update
    uint32_t ticks = GSL_DEL_Ticks() - last_update;
    last_update += ticks;
    // add the current target to each intensity
    for (uint16_t c = 0; c < 3; ++c) {
      color_tally[c] += ticks * color.rgb[c];
    }
    // if LED was on, then decrement tally by the amount of ticks
    for (uint16_t c = 0; c < 3; ++c) {
      if (led[c].IsOn()) {
        color_tally[c] -= ticks;
      }
    }
    // now set each LED on or off depending on the value
    // if tally is positive, turn on the LED
    for (uint16_t c = 0; c < 3; ++c) {
      if (color_tally[c] >= 0.0f) {
        //color_tally[c] -= 1.0f;
        led[c].On();
      } else {
        led[c].Off();
      }
    }
  }

  // constructor with RGB pins defined
  GSL_RGBLED(PinEnum red,
             PinEnum green,
             PinEnum blue,
             GSL_LED::OnStateEnum on_state) :
      led({GSL_LED(red, on_state),
           GSL_LED(green, on_state),
           GSL_LED(blue, on_state)}),
      color(kColorBlack),
      last_update(0) {
    for (uint16_t c = 0; c < 3; ++c) {
      color_tally[c] = 0.0f;
    }
  }

  // set to the given r,g,b values
  void Set(float red, float green, float blue) {
    Update();
    color = GSL_COL_RGB(red, green, blue);
    Update();
  }

  // set to the given color
  void Set(const GSL_COL_RGB & color) {
    Set(color.rgb[0], color.rgb[1], color.rgb[2]);
  }

  // set to red
  void SetRed(void) {
    Set(kColorRed);
  }

  // set to green
  void SetGreen(void) {
    Set(kColorGreen);
  }

  // set to blue
  void SetBlue(void) {
    Set(kColorBlue);
  }

  // turn off
  void Off(void) {
    Set(kColorBlack);
  }

};

// sanity checks
// must be called with a minimum of other functions running
void GSL_RGBLED_SanityCheck(void) {
  LOG("\nPerforming GSL_RGBLED_SanityCheck()...");
  // count
  const uint16_t ms_count = 100;
  // tolerance
  const float tolerance = 0.05f;
  // create a new led on some random pins
  GSL_RGBLED rgb_led(kPinA0, kPinA1, kPinA2, GSL_LED::kOnStateHigh);
  /*rgb_led.Update();
  // do 100 iterations, make sure all are dark
  for (uint16_t i = 0; i < 100; ++i) {
    while (GSL_DEL_ElapsedSFloat(start_tick) < i * 0.001f) {
    }
    GSL_DEL_MS(1);
    rgb_led.Update();
    for (uint16_t c = 0; c < 3; ++c) {
      ASSERT(!GSL_PIN_GetValue(rgb_led.led[c].pin_));
    }
  }*/
  // now do 100 iterations of a nonzero value
  rgb_led.Set(0.0f, 0.3f, 1.0f);
  uint16_t on_count[3] = {0, 0, 0};
  uint32_t start_tick = GSL_DEL_Ticks();
  for (uint16_t i = 0; i < ms_count; ++i) {
    while (GSL_DEL_ElapsedSFloat(start_tick) < i * 0.001f) {
    }
    rgb_led.Update();
    for (uint16_t c = 0; c < 3; ++c) {
      if (GSL_PIN_GetValue(rgb_led.led[c].pin_)) {
        ++on_count[c];
      }
    }
  }
  // make sure the value matches within 5%
  for (uint16_t c = 0; c < 3; ++c) {
    if (rgb_led.color.rgb[c] == 0.0f) {
      ASSERT_EQ(on_count[c], 0);
    } else if (rgb_led.color.rgb[c] == 1.0f) {
      ASSERT_EQ(on_count[c], ms_count);
    } else {
      float on_amount = (float) on_count[c] / ms_count;
      float low = rgb_led.color.rgb[c] - tolerance;
      float high = rgb_led.color.rgb[c] + tolerance;
      ASSERT_GT(on_amount, low);
      ASSERT_LT(on_amount, high);
    }
  }
  LOG(" passed!");
}
