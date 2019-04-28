#pragma once

#include "gsl_includes.h"

// This file interfaces with an SSD1306 OLED controller
//
// The interface used is 4-wire SPI.

// commands
enum GSL_SSD1306_CommandEnum : uint8_t {
  kCommandScrollRight = 0x26,
  kCommandScrollLeft = 0x27,
  kCommandScrollVerticalRight = 0x29,
  kCommandScrollVerticalLeft = 0x2A,
  kCommandScrollVertical = 0xA3,

  kCommandSetMemoryAddressMode = 0x20,

  kCommandSetColumnAddress = 0x21,

  kCommandSetPageAddress = 0x22,

  kCommandDeactivateScroll = 0x2E,
  kCommandActivateScroll = 0x2F,

  kCommandSetDisplayStartLine = 0x40,
  kCommandSetContrast = 0x81,

  kCommandSetSegmentRemapNormal = 0xA0,
  kCommandSetSegmentRemapReversed = 0xA1,
  kCommandDisplayUseRAM = 0xA4,
  kCommandDisplayIgnoreRAM = 0xA5,
  kCommandSetNormalDisplay = 0xA6,
  kCommandSetInverseDisplay = 0xA7,
  kCommandSetMultiplexRatio = 0xA8,
  kCommandSetDisplayOff = 0xAE,
  kCommandSetDisplayOn = 0xAF,

  kCommandSetPageStartAddress0 = 0xB0,

  kCommandSetCOMOutputScanDirectionNormal = 0xC0,
  kCommandSetCOMOutputScanDirectionFlipped = 0xC8,

  kCommandSetDisplayOffset = 0xD3,
  kCommandSetDisplayClockDivideRatio = 0xD5,
  kCommandSetPreChargePeriod = 0xD9,
  kCommandSetCOMPinsHardwareConfiguration = 0xDA,
  kCommandSetVcomhDeselectLevel = 0xDB,
  kCommandNOP = 0xE3,
};

struct GSL_SSD1306 {

  // max speed of SPI clock (10 MHz per datasheet)
  const uint32_t kMaxSPIClockSpeed = 10000000;

  // if true, use SPI peripheral, else use the I2C peripheral
  bool spi_mode;

  // if true and in SPI mode, use the SPI_TX buffer
  //bool spi_dma_mode;

  // SPI peripheral to use
  SPI_TypeDef * SPIx;

  // CS pin
  PinEnum pinCS;

  // D/C pin
  PinEnum pinDC;

  // RST pin
  PinEnum pinRST;

  // screen buffer
  GSL_MSBUF screen;

  // if true, display is rotated 180 degrees
  bool rotated;

  // if true, display is mirrored about x axis (long axis)
  bool mirror_horizontal;

  // if true, display is mirrored about y axis (short axis)
  bool mirror_vertical;

  // if true, display is transposed (swap x and y coordinates)
  bool transposed;

  // number of columns
  const uint16_t width = 128;

  // number of rows
  uint16_t height;

  // send command
  void SendCommand(uint8_t command) {
    // set low to send command
    GSL_PIN_SetLow(pinDC);
    GSL_SPI_SetCSPin(SPIx, pinCS);
    GSL_SPI_Send(SPIx, command);
  }

  // send a two byte command
  void SendCommand(uint8_t command, uint8_t data) {
    // set low to send command
    uint8_t value[2] = {command, data};
    GSL_PIN_SetLow(pinDC);
    GSL_SPI_SetCSPin(SPIx, pinCS);
    GSL_SPI_SendMulti(SPIx, value, 2);
  }

  // send a three byte command
  void SendCommand(uint8_t command, uint8_t data1, uint8_t data2) {
    // set low to send command
    uint8_t value[3] = {command, data1, data2};
    GSL_PIN_SetLow(pinDC);
    GSL_SPI_SetCSPin(SPIx, pinCS);
    GSL_SPI_SendMulti(SPIx, value, 3);
  }

  // SPI constructor
  GSL_SSD1306(
      SPI_TypeDef * SPIx_,
      PinEnum pinCS_,
      PinEnum pinDC_,
      PinEnum pinRST_,
      uint16_t height_) {
    spi_mode = true;
    SPIx = SPIx_;
    pinCS = pinCS_;
    pinDC = pinDC_;
    pinRST = pinRST_;
    rotated = false;
    mirror_horizontal = true;
    mirror_vertical = true;
    transposed = false;
    height = height_;
  }

