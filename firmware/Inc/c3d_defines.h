#pragma once

#include "c3d_includes.h"

// adc peripheral
ADC_TypeDef * const c3d_adc = ADC1;

// adc channels
const uint32_t c3d_adc_channel[] = {
    ADC_CHANNEL_10,
    ADC_CHANNEL_11,
    ADC_CHANNEL_12,
    ADC_CHANNEL_13,
    ADC_CHANNEL_0,
    ADC_CHANNEL_1,
    ADC_CHANNEL_2,
    ADC_CHANNEL_3,
    ADC_CHANNEL_6,
    ADC_CHANNEL_7,
    ADC_CHANNEL_14,
    ADC_CHANNEL_15,
    ADC_CHANNEL_8,
    ADC_CHANNEL_9,
};

// channel count
const uint16_t c3d_adc_channel_count =
    sizeof(c3d_adc_channel) / sizeof(*c3d_adc_channel);

// timer to use to trigger ADC readings
TIM_TypeDef * const c3d_adc_timer = TIM2;

// approximate frequency of ADC readings
const float c3d_adc_frequency = 1000.0f;

// high resistor on adc resistor divider
const float c3d_adc_high_resistor = 7500.0f;

// low resistor on adc resistor divider
const float c3d_adc_low_resistor = 1000.0f;

// ADC buffer size
const uint16_t c3d_adc_buffer_capacity = c3d_adc_channel_count * 4;

// ADC output buffer
uint16_t c3d_adc_buffer[c3d_adc_buffer_capacity];

// class for monitoring a DIR or STEP signal
struct SignalMonitorStruct {
  // name of channel
  const char * const name;
  // TIMER on this channel
  TIM_TypeDef * const TIMx;
  // timer channel
  uint32_t timer_channel;
  // DMA stream for this signal
  DMA_Stream_TypeDef * const dma_stream;
  // DMA channel
  const uint32_t dma_channel;
  // buffer
  uint16_t * buffer;
  // buffer capacity
  uint16_t const buffer_capacity;
};

// buffer for DIR signals
const uint16_t c3d_dir_buffer_capacity = 300;

// buffer for STEP signals
const uint16_t c3d_step_buffer_capacity = 300;

// channels we're monitoring
SignalMonitorStruct c3d_signal[] = {
    {"X_STEP", TIM1, TIM_CHANNEL_4, DMA2_Stream4, DMA_CHANNEL_6, nullptr, c3d_step_buffer_capacity},
    {"X_DIR", TIM1, TIM_CHANNEL_3, DMA2_Stream6, DMA_CHANNEL_6, nullptr, c3d_dir_buffer_capacity},
    {"Y_STEP", TIM1, TIM_CHANNEL_2, DMA2_Stream2, DMA_CHANNEL_6, nullptr, c3d_step_buffer_capacity},
    {"Y_DIR", TIM1, TIM_CHANNEL_1, DMA2_Stream1, DMA_CHANNEL_6, nullptr, c3d_dir_buffer_capacity},
    {"Z_STEP", TIM3, TIM_CHANNEL_1, DMA1_Stream4, DMA_CHANNEL_5, nullptr, c3d_step_buffer_capacity},
    {"Z_DIR", TIM3, TIM_CHANNEL_2, DMA1_Stream5, DMA_CHANNEL_5, nullptr, c3d_dir_buffer_capacity},
    {"E_STEP", TIM8, TIM_CHANNEL_4, DMA2_Stream7, DMA_CHANNEL_7, nullptr, c3d_step_buffer_capacity},
    {"E_DIR", TIM8, TIM_CHANNEL_2, DMA2_Stream3, DMA_CHANNEL_7, nullptr, c3d_dir_buffer_capacity},
};

// timer to trigger all other timers
TIM_TypeDef * const c3d_master_timer = TIM4;

// number of signals
const uint16_t c3d_signal_count = sizeof(c3d_signal) / sizeof(*c3d_signal);

// monitor for each signal DMA channel
C3D_DMA_Monitor<uint16_t> c3d_signal_usb_dma_monitor[c3d_signal_count];

// step monitor for each signal DMA channel
C3D_DMA_Monitor<uint16_t> c3d_signal_step_dma_monitor[c3d_signal_count];

// monitor for the ADC DMA channel
C3D_DMA_Monitor<uint16_t> c3d_adc_dma_monitor;

// number of times we processed data
uint32_t c3d_process_count = 0;

// number of edges sent
uint32_t c3d_edge_count = 0xFFFFFFFF;

// averaged output bits per process
GSL_LPFVAL c3d_output_bits_per_process(10.0f);

// chunk buffer for USB data
C3D_ChunkBuffer c3d_usb_buffer;

// if true, stream data to USB
bool c3d_output_to_usb = false;

// commands
struct CommandStruct {
  // command string
  const char * text;
  // function to call
  void (*function)(void);
};

void C3D_EnableStartStreamingFlag(void);
void C3D_StopStreaming(void);
void C3D_SendInfoPacket(void);
void C3D_Debug(void);
void C3D_Reset(void);

// commands
const CommandStruct c3d_command[] = {
    {"start", C3D_EnableStartStreamingFlag},
    {"stop", C3D_StopStreaming},
    {"info", C3D_SendInfoPacket},
    {"debug", C3D_Debug},
    {"reset", C3D_Reset},
};

// number of commands
const uint16_t c3d_command_count = sizeof(c3d_command) / sizeof(*c3d_command);

// debug flag
bool c3d_debug_flag = false;

// if True, will ignore data sent to USB
bool c3d_ignore_usb_output = false;

// trigger to start streaming
bool c3d_start_streaming_flag = false;

// number of axes
const uint16_t c3d_motor_count = c3d_signal_count / 2;
static_assert(c3d_signal_count % 2 == 0, "");

// stepper motor information
struct C3D_StepperMotorStruct {
  // steps per mm
  float steps_per_mm;
  // range in mm (or -1 if unlimited)
  float range_mm;
  // absolute position in steps
  int32_t step_position;
  // true if DIR signal is high
  bool dir_is_high;
  // true if STEP signal is high
  bool step_is_high;
  // half of the counter overflow value
  uint16_t half_counter_overflow;
  // mm range
  // steps at zero position
  int32_t zero_step_offset;
  // low endstop
  C3D_EndStopStruct low_stop;
  // high endstop
  C3D_EndStopStruct high_stop;
  // return absolute position in mm
  float GetPositionMM() const {
    return (step_position - zero_step_offset) / steps_per_mm;
  }
};

C3D_StepperMotorStruct c3d_motor[c3d_motor_count] = {
    {80.0f, 230.0f, 0, false, false, 32768, -1000, {kPinB7}, {kPinB6}},
    {80.0f, 230.0f, 0, false, false, 32768, -1000, {kPinB3}, {kPinD2}},
    {400.0f, 200.0f, 0, false, false, 32768 / 2, -1000, {kPinC12}, {kPinC11}},
    {93.0f, -1.0f, 0, false, false, 32768, -1000, {kPinC10}, {kPinA15}},
};

// printer geometry
struct C3D_PrinterGeometryStruct {
  // current absolute step position
  int32_t step_position[c3d_motor_count];
  // steps per mm
  float steps_per_mm[c3d_motor_count];
} c3d_geometry;
