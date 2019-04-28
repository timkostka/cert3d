#pragma once

#include "gsl_includes.h"

// pins structure
struct GSL_TIM_PinsStruct {
  // CH1
  PinEnum ch1;
  // CH2
  PinEnum ch2;
  // CH3
  PinEnum ch3;
  // CH4
  PinEnum ch4;
  // ETR
  PinEnum etr;
  // CH1N
  PinEnum ch1n;
  // CH2N
  PinEnum ch2n;
  // CH3N
  PinEnum ch3n;
};

// triggers
struct GSL_TIM_TriggerStruct {
  TIM_TypeDef * target;
  TIM_TypeDef * source;
  uint32_t trigger;
};

const GSL_TIM_TriggerStruct gsl_tim_trigger[] = {
    {TIM1, TIM5, TIM_TS_ITR0},
    {TIM1, TIM2, TIM_TS_ITR1},
    {TIM1, TIM3, TIM_TS_ITR2},
    {TIM1, TIM4, TIM_TS_ITR3},

    {TIM8, TIM1, TIM_TS_ITR0},
    {TIM8, TIM2, TIM_TS_ITR1},
    {TIM8, TIM4, TIM_TS_ITR2},
    {TIM8, TIM5, TIM_TS_ITR3},

    {TIM2, TIM1, TIM_TS_ITR0},
    {TIM2, TIM8, TIM_TS_ITR1},
    {TIM2, TIM3, TIM_TS_ITR2},
    {TIM2, TIM4, TIM_TS_ITR3},

    {TIM3, TIM1, TIM_TS_ITR0},
    {TIM3, TIM2, TIM_TS_ITR1},
    {TIM3, TIM5, TIM_TS_ITR2},
    {TIM3, TIM4, TIM_TS_ITR3},

    {TIM4, TIM1, TIM_TS_ITR0},
    {TIM4, TIM2, TIM_TS_ITR1},
    {TIM4, TIM3, TIM_TS_ITR2},
    {TIM4, TIM8, TIM_TS_ITR3},

    {TIM5, TIM2, TIM_TS_ITR0},
    {TIM5, TIM3, TIM_TS_ITR1},
    {TIM5, TIM4, TIM_TS_ITR2},
    {TIM5, TIM8, TIM_TS_ITR3},

    {TIM9, TIM2, TIM_TS_ITR0},
    {TIM9, TIM3, TIM_TS_ITR1},

    {TIM12, TIM4, TIM_TS_ITR0},
    {TIM12, TIM5, TIM_TS_ITR1},
};

// number of triggers
const uint16_t gsl_tim_trigger_count =
    sizeof(gsl_tim_trigger) / sizeof(*gsl_tim_trigger);

// ************
// *** TIM1 ***
// ************

// TIM1_CH1 pin
#ifndef GSL_TIM1_PIN_CH1
#define GSL_TIM1_PIN_CH1 kPinA8
#endif

// TIM1_CH2 pin
#ifndef GSL_TIM1_PIN_CH2
#define GSL_TIM1_PIN_CH2 kPinA9
#endif

// TIM1_CH3 pin
#ifndef GSL_TIM1_PIN_CH3
#define GSL_TIM1_PIN_CH3 kPinA10
#endif

// TIM1_CH4 pin
#ifndef GSL_TIM1_PIN_CH4
#define GSL_TIM1_PIN_CH4 kPinA11
#endif

// TIM1_ETR pin
#ifndef GSL_TIM1_PIN_ETR
#define GSL_TIM1_PIN_ETR kPinA12
#endif

// TIM1_CH1N pin
#ifndef GSL_TIM1_PIN_CH1N
#define GSL_TIM1_PIN_CH1N kPinA7
#endif

// TIM1_CH2N pin
#ifndef GSL_TIM1_PIN_CH2N
#define GSL_TIM1_PIN_CH2N kPinB0
#endif

// TIM1_CH3N pin
#ifndef GSL_TIM1_PIN_CH3N
#define GSL_TIM1_PIN_CH3N kPinB1
#endif

