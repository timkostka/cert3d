#pragma once

// This file includes an interface for communicating with an LCD screen through
// the HX8357D controller.
// for instance, on this breakout board:
// https://www.adafruit.com/products/2050

#include "gsl_includes.h"

struct GSL_LCD_HX8357D {
  // color scheme
  struct ColorSchemeStruct {
    uint16_t back;
    uint16_t fill;
    uint16_t border;
    uint16_t fore;
    uint16_t text;
    uint16_t highlight;
    uint16_t shadow;
    uint16_t button_face;
    uint16_t text_pressed;
    uint16_t button_face_pressed;
  };
  // current color scheme
  ColorSchemeStruct color_;
  // lcd width
  const uint16_t width_ = 480;
  // lcd height
  const uint16_t height_ = 320;
  // interface enum
  enum InterfaceEnum {
    kInterfaceNone,
    kInterfaceParallel8Bit,
    kInterfaceParallel9Bit,
    kInterfaceParallel16Bit,
    kInterfaceParallel18Bit,
    kInterfaceParallel24Bit,
    kInterfaceSerial3Wire,
    kInterfaceSerial4Wire,
  };
  // commands
  enum CommandEnum : uint8_t {
    kCommandNOP = 0x00,
    // this resets the display, must wait 5-120ms for it to reset
    kCommandSoftwareReset = 0x01,
    // read 4 bytes:
    // 0: dummy byte
    // 1: LCD module manufacturer's ID
    // 2: LCD module/driver version ID
    // 3: LCD module/drive ID
    //kCommandReadIdentification = 0x02,
    // don't invert the display
    kCommandDisplayInversionOff = 0x20,
    // invert the display
    kCommandDisplayInversionOn = 0x21,
    // turn all pixels black
    kCommandAllPixelsOff = 0x22,
    // turn all pixels white
    kCommandAllPixelsOn = 0x23,
    // turn off the display (memory not transferred to screen)
    kCommandDisplayOff = 0x28,
    // turn on the display (transfer memory to screen)
    kCommandDisplayOn = 0x29,
    // set the column address
    kCommandColumnAddressSet = 0x2A,
    // set the page address
    kCommandPageAddressSet = 0x2B,
    // set the page address
    kCommandMemoryWrite = 0x2C,
    // turn off the tearing effect signal
    kCommandTearingEffectLineOff = 0x34,
    // turn on the tearing effect signal
    kCommandTearingEffectLineOn = 0x35,
    // memory access control
    kCommandMemoryAccessControl = 0x36,
    // turn idle mode off
    kCommandIdleModeOff = 0x38,
    // turn idle mode on
    kCommandIdleModeOn = 0x39,
  };
  // interface type
  InterfaceEnum interface_;
  // SPI peripheral to use, if any
  SPI_TypeDef * SPIx_;
  // CS pin, if any
  PinEnum pinCS_;
  // DC pin, if any
  // low = command, high = data
  PinEnum pinDC_;
  // RST pin, if any
  PinEnum pinRST_;
  // current font
  const GSL_FONT_FontStruct * font_;


  // convert R, G, B values into a color
  // values for r,g,b can range from 0 (off) to 1 (on)
  uint16_t ToColor(float r, float g, float b) {
    // validity check
    if (r > 1.0f) {
      r = 1.0f;
    } else if (!(r >= 0.0f)) {
      r = 0.0f;
    }
    if (g > 1.0f) {
      g = 1.0f;
    } else if (!(g >= 0.0f)) {
      g = 0.0f;
    }
    if (b > 1.0f) {
      b = 1.0f;
    } else if (!(b >= 0.0f)) {
      b = 0.0f;
    }
    uint16_t color = 0;
    /*uint16_t redcomp = r * ((1 << 5) - 0.5f);
    uint16_t greencomp = g * ((1 << 6) - 0.5f);
    uint16_t bluecomp = b * ((1 << 5) - 0.5f);
    ASSERT(redcomp <= 0b11111);
    ASSERT(greencomp <= 0b111111);
    ASSERT(bluecomp <= 0b11111);
    color = redcomp;
    color <<= 6;
    color <<= 6;*/
    // now make a color with 5 bits r, 6 bits green, 5 bits blue
    color |= (uint16_t) (r * ((1 << 5) - 1));
    color <<= 6;
    color |= (uint16_t) (g * ((1 << 6) - 1));
    color <<= 5;
    color |= (uint16_t) (b * ((1 << 5) - 1));
    return color;
  }

