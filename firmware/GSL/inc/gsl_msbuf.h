#pragma once

// This provides access to a monochrome (1 bit per pixel) screen buffer.  White
// pixels are considered ON/SET, black pixels are OFF/RESET.
// The top left pixels is position (0, 0)
// pixels are stored MSB first.

// TODO: bounds checking on all routines

#include "gsl_includes.h"

struct GSL_MSBUF {

  // width in pixels
  uint16_t width;

  // height in pixels
  uint16_t height;

  // width in bytes
  uint16_t width_in_bytes;

  // total buffer size
  uint32_t buffer_size;

  // buffer pointer
  uint8_t * buffer;

  // current x position (0 = leftmost column)
  //uint16_t x_position;

  // current y position (0 = topmost row)
  //uint16_t y_position;

  // current font
  const GSL_FONT_FontStruct * font;

  // return the total number of pixels that differ between this screen
  // and the target
  uint32_t CountDifferences(const GSL_MSBUF & that) {
    // if sizes are different, then return the max possible
    if (width != that.width || height != that.height) {
      return width * height;
    }
    uint32_t count = 0;
    for (uint16_t x = 0; x < width; ++x) {
      for (uint16_t y = 0; y < height; ++y) {
        if (GetPixel(x, y) != that.GetPixel(x, y)) {
          ++count;
        }
      }
    }
    return count;
  }

  // return true if the given position is in bounds
  bool InBounds(int32_t x, int32_t y) {
    return x >= 0 && x < width && y >= 0 && y < height;
  }

  // set display to black
  void FillBlack(void) {
    memset(buffer, 0, buffer_size);
  }

  // set display to black
  void Clear(void) {
    FillBlack();
  }

  // set display to white
  void FillWhite(void) {
    memset(buffer, 0xFF, buffer_size);
  }

  // set the screen size
  // (can only be called once)
  void SetSize(uint16_t target_width, uint16_t target_height) {
    ASSERT(buffer == nullptr);
    width = target_width;
    height = target_height;
    width_in_bytes = (width + 7) / 8;
    buffer_size = (uint32_t) width_in_bytes * height;
    if (buffer_size != 0) {
      buffer = (uint8_t *) GSL_BUF_Create(buffer_size);
      Clear();
    } else {
      buffer = nullptr;
    }
  }

  // constructor
  GSL_MSBUF(uint16_t target_width = 0, uint16_t target_height = 0) {
    width = 0;
    height = 0;
    width_in_bytes = 0;
    buffer_size = 0;
    buffer = nullptr;
    SetSize(target_width, target_height);
    //x_position = 0;
    //y_position = 0;
    font = nullptr;
  }

  // assignment operator
  GSL_MSBUF & operator = (const GSL_MSBUF & that) {
    ASSERT_EQ(width, that.width);
    ASSERT_EQ(height, that.height);
    memcpy(buffer, that.buffer, buffer_size);
    return *this;
  }

  // invert the display
  void Invert(void) {
    uint8_t * ptr = buffer;
    uint8_t * end = ptr + buffer_size;
    while (ptr != end) {
      *ptr ^= 0xFF;
      ++ptr;
    }
  }

  // get pointer and bitmask for the given pixel
  void GetPixelAddress(uint16_t x, uint16_t y, uint8_t * & ptr, uint8_t & mask) const {
    //ptr = buffer + x / 8 + y * width_in_bytes;
    //mask = 0x80 >> (x % 8);
    ptr = buffer + x + y / 8 * width;
    mask = 0x01 << (y % 8);
  }

  // set a pixel to white
  void SetPixel(uint16_t x, uint16_t y) {
    uint8_t * ptr;
    uint8_t mask;
    GetPixelAddress(x, y, ptr, mask);
    *ptr |= mask;
  }

  // set a pixel to black
  void ClearPixel(uint16_t x, uint16_t y) {
    uint8_t * ptr;
    uint8_t mask;
    GetPixelAddress(x, y, ptr, mask);
    *ptr &= ~mask;
  }

  // set a pixel to white (true) or black (false)
  void DrawPixel(uint16_t x, uint16_t y, bool value) {
    if (!InBounds(x, y)) { // DEBUG
      return;
    }
    //ASSERT_LT(x, width);
    //ASSERT_LT(y, height);
    if (value) {
      SetPixel(x, y);
    } else {
      ClearPixel(x, y);
    }
  }