  // clear the memory (set all pixels to black)
  void Clear(void) {
    // set horizontal addressing mode
    SendCommand(kCommandSetMemoryAddressMode, 0);
    // set 128 columns
    SendCommand(kCommandSetColumnAddress, 0, width - 1);
    // set 4 or 8 pages (4*8 = 32 rows)
    SendCommand(kCommandSetPageAddress, 0, height / 8 - 1);
    GSL_PIN_SetHigh(pinDC);
    GSL_SPI_SetCSPin(SPIx, pinCS);
    for (uint16_t i = 0; i < width * height / 8; ++i) {
      GSL_SPI_Send(SPIx, 0);
    }
  }

  // reset the display
  // (we need to clear the memory as well, as it persists past reset)
  void Reset(void) {
    GSL_PIN_SetLow(pinRST);
    GSL_DEL_US(4);
    GSL_PIN_SetHigh(pinRST);
    GSL_DEL_US(500);

    // clear the memory
    //Clear();

    // this sequence follows the software initialization flow chart found in
    // the SSD1306 datasheet
    SendCommand(kCommandSetMultiplexRatio, 0x3F);
    //SendCommand();
    SendCommand(kCommandSetDisplayOffset, 0x00);
    SendCommand(kCommandSetDisplayStartLine);
    SendCommand(kCommandSetSegmentRemapNormal);
    SendCommand(kCommandSetCOMOutputScanDirectionNormal);
    //SendCommand(kCommandSetCOMOutputScanDirectionFlipped); // DEBUG
    SendCommand(kCommandSetCOMPinsHardwareConfiguration,
                (height == 32) ? 0x02 : 0x12);
    //SendCommand(0x02);
    SendCommand(kCommandSetContrast, 0xFF); // set brightness
    SendCommand(kCommandDisplayUseRAM);
    SendCommand(kCommandSetNormalDisplay);
    SendCommand(kCommandSetDisplayClockDivideRatio, 0x80);
    //SendCommand(0x80);

    //SendCommand(kCommandSetInverseDisplay); // DEBUG
    // enable charge pump regulator
    SendCommand(0x8D, 0x14);
    SendCommand(kCommandSetDisplayOn);

    // set horizontal addressing mode
    if (transposed) {
      SendCommand(kCommandSetMemoryAddressMode, 1);
    } else {
      SendCommand(kCommandSetMemoryAddressMode, 0);
    }
    // set segment remap
    if (mirror_horizontal) {
      SendCommand(kCommandSetSegmentRemapReversed, 0, height / 8 - 1);
    } else {
      SendCommand(kCommandSetSegmentRemapNormal, 0, height / 8 - 1);
    }
    // set column
    if (mirror_vertical) {
      SendCommand(kCommandSetCOMOutputScanDirectionFlipped, 0, height / 8 - 1);
    } else {
      SendCommand(kCommandSetCOMOutputScanDirectionNormal, 0, height / 8 - 1);
    }
    // set 128 columns
    SendCommand(kCommandSetColumnAddress, 0, width - 1);
    // set X pages
    SendCommand(kCommandSetPageAddress, 0, height / 8 - 1);

  }

  // set the brightness level (0-255)
  void SetContrast(uint8_t brightness) {
    SendCommand(kCommandSetContrast, brightness); // set brightness
  }

  // initialize
  void Initialize(void) {
    // initialize SPI peripheral
    GSL_SPI_SetMaxSpeed(SPIx, kMaxSPIClockSpeed);
    GSL_SPI_Initialize(SPIx);
    // initialize pins
    // add CS pin
    GSL_SPI_AddCSPin(SPIx, pinCS);
    GSL_SPI_SetCSPin(SPIx, pinCS);
    // initialize pins
    GSL_PIN_Initialize(pinDC, GPIO_MODE_OUTPUT_PP);
    GSL_PIN_SetHigh(pinDC);
    GSL_PIN_Initialize(pinRST, GPIO_MODE_OUTPUT_PP);
    // initialize the screen
    if (transposed) {
      screen.SetSize(height, width);
    } else {
      screen.SetSize(width, height);
    }
    // reset display
    Reset();
  }

