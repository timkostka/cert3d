#pragma once

// This provides the GSL_COLOR class, which is used as a color

#include "gsl_includes.h"

// 16-bit 5/6/5 color format
struct GSL_COL_RGB565 {
  uint16_t value;
  // default constructor
  GSL_COL_RGB565(void) {
    value = 0;
  }
  // construct from rgb values
  GSL_COL_RGB565(float r, float g, float b) {
    GSL_GEN_ClipFloat(r);
    GSL_GEN_ClipFloat(g);
    GSL_GEN_ClipFloat(b);
    value = 0;
    // now make a color with 5 bits r, 6 bits green, 5 bits blue
    value |= (uint16_t) (r * ((1 << 5) - 0.5f));
    value <<= 6;
    value |= (uint16_t) (g * ((1 << 6) - 0.5f));
    value <<= 5;
    value |= (uint16_t) (b * ((1 << 5) - 0.5f));
  }
  // implicit conversion to uint16_t
  operator uint16_t() const {
    return value;
  }
};

// generic color class with RGB components
struct GSL_COL_RGB {
  // three floats
  float rgb[3];
  // normalize the values
  void Normalize(void) {
    for (uint16_t i = 0; i < 3; ++i) {
      GSL_GEN_ClipFloat(rgb[i]);
    }
  }
  // construct from 3 rgb values
  GSL_COL_RGB(float r = 0.0f, float g = 0.0f, float b = 0.0f) {
    rgb[0] = r;
    rgb[1] = g;
    rgb[2] = b;
    Normalize();
  }
  // construct from HSV values
  // from http://www.rapidtables.com/convert/color/hsv-to-rgb.htm
  static GSL_COL_RGB FromHSV(float hue, float saturation, float value) {
    GSL_GEN_ClipFloat(hue);
    GSL_GEN_ClipFloat(saturation);
    GSL_GEN_ClipFloat(value);
    float high = value;
    float low = value * (1.0f - saturation);
    float alpha = 1.0f - fabs(fmod(hue * 6.0f, 2.0f) - 1.0f);
    float mid = low + alpha * (high - low);
    GSL_GEN_ClipFloat(low);
    GSL_GEN_ClipFloat(mid);
    GSL_GEN_ClipFloat(high);
    if (hue < 1.0f / 6.0f) {
      return GSL_COL_RGB(high, mid, low);
    } else if (hue < 2.0f / 6.0f) {
      return GSL_COL_RGB(mid, high, low);
    } else if (hue < 3.0f / 6.0f) {
      return GSL_COL_RGB(low, high, mid);
    } else if (hue < 4.0f / 6.0f) {
      return GSL_COL_RGB(low, mid, high);
    } else if (hue < 5.0f / 6.0f) {
      return GSL_COL_RGB(mid, low, high);
    } else {
      return GSL_COL_RGB(high, low, mid);
    }
  }
  // blend two colors and return the result
  // fraction = 0 will return color one
  // fraction = 1 will return color two
  static GSL_COL_RGB Blend(
      const GSL_COL_RGB & one,
      const GSL_COL_RGB & two,
      float fraction) {
    GSL_GEN_ClipFloat(fraction);
    return GSL_COL_RGB(one.rgb[0] * (1.0f - fraction) + two.rgb[0] * fraction,
                       one.rgb[1] * (1.0f - fraction) + two.rgb[1] * fraction,
                       one.rgb[2] * (1.0f - fraction) + two.rgb[2] * fraction);
  }
  // blend another color and return the result
  // fraction = 0 will return the current color
  // fraction = 1 will return the new color
  GSL_COL_RGB Blend(const GSL_COL_RGB & that, float fraction) const {
    return Blend(*this, that, fraction);
  }
  // implicit conversion to GSL_COLOR565
  operator GSL_COL_RGB565() const {
    return GSL_COL_RGB565(rgb[0], rgb[1], rgb[2]);
  }
  // implicit conversion to uint16_t (via GSL_COLOR565)
  operator uint16_t() const {
    return GSL_COL_RGB565(rgb[0], rgb[1], rgb[2]);
  }
};

// some generic color values
const GSL_COL_RGB kColorBlack(0.0f, 0.0f, 0.0f);
const GSL_COL_RGB kColorWhite(1.0f, 1.0f, 1.0f);
const GSL_COL_RGB kColorGray50(0.5f, 0.5f, 0.5f);
const GSL_COL_RGB kColorRed(1.0f, 0.0f, 0.0f);
const GSL_COL_RGB kColorGreen(0.0f, 1.0f, 0.0f);
const GSL_COL_RGB kColorBlue(0.0f, 0.0f, 1.0f);
const GSL_COL_RGB kColorYellow(1.0f, 1.0f, 0.0f);
const GSL_COL_RGB kColorCyan(0.0f, 1.0f, 1.0f);
const GSL_COL_RGB kColorMagenta(1.0f, 0.0f, 1.0f);