// pins
GSL_TIM_PinsStruct gsl_tim1_pins = {
    GSL_TIM1_PIN_CH1,
    GSL_TIM1_PIN_CH2,
    GSL_TIM1_PIN_CH3,
    GSL_TIM1_PIN_CH4,
    GSL_TIM1_PIN_ETR,
    GSL_TIM1_PIN_CH1N,
    GSL_TIM1_PIN_CH2N,
    GSL_TIM1_PIN_CH3N};

// structure for handling TIM1 functions
TIM_HandleTypeDef gsl_tim1_htim = {
    TIM1,
    {
        0xFFFF, // Prescaler
        TIM_COUNTERMODE_UP, // CounterMode
        0xFFFF, // Period
        TIM_CLOCKDIVISION_DIV1, // ClockDivision
        0x00, // RepetitionCounter
    },
    HAL_TIM_ACTIVE_CHANNEL_CLEARED,
    {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
    HAL_UNLOCKED,
    HAL_TIM_STATE_RESET};

// ************
// *** TIM2 ***
// ************

// TIM2_CH1 pin
#ifndef GSL_TIM2_PIN_CH1
#define GSL_TIM2_PIN_CH1 kPinA0
#endif

// TIM2_CH2 pin
#ifndef GSL_TIM2_PIN_CH2
#define GSL_TIM2_PIN_CH2 kPinA1
#endif

// TIM2_CH3 pin
#ifndef GSL_TIM2_PIN_CH3
#define GSL_TIM2_PIN_CH3 kPinA2
#endif

// TIM2_CH4 pin
#ifndef GSL_TIM2_PIN_CH4
#define GSL_TIM2_PIN_CH4 kPinA3
#endif

// TIM2_ETR pin
#ifndef GSL_TIM2_PIN_ETR
#define GSL_TIM2_PIN_ETR kPinA5
#endif

// pins
GSL_TIM_PinsStruct gsl_tim2_pins = {
    GSL_TIM2_PIN_CH1,
    GSL_TIM2_PIN_CH2,
    GSL_TIM2_PIN_CH3,
    GSL_TIM2_PIN_CH4,
    GSL_TIM2_PIN_ETR,
    kPinInvalid,
    kPinInvalid,
    kPinInvalid};

// structure for handling TIM2 functions
TIM_HandleTypeDef gsl_tim2_htim = {
    TIM2,
    {
        0xFFFF, // Prescaler
        TIM_COUNTERMODE_UP, // CounterMode
        0xFFFF, // Period
        TIM_CLOCKDIVISION_DIV1, // ClockDivision
        0x00, // RepetitionCounter
    },
    HAL_TIM_ACTIVE_CHANNEL_CLEARED,
    {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
    HAL_UNLOCKED,
    HAL_TIM_STATE_RESET};

// ************
// *** TIM3 ***
// ************

// TIM3_CH1 pin
#ifndef GSL_TIM3_PIN_CH1
#define GSL_TIM3_PIN_CH1 kPinA6
#endif

// TIM3_CH2 pin
#ifndef GSL_TIM3_PIN_CH2
#define GSL_TIM3_PIN_CH2 kPinA7
#endif

// TIM3_CH3 pin
#ifndef GSL_TIM3_PIN_CH3
#define GSL_TIM3_PIN_CH3 kPinB0
#endif

// TIM3_CH4 pin
#ifndef GSL_TIM3_PIN_CH4
#define GSL_TIM3_PIN_CH4 kPinB1
#endif

// TIM3_ETR pin
#ifndef GSL_TIM3_PIN_ETR
#define GSL_TIM3_PIN_ETR kPinD2
#endif

// pins
GSL_TIM_PinsStruct gsl_tim3_pins = {
    GSL_TIM3_PIN_CH1,
    GSL_TIM3_PIN_CH2,
    GSL_TIM3_PIN_CH3,
    GSL_TIM3_PIN_CH4,
    GSL_TIM3_PIN_ETR,
    kPinInvalid,
    kPinInvalid,
    kPinInvalid};

// structure for handling TIM3 functions
TIM_HandleTypeDef gsl_tim3_htim = {
    TIM3,
    {
        0xFFFF, // Prescaler
        TIM_COUNTERMODE_UP, // CounterMode
        0xFFFF, // Period
        TIM_CLOCKDIVISION_DIV1, // ClockDivision
        0x00, // RepetitionCounter
    },
    HAL_TIM_ACTIVE_CHANNEL_CLEARED,
    {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
    HAL_UNLOCKED,
    HAL_TIM_STATE_RESET};

// ************
// *** TIM4 ***
// ************

// TIM4_CH1 pin
#ifndef GSL_TIM4_PIN_CH1
#define GSL_TIM4_PIN_CH1 kPinB6
#endif

// TIM4_CH2 pin
#ifndef GSL_TIM4_PIN_CH2
#define GSL_TIM4_PIN_CH2 kPinB7
#endif

// TIM4_CH3 pin
#ifndef GSL_TIM4_PIN_CH3
#define GSL_TIM4_PIN_CH3 kPinB8
#endif

// TIM4_CH4 pin
#ifndef GSL_TIM4_PIN_CH4
#define GSL_TIM4_PIN_CH4 kPinB9
#endif

// TIM4_ETR pin
#ifndef GSL_TIM4_PIN_ETR
#define GSL_TIM4_PIN_ETR kPinE0
#endif

// pins
GSL_TIM_PinsStruct gsl_tim4_pins = {
    GSL_TIM4_PIN_CH1,
    GSL_TIM4_PIN_CH2,
    GSL_TIM4_PIN_CH3,
    GSL_TIM4_PIN_CH4,
    GSL_TIM4_PIN_ETR,
    kPinInvalid,
    kPinInvalid,
    kPinInvalid};

// structure for handling TIM4 functions
TIM_HandleTypeDef gsl_tim4_htim = {
    TIM4,
    {
        0xFFFF, // Prescaler
        TIM_COUNTERMODE_UP, // CounterMode
        0xFFFF, // Period
        TIM_CLOCKDIVISION_DIV1, // ClockDivision
        0x00, // RepetitionCounter
    },
    HAL_TIM_ACTIVE_CHANNEL_CLEARED,
    {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
    HAL_UNLOCKED,
    HAL_TIM_STATE_RESET};

// ************
// *** TIM5 ***
// ************

// TIM5_CH1 pin
#ifndef GSL_TIM5_PIN_CH1
#define GSL_TIM5_PIN_CH1 kPinA0
#endif

// TIM5_CH2 pin
#ifndef GSL_TIM5_PIN_CH2
#define GSL_TIM5_PIN_CH2 kPinA1
#endif

// TIM5_CH3 pin
#ifndef GSL_TIM5_PIN_CH3
#define GSL_TIM5_PIN_CH3 kPinA2
#endif

// TIM5_CH4 pin
#ifndef GSL_TIM5_PIN_CH4
#define GSL_TIM5_PIN_CH4 kPinA3
#endif

// TIM5_ETR pin
#define GSL_TIM5_PIN_ETR kPinInvalid

// pins
GSL_TIM_PinsStruct gsl_tim5_pins = {
    GSL_TIM5_PIN_CH1,
    GSL_TIM5_PIN_CH2,
    GSL_TIM5_PIN_CH3,
    GSL_TIM5_PIN_CH4,
    GSL_TIM5_PIN_ETR,
    kPinInvalid,
    kPinInvalid,
    kPinInvalid};

// structure for handling TIM5 functions
TIM_HandleTypeDef gsl_tim5_htim = {
    TIM5,
    {
        0xFFFF, // Prescaler
        TIM_COUNTERMODE_UP, // CounterMode
        0xFFFF, // Period
        TIM_CLOCKDIVISION_DIV1, // ClockDivision
        0x00, // RepetitionCounter
    },
    HAL_TIM_ACTIVE_CHANNEL_CLEARED,
    {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
    HAL_UNLOCKED,
    HAL_TIM_STATE_RESET};

// ************
// *** TIM6 ***
// ************

#ifdef TIM6

// TIM6_CH1 pin
#define GSL_TIM6_PIN_CH1 kPinInvalid

// TIM6_CH2 pin
#define GSL_TIM6_PIN_CH2 kPinInvalid

// TIM6_CH3 pin
#define GSL_TIM6_PIN_CH3 kPinInvalid

// TIM6_CH4 pin
#define GSL_TIM6_PIN_CH4 kPinInvalid

// TIM6_ETR pin
#define GSL_TIM6_PIN_ETR kPinInvalid

// pins
GSL_TIM_PinsStruct gsl_tim6_pins = {
    GSL_TIM6_PIN_CH1,
    GSL_TIM6_PIN_CH2,
    GSL_TIM6_PIN_CH3,
    GSL_TIM6_PIN_CH4,
    GSL_TIM6_PIN_ETR,
    kPinInvalid,
    kPinInvalid,
    kPinInvalid};

// structure for handling TIM6 functions
TIM_HandleTypeDef gsl_tim6_htim = {
    TIM6,
    {
        0xFFFF, // Prescaler
        TIM_COUNTERMODE_UP, // CounterMode
        0xFFFF, // Period
        TIM_CLOCKDIVISION_DIV1, // ClockDivision
        0x00, // RepetitionCounter
    },
    HAL_TIM_ACTIVE_CHANNEL_CLEARED,
    {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
    HAL_UNLOCKED,
    HAL_TIM_STATE_RESET};

#endif // TIM6

// ************
// *** TIM7 ***
// ************

#ifdef TIM7

// TIM7_CH1 pin
#define GSL_TIM7_PIN_CH1 kPinInvalid

// TIM7_CH2 pin
#define GSL_TIM7_PIN_CH2 kPinInvalid

// TIM7_CH3 pin
#define GSL_TIM7_PIN_CH3 kPinInvalid

// TIM7_CH4 pin
#define GSL_TIM7_PIN_CH4 kPinInvalid

// TIM7_ETR pin
#define GSL_TIM7_PIN_ETR kPinInvalid

// pins
GSL_TIM_PinsStruct gsl_tim7_pins = {
    GSL_TIM7_PIN_CH1,
    GSL_TIM7_PIN_CH2,
    GSL_TIM7_PIN_CH3,
    GSL_TIM7_PIN_CH4,
    GSL_TIM7_PIN_ETR,
    kPinInvalid,
    kPinInvalid,
    kPinInvalid};

// structure for handling TIM7 functions
TIM_HandleTypeDef gsl_tim7_htim = {
    TIM7,
    {
        0xFFFF, // Prescaler
        TIM_COUNTERMODE_UP, // CounterMode
        0xFFFF, // Period
        TIM_CLOCKDIVISION_DIV1, // ClockDivision
        0x00, // RepetitionCounter
    },
    HAL_TIM_ACTIVE_CHANNEL_CLEARED,
    {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
    HAL_UNLOCKED,
    HAL_TIM_STATE_RESET};

#endif

// ************
// *** TIM8 ***
// ************

#ifdef TIM8

// TIM8_CH1 pin
#ifndef GSL_TIM8_PIN_CH1
#define GSL_TIM8_PIN_CH1 kPinC6
#endif

// TIM8_CH2 pin
#ifndef GSL_TIM8_PIN_CH2
#define GSL_TIM8_PIN_CH2 kPinC7
#endif

// TIM8_CH3 pin
#ifndef GSL_TIM8_PIN_CH3
#define GSL_TIM8_PIN_CH3 kPinC8
#endif

// TIM8_CH4 pin
#ifndef GSL_TIM8_PIN_CH4
#define GSL_TIM8_PIN_CH4 kPinC9
#endif

// TIM8_ETR pin
#ifndef GSL_TIM8_PIN_ETR
#define GSL_TIM8_PIN_ETR kPinA0
#endif

// TIM8_CH1N pin
#ifndef GSL_TIM8_PIN_CH1N
#define GSL_TIM8_PIN_CH1N kPinA5
#endif

// TIM8_CH2N pin
#ifndef GSL_TIM8_PIN_CH2N
#define GSL_TIM8_PIN_CH2N kPinB14
#endif

// TIM8_CH3N pin
#ifndef GSL_TIM8_PIN_CH3N
#define GSL_TIM8_PIN_CH3N kPinB15
#endif

// pins
GSL_TIM_PinsStruct gsl_tim8_pins = {
    GSL_TIM8_PIN_CH1,
    GSL_TIM8_PIN_CH2,
    GSL_TIM8_PIN_CH3,
    GSL_TIM8_PIN_CH4,
    GSL_TIM8_PIN_ETR,
    kPinInvalid,
    kPinInvalid,
    kPinInvalid};

// structure for handling TIM8 functions
TIM_HandleTypeDef gsl_tim8_htim = {
    TIM8,
    {
        0xFFFF, // Prescaler
        TIM_COUNTERMODE_UP, // CounterMode
        0xFFFF, // Period
        TIM_CLOCKDIVISION_DIV1, // ClockDivision
        0x00, // RepetitionCounter
    },
    HAL_TIM_ACTIVE_CHANNEL_CLEARED,
    {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
    HAL_UNLOCKED,
    HAL_TIM_STATE_RESET};

#endif

// ************
// *** TIM9 ***
// ************

#ifdef TIM9

// TIM9_CH1 pin
#ifndef GSL_TIM9_PIN_CH1
#define GSL_TIM9_PIN_CH1 kPinA2
#endif

// TIM9_CH2 pin
#ifndef GSL_TIM9_PIN_CH2
#define GSL_TIM9_PIN_CH2 kPinA3
#endif

// TIM9_CH3 pin
#define GSL_TIM9_PIN_CH3 kPinInvalid

// TIM9_CH4 pin
#define GSL_TIM9_PIN_CH4 kPinInvalid

// TIM9_ETR pin
#define GSL_TIM9_PIN_ETR kPinInvalid

// pins
GSL_TIM_PinsStruct gsl_tim9_pins = {
    GSL_TIM9_PIN_CH1,
    GSL_TIM9_PIN_CH2,
    GSL_TIM9_PIN_CH3,
    GSL_TIM9_PIN_CH4,
    GSL_TIM9_PIN_ETR,
    kPinInvalid,
    kPinInvalid,
    kPinInvalid};

// structure for handling TIM9 functions
TIM_HandleTypeDef gsl_tim9_htim = {
    TIM9,
    {
        0xFFFF, // Prescaler
        TIM_COUNTERMODE_UP, // CounterMode
        0xFFFF, // Period
        TIM_CLOCKDIVISION_DIV1, // ClockDivision
        0x00, // RepetitionCounter
    },
    HAL_TIM_ACTIVE_CHANNEL_CLEARED,
    {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
    HAL_UNLOCKED,
    HAL_TIM_STATE_RESET};

#endif

// *************
// *** TIM10 ***
// *************

#ifdef TIM10

// TIM10_CH1 pin
#ifndef GSL_TIM10_PIN_CH1
#define GSL_TIM10_PIN_CH1 kPinB8
#endif

// TIM10_CH2 pin
#define GSL_TIM10_PIN_CH2 kPinInvalid

// TIM10_CH3 pin
#define GSL_TIM10_PIN_CH3 kPinInvalid

// TIM10_CH4 pin
#define GSL_TIM10_PIN_CH4 kPinInvalid

// TIM10_ETR pin
#define GSL_TIM10_PIN_ETR kPinInvalid

// pins
GSL_TIM_PinsStruct gsl_tim10_pins = {
    GSL_TIM10_PIN_CH1,
    GSL_TIM10_PIN_CH2,
    GSL_TIM10_PIN_CH3,
    GSL_TIM10_PIN_CH4,
    GSL_TIM10_PIN_ETR,
    kPinInvalid,
    kPinInvalid,
    kPinInvalid};

// structure for handling TIM10 functions
TIM_HandleTypeDef gsl_tim10_htim = {
    TIM10,
    {
        0xFFFF, // Prescaler
        TIM_COUNTERMODE_UP, // CounterMode
        0xFFFF, // Period
        TIM_CLOCKDIVISION_DIV1, // ClockDivision
        0x00, // RepetitionCounter
    },
    HAL_TIM_ACTIVE_CHANNEL_CLEARED,
    {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
    HAL_UNLOCKED,
    HAL_TIM_STATE_RESET};

#endif

// *************
// *** TIM11 ***
// *************

#ifdef TIM11

// TIM11_CH1 pin
#ifndef GSL_TIM11_PIN_CH1
#define GSL_TIM11_PIN_CH1 kPinB9
#endif

// TIM11_CH2 pin
#define GSL_TIM11_PIN_CH2 kPinInvalid

// TIM11_CH3 pin
#define GSL_TIM11_PIN_CH3 kPinInvalid

// TIM11_CH4 pin
#define GSL_TIM11_PIN_CH4 kPinInvalid

// TIM11_ETR pin
#define GSL_TIM11_PIN_ETR kPinInvalid

// pins
GSL_TIM_PinsStruct gsl_tim11_pins = {
    GSL_TIM11_PIN_CH1,
    GSL_TIM11_PIN_CH2,
    GSL_TIM11_PIN_CH3,
    GSL_TIM11_PIN_CH4,
    GSL_TIM11_PIN_ETR,
    kPinInvalid,
    kPinInvalid,
    kPinInvalid};

// structure for handling TIM11 functions
TIM_HandleTypeDef gsl_tim11_htim = {
    TIM11,
    {
        0xFFFF, // Prescaler
        TIM_COUNTERMODE_UP, // CounterMode
        0xFFFF, // Period
        TIM_CLOCKDIVISION_DIV1, // ClockDivision
        0x00, // RepetitionCounter
    },
    HAL_TIM_ACTIVE_CHANNEL_CLEARED,
    {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
    HAL_UNLOCKED,
    HAL_TIM_STATE_RESET};

#endif

// *************
// *** TIM12 ***
// *************

#ifdef TIM12

// TIM12_CH1 pin
#ifndef GSL_TIM12_PIN_CH1
#define GSL_TIM12_PIN_CH1 kPinB14
#endif

// TIM12_CH2 pin
#ifndef GSL_TIM12_PIN_CH2
#define GSL_TIM12_PIN_CH2 kPinB15
#endif

// TIM12_CH3 pin
#define GSL_TIM12_PIN_CH3 kPinInvalid

// TIM12_CH4 pin
#define GSL_TIM12_PIN_CH4 kPinInvalid

// TIM12_ETR pin
#define GSL_TIM12_PIN_ETR kPinInvalid

// pins
GSL_TIM_PinsStruct gsl_tim12_pins = {
    GSL_TIM12_PIN_CH1,
    GSL_TIM12_PIN_CH2,
    GSL_TIM12_PIN_CH3,
    GSL_TIM12_PIN_CH4,
    GSL_TIM12_PIN_ETR,
    kPinInvalid,
    kPinInvalid,
    kPinInvalid};

// structure for handling TIM12 functions
TIM_HandleTypeDef gsl_tim12_htim = {
    TIM12,
    {
        0xFFFF, // Prescaler
        TIM_COUNTERMODE_UP, // CounterMode
        0xFFFF, // Period
        TIM_CLOCKDIVISION_DIV1, // ClockDivision
        0x00, // RepetitionCounter
    },
    HAL_TIM_ACTIVE_CHANNEL_CLEARED,
    {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
    HAL_UNLOCKED,
    HAL_TIM_STATE_RESET};

#endif

// *************
// *** TIM13 ***
// *************

#ifdef TIM13

// TIM13_CH1 pin
#ifndef GSL_TIM13_PIN_CH1
#define GSL_TIM13_PIN_CH1 kPinA6
#endif

// TIM13_CH2 pin
#define GSL_TIM13_PIN_CH2 kPinInvalid

// TIM13_CH3 pin
#define GSL_TIM13_PIN_CH3 kPinInvalid

// TIM13_CH4 pin
#define GSL_TIM13_PIN_CH4 kPinInvalid

// TIM13_ETR pin
#define GSL_TIM13_PIN_ETR kPinInvalid

// pins
GSL_TIM_PinsStruct gsl_tim13_pins = {
    GSL_TIM13_PIN_CH1,
    GSL_TIM13_PIN_CH2,
    GSL_TIM13_PIN_CH3,
    GSL_TIM13_PIN_CH4,
    GSL_TIM13_PIN_ETR,
    kPinInvalid,
    kPinInvalid,
    kPinInvalid};

// structure for handling TIM13 functions
TIM_HandleTypeDef gsl_tim13_htim = {
    TIM13,
    {
        0xFFFF, // Prescaler
        TIM_COUNTERMODE_UP, // CounterMode
        0xFFFF, // Period
        TIM_CLOCKDIVISION_DIV1, // ClockDivision
        0x00, // RepetitionCounter
    },
    HAL_TIM_ACTIVE_CHANNEL_CLEARED,
    {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
    HAL_UNLOCKED,
    HAL_TIM_STATE_RESET};

#endif

// *************
// *** TIM14 ***
// *************

#ifdef TIM14

// TIM14_CH1 pin
#ifndef GSL_TIM14_PIN_CH1
#define GSL_TIM14_PIN_CH1 kPinA7
#endif

// TIM14_CH2 pin
#define GSL_TIM14_PIN_CH2 kPinInvalid

// TIM14_CH3 pin
#define GSL_TIM14_PIN_CH3 kPinInvalid

// TIM14_CH4 pin
#define GSL_TIM14_PIN_CH4 kPinInvalid

// TIM14_ETR pin
#define GSL_TIM14_PIN_ETR kPinInvalid

// pins
GSL_TIM_PinsStruct gsl_tim14_pins = {
    GSL_TIM14_PIN_CH1,
    GSL_TIM14_PIN_CH2,
    GSL_TIM14_PIN_CH3,
    GSL_TIM14_PIN_CH4,
    GSL_TIM14_PIN_ETR,
    kPinInvalid,
    kPinInvalid,
    kPinInvalid};

// structure for handling TIM14 functions
TIM_HandleTypeDef gsl_tim14_htim = {
    TIM14,
    {
        0xFFFF, // Prescaler
        TIM_COUNTERMODE_UP, // CounterMode
        0xFFFF, // Period
        TIM_CLOCKDIVISION_DIV1, // ClockDivision
        0x00, // RepetitionCounter
    },
    HAL_TIM_ACTIVE_CHANNEL_CLEARED,
    {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr},
    HAL_UNLOCKED,
    HAL_TIM_STATE_RESET};

#endif

// end of timer definitions

// this holds pointers to the various structures we need
struct GSL_TIM_InfoStruct {
  // pointer to the timer
  TIM_TypeDef * TIMx;
  // pointer to the timer handle
  TIM_HandleTypeDef * handle;
  // pointer to the pin structure
  GSL_TIM_PinsStruct * pins;
  // IRQ for updates
  IRQn_Type irq_update;
  // callback for timer update
  void (*UpdateCallback)(void);
  // true when timer is locked
  bool locked;
  // pointer to AHB EN register
  volatile uint32_t * ahb_en_reg;
  // enable bit mask
  uint32_t ahb_en_mask;
};

// here we define the info structure which points to all the various other
// structures
GSL_TIM_InfoStruct GSL_TIM_Info[] = {

#ifdef TIM1
    {
        TIM1,
        &gsl_tim1_htim,
        &gsl_tim1_pins,
        TIM1_UP_TIM10_IRQn,
        nullptr,
        false,
        &RCC->APB2ENR,
        RCC_APB2ENR_TIM1EN,
    },
#endif // #ifdef TIM1

#ifdef TIM2
    {
        TIM2,
        &gsl_tim2_htim,
        &gsl_tim2_pins,
        TIM2_IRQn,
        nullptr,
        false,
        &RCC->APB1ENR,
        RCC_APB1ENR_TIM2EN,
    },
#endif // #ifdef TIM2

#ifdef TIM3
    {
        TIM3,
        &gsl_tim3_htim,
        &gsl_tim3_pins,
        TIM3_IRQn,
        nullptr,
        false,
        &RCC->APB1ENR,
        RCC_APB1ENR_TIM3EN,
    },
#endif // #ifdef TIM3

#ifdef TIM4
    {
        TIM4,
        &gsl_tim4_htim,
        &gsl_tim4_pins,
        TIM4_IRQn,
        nullptr,
        false,
        &RCC->APB1ENR,
        RCC_APB1ENR_TIM4EN,
    },
#endif // #ifdef TIM4

#ifdef TIM5
    {
        TIM5,
        &gsl_tim5_htim,
        &gsl_tim5_pins,
        TIM5_IRQn,
        nullptr,
        false,
        &RCC->APB1ENR,
        RCC_APB1ENR_TIM5EN,
    },
#endif // #ifdef TIM5

#ifdef TIM6
    {
        TIM6,
        &gsl_tim6_htim,
        &gsl_tim6_pins,
        TIM6_DAC_IRQn,
        nullptr,
        false,
        &RCC->APB1ENR,
        RCC_APB1ENR_TIM6EN,
    },
#endif // #ifdef TIM6

#ifdef TIM7
    {
        TIM7,
        &gsl_tim7_htim,
        &gsl_tim7_pins,
        TIM7_IRQn,
        nullptr,
        false,
        &RCC->APB1ENR,
        RCC_APB1ENR_TIM7EN,
    },
#endif // #ifdef TIM7

#ifdef TIM8
    {
        TIM8,
        &gsl_tim8_htim,
        &gsl_tim8_pins,
        TIM8_UP_TIM13_IRQn,
        nullptr,
        false,
        &RCC->APB2ENR,
        RCC_APB2ENR_TIM8EN,
    },
#endif // #ifdef TIM8

#ifdef TIM9
    {
        TIM9,
        &gsl_tim9_htim,
        &gsl_tim9_pins,
        TIM1_BRK_TIM9_IRQn,
        nullptr,
        false,
        &RCC->APB2ENR,
        RCC_APB2ENR_TIM9EN,
    },
#endif // #ifdef TIM9

#ifdef TIM10
    {
        TIM10,
        &gsl_tim10_htim,
        &gsl_tim10_pins,
        TIM1_UP_TIM10_IRQn,
        nullptr,
        false,
        &RCC->APB2ENR,
        RCC_APB2ENR_TIM10EN,
    },
#endif // #ifdef TIM10

#ifdef TIM11
    {
        TIM11,
        &gsl_tim11_htim,
        &gsl_tim11_pins,
        TIM1_TRG_COM_TIM11_IRQn,
        nullptr,
        false,
        &RCC->APB2ENR,
        RCC_APB2ENR_TIM11EN,
    },
#endif // #ifdef TIM11

#ifdef TIM12
    {
        TIM12,
        &gsl_tim12_htim,
        &gsl_tim12_pins,
        TIM8_BRK_TIM12_IRQn,
        nullptr,
        false,
        &RCC->APB1ENR,
        RCC_APB1ENR_TIM12EN,
    },
#endif // #ifdef TIM12

#ifdef TIM13
    {
        TIM13,
        &gsl_tim13_htim,
        &gsl_tim13_pins,
        TIM8_UP_TIM13_IRQn,
        nullptr,
        false,
        &RCC->APB1ENR,
        RCC_APB1ENR_TIM13EN,
    },
#endif // #ifdef TIM3

#ifdef TIM14
    {
        TIM14,
        &gsl_tim14_htim,
        &gsl_tim14_pins,
        TIM8_TRG_COM_TIM14_IRQn,
        nullptr,
        false,
        &RCC->APB1ENR,
        RCC_APB1ENR_TIM14EN,
    },
#endif // #ifdef TIM14

};

// number of TIM peripherals
const uint32_t GSL_TIM_PeripheralCount =
    sizeof(GSL_TIM_Info) / sizeof(*GSL_TIM_Info);

// bytes of RAM used to store all the handler structs
const uint32_t GSL_TIM_StorageByteCount =
    sizeof(GSL_TIM_Info) +
    GSL_TIM_PeripheralCount * sizeof(TIM_HandleTypeDef) +
    GSL_TIM_PeripheralCount * sizeof(GSL_TIM_PinsStruct);

// timer to use for path processing callback
//TIM_TypeDef * const qb_process_timer = TIM2;