  // update the given portion of the display
  // (this will be increased to line up with the page boundary)
  /*
  void UpdateBlock(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    HALT("TODO");
    // if range is empty, do nothing
    if (x2 < x1  || y2 < y1) {
      return;
    }
    // set horizontal addressing mode
    SendCommand(kCommandSetMemoryAddressMode, 0);
    // set 128 columns
    SendCommand(kCommandSetColumnAddress, x1, x2);
    // set X pages
    // move y1 down to the nearest multiple of 8
    y1 = (y1 / 8) * 8;
    // move y2 down to the next multiple of 8
    // 0-7 -> 0
    // 8-15 -> 8
    y2 = (y2 / 8) * 8;
    SendCommand(kCommandSetPageAddress, y1 / 8, y2 / 8);
    // get data and write it to the display
    GSL_PIN_SetHigh(pinDC);
    GSL_SPI_SetCSPin(SPIx, pinCS);
    static uint8_t buffer[1024];
    uint16_t pos = 0;
    for (
        uint16_t y = (mirror_vertical) ? height - 8 : 0;
        y < height;
        y += (mirror_vertical) ? -8 : 8) {
      for (
          uint16_t x = (mirror_horizontal) ? width - 1 : 0;
          x < 128;
          x += (mirror_horizontal) ? -1 : 1) {
        uint8_t data = 0;
        for (
            uint8_t dy = (!mirror_vertical) ? 7 : 0;
            dy < 8;
            dy += (!mirror_vertical) ? -1 : 1) {
          data <<= 1;
          if (transposed) {
            if (screen.GetPixel(y + dy, x)) {
              data |= 1;
            }
          } else {
            if (screen.GetPixel(x, y + dy)) {
              data |= 1;
            }
          }
        }
        buffer[pos++] = data;
        //GSL_SPI_Send(SPIx, data);
      }
    }
    ASSERT_EQ(pos, 16 * height);
    GSL_SPI_SendMulti(SPIx, buffer, 16 * height);
  }*/

  // update the entire display
  void Update(void) {
    //GSL_PIN_Toggle(kPinB1); // DEBUG
    // TODO: implement this for a transposed display
    ASSERT(!transposed);

    // set 128 columns
    SendCommand(kCommandSetColumnAddress, 0, width - 1);
    // set X pages
    SendCommand(kCommandSetPageAddress, 0, height / 8 - 1);
    //GSL_DEL_MS(10); // DEBUG: take out?
    // send raw data in screen buffer
    ASSERT_EQ(screen.buffer_size, width * height / 8U);
    GSL_PIN_SetHigh(pinDC);
    GSL_SPI_SetCSPin(SPIx, pinCS);
    GSL_SPI_SendMulti(SPIx, screen.buffer, screen.buffer_size);
    //GSL_SPI_SendMulti_DMA(SPIx, screen.buffer, screen.buffer_size);
  }

  // update the entire display with the buffer
  void Update_OLD(void) {
    // set horizontal addressing mode
    SendCommand(kCommandSetMemoryAddressMode, 0);
    // set 128 columns
    SendCommand(kCommandSetColumnAddress, 0, width - 1);
    // set X pages
    SendCommand(kCommandSetPageAddress, 0, height / 8 - 1);
    // get data and write it to the display
    GSL_PIN_SetHigh(pinDC);
    GSL_SPI_SetCSPin(SPIx, pinCS);
    static uint8_t buffer[1024];
    uint16_t pos = 0;
    for (
        uint16_t y = (mirror_vertical) ? height - 8 : 0;
        y < height;
        y += (mirror_vertical) ? -8 : 8) {
      for (
          uint16_t x = (mirror_horizontal) ? width - 1 : 0;
          x < width;
          x += (mirror_horizontal) ? -1 : 1) {
        uint8_t data = 0;
        for (
            uint8_t dy = (!mirror_vertical) ? 7 : 0;
            dy < 8;
            dy += (!mirror_vertical) ? -1 : 1) {
          data <<= 1;
          if (transposed) {
            if (screen.GetPixel(y + dy, x)) {
              data |= 1;
            }
          } else {
            //LOG("\npixel (", x, ", ", y + dy, ")"); // DEBUG
            if (screen.GetPixel(x, y + dy)) {
              data |= 1;
            }
          }
        }
        buffer[pos++] = data;
        //GSL_SPI_Send(SPIx, data);
      }
    }
    ASSERT_EQ(pos, 16 * height);
    GSL_SPI_SendMulti(SPIx, buffer, 16 * height);
  }

  // swipe function
  // 0 = current screen
  // 1 = next screen
  // 2 = swipe bar (white)
  // 3 = swipe bar (black)
  // i increments from 0
  uint8_t SwipeFunction(uint16_t i, uint16_t x, uint16_t y) {
    int16_t phi = x + y / 2 - i;
    if (phi >= 0) {
      return 0;
    } else if (phi >= -2) {
      return 2;
    } else if (phi >= -4) {
      return 3;
    } else if (phi >= -6) {
      return 2;
    } else {
      return 1;
    }
  }