  // invert a pixel
  void InvertPixel(uint16_t x, uint16_t y) {
    DrawPixel(x, y, !GetPixel(x, y));
  }

  // return true if the given pixel is on
  bool GetPixel(uint16_t x, uint16_t y) const {
    uint8_t * ptr;
    uint8_t mask;
    GetPixelAddress(x, y, ptr, mask);
    return *ptr & mask;
  }

  // draw a rectangle
  void DrawRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    for (uint16_t x = x1; x <= x2; ++x) {
      if (x >= width) {
        break;
      }
      SetPixel(x, y1);
      SetPixel(x, y2);
    }
    for (uint16_t y = y1 + 1; y < y2; ++y) {
      if (y >= height) {
        break;
      }
      SetPixel(x1, y);
      SetPixel(x2, y);
    }
  }

  // draw a filled rectangle
  void DrawFilledRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    for (uint16_t x = x1; x <= x2; ++x) {
      if (x >= width) {
        break;
      }
      for (uint16_t y = y1; y <= y2; ++y) {
        if (y >= height) {
          break;
        }
        SetPixel(x, y);
      }
    }
  }

  // erase a filled rectangle
  void EraseFilledRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    for (uint16_t x = x1; x <= x2; ++x) {
      if (x >= width) {
        break;
      }
      for (uint16_t y = y1; y <= y2; ++y) {
        if (y >= height) {
          break;
        }
        ClearPixel(x, y);
      }
    }
  }

  // invert a filled rectangle
  void InvertFilledRectangle(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    for (uint16_t x = x1; x <= x2; ++x) {
      if (x >= width) {
        break;
      }
      for (uint16_t y = y1; y <= y2; ++y) {
        if (y >= height) {
          break;
        }
        if (GetPixel(x, y)) {
          ClearPixel(x, y);
        } else {
          SetPixel(x, y);
        }
      }
    }
  }

  // copy a region to another screen buffer
  void CopyRegion(uint16_t x1,
                  uint16_t y1,
                  uint16_t x2,
                  uint16_t y2,
                  GSL_MSBUF & target_buffer,
                  uint16_t x3,
                  uint16_t y3) {
    for (uint16_t x = x1; x <= x2 && x < width; ++x) {
      for (uint16_t y = y1; y <= y2 && y < height; ++y) {
        target_buffer.DrawPixel(x3 + x - x1,
                                y3 + y - y1,
                                GetPixel(x, y));
      }
    }
  }

  // draw a bitmap in the given position
  void DrawBitmap(
      const GSL_BMP_InfoStruct * bitmap,
      int16_t x_offset,
      int16_t y_offset) {
    ASSERT(bitmap->color == kGslBmpColorMonochrome);
    for (uint16_t x = 0; x < bitmap->width; ++x) {
      if (x + x_offset < 0 ||
          x + x_offset >= width) {
        continue;
      }
      for (uint16_t y = 0; y < bitmap->height; ++y) {
        if (y + y_offset < 0 ||
            y + y_offset >= height) {
          continue;
        }
        DrawPixel(x + x_offset, y + y_offset, GSL_BMP_GetPixel(bitmap, x, y));
      }
    }
  }

  // draw a null-terminated string in the given position using the current font
  // this is generalized to allow for a number of operations
  // if draw_foreground, then foreground pixels are set to foreground_value
  // if draw_background, then background pixels are set to background_value
  void DrawText(
      const char * string,
      int16_t x_offset,
      int16_t y_offset,
      bool draw_foreground = true,
      bool foreground_value = true,
      bool draw_background = true,
      bool background_value = false) {
    ASSERT(font != nullptr);
    // save the starting position
    int16_t initial_x_offset = x_offset;
    // loop over each character
    while (*string) {
      // save this char
      char c = *string;
      ++string;
      // handle the newline special case
      if (c == '\n') {
        x_offset = initial_x_offset;
        y_offset += font->height_;
        y_offset += font->space_between_lines_;
        continue;
      }
      // skip space unless it's the first character on the line
      if (x_offset != initial_x_offset) {
        if (draw_background) {
          for (uint8_t x = 0; x < font->space_between_chars_; ++x) {
            for (uint8_t y = 0; y < font->height_; ++y) {
              DrawPixel(x_offset + x, y_offset + y, background_value);
            }
          }
        }
        x_offset += font->space_between_chars_;
      }
      // if char doesn't exist, skip it save some time
      if (!font->HasChar(c)) {
        if (draw_background) {
          for (uint8_t x = 0; x < font->GetCharWidth(c); ++x) {
            for (uint8_t y = 0; y < font->height_; ++y) {
              DrawPixel(x_offset + x, y_offset + y, background_value);
            }
          }
        }
        // draw background
        x_offset += font->GetCharWidth(c);
        continue;
      }
      // print this character
      for (uint8_t x = 0; x < font->GetCharWidth(c); ++x) {
        for (uint8_t y = 0; y < font->height_; ++y) {
          // if not in bounds, skip it
          if (!InBounds(x_offset + x, y_offset + y)) {
            continue;
          }
          if (font->GetPixel(c, x, y)) {
            if (draw_foreground) {
              DrawPixel(x_offset + x, y_offset + y, foreground_value);
            }
          } else {
            if (draw_background) {
              DrawPixel(x_offset + x, y_offset + y, background_value);
            }
          }
        }
      }
      x_offset += font->GetCharWidth(c);
    }
  }

  // draw a null-terminated string in the given position using the current font
  /*
  void DrawString(
      const char * string,
      int16_t x_offset,
      int16_t y_offset) {
    DrawText(string)
    ASSERT(font != nullptr);
    // save the starting position
    int16_t initial_x_offset = x_offset;
    // loop over each character
    while (*string) {
      // save this char
      char c = *string;
      ++string;
      // handle the newline special case
      if (c == '\n') {
        x_offset = initial_x_offset;
        y_offset += font->height_;
        y_offset += font->space_between_lines_;
        continue;
      }
      // skip space unless it's the first character on the line
      if (x_offset != initial_x_offset) {
        x_offset += font->space_between_chars_;
      }
      // if char doesn't exist, skip it save some time
      if (!font->HasChar(c)) {
        x_offset += font->GetCharWidth(c);
        continue;
      }
      // print this character
      for (uint8_t x = 0; x < font->width_; ++x) {
        for (uint8_t y = 0; y < font->height_; ++y) {
          // if not in bounds, skip it
          if (!InBounds(x_offset + x, y_offset + y)) {
            continue;
          }
          DrawPixel(x_offset + x, y_offset + y, font->GetPixel(c, x, y));
        }
      }
      x_offset += font->GetCharWidth(c);
    }
  }*/

  // draw a null-terminated string in the given position using the current font
  // do not draw background pixels
  // if value, then bits are turned to 1's, else bits are turned to 0's
  void DrawTextNoBackground(
      const char * string,
      int16_t x_offset,
      int16_t y_offset,
      bool value = true) {
    DrawText(string, x_offset, y_offset, true, value, false, false);
    /*
    ASSERT(font != nullptr);
    // save the starting position
    int16_t initial_x_offset = x_offset;
    // loop over each character
    while (*string) {
      // save this char
      char c = *string;
      ++string;
      // handle the newline special case
      if (c == '\n') {
        x_offset = initial_x_offset;
        y_offset += font->height_;
        y_offset += font->space_between_lines_;
        continue;
      }
      // skip space unless it's the first character on the line
      if (x_offset != initial_x_offset) {
        x_offset += font->space_between_chars_;
      }
      // if char doesn't exist, skip it save some time
      if (!font->HasChar(c)) {
        x_offset += font->GetCharWidth(c);
        continue;
      }
      // print this character
      for (uint8_t x = 0; x < font->GetCharWidth(c); ++x) {
        for (uint8_t y = 0; y < font->height_; ++y) {
          // if not in bounds, skip it
          if (!InBounds(x_offset + x, y_offset + y)) {
            continue;
          }
          if (font->GetPixel(c, x, y)) {
            DrawPixel(x_offset + x, y_offset + y, value);
          }
        }
      }
      x_offset += font->GetCharWidth(c);
    }
    */
  }

  // draw a null-terminated string
  void DrawTextFull(
      const char * string,
      int16_t x_offset,
      int16_t y_offset,
      GSL_FONT_AlignmentEnum alignment = kGslFontAlignmentTopLeft,
      GSL_FONT_DirectionEnum direction = kGslFontDirectionToRight,
      bool on_value = true,
      bool draw_background = false) {
    // first, get the total width and height of the text, including newlines
    uint16_t total_width = font->GetStringWidth(string);
    uint16_t max_total_width = total_width;
    uint16_t total_height = font->GetStringHeight(string);
    const bool vertical_text = (direction == kGslFontDirectionToTop ||
                                direction == kGslFontDirectionToBottom);
    if (vertical_text) {
      GSL_GEN_Swap(total_width, total_height);
    }
    // find the top-left of the rectangle which contains the string
    int32_t top = y_offset;
    // adjust for text height
    if (alignment == kGslFontAlignmentTopLeft ||
        alignment == kGslFontAlignmentTopCenter ||
        alignment == kGslFontAlignmentTopRight) {
    } else if (alignment == kGslFontAlignmentCenterLeft ||
               alignment == kGslFontAlignmentCenterCenter ||
               alignment == kGslFontAlignmentCenterRight) {
      top -= (total_height - 1) / 2;
    } else if (alignment == kGslFontAlignmentBottomLeft ||
               alignment == kGslFontAlignmentBottomCenter ||
               alignment == kGslFontAlignmentBottomRight) {
      top -= total_height - 1;
    } else {
      HALT("Unexpected value");
    }
    // adjust for text width
    int32_t left = x_offset;
    if (alignment == kGslFontAlignmentTopLeft ||
        alignment == kGslFontAlignmentCenterLeft ||
        alignment == kGslFontAlignmentBottomLeft) {
    } else if (alignment == kGslFontAlignmentTopCenter ||
               alignment == kGslFontAlignmentCenterCenter ||
               alignment == kGslFontAlignmentBottomCenter) {
      left -= (total_width - 1) / 2;
    } else if (alignment == kGslFontAlignmentTopRight ||
               alignment == kGslFontAlignmentCenterRight ||
               alignment == kGslFontAlignmentBottomRight) {
      left -= total_width - 1;
    } else {
      HALT("Unexpected value");
    }
    // find increment
    int32_t dx1 = 0;
    int32_t dy1 = 0;
    int32_t dx2 = 0;
    int32_t dy2 = 0;
    if (direction == kGslFontDirectionToRight) {
      dx1 = 1;
      dy2 = 1;
    } else if (direction == kGslFontDirectionToLeft) {
      dx1 = -1;
      dy2 = -1;
    } else if (direction == kGslFontDirectionToTop) {
      dy1 = -1;
      dx2 = 1;
    } else if (direction == kGslFontDirectionToBottom) {
      dy1 = 1;
      dx2 = -1;
    } else {
      HALT("Unexpected value");
    }
    // find top-left pixel of first line, assuming first line is the full width
    if (direction == kGslFontDirectionToRight) {
    } else if (direction == kGslFontDirectionToLeft) {
      top += total_height - 1;
      left += total_width - 1;
    } else if (direction == kGslFontDirectionToTop) {
      top += total_height - 1;
    } else if (direction == kGslFontDirectionToBottom) {
      left += total_width - 1;
    } else {
      HALT("Unexpected value");
    }
    // process each line separately
    while (*string != 0) {
      // skip newlines
      if (*string == '\n') {
        left += dx2 * (font->height_ + font->space_between_lines_);
        top += dy2 * (font->height_ + font->space_between_lines_);
        ++string;
        continue;
      }
      // look for the end of this line
      const char * end = string;
      while (*end != 0 && *end != '\n') {
        ++end;
      }
      // get width of this substring
      uint16_t substring_width = 0;
      const char * ptr = string;
      while (ptr != end) {
        if (substring_width > 0) {
          substring_width += font->space_between_chars_;
        }
        substring_width += font->GetCharWidth(*ptr);
        ++ptr;
      }
      // find top-left pixel of first character in this line
      int32_t this_left = left;
      int32_t this_top = top;
      if (alignment == kGslFontAlignmentTopLeft ||
          alignment == kGslFontAlignmentCenterLeft ||
          alignment == kGslFontAlignmentBottomLeft) {
      } else if (alignment == kGslFontAlignmentTopCenter ||
                 alignment == kGslFontAlignmentCenterCenter ||
                 alignment == kGslFontAlignmentBottomCenter) {
        this_left += dx1 * (max_total_width - substring_width) / 2;
        this_top += dy1 * (max_total_width - substring_width) / 2;
      } else if (alignment == kGslFontAlignmentTopRight ||
                 alignment == kGslFontAlignmentCenterRight ||
                 alignment == kGslFontAlignmentBottomRight) {
        this_left += dx1 * (max_total_width - substring_width);
        this_top += dy1 * (max_total_width - substring_width);
      } else {
        HALT("Unexpected value");
      }
      // draw each character
      bool first_char_in_line = true;
      while (*string != *end) {
        // save this char
        char c = *string;
        ++string;
        // skip space unless it's the first character on the line
        if (!first_char_in_line) {
          if (draw_background) {
            for (uint8_t x = 0; x < font->space_between_chars_; ++x) {
              for (uint8_t y = 0; y < font->height_; ++y) {
                DrawPixel(this_left + dx1 * x + dx2 * y,
                          this_top + dy1 * x + dy2 * y,
                          !on_value);
              }
            }
          }
          this_left += dx1 * font->space_between_chars_;
          this_top += dy1 * font->space_between_chars_;
        }
        first_char_in_line = false;
        // if char doesn't exist, skip it
        if (!font->HasChar(c)) {
          if (draw_background) {
            for (uint8_t x = 0; x < font->GetCharWidth(c); ++x) {
              for (uint8_t y = 0; y < font->height_; ++y) {
                DrawPixel(this_left + dx1 * x + dx2 * y,
                          this_top + dy1 * x + dy2 * y,
                          !on_value);
              }
            }
          }
          // draw background
          this_left += dx1 * font->GetCharWidth(c);
          this_top += dy1 * font->GetCharWidth(c);
          continue;
        }
        // print this character
        for (uint8_t x = 0; x < font->GetCharWidth(c); ++x) {
          for (uint8_t y = 0; y < font->height_; ++y) {
            // get coordinates of this pixel
            int32_t this_x = this_left + dx1 * x + dx2 * y;
            int32_t this_y = this_top + dy1 * x + dy2 * y;
            // if it's not in bounds, skip it
            if (!InBounds(this_x, this_y)) {
              continue;
            }
            if (font->GetPixel(c, x, y)) {
              DrawPixel(this_x, this_y, on_value);
            } else {
              if (draw_background) {
                DrawPixel(this_x, this_y, !on_value);
              }
            }
          }
        }
        this_left += dx1 * font->GetCharWidth(c);
        this_top += dy1 * font->GetCharWidth(c);
      }
    }
  }

  // draw a null-terminated string in the given position using the current font
  // do not draw background pixels
  // if value, then bits are turned to 1's, else bits are turned to 0's
  void DrawAlignedTextNoBackground(
      const char * string,
      int16_t x_offset,
      int16_t y_offset,
      GSL_FONT_AlignmentEnum alignment,
      bool value = true) {
    // first, get the total width and height of the text, including newlines
    //uint16_t total_width = font->GetStringWidth(string);
    uint16_t total_height = font->GetStringHeight(string);
    // get the top line
    int32_t top = y_offset;
    if (alignment == kGslFontAlignmentTopLeft ||
        alignment == kGslFontAlignmentTopCenter ||
        alignment == kGslFontAlignmentTopRight) {
    } else if (alignment == kGslFontAlignmentCenterLeft ||
               alignment == kGslFontAlignmentCenterCenter ||
               alignment == kGslFontAlignmentCenterRight) {
      top -= (total_height - 1) / 2;
    } else if (alignment == kGslFontAlignmentBottomLeft ||
               alignment == kGslFontAlignmentBottomCenter ||
               alignment == kGslFontAlignmentBottomRight) {
      top -= total_height - 1;
    } else {
      HALT("Unexpected value");
    }
    // process each line separately
    while (*string != 0) {
      // skip newlines
      if (*string == '\n') {
        top += font->height_ + font->space_between_lines_;
        ++string;
        continue;
      }
      // look for the end of this line
      const char * end = string;
      while (*end != 0 && *end != '\n') {
        ++end;
      }
      // get width of this substring
      uint16_t substring_width = 0;
      const char * ptr = string;
      while (ptr != end) {
        if (substring_width > 0) {
          substring_width += font->space_between_chars_;
        }
        substring_width += font->GetCharWidth(*ptr);
        ++ptr;
      }
      // find left pixel for this string
      int32_t left = x_offset;
      if (alignment == kGslFontAlignmentTopLeft ||
          alignment == kGslFontAlignmentCenterLeft ||
          alignment == kGslFontAlignmentBottomLeft) {
      } else if (alignment == kGslFontAlignmentTopCenter ||
                 alignment == kGslFontAlignmentCenterCenter ||
                 alignment == kGslFontAlignmentBottomCenter) {
        left -= (substring_width - 1) / 2;
      } else if (alignment == kGslFontAlignmentTopRight ||
                 alignment == kGslFontAlignmentCenterRight ||
                 alignment == kGslFontAlignmentBottomRight) {
        left -= substring_width - 1;
      } else {
        HALT("Unexpected value");
      }
      // draw each character
      //char end_char = *end;
      char text[2] = " ";
      while (string != end) {
        text[0] = *string;
        DrawTextNoBackground(text, left, top, value);
        left += font->space_between_chars_;
        left += font->GetCharWidth(*string);
        ++string;
      }
    }
  }

  // draw a null-terminated string in the given position using the current font
  void DrawOutlinedText(
      const char * text,
      int16_t x_offset,
      int16_t y_offset,
      bool text_value = true,
      bool outline_value = false) {
    ASSERT(font != nullptr);
    // save the starting position
    int16_t initial_x_offset = x_offset;
    // loop over each character
    while (*text) {
      // save this char
      char c = *text;
      ++text;
      // handle the newline special case
      if (c == '\n') {
        x_offset = initial_x_offset;
        y_offset += font->height_;
        y_offset += font->space_between_lines_;
        continue;
      }
      // skip space unless it's the first character on the line
      if (x_offset != initial_x_offset) {
        x_offset += font->space_between_chars_;
      }
      // get the character width
      uint8_t char_width = font->GetCharWidth(c);
      // print this character's outline
      for (uint8_t x = 0; x < char_width; ++x) {
        for (uint8_t y = 0; y < font->height_; ++y) {
          // if not in bounds, skip it
          if (!InBounds(x_offset + x, y_offset + y)) {
            continue;
          }
          if (font->GetPixel(c, x, y)) {
            // outline adjacent pixels
            if (x_offset + x > 0) {
              DrawPixel(x_offset + x - 1, y_offset + y, outline_value);
            }
            if (y_offset + y > 0) {
              DrawPixel(x_offset + x, y_offset + y - 1, outline_value);
            }
            if (x_offset + x < width - 1) {
              DrawPixel(x_offset + x + 1, y_offset + y, outline_value);
            }
            if (y_offset + y < height - 1) {
              DrawPixel(x_offset + x, y_offset + y + 1, outline_value);
            }
            // outline diagonally adjacent pixels
            if (x_offset + x > 0 && y_offset + y > 0) {
              DrawPixel(x_offset + x - 1, y_offset + y - 1, outline_value);
            }
            if (x_offset + x < width - 1 && y_offset + y > 0) {
              DrawPixel(x_offset + x + 1, y_offset + y - 1, outline_value);
            }
            if (x_offset + x > 0 && y_offset + y < height - 1) {
              DrawPixel(x_offset + x - 1, y_offset + y + 1, outline_value);
            }
            if (x_offset + x < width - 1 && y_offset + y < height - 1) {
              DrawPixel(x_offset + x + 1, y_offset + y + 1, outline_value);
            }
          }
        }
      }
      // print this character
      for (uint8_t x = 0; x < char_width; ++x) {
        for (uint8_t y = 0; y < font->height_; ++y) {
          // if not in bounds, skip it
          if (!InBounds(x_offset + x, y_offset + y)) {
            continue;
          }
          if (font->GetPixel(c, x, y)) {
            DrawPixel(x_offset + x, y_offset + y, text_value);
          }
        }
      }
      x_offset += char_width;
    }
  }

  // scroll display to the right by the given number of pixels
  /*void ScrollRight(uint16_t amount, bool wrapping) {
    HALT("TODO");
    uint8_t * end = buffer + buffer_size;
    // can't scroll more than the width
    if (amount >= width && !wrapping) {
      Clear();
      return;
    }
    amount %= width;
    // loop through each line
    for (uint8_t * ptr = buffer; ptr != end; ptr += width_in_bytes) {

    }
  }*/

};
