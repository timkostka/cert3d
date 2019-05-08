#pragma once

#include "gsl_includes.h"

// enum for a particular GPIO pin
// (Referring to pins in this manner is a bit easier and less prone to error
// than storing separate variables for the port and pin number.)
// kPinInvalid - trying to initialize this will cause an error
// kPinNone - initializations will be ignored
enum PinEnum : uint8_t {
    kPinInvalid = 0,
    kPinNone = 1,
    kPinA0, kPinA1, kPinA2, kPinA3, kPinA4, kPinA5, kPinA6, kPinA7,
    kPinA8, kPinA9, kPinA10, kPinA11, kPinA12, kPinA13, kPinA14, kPinA15,
    kPinB0, kPinB1, kPinB2, kPinB3, kPinB4, kPinB5, kPinB6, kPinB7,
    kPinB8, kPinB9, kPinB10, kPinB11, kPinB12, kPinB13, kPinB14, kPinB15,
    kPinC0, kPinC1, kPinC2, kPinC3, kPinC4, kPinC5, kPinC6, kPinC7,
    kPinC8, kPinC9, kPinC10, kPinC11, kPinC12, kPinC13, kPinC14, kPinC15,
#ifdef GPIOD
    kPinD0, kPinD1, kPinD2, kPinD3, kPinD4, kPinD5, kPinD6, kPinD7,
    kPinD8, kPinD9, kPinD10, kPinD11, kPinD12, kPinD13, kPinD14, kPinD15,
#endif
#ifdef GPIOE
    kPinE0, kPinE1, kPinE2, kPinE3, kPinE4, kPinE5, kPinE6, kPinE7,
    kPinE8, kPinE9, kPinE10, kPinE11, kPinE12, kPinE13, kPinE14, kPinE15,
#endif
#ifdef GPIOF
    kPinF0, kPinF1, kPinF2, kPinF3, kPinF4, kPinF5, kPinF6, kPinF7,
    kPinF8, kPinF9, kPinF10, kPinF11, kPinF12, kPinF13, kPinF14, kPinF15,
#endif
#ifdef GPIOG
    kPinG0, kPinG1, kPinG2, kPinG3, kPinG4, kPinG5, kPinG6, kPinG7,
    kPinG8, kPinG9, kPinG10, kPinG11, kPinG12, kPinG13, kPinG14, kPinG15,
#endif
#ifdef GPIOH
    kPinH0, kPinH1, kPinH2, kPinH3, kPinH4, kPinH5, kPinH6, kPinH7,
    kPinH8, kPinH9, kPinH10, kPinH11, kPinH12, kPinH13, kPinH14, kPinH15,
#endif
#ifdef GPIOI
    kPinI0, kPinI1, kPinI2, kPinI3, kPinI4, kPinI5, kPinI6, kPinI7,
    kPinI8, kPinI9, kPinI10, kPinI11, kPinI12, kPinI13, kPinI14, kPinI15,
#endif
#ifdef GPIOJ
    kPinJ0, kPinJ1, kPinJ2, kPinJ3, kPinJ4, kPinJ5, kPinJ6, kPinJ7,
    kPinJ8, kPinJ9, kPinJ10, kPinJ11, kPinJ12, kPinJ13, kPinJ14, kPinJ15,
#endif
#ifdef GPIOK
    kPinK0, kPinK1, kPinK2, kPinK3, kPinK4, kPinK5, kPinK6, kPinK7,
    kPinK8, kPinK9, kPinK10, kPinK11, kPinK12, kPinK13, kPinK14, kPinK15,
#endif
    kPinFinal,
    };