  // swipe in/out the picture with an animation
  void Swipe(bool swipe_in, GSL_MSBUF * other_screen = nullptr) {
    // sanity check
    if (other_screen) {
      ASSERT_EQ(other_screen->width, screen.width);
      ASSERT_EQ(other_screen->height, screen.height);
    }
    // set horizontal addressing mode
    SendCommand(kCommandSetMemoryAddressMode, 0);
    // set 128 columns
    SendCommand(kCommandSetColumnAddress, 0, width - 1);
    // set 4 pages (4*8 = 32 rows)
    SendCommand(kCommandSetPageAddress, 0, (height == 32) ? 3 : 7);
    // get data and write it to the display
    GSL_PIN_SetHigh(pinDC);
    GSL_SPI_SetCSPin(SPIx, pinCS);
    static uint8_t buffer[1024];
    const float fps = 240;
    auto start_tick = GSL_DEL_GetLongTime();
    bool finished = false;
    uint32_t ticks = GSL_DEL_Ticks();
    uint32_t used_ticks = 0;
    uint32_t wait_ticks = 0;
    for (uint32_t i = 0; !finished; i += 1) {
      used_ticks += GSL_DEL_Ticks() - ticks;
      ticks = GSL_DEL_Ticks();
      while (GSL_DEL_ElapsedMS(start_tick) <= i * 1000 / fps) {
      }
      wait_ticks += GSL_DEL_Ticks() - ticks;
      ticks = GSL_DEL_Ticks();
      while (GSL_DEL_ElapsedMS(start_tick) > (i + 1) * 1000 / fps) {
        ++i;
      }
      finished = true;
      // wait for next update
      uint16_t pos = 0;
      for (
          uint16_t y = (mirror_vertical) ? height - 8 : 0;
          y < height;
          y += (mirror_vertical) ? -8 : 8) {
        for (
            uint16_t x = (mirror_horizontal) ? width - 1 : 0;
            x < width;
            x += (mirror_horizontal) ? -1 : 1) {
          uint8_t data = 0;
          for (
              uint8_t dy = (!mirror_vertical) ? 7 : 0;
              dy < 8;
              dy += (!mirror_vertical) ? -1 : 1) {
            data <<= 1;
            switch (SwipeFunction(i, x, y + dy)) {
              case 0:
                finished = false;
                if (!swipe_in) {
                  if (transposed) {
                    if (screen.GetPixel(y + dy, x)) {
                      data |= 1;
                    }
                  } else {
                    if (screen.GetPixel(x, y + dy)) {
                      data |= 1;
                    }
                  }
                } else if (other_screen) {
                  if (transposed) {
                    if (other_screen->GetPixel(y + dy, x)) {
                      data |= 1;
                    }
                  } else {
                    if (other_screen->GetPixel(x, y + dy)) {
                      data |= 1;
                    }
                  }
                }
                break;
              case 1:
                if (swipe_in) {
                  if (transposed) {
                    if (screen.GetPixel(y + dy, x)) {
                      data |= 1;
                    }
                  } else {
                    if (screen.GetPixel(x, y + dy)) {
                      data |= 1;
                    }
                  }
                } else if (other_screen) {
                  if (transposed) {
                    if (other_screen->GetPixel(y + dy, x)) {
                      data |= 1;
                    }
                  } else {
                    if (other_screen->GetPixel(x, y + dy)) {
                      data |= 1;
                    }
                  }
                }
                break;
              case 2:
                finished = false;
                data |= 1;
                break;
              case 3:
                finished = false;
                break;
            }
          }
          buffer[pos++] = data;
        }
      }
      ASSERT_EQ(pos, (height == 32) ? 512 : 1024);
      GSL_SPI_SendMulti(SPIx, buffer, (height == 32) ? 512 : 1024);
    }
    used_ticks += GSL_DEL_Ticks() - ticks;
    ticks = GSL_DEL_Ticks();
    float usage = (float) (used_ticks) / (used_ticks + wait_ticks);
    LOG("\nSwipe used ", GSL_OUT_FixedFloat(usage * 100.0f, 1), "% cpu");
  }

  // swipe in the picture with an animation
  void SwipeIn(void) {
    Swipe(true);
  }

  // swipe out the picture with an animation
  void SwipeOut(void) {
    Swipe(false);
  }

  // swipe out the current picture and transition to the given buffer
  // if new_screen == nullptr, this transitions to a black screen
  void SwipeTo(GSL_MSBUF * new_screen) {
    Swipe(false, new_screen);
    if (new_screen) {
      memcpy(screen.buffer, new_screen->buffer, screen.buffer_size);
    } else {
      screen.Clear();
    }
  }

};
