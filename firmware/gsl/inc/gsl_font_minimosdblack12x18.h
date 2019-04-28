#pragma once

// This is a 12x18 font used by the MinimOSD

#include "gsl_includes.h"

// character data
const uint8_t gsl_font_minimosdblack12x18_data[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // '0'
    0x00, 0x00, 0x00, 0x1E, 0x02, 0x10, 0x5E, 0x85, 0x28,
    0x52, 0x85, 0x28, 0x52, 0x85, 0xE8, 0x21, 0x01, 0xE0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // '1'
    0x00, 0x00, 0x00, 0x0C, 0x01, 0x40, 0x24, 0x03, 0x40,
    0x14, 0x01, 0x40, 0x14, 0x03, 0x60, 0x22, 0x03, 0xE0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // '2'
    0x00, 0x00, 0x00, 0x1E, 0x02, 0x10, 0x5E, 0x87, 0x50,
    0x0A, 0x01, 0x40, 0x28, 0x05, 0xF0, 0x40, 0x83, 0xF0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // '3'
    0x00, 0x00, 0x00, 0x3C, 0x04, 0x20, 0x3D, 0x01, 0xD0,
    0x22, 0x01, 0xD0, 0x05, 0x03, 0xD0, 0x42, 0x03, 0xC0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // '4'
    0x00, 0x00, 0x00, 0x04, 0x00, 0xA0, 0x12, 0x02, 0xA0,
    0x5A, 0x04, 0x10, 0x7A, 0x00, 0xA0, 0x0A, 0x00, 0x40,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // '5'
    0x00, 0x00, 0x00, 0x3E, 0x04, 0x10, 0x5E, 0x05, 0xC0,
    0x42, 0x03, 0xD0, 0x05, 0x03, 0xD0, 0x41, 0x03, 0xE0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // '6'
    0x00, 0x00, 0x00, 0x1C, 0x02, 0x20, 0x5C, 0x05, 0x00,
    0x5C, 0x04, 0x20, 0x5D, 0x05, 0xD0, 0x22, 0x01, 0xC0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // '7'
    0x00, 0x00, 0x00, 0x3F, 0x04, 0x08, 0x3E, 0x80, 0x50,
    0x0A, 0x01, 0x40, 0x14, 0x01, 0x40, 0x14, 0x00, 0x80,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // '8'
    0x00, 0x00, 0x00, 0x1E, 0x02, 0x10, 0x5E, 0x85, 0xE8,
    0x21, 0x05, 0xE8, 0x52, 0x85, 0xE8, 0x21, 0x01, 0xE0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // '9'
    0x00, 0x00, 0x00, 0x1E, 0x02, 0x10, 0x5E, 0x85, 0xE8,
    0x20, 0x81, 0xE8, 0x02, 0x83, 0xE8, 0x41, 0x03, 0xE0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ':'
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x01, 0x40,
    0x08, 0x00, 0x00, 0x08, 0x01, 0x40, 0x08, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // ';'
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x01, 0x40,
    0x08, 0x00, 0x00, 0x08, 0x01, 0x40, 0x28, 0x01, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // '<'
    0x01, 0x80, 0x24, 0x04, 0x80, 0x90, 0x12, 0x02, 0x40,
    0x24, 0x01, 0x20, 0x09, 0x00, 0x48, 0x02, 0x40, 0x18,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // '='
    0x00, 0x01, 0xF8, 0x20, 0x42, 0x04, 0x1F, 0x80, 0x00,
    0x00, 0x01, 0xF8, 0x20, 0x42, 0x04, 0x1F, 0x80, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // '>'
    0x30, 0x04, 0x80, 0x24, 0x01, 0x20, 0x09, 0x00, 0x48,
    0x04, 0x80, 0x90, 0x12, 0x02, 0x40, 0x48, 0x03, 0x00,
    0x00, 0x00, 0x00, 0x1F, 0x82, 0x04, 0x40, 0x28, 0xF1, // '?'
    0x90, 0x96, 0x09, 0x01, 0x10, 0x22, 0x04, 0x40, 0x88,
    0x09, 0x00, 0x60, 0x06, 0x00, 0x90, 0x09, 0x00, 0x60,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x01, 0x08, // '@'
    0x2F, 0x45, 0xDA, 0xB2, 0xDA, 0xCD, 0xAE, 0xDA, 0xCD,
    0xB2, 0xAA, 0xD4, 0x50, 0xC2, 0xFA, 0x10, 0x40, 0xF8,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'A'
    0x06, 0x00, 0x90, 0x10, 0x81, 0x08, 0x26, 0x42, 0x64,
    0x26, 0x42, 0x04, 0x26, 0x42, 0x64, 0x26, 0x41, 0x98,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'B'
    0x1F, 0x02, 0x08, 0x20, 0x42, 0x64, 0x26, 0x42, 0x08,
    0x20, 0x42, 0x64, 0x26, 0x42, 0x04, 0x20, 0x81, 0xF0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'C'
    0x0F, 0x01, 0x08, 0x10, 0x42, 0x64, 0x25, 0x82, 0x40,
    0x24, 0x02, 0x58, 0x26, 0x41, 0x04, 0x10, 0x80, 0xF0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'D'
    0x1F, 0x02, 0x08, 0x20, 0x42, 0x64, 0x26, 0x42, 0x64,
    0x26, 0x42, 0x64, 0x26, 0x42, 0x04, 0x20, 0x81, 0xF0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'E'
    0x1F, 0x82, 0x04, 0x20, 0x42, 0x78, 0x26, 0x02, 0x10,
    0x21, 0x02, 0x60, 0x27, 0x82, 0x04, 0x20, 0x41, 0xF8,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'F'
    0x1F, 0x82, 0x04, 0x20, 0x42, 0x78, 0x26, 0x02, 0x10,
    0x21, 0x02, 0x60, 0x24, 0x02, 0x40, 0x24, 0x01, 0x80,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'G'
    0x0F, 0x01, 0x08, 0x20, 0x42, 0x64, 0x27, 0x82, 0x78,
    0x24, 0x42, 0x44, 0x26, 0x42, 0x04, 0x10, 0x80, 0xF0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'H'
    0x19, 0x82, 0x64, 0x26, 0x42, 0x64, 0x26, 0x42, 0x04,
    0x20, 0x42, 0x64, 0x26, 0x42, 0x64, 0x26, 0x41, 0x98,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'I'
    0x06, 0x00, 0x90, 0x09, 0x00, 0x90, 0x09, 0x00, 0x90,
    0x09, 0x00, 0x90, 0x09, 0x00, 0x90, 0x09, 0x00, 0x60,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'J'
    0x01, 0x80, 0x24, 0x02, 0x40, 0x24, 0x02, 0x40, 0x24,
    0x1A, 0x42, 0x64, 0x26, 0x42, 0x04, 0x10, 0x80, 0xF0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'K'
    0x18, 0xC2, 0x52, 0x26, 0x22, 0x44, 0x20, 0x82, 0x10,
    0x20, 0x82, 0x44, 0x26, 0x42, 0x52, 0x25, 0x21, 0x8C,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'L'
    0x18, 0x02, 0x40, 0x24, 0x02, 0x40, 0x24, 0x02, 0x40,
    0x24, 0x02, 0x40, 0x27, 0x82, 0x04, 0x20, 0x41, 0xF8,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'M'
    0x30, 0xC4, 0x92, 0x49, 0x24, 0x62, 0x40, 0x24, 0x02,
    0x49, 0x24, 0xF2, 0x49, 0x24, 0x92, 0x49, 0x23, 0x0C,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'N'
    0x19, 0x82, 0x64, 0x26, 0x42, 0x24, 0x20, 0x42, 0x04,
    0x20, 0x42, 0x44, 0x26, 0x42, 0x64, 0x26, 0x41, 0x98,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'O'
    0x0F, 0x01, 0x08, 0x20, 0x42, 0x64, 0x26, 0x42, 0x64,
    0x26, 0x42, 0x64, 0x26, 0x42, 0x04, 0x10, 0x80, 0xF0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'P'
    0x1F, 0x02, 0x08, 0x20, 0x42, 0x64, 0x26, 0x42, 0x04,
    0x20, 0x82, 0x70, 0x24, 0x02, 0x40, 0x24, 0x01, 0x80,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, // 'Q'
    0x10, 0x82, 0x04, 0x26, 0x42, 0x64, 0x26, 0x42, 0x64,
    0x24, 0x42, 0x44, 0x20, 0x41, 0x04, 0x0F, 0x20, 0x0C,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'R'
    0x1F, 0x02, 0x08, 0x20, 0x42, 0x64, 0x26, 0x42, 0x64,
    0x20, 0x82, 0x48, 0x26, 0x42, 0x64, 0x26, 0x41, 0x98,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'S'
    0x0F, 0x01, 0x08, 0x20, 0x42, 0x64, 0x27, 0x81, 0x10,
    0x08, 0x81, 0xE4, 0x26, 0x42, 0x04, 0x10, 0x80, 0xF0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'T'
    0x1F, 0x82, 0x04, 0x20, 0x41, 0x98, 0x09, 0x00, 0x90,
    0x09, 0x00, 0x90, 0x09, 0x00, 0x90, 0x09, 0x00, 0x60,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'U'
    0x19, 0x82, 0x64, 0x26, 0x42, 0x64, 0x26, 0x42, 0x64,
    0x26, 0x42, 0x64, 0x26, 0x42, 0x04, 0x10, 0x80, 0xF0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'V'
    0x19, 0x82, 0x64, 0x26, 0x42, 0x64, 0x26, 0x42, 0x64,
    0x26, 0x42, 0x64, 0x10, 0x81, 0x08, 0x09, 0x00, 0x60,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'W'
    0x30, 0xC4, 0x92, 0x49, 0x24, 0x92, 0x49, 0x24, 0xF2,
    0x49, 0x24, 0x92, 0x20, 0x42, 0x64, 0x26, 0x41, 0x98,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'X'
    0x30, 0xC4, 0x92, 0x49, 0x22, 0x64, 0x20, 0x41, 0x08,
    0x10, 0x82, 0x04, 0x26, 0x44, 0x92, 0x49, 0x23, 0x0C,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'Y'
    0x19, 0x82, 0x64, 0x26, 0x42, 0x64, 0x10, 0x81, 0x08,
    0x09, 0x00, 0x90, 0x09, 0x00, 0x90, 0x09, 0x00, 0x60,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'Z'
    0x1F, 0x82, 0x04, 0x20, 0x41, 0xE4, 0x04, 0x40, 0x88,
    0x11, 0x02, 0x20, 0x27, 0x82, 0x04, 0x20, 0x41, 0xF8,
    0x00, 0x00, 0x00, 0x07, 0x80, 0x84, 0x08, 0x40, 0x98, // '['
    0x09, 0x00, 0x90, 0x09, 0x00, 0x90, 0x09, 0x00, 0x90,
    0x09, 0x80, 0x84, 0x08, 0x40, 0x78, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x09, 0x00, // '\\'
    0x88, 0x04, 0x40, 0x22, 0x01, 0x10, 0x08, 0x80, 0x44,
    0x02, 0x20, 0x11, 0x00, 0x90, 0x06, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x1E, 0x02, 0x10, 0x21, 0x01, 0x90, // ']'
    0x09, 0x00, 0x90, 0x09, 0x00, 0x90, 0x09, 0x00, 0x90,
    0x19, 0x02, 0x10, 0x21, 0x01, 0xE0, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x09, 0x01, 0x08, // '^'
    0x26, 0x42, 0x94, 0x30, 0xC0, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // '_'
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x07, 0xFE, 0x80, 0x18, 0x01, 0x7F, 0xE0, 0x00,
    0x00, 0x03, 0x00, 0x48, 0x02, 0x40, 0x12, 0x00, 0x90, // '`'
    0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'a'
    0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x11, 0x02, 0xE8,
    0x2E, 0x82, 0x08, 0x2E, 0x82, 0xA8, 0x2A, 0x81, 0x10,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'b'
    0x00, 0x00, 0x00, 0x00, 0x01, 0xF0, 0x20, 0x82, 0xF4,
    0x2F, 0x42, 0x08, 0x2F, 0x42, 0xF4, 0x20, 0x81, 0xF0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'c'
    0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x10, 0x82, 0xF4,
    0x28, 0x82, 0x80, 0x28, 0x82, 0xF4, 0x10, 0x80, 0xF0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'd'
    0x00, 0x00, 0x00, 0x00, 0x03, 0xE0, 0x21, 0x02, 0xE8,
    0x2A, 0x82, 0xA8, 0x2A, 0x82, 0xE8, 0x21, 0x03, 0xE0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'e'
    0x00, 0x00, 0x00, 0x00, 0x01, 0xF0, 0x20, 0x82, 0xF0,
    0x2E, 0x02, 0x10, 0x2E, 0x02, 0xF0, 0x20, 0x81, 0xF0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'f'
    0x00, 0x00, 0x00, 0x00, 0x01, 0xF0, 0x20, 0x82, 0xF0,
    0x2E, 0x02, 0x10, 0x2E, 0x02, 0x80, 0x28, 0x01, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'g'
    0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x10, 0x82, 0xF4,
    0x28, 0x82, 0xF8, 0x2C, 0x42, 0xF4, 0x10, 0x80, 0xF0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'h'
    0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x28, 0x82, 0xE8,
    0x20, 0x82, 0xE8, 0x2A, 0x82, 0xA8, 0x2A, 0x81, 0x10,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'i'
    0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x11, 0x00, 0xA0,
    0x0A, 0x00, 0xA0, 0x0A, 0x00, 0xA0, 0x11, 0x00, 0xE0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'j'
    0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x08, 0x80, 0x50,
    0x05, 0x00, 0x50, 0x15, 0x02, 0xD0, 0x12, 0x00, 0xC0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'k'
    0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x2A, 0x82, 0xD0,
    0x2A, 0x02, 0x40, 0x2A, 0x02, 0xD0, 0x2A, 0x81, 0x10,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'l'
    0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x28, 0x02, 0x80,
    0x28, 0x02, 0x80, 0x28, 0x02, 0xF0, 0x20, 0x81, 0xF0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'm'
    0x00, 0x00, 0x00, 0x00, 0x03, 0x18, 0x4A, 0x45, 0x54,
    0x5B, 0x45, 0x54, 0x51, 0x45, 0x14, 0x51, 0x42, 0x08,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'n'
    0x00, 0x00, 0x00, 0x00, 0x01, 0x88, 0x25, 0x42, 0xB4,
    0x2D, 0x42, 0xE4, 0x2B, 0x42, 0x94, 0x29, 0x41, 0x08,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'o'
    0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x11, 0x02, 0xE8,
    0x2A, 0x82, 0xA8, 0x2A, 0x82, 0xE8, 0x11, 0x00, 0xE0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'p'
    0x00, 0x00, 0x00, 0x00, 0x01, 0xE0, 0x21, 0x02, 0xE8,
    0x2E, 0x82, 0x10, 0x2E, 0x02, 0x80, 0x28, 0x01, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'q'
    0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x11, 0x02, 0xE8,
    0x2E, 0x82, 0xE8, 0x28, 0x82, 0xC8, 0x10, 0x80, 0xF8,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'r'
    0x00, 0x00, 0x00, 0x00, 0x01, 0xE0, 0x21, 0x02, 0xE8,
    0x2E, 0x82, 0x10, 0x2A, 0x02, 0xD0, 0x2A, 0x81, 0x10,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 's'
    0x00, 0x00, 0x00, 0x00, 0x00, 0xF0, 0x10, 0x82, 0xF4,
    0x2F, 0x81, 0x08, 0x1F, 0x42, 0xF4, 0x10, 0x80, 0xF0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 't'
    0x00, 0x00, 0x00, 0x00, 0x01, 0xFC, 0x20, 0x21, 0xDC,
    0x05, 0x00, 0x50, 0x05, 0x00, 0x50, 0x05, 0x00, 0x20,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'u'
    0x00, 0x00, 0x00, 0x00, 0x01, 0x08, 0x29, 0x42, 0x94,
    0x29, 0x42, 0x94, 0x29, 0x42, 0xF4, 0x10, 0x80, 0xF0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'v'
    0x00, 0x00, 0x00, 0x00, 0x01, 0x08, 0x29, 0x42, 0x94,
    0x29, 0x42, 0x94, 0x29, 0x41, 0x68, 0x09, 0x00, 0x60,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'w'
    0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x2A, 0x82, 0xE8,
    0x2A, 0x82, 0xA8, 0x2A, 0x82, 0x08, 0x15, 0x00, 0xA0,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'x'
    0x00, 0x00, 0x00, 0x00, 0x03, 0x9C, 0x26, 0x41, 0x08,
    0x09, 0x00, 0x90, 0x10, 0x82, 0x64, 0x29, 0x41, 0x08,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'y'
    0x00, 0x00, 0x00, 0x00, 0x01, 0x10, 0x2A, 0x82, 0xA8,
    0x15, 0x01, 0x10, 0x0A, 0x00, 0xA0, 0x0A, 0x00, 0x40,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 'z'
    0x00, 0x00, 0x00, 0x00, 0x01, 0xF0, 0x20, 0x81, 0xE8,
    0x05, 0x00, 0xA0, 0x14, 0x02, 0xF0, 0x20, 0x81, 0xF0};

// font data
const GSL_FONT_FontStruct gsl_font_minimosdblack12x18 = {
    12, // width
    18, // height (not a typo, this is 8 and not 7)
    0, // space between chars
    0, // space between lines
    '0', // starting character
    'z', // ending character
    kGslFontMonospaceRowMajorMSBPacked, // font data format
    gsl_font_minimosdblack12x18_data, // pixel data
    sizeof(gsl_font_minimosdblack12x18_data), // pixel data size
    nullptr, // width data
    0}; // width data size
