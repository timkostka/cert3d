#pragma once

// This file provides the necessary includes for each gsl file.  The intention
// is to have a single include line at the top of each "gsl_*.h" file instead
// of including each one separately.

// for memset
#include <string.h>

// include system include file
#include "stm32f4xx.h"

// include user-defined definitions
#include "defines.h"

#include "gsl_pin_defines.h"
#include "gsl_error.h" // needs PIN_DEFINES

#include "gsl_initializer.h"

#include "gsl_pin.h"

#include "gsl_buf.h"

#include "gsl_codelock.h"

#include "gsl_fifo.h"

#include "gsl_signal.h"

#include "gsl_bitpacker.h"
#include "gsl_bitvector.h"
#include "gsl_vector.h"

#include "gsl_clk.h"

#include "gsl_out.h"

#include "gsl_dataset.h"

#include "gsl_tim_defines.h"
#include "gsl_tim.h"

#include "gsl_rtc.h"

#include "gsl_secnt.h" // needs TIM

#include "gsl_del.h" // needs INITIALIZER, SECNT

#include "gsl_lpfval.h" // needs DEL

#include "gsl_button.h" // needs DEL

#include "gsl_crc.h" // needs DEL and OUT

#include "gsl_gen.h" // needs CRC

#include "gsl_fir.h" // needs GEN

#include "gsl_col.h" // needs GEN

#include "gsl_colsig.h"

#include "gsl_led.h"
#include "gsl_rgbled.h"

#include "gsl_date_defines.h"
#include "gsl_date.h"

#include "gsl_dbuf.h"
#include "gsl_cbuf.h"

#include "gsl_exti.h"

#include "gsl_adc_defines.h"
#include "gsl_spi_defines.h"
#include "gsl_spirx_defines.h"
#include "gsl_spim_defines.h"
#include "gsl_i2c_defines.h"
#include "gsl_uart_defines.h"
#include "gsl_uartrx_defines.h"
#include "gsl_uarttx_defines.h"

#ifdef DAC
#include "gsl_dac_defines.h"
#endif

#include "gsl_dma.h" // needs the peripheral defines

#include "gsl_adc.h"
#include "gsl_spi.h"
#include "gsl_spirx.h"
#include "gsl_spim.h"
#include "gsl_i2c.h"
#include "gsl_uart.h"
#include "gsl_uartrx.h"
#include "gsl_uarttx.h"

#include "gsl_delog.h" // needs UART

#include "gsl_pro.h"

#ifdef DAC
#include "gsl_dac.h"
#endif

#include "gsl_led.h"
#include "gsl_fwf.h"
#include "gsl_ina226.h"
#include "gsl_ina260.h"
#include "gsl_tmp100.h"
#include "gsl_sohstream.h"
#include "gsl_b64.h"
#include "gsl_bmp.h"

#include "gsl_bmp_ohm.h"

#include "gsl_pset.h"

#include "gsl_font.h"

#include "gsl_font_3x5.h"
#include "gsl_font_5x7var.h"
#include "gsl_font_10x14.h"
#include "gsl_font_5x7.h"
#include "gsl_font_6x10boldvar.h"
#include "gsl_font_12x20boldvar.h"
#include "gsl_font_13x24digits.h"
#include "gsl_font_6x14digits.h"
#include "gsl_font_adafruit5x7.h"
#include "gsl_font_arial11pt.h"
#include "gsl_font_arialbold12pt.h"
#include "gsl_font_minimosdblack12x18.h"
#include "gsl_font_minimosdwhite12x18.h"

#include "gsl_msbuf.h"

#include "gsl_ssd1306_bitmaps.h"
#include "gsl_ssd1306.h"

#include "gsl_bmp280.h"

#include "gsl_ptc2075.h"

#include "gsl_lcd_hx8357d.h"

#include "gsl_rtc6715.h"

#include "gsl_touch_stmpe610.h"

#include "gsl_tsm.h"

#include "gsl_otp.h"
