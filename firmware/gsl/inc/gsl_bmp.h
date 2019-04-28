#pragma once

#include "gsl_includes.h"

// color data
enum GSL_BMP_ColorEnum : uint8_t {
  // monochrome, 1 = on/white, 2 = off/black
  kGslBmpColorMonochrome,
  kGslBmpColor4Bit,
};

// bit packing data
enum GSL_BMP_PackingEnum : uint8_t {
  // bits are packed starting with top-left and going through each row
  // first pixel is the LSB of the first byte
  kGslBmpPackingHorizontal,
  // bits are packed starting with top-left and going through each row
  // each row is padded with 0 bits to finish out a byte
  // first pixel is the LSB of the first byte
  kGslBmpPackingHorizontalPadded,
  // bits are packed starting with top-left and going through each row
  // each row is padded with 0 bits to finish out a byte
  // first pixel is the MSB of the first byte
  kGslBmpPackingHorizontalPaddedMSB,
};

// bitmap info struct
struct GSL_BMP_InfoStruct {
  // width in pixels
  uint16_t width;
  // height in pixels
  uint16_t height;
  // color data
  GSL_BMP_ColorEnum color;
  // packing mode
  GSL_BMP_PackingEnum packing;
  // raw data
  const unsigned char * data;
};

// return a pixel color in the given bitmap
uint32_t GSL_BMP_GetPixel(
    const GSL_BMP_InfoStruct * bitmap,
    uint16_t x,
    uint16_t y) {
  if (bitmap->packing == kGslBmpPackingHorizontal &&
      bitmap->color == kGslBmpColorMonochrome) {
    uint32_t bit_offset = y * bitmap->width + x;
    uint32_t byte_offset = bit_offset / 8;
    bit_offset %= 8;
    uint32_t color = bitmap->data[byte_offset] & (0x80 >> bit_offset);
    color >>= (7 - bit_offset);
    return color;
  } else if (bitmap->packing == kGslBmpPackingHorizontalPaddedMSB &&
             bitmap->color == kGslBmpColorMonochrome) {
    uint32_t bytes_per_line = (bitmap->width - 1) / 8 + 1;
    uint32_t bit_offset = y * bytes_per_line * 8 + x;
    uint32_t byte_offset = bit_offset / 8;
    bit_offset = 7 - bit_offset % 8;
    uint32_t color = bitmap->data[byte_offset] >> bit_offset;
    color &= 0b1;
    return color;
/*  } else if (bitmap->packing == kGslBmpPackingHorizontalPadded &&
             bitmap->color == kGslBmpColorMonochrome) {
      uint32_t bit_offset = y * ((bitmap->width + 7) / 8) * 8 + x;
      uint32_t byte_offset = bit_offset / 8;
      bit_offset %= 8;
      uint32_t color = bitmap->data[byte_offset] & (0x80 >> bit_offset);
      color >>= (7 - bit_offset);
      return color;*/
  } else if (bitmap->packing == kGslBmpPackingHorizontal &&
             bitmap->color == kGslBmpColor4Bit) {
    uint32_t bit_offset = (uint32_t) y * bitmap->width + x;
    bit_offset *= 4;
    uint32_t byte_offset = bit_offset / 8;
    bit_offset %= 8;
    ASSERT(bit_offset % 4 == 0);
    uint32_t color = bitmap->data[byte_offset];
    if (bit_offset == 4) {
      color <<= 4;
    }
    color >>= 4;
    color &= 0b00001111;
    return color;
  } else {
    ASSERT(0);
  }
}