  // start a command
  void StartCommand(uint8_t command) {
    // send the command
    GSL_SPI_SetCSPin(SPIx_, pinCS_);
    // set CS managed mode
    GSL_SPI_SetCSMode(SPIx_, GSL_SPI_CS_MODE_MANUAL);
    // we are sending a command
    GSL_PIN_SetLow(pinDC_);
    // set CS low
    GSL_SPI_SetCSLow(SPIx_);
    // send the command
    GSL_SPI_Send(SPIx_, command);
    // we are sending data
    GSL_PIN_SetHigh(pinDC_);
  }
  // end a command
  void EndCommand(void) {
    // wait for it to be ready
    GSL_SPI_WaitTX(SPIx_);
    // return CS pin high
    GSL_SPI_SetCSHigh(SPIx_);
  }
  // send a command and a number of parameters to the display
  void SendCommand(uint8_t command, uint8_t * data, uint16_t data_count) {
    // only one interface currently supported
    ASSERT(interface_ == kInterfaceSerial4Wire);
    StartCommand(command);
    // send the data
    GSL_SPI_SendMulti(SPIx_, data, data_count);
    // end the command
    EndCommand();
  }
  // send a command to the display
  void SendCommand(uint8_t command) {
    SendCommand(command, nullptr, 0);
  }
  // send a command and one parameter to the display
  void SendCommand(uint8_t command, uint8_t data) {
    SendCommand(command, &data, 1);
  }
  // send a command and two parameters to the display
  void SendCommand(uint8_t command, uint8_t data1, uint8_t data2) {
    uint8_t data[2] = {data1, data2};
    SendCommand(command, data, sizeof(data));
  }
  // send a command and two 16-bit parameters to the display
  void SendCommand1616(uint8_t command, uint16_t data1, uint16_t data2) {
    uint8_t data[4] = {
        (uint8_t) (data1 >> 8),
        (uint8_t) data1,
        (uint8_t) (data2 >> 8),
        (uint8_t) data2};
    SendCommand(command, data, sizeof(data));
  }
  // send a command and one 32-bit parameters to the display
  void SendCommand32(uint8_t command, uint32_t data1) {
    uint8_t data[4] = {
        (uint8_t) (data1 >> 24),
        (uint8_t) (data1 >> 16),
        (uint8_t) (data1 >> 8),
        (uint8_t) data1};
    SendCommand(command, data, sizeof(data));
  }
  // send a command and three parameters to the display
  void SendCommand(uint8_t command, uint8_t data1, uint8_t data2, uint8_t data3) {
    uint8_t data[3] = {data1, data2, data3};
    SendCommand(command, data, sizeof(data));
  }
  // send a command and four parameters to the display
  void SendCommand(uint8_t command, uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4) {
    uint8_t data[4] = {data1, data2, data3, data4};
    SendCommand(command, data, sizeof(data));
  }

  // initializer
  void Initialize(void) {
    // only one interface currently supported
    ASSERT(interface_ == kInterfaceSerial4Wire);
    // ensure definitions are there
    ASSERT(GSL_PIN_IsReal(pinCS_));
    ASSERT(GSL_PIN_IsReal(pinDC_));
    ASSERT(GSL_PIN_IsReal(pinRST_));
    ASSERT(SPIx_ != nullptr);
    // ensure CS pin is there and set it to be active
    GSL_SPI_AddCSPin(SPIx_, pinCS_);
    GSL_SPI_SetCSPin(SPIx_, pinCS_);
    // I couldn't find the max SPI interface speed in the datasheet, but this
    // is where I'd set it.  I think it's high.
    //GSL_SPI_SetMaxSpeed(SPIx_, 1000000);
    GSL_SPI_Initialize(SPIx_);
    // initialize pins
    GSL_PIN_SetLow(pinRST_);
    GSL_PIN_Initialize(pinRST_, GPIO_MODE_OUTPUT_PP);
    GSL_PIN_Initialize(pinDC_, GPIO_MODE_OUTPUT_PP);
    // bring it out of reset (must hold RST low for 10us)
    GSL_DEL_US(100);
    GSL_PIN_SetHigh(pinRST_);
    // wait for it to boot
    GSL_DEL_MS(120);

    // reset the display
    //SendCommand(0x01);
    //GSL_DEL_MS(150);
    // enable extended command set
    SendCommand(0xB9, 0xFF, 0x83, 0x57);
    // set RGB interface
    SendCommand(0xB3, 0x00, 0x00, 0x06, 0x06);
    // set VCOM voltage to -1.65V
    SendCommand(0xB6, 0x25);
    // set panel color order and gate direction
    SendCommand(0xCC, 0x05);
    // set power control
    {
      uint8_t data[] = {0x00, 0x15, 0x1C, 0x1C, 0x83, 0xAA};
      SendCommand(0xB1, data, sizeof(data));
    }
    // set source circuit option
    {
      uint8_t data[] = {0x50, 0x50, 0x01, 0x3C, 0x1E, 0x08};
      SendCommand(0xC0, data, sizeof(data));
    }
    // set display cycle register
    {
      uint8_t data[] = {0x02, 0x40, 0x00, 0x2A, 0x2A, 0x0D, 0x78};
      SendCommand(0xB4, data, sizeof(data));
    }
    // set interface pixel format to 16-bit colors
    SendCommand(0x3A, 0b01010101);
    // set memory access control
    // landscape (480x320), with top left corner as (0, 0)
    SendCommand(kCommandMemoryAccessControl, 0b01000000);
    // set tearing effect line on
    //SendCommand(kCommandTearingEffectLineOn, 0x00);
    // set tear scan line
    //SendCommand(0x44, (uint8_t) 0x00, 0x02);
    // turn off sleep mode
    SendCommand(0x11);
    GSL_DEL_MS(5);
    // turn display on mode
    SendCommand(kCommandDisplayOn);
    GSL_DEL_MS(150);
  }

