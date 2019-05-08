// This file includes several ohm symbol bitmaps

#pragma once

#include "gsl_includes.h"

// data
const uint8_t gsl_bmp_ohm_5x7_data[] = {
    0b01110000,
    0b10001000,
    0b10001000,
    0b10001000,
    0b01010000,
    0b01010000,
    0b11011000,
};

// Ohm symbol in 5x7 font
const GSL_BMP_InfoStruct gsl_bmp_ohm_5x7 = {
    5,
    7,
    kGslBmpColorMonochrome,
    kGslBmpPackingHorizontalPaddedMSB,
    gsl_bmp_ohm_5x7_data};

// data
const uint8_t gsl_bmp_ohm_10x14_data[] = {
    0b00111111, 0b00000000,
    0b01111111, 0b10000000,
    0b11100001, 0b11000000,
    0b11000000, 0b11000000,
    0b11000000, 0b11000000,
    0b11000000, 0b11000000,
    0b11000000, 0b11000000,
    0b11100001, 0b11000000,
    0b01110011, 0b10000000,
    0b00110011, 0b00000000,
    0b00110011, 0b00000000,
    0b00110011, 0b00000000,
    0b11110011, 0b11000000,
    0b11110011, 0b11000000,
};

// Ohm symbol in 10x14 font
const GSL_BMP_InfoStruct gsl_bmp_ohm_10x14 = {
    10,
    14,
    kGslBmpColorMonochrome,
    kGslBmpPackingHorizontalPaddedMSB,
    gsl_bmp_ohm_10x14_data};
