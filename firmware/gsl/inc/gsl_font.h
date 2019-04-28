#pragma once

// This file is an interface for accessing fonts.

#include "gsl_includes.h"

// font character format
enum GSL_FONT_DataFormatEnum {
  // top left pixel is first, then the rest of the row
  // each row is an integer number of bytes, padded with zeros.
  // (this is the format the TheDotFactory outputs)
  kGslFontTopLeftRowMajor,
  // bottom left pixel is first, then the rest of the column
  kGslFontMonospaceColumnMajorLSB,
  // bottom left pixel is first, then the rest of the column
  kGslFontVariableColumnMajorLSB,
  // top left pixel is first (MSB), then the rest of the row, no padding
  kGslFontMonospaceRowMajorMSBPacked,
  // top left pixel is first (LSB), then the rest of the row, no padding
  // between rows but each character starts on a new byte
  kGslFontTopLeftRowMajorLSBPacked,
  // special case for the 3x5 font
  kGslFontMonospace3x5,
};

// string alignment
enum GSL_FONT_AlignmentEnum {
  kGslFontAlignmentTopLeft,
  kGslFontAlignmentTopCenter,
  kGslFontAlignmentTopRight,
  kGslFontAlignmentCenterLeft,
  kGslFontAlignmentCenterCenter,
  kGslFontAlignmentCenterRight,
  kGslFontAlignmentBottomLeft,
  kGslFontAlignmentBottomCenter,
  kGslFontAlignmentBottomRight,
};

// text direction
enum GSL_FONT_DirectionEnum {
  kGslFontDirectionToRight,
  kGslFontDirectionToTop,
  kGslFontDirectionToLeft,
  kGslFontDirectionToBottom,
};

// character width and offset struct
struct GSL_FONT_CharWidthStruct {
  // width in pixels
  uint8_t width;
  // offset into array in bytes
  uint16_t offset;
};