  // set the window to be written
  void SetWindow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    // ensure parameters are okay
    ASSERT(x1 <= x2);
    ASSERT(y1 <= y2);
    ASSERT(x2 < width_);
    ASSERT(y2 < height_);
    // set the column window
    SendCommand1616(kCommandColumnAddressSet, y1, y2);
    // set the page
    SendCommand1616(kCommandPageAddressSet, x1, x2);
  }

  // output a color to the SPI buffer
  void SendColor(uint16_t col) {
    // wait for it to be ready
    while ((SPIx_->SR & 0b10) == 0) {
    }
    // send top 8 bits
    SPIx_->DR = (uint8_t) (col >> 8);
    // wait for it to be ready
    while ((SPIx_->SR & 0b10) == 0) {
    }
    // send bottom 8 bits
    SPIx_->DR = (uint8_t) col;
    /*
    col = GSL_GEN_SwapEndian(col);
    GSL_SPI_SendMulti(SPIx_, (uint8_t *) &col, 2);*/
  }

  // fill the screen with a given color
  void Fill(uint16_t color) {
    // for black or white, use the special command
    /*if (color == 0x0000) {
      SendCommand(kCommandAllPixelsOff);
      return;
    } else if (color == 0xFFFF) {
      SendCommand(kCommandAllPixelsOn);
      return;
    }*/
    /*uint8_t data[64];
    color = GSL_GEN_SwapEndian(color);
    for (uint8_t i = 0; i < 32; ++i) {
      data[i * 2] = color >> 8;
      data[i * 2 + 1] = color;
    }*/
    // set the window to the entire display
    SetWindow(0, 0, width_ - 1, height_ - 1);
    // send the command
    StartCommand(kCommandMemoryWrite);
    // send the data
    uint32_t count = width_ * height_;
    while (count--) {
      SendColor(color);
    }
    /*
    while (count >= sizeof(data) / 2) {
      GSL_SPI_SendMulti(SPIx_, data, sizeof(data));
      count -= sizeof(data) / 2;
    }
    GSL_SPI_SendMulti(SPIx_, data, count * 2);*/
    // return CS pin high
    EndCommand();
  }

  // fill the screen with black
  void FillBlack(void) {
    Fill(0x0000);
  }

  // fill the screen with black
  void FillWhite(void) {
    Fill(0xFFFF);
  }

  // clear the screen
  void Clear(void) {
    Fill(color_.back);
  }

  // Draw a filled rectangle
  void DrawFilledRectangle(
      uint16_t x1,
      uint16_t y1,
      uint16_t x2,
      uint16_t y2,
      uint16_t border) {
    uint16_t width = x2 - x1 + 1;
    uint16_t height = y2 - y1 + 1;
    // set the rectangle
    SetWindow(x1, y1, x2, y2);
    StartCommand(kCommandMemoryWrite);
    // if rectangle is fully filled, then just do that
    if (width <= border * 2 || height <= border * 2) {
      for (uint32_t i = 0; i < (uint32_t) width * height; ++i) {
        SendColor(color_.border);
      }
      EndCommand();
      return;
    }
    // draw left border
    for (uint16_t i = 0; i < border * height; ++i) {
      SendColor(color_.border);
    }
    // draw mid section
    for (uint16_t x = 0; x < width - 2 * border; ++x) {
      for (uint16_t i = 0; i < border; ++i) {
        SendColor(color_.border);
      }
      for (uint16_t i = 0; i < height - 2 * border; ++i) {
        SendColor(color_.fill);
      }
      for (uint16_t i = 0; i < border; ++i) {
        SendColor(color_.border);
      }
    }
    // draw right border
    for (uint16_t i = 0; i < border * height; ++i) {
      SendColor(color_.border);
    }
    // end the command
    EndCommand();
  }

  // Fill a region with the given color
  void Fill(
      uint16_t x1,
      uint16_t y1,
      uint16_t x2,
      uint16_t y2,
      uint16_t color) {
    // for empty areas, just exit
    if (x2 < x1 || y2 < y1) {
      return;
    }
    uint16_t width = x2 - x1 + 1;
    uint16_t height = y2 - y1 + 1;
    SetWindow(x1, y1, x2, y2);
    StartCommand(kCommandMemoryWrite);
    // fill the space
    for (uint32_t i = 0; i < (uint32_t) width * height; ++i) {
      SendColor(color);
    }
    // end the command
    EndCommand();
  }

  // draw some text
  void DrawText(const char * string, uint16_t x, uint16_t y) {
    ASSERT(font_);

    // save the original x value
    uint16_t original_x = x;

    // draw each character
    while (*string) {
      // do special case of a newline character
      if (*string == '\n') {
        x = original_x;
        y += font_->height_ + font_->space_between_lines_;

      } else if (*string == ' ') {
        // special case of a space character
        x += font_->width_ + font_->space_between_chars_;
      } else {
        // draw the character
        SetWindow(x, y, x + font_->GetCharWidth(*string) - 1, y + font_->height_ - 1);
        StartCommand(kCommandMemoryWrite);
        for (uint16_t dx = 0; dx < font_->GetCharWidth(*string); ++dx) {
          for (uint16_t dy = 0; dy < font_->height_; ++dy) {
            if (font_->GetPixel(*string, dx, dy)) {
              SendColor(color_.fore);
            } else {
              SendColor(color_.back);
            }
          }
        }
        EndCommand();
        // move to next character
        x += font_->GetCharWidth(*string) + font_->space_between_chars_;
      }
      ++string;
    }
  }

  // swap two values
  template <class T = uint16_t>
  static void Swap(T & one, T & two) {
    T three = one;
    one = two;
    two = three;
  }

  // flip between pressed and unpressed color scheme
  void ToggleColorScheme(void) {
    Swap(color_.text, color_.text_pressed);
    Swap(color_.button_face, color_.button_face_pressed);
    Swap(color_.shadow, color_.highlight);
  }

  // draw a button with text, possible inverted
  void DrawButton(
      const char * string,
      uint16_t x1,
      uint16_t y1,
      uint16_t x2,
      uint16_t y2,
      bool pressed = false) {
    // if pressed, then reverse the color scheme
    const uint16_t border = 2;
    // minimum number of pixels between border and text
    const uint16_t text_border = 1;
    uint16_t width = x2 - x1 + 1;
    uint16_t height = y2 - y1 + 1;
    // if rectangle isn't big enough to display any text, just create a rectangle
    if (width <= (border + text_border) * 2 ||
        height <= (border + text_border) * 2) {
      DrawFilledRectangle(x1, y1, x2, y2, border);
      return;
    }
    // temporarily toggle the color scheme
    if (pressed) {
      ToggleColorScheme();
    }
    //uint16_t string_length = strlen(string);
    // get the leftmost x value of the string
    int16_t string_x = ((int16_t) width - font_->GetStringWidth(string)) / 2;
    // get the topmost y value of the string
    int16_t string_y = ((int16_t) height - font_->height_) / 2;
    // set the rectangle
    SetWindow(x1, y1, x2, y2);
    StartCommand(kCommandMemoryWrite);
    // draw left border
    for (uint16_t x = 0; x < border; ++x) {
      for (uint16_t i = 0; i < height; ++i) {
        if (x + i + 1 < height) {
          SendColor(color_.highlight);
        } else {
          SendColor(color_.shadow);
        }
      }
    }
    // draw buffer on left
    for (uint16_t x = 0; x < text_border; ++x) {
      // top border
      for (uint16_t i = 0; i < border; ++i) {
        SendColor(color_.highlight);
      }
      // midsection
      for (uint16_t i = 0; i < height - 2 * border; ++i) {
        SendColor(color_.button_face);
      }
      // bottom border
      for (uint16_t i = 0; i < border; ++i) {
        SendColor(color_.shadow);
      }
    }
    // number of columns before we encounter the string
    int16_t empty_columns = string_x - border - text_border;
    // note: *string points to the current character
    // current character column
    uint16_t char_column = 0;
    // increment string to account for clipping
    while (empty_columns < 0) {
      ++char_column;
      if (char_column == font_->GetCharWidth(*string)) {
        ++empty_columns;
        if (*string) {
          ++string;
        }
        char_column = 0;
      }
      ++empty_columns;
    }
    // draw mid section
    for (uint16_t x = 0; x < width - 2 * border - 2 * text_border; ++x) {
      // top border
      for (uint16_t i = 0; i < border; ++i) {
        SendColor(color_.highlight);
      }
      // buffer
      for (uint16_t i = 0; i < text_border; ++i) {
        SendColor(color_.button_face);
      }
      // if this columns is empty...
      if (*string == 0 || empty_columns > 0) {
        for (uint16_t i = 0; i < height - 2 * border - 2 * text_border; ++i) {
          SendColor(color_.button_face);
        }
        if (empty_columns > 0) {
          --empty_columns;
        }
      } else {
        // midsection
        int16_t char_y = border + text_border - string_y;
        for (uint16_t i = 0; i < height - 2 * border - 2 * text_border; ++i) {
          if (char_y >= 0 && char_y < font_->height_) {
            if (font_->GetPixel(*string, char_column, char_y)) {
              SendColor(color_.text);
            } else {
              SendColor(color_.button_face);
            }
          } else {
            SendColor(color_.button_face);
          }
          ++char_y;
        }
        // increment column
        ++char_column;
        if (char_column >= font_->GetCharWidth(*string)) {
          char_column = 0;
          empty_columns = font_->space_between_chars_;
          ++string;
        }
      }
      // buffer
      for (uint16_t i = 0; i < text_border; ++i) {
        SendColor(color_.button_face);
      }
      // bottom border
      for (uint16_t i = 0; i < border; ++i) {
        SendColor(color_.shadow);
      }
    }
    // draw buffer on right
    for (uint16_t x = 0; x < text_border; ++x) {
      // top border
      for (uint16_t i = 0; i < border; ++i) {
        SendColor(color_.highlight);
      }
      // midsection
      for (uint16_t i = 0; i < height - 2 * border; ++i) {
        SendColor(color_.button_face);
      }
      // bottom border
      for (uint16_t i = 0; i < border; ++i) {
        SendColor(color_.shadow);
      }
    }
    // draw right border
    for (uint16_t x = 0; x < border; ++x) {
      for (uint16_t i = 0; i < height; ++i) {
        if (border <= i + x) {
          SendColor(color_.shadow);
        } else {
          SendColor(color_.highlight);
        }
      }
    }
    // end the command
    EndCommand();
    // switch back to original color scheme
    if (pressed) {
      ToggleColorScheme();
    }
  }

  // constructor
  GSL_LCD_HX8357D(void) {
    SPIx_ = nullptr;
    interface_ = kInterfaceNone;
    pinCS_ = kPinNone;
    pinDC_ = kPinNone;
    pinRST_ = kPinNone;
    // initialize defaults
    font_ = &gsl_font_5x7;
    // initialize default color scheme
    color_.back = ToColor(0.0f, 0.0f, 0.0f);
    color_.fill = ToColor(0.25f, 0.25f, 0.25f);
    color_.border = ToColor(1.0f, 1.0f, 1.0f);
    color_.fore = ToColor(1.0f, 1.0f, 1.0f);
    color_.text = ToColor(0.0f, 0.0f, 0.0f);
    color_.highlight = ToColor(1.0f, 1.0f, 1.0f);
    color_.shadow = ToColor(0.125f, 0.125f, 0.125f);
    color_.button_face = ToColor(0.75f, 0.75f, 0.75f);
    color_.text_pressed = 0xFFFF;
    color_.button_face_pressed = ToColor(0.25f, 0.25f, 0.25f);
    // yellow on black color scheme
    {
      GSL_COL_RGB foreground = kColorYellow;
      GSL_COL_RGB background = kColorBlack;
      color_.back = background;
      color_.fill = foreground;
      color_.border = foreground;
      color_.fore = foreground;
      color_.text = foreground;
      color_.highlight = foreground;
      color_.shadow = foreground;
      color_.button_face = background;
      color_.text_pressed = background;
      color_.button_face_pressed = foreground;
    }
  }

};
