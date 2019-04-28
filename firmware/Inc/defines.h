#pragma once

// These are defines for the Cert3D rev1 board.

#define GSL_LOG_IRQ_ONCE

#define GSL_LED_ERROR_PIN kPinC15
#define GSL_LED_STATUS_PIN kPinNone

#define GSL_DELOG_CAPACITY 12192
#define GSL_DELOG_PORT USART3
#define GSL_DELOG_PRIORITY 12

// USART3 is the debug output
#define GSL_USART3_PIN_TX kPinB10
#define GSL_USART3_PIN_RX kPinB11
#define GSL_USART3_MODE USART_MODE_TX_RX

// TIM1 pins
#define GSL_TIM1_PIN_CH1 kPinA8
#define GSL_TIM1_PIN_CH2 kPinA9
#define GSL_TIM1_PIN_CH3 kPinA10
#define GSL_TIM1_PIN_CH4 kPinA11

// TIM3 pins
#define GSL_TIM3_PIN_CH1 kPinB4
#define GSL_TIM3_PIN_CH2 kPinB5

// TIM8 pins
#define GSL_TIM8_PIN_CH2 kPinC7
#define GSL_TIM8_PIN_CH4 kPinC9

// general purpose buffer size
#define GSL_BUF_CAPACITY 100000

// priority of the second counter interrupt
#define GSL_SECNT_PRIORITY 15

// DEBUG
#define GSL_UART4_BAUDRATE 100000
#define GSL_UART4_PIN_TX kPinC10
#define GSL_UART4_MODE USART_MODE_TX