// font structure
struct GSL_FONT_FontStruct {
  // width of the space character
  uint8_t width_;
  // character height
  uint8_t height_;
  // space between consecutive characters
  uint8_t space_between_chars_;
  // space between lines
  uint8_t space_between_lines_;
  // the starting character, typically ' ' or '!'
  char starting_character_;
  // ending character, typically '~'
  char ending_character_;
  // character data format
  GSL_FONT_DataFormatEnum data_format_;
  // data array
  const uint8_t * char_data_;
  // size of the data array
  uint32_t char_data_size_;
  // width array, or nullptr for monospaced font
  const GSL_FONT_CharWidthStruct * width_data_;
  // size of the width data array
  uint32_t width_data_size_;
  // return true if font is monospaced
  bool IsMonospaced(void) const {
    return width_data_ == nullptr;
  }
  // return true if the character is included in the font
  bool HasChar(char c) const {
    return c >= starting_character_ && c <= ending_character_;
  }
  // return the bits used per column
  uint32_t BitsPerColumn(void) const {
    if (data_format_ == kGslFontMonospaceColumnMajorLSB) {
      return (height_ + 7) / 8 * 8;
    } else {
      HALT("Unexpected format");
    }
  }
  // return the bits used per character in the data array
  uint32_t BitsPerChar(void) const {
    if (data_format_ == kGslFontMonospaceColumnMajorLSB) {
      return width_ * BitsPerColumn();
    } else if (data_format_ == kGslFontMonospaceRowMajorMSBPacked) {
      return width_ * height_;
    } else if (data_format_ == kGslFontMonospace3x5) {
      return 16;
    } else {
      HALT("Unexpected format");
    }
  }
  // return a pixel in the font
  // on = true, off = false
  // (0, 0) is at the top left of the character
  bool GetPixel(char c, uint8_t x, uint8_t y) const {
    // special case -- all pixels in a space are off
    if (c == ' ') {
      return false;
    }
    // character bound checking
    if (!HasChar(c)) {
      return false;
    }
    // position bound checking
    if (y >= height_) {
      return false;
    }
    // get width of this char
    const uint16_t char_width = GetCharWidth(c);
    if (x >= char_width) {
      return false;
    }
    // get pointer to data array
    const uint8_t * data = char_data_;
    if (data_format_ == kGslFontMonospaceColumnMajorLSB) {
      // get offset to character
      data += (c - starting_character_) * BitsPerChar() / 8;
      // get offset to column
      data += x * BitsPerColumn() / 8;
      // return pixel
      data += y / 8;
      return (*data) & (1 << (y % 8));
    } else if (data_format_ == kGslFontMonospaceRowMajorMSBPacked) {
        // get offset to character
        uint16_t bit_offset = (c - starting_character_) * BitsPerChar();
        bit_offset += x + y * width_;
        // return pixel
        return data[bit_offset / 8] & (0x80 >> (bit_offset % 8));
    } else if (data_format_ == kGslFontMonospace3x5) {
      data += (c - starting_character_) * BitsPerChar() / 8;
      uint16_t value = 0;
      // first two rows
      value += (*data & 0b11111100) << 8;
      // last two rows
      value += (*(data + 1) & 0b11111100) >> 1;
      // middle row (why did he do it this way??)
      value += (*data & 0b00000011) << 8;
      value += (*(data + 1) & 0b00000010) << 6;
      // now we do a simple bit offset
      return value & ((0x8000) >> (y * 3 + x));
    } else if (data_format_ == kGslFontTopLeftRowMajor) {
      ASSERT(width_data_);
      // offset to character data
      data += width_data_[c - starting_character_].offset;
      // offset to row data
      data += (width_data_[c - starting_character_].width + 7) / 8 * y;
      return data[x / 8] & (0x80 >> (x % 8));
    } else if (data_format_ == kGslFontVariableColumnMajorLSB) {
      ASSERT(width_data_);
      // get offset to this character
      data += width_data_[c - starting_character_].offset;
      ASSERT(x < width_data_[c - starting_character_].width);
      data += x;
      // switch to bottom
      data += y / 8;
      return *data & (1 << (y % 8));
    } else if (data_format_ == kGslFontTopLeftRowMajorLSBPacked) {
      ASSERT(width_data_);
      // offset to character data
      data += width_data_[c - starting_character_].offset;
      uint16_t bit_offset = y * char_width + x;
      // offset to row data
      return data[bit_offset / 8] & (0x01 << (bit_offset % 8));
    } else {
      HALT("Unexpected format");
    }
  }
  // return the width of the given character
  uint16_t GetCharWidth(char c) const {
    if (IsMonospaced()) {
      return width_;
    }
    // check for space character, which isn't typically stored in the array
    if (c == ' ') {
      return width_;
    }
    // if not defined, it's zero width
    if (!HasChar(c)) {
      return 0;
    }
    // else return the width
    return width_data_[c - starting_character_].width;
  }
  // return the width of a null terminated string that may include newlines
  uint16_t GetStringWidth(const char * text) const {
    // if string is empty, width is 0
    if (*text == 0) {
      return 0;
    }
    uint16_t width = 0;
    while (*text != 0) {
      // skip newlines at the start
      if (*text == '\n') {
        ++text;
        continue;
      }
      // look for end of this line
      const char * end = text;
      while (*end != 0 && *end != '\n') {
        ++end;
      }
      uint16_t this_width = 0;
      while (text != end) {
        if (this_width != 0) {
          this_width += space_between_chars_;
        }
        this_width += GetCharWidth(*text);
        ++text;
      }
      if (this_width > width) {
        width = this_width;
      }
    }
    return width;

    // process each newline

    // if monospaced...
    if (IsMonospaced()) {
      uint16_t result = strlen(text);
      return result * width_ + (result - 1) * space_between_chars_;
    }
    // else this is variable spaced
    uint16_t result = -space_between_chars_;
    while (*text) {
      result += space_between_chars_;
      result += GetCharWidth(*text);
      ++text;
    }
    return result;
  }
  // return the height of a null terminated string
  // the \n character represents a newline
  uint16_t GetStringHeight(const char * text) const {
    // if string is empty, height is 0
    if (*text == 0) {
      return 0;
    }
    // count the number of additional newlines
    uint16_t newline_count = 1;
    while (*text) {
      if (*text == '\n') {
        ++newline_count;
      }
      ++text;
    }
    // calculate the height
    return newline_count * (height_ + space_between_lines_) -
        space_between_lines_;
  }
};

// perform a validity check on the given font
void GSL_FONT_Validate(const GSL_FONT_FontStruct & font) {
  uint8_t char_count = font.ending_character_ - font.starting_character_ + 1;
  // for variable width fonts, ensure the width array is valid
  if (font.width_data_) {
    uint16_t offset = 0;
    for (char c = font.starting_character_; c <= font.ending_character_; ++c) {
      ASSERT(font.width_data_[c - font.starting_character_].offset == offset);
      offset += font.width_data_[c - font.starting_character_].width;
    }
    ASSERT(font.char_data_size_ == offset);
  }
  // for monospaced fonts, ensure pixel data is the appropriate size
  if (!font.width_data_) {
    ASSERT(font.char_data_size_ == (uint32_t) font.width_ * char_count);
  }
}
