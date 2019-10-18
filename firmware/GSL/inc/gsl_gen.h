#pragma once

// This contains general utilities used by other libraries that don't really
// fit well anywhere else.  In particular, this file has minimal dependencies.

#include "gsl_includes.h"

// minimum value macro
#define GSL_MIN(x, y) (((x) < (y)) ? (x) : (y))
#define GSL_MAX(x, y) (((x) > (y)) ? (x) : (y))

// time as a string in the format "HHMMSS"
#define FIRMWARE_TIME {__TIME__[0], __TIME__[1], \
  __TIME__[3], __TIME__[4], \
  __TIME__[6], __TIME__[7]}

// year macro as a string (e.g. "2015")
#define FIRMWARE_YEAR {__DATE__[7], __DATE__[8], __DATE__[9], __DATE__[10]}

// month as a string ("01" to "12")
#define FIRMWARE_MONTH (\
  __DATE__ [2] == 'n' ? (__DATE__ [1] == 'a' ? "01" : "06") \
: __DATE__ [2] == 'b' ? "02" \
: __DATE__ [2] == 'r' ? (__DATE__ [0] == 'M' ? "03" : "04") \
: __DATE__ [2] == 'y' ? "05" \
: __DATE__ [2] == 'l' ? "07" \
: __DATE__ [2] == 'g' ? "08" \
: __DATE__ [2] == 'p' ? "09" \
: __DATE__ [2] == 't' ? "10" \
: __DATE__ [2] == 'v' ? "11" \
: "12")

// day macro as a string ("01" to "31")
#define FIRMWARE_DAY {__DATE__[4] == ' ' ? '0' : '1', __DATE__[5]}

// timestamp is a null-terminated string of the form
// "YYYYMMDD_HHMMSS"
#define FIRMWARE_TIMESTAMP FIRMWARE_YEAR FIRMWARE_MONTH FIRMWARE_DAY "_" FIRMWARE_TIME

// return the compilation date
// format: YYYY-MMM-DD
// e.g. 2016-JUL-25
const char * GSL_GEN_GetFirmwareDatestampShort(void) {
  static char timestamp[] = "YYYY-MMM-DD";
  // update the first time it's called
  if (timestamp[0] == 'Y') {
    // year
    timestamp[0] = __DATE__[7];
    timestamp[1] = __DATE__[8];
    timestamp[2] = __DATE__[9];
    timestamp[3] = __DATE__[10];
    // month
    timestamp[5] = __DATE__ [0];
    timestamp[6] = __DATE__ [1];
    timestamp[7] = __DATE__ [2];
    for (uint8_t i = 5; i < 8; ++i) {
      if (timestamp[i] >= 'a' && timestamp[i] <= 'z') {
        timestamp[i] += 'A' - 'a';
      }
    }
    // day
    timestamp[9] = __DATE__[4] == ' ' ? '0' : __DATE__[4];
    timestamp[10] = __DATE__[5];
  }
  return timestamp;
}

// return the compilation time
// format: 12:23:11 AM
char * GSL_GEN_GetFirmwareTimestampShort(void) {
  static char timestamp[] = "HH:MM:SS AM";
  // update the first time it's called
  if (timestamp[0] == 'H') {
    // hour
    timestamp[0] = __TIME__[0];
    timestamp[1] = __TIME__[1];
    // minute
    timestamp[3] = __TIME__[3];
    timestamp[4] = __TIME__[4];
    // second
    timestamp[6] = __TIME__[6];
    timestamp[7] = __TIME__[7];

    uint8_t hour = (timestamp[0] - '0') * 10 + (timestamp[1] - '0');
    // do AM/PM
    if (hour == 0 || hour > 12) {
      timestamp[9] = 'P';
    } else {
      timestamp[9] = 'A';
    }
    if (hour == 0) {
      hour += 12;
    }
    if (hour > 12) {
      hour -= 12;
    }
    timestamp[0] = '0' + (hour / 10);
    timestamp[1] = '0' + (hour % 10);
  }
  return timestamp;
}

// return the compilation date and timestamp
// YYYYMMDD_hhmmss
char * GSL_GEN_GetFirmwareTimestamp(void) {
  static char timestamp[] = "YYYYMMDD_hhmmss";
  // initialize
  if (timestamp[0] == 'Y') {
    // year
    timestamp[0] = __DATE__[7];
    timestamp[1] = __DATE__[8];
    timestamp[2] = __DATE__[9];
    timestamp[3] = __DATE__[10];
    // month
    uint8_t month = 0;
    switch (__DATE__ [2]) {
      case 'n': month = (__DATE__ [1] == 'a' ? 1 : 6); break;
      case 'b': month = 2; break;
      case 'r': month = (__DATE__ [0] == 'M' ? 3 : 4); break;
      case 'y': month = 5; break;
      case 'l': month = 7; break;
      case 'g': month = 8; break;
      case 'p': month = 9; break;
      case 't': month = 10; break;
      case 'v': month = 11; break;
      case 'c': month = 12; break;
      default: break;
    }
    timestamp[4] = '0' + month / 10;
    timestamp[5] = '0' + month % 10;
    // day
    timestamp[6] = __DATE__[4] == ' ' ? '0' : __DATE__[4];
    timestamp[7] = __DATE__[5];
    // hour
    timestamp[9] = __TIME__[0];
    timestamp[10] = __TIME__[1];
    // minute
    timestamp[11] = __TIME__[3];
    timestamp[12] = __TIME__[4];
    // second
    timestamp[13] = __TIME__[6];
    timestamp[14] = __TIME__[7];
  }
  return timestamp;
}

// return the compilation date and timestamp in hex
// 0xYYYYMMDD 0xhhmmss (plus zero terminated)
char * GSL_GEN_GetFirmwareTimestampHex(void) {
  static char hex_timestamp[8] = "       ";
  char * timestamp = GSL_GEN_GetFirmwareTimestamp();
  hex_timestamp[0] = (timestamp[0] - '0') * 0x10 + (timestamp[1] - '0');
  hex_timestamp[1] = (timestamp[2] - '0') * 0x10 + (timestamp[3] - '0');
  hex_timestamp[2] = (timestamp[4] - '0') * 0x10 + (timestamp[5] - '0');
  hex_timestamp[3] = (timestamp[6] - '0') * 0x10 + (timestamp[7] - '0');
  hex_timestamp[4] = (timestamp[9] - '0') * 0x10 + (timestamp[10] - '0');
  hex_timestamp[5] = (timestamp[11] - '0') * 0x10 + (timestamp[12] - '0');
  hex_timestamp[6] = (timestamp[13] - '0') * 0x10 + (timestamp[14] - '0');
  hex_timestamp[7] = 0;
  return hex_timestamp;
}

// return the compilation date and timestamp
// YYYY-MM-DD hh:mm:ss
char * GSL_GEN_GetFirmwareTimestampLong(void) {
  static char long_timestamp[] = "YYYY-MM-DD hh:mm:ss";
  char * timestamp = GSL_GEN_GetFirmwareTimestamp();
  long_timestamp[0] = timestamp[0];
  long_timestamp[1] = timestamp[1];
  long_timestamp[2] = timestamp[2];
  long_timestamp[3] = timestamp[3];
  long_timestamp[5] = timestamp[4];
  long_timestamp[6] = timestamp[5];
  long_timestamp[8] = timestamp[6];
  long_timestamp[9] = timestamp[7];
  long_timestamp[11] = timestamp[9];
  long_timestamp[12] = timestamp[10];
  long_timestamp[14] = timestamp[11];
  long_timestamp[15] = timestamp[12];
  long_timestamp[17] = timestamp[13];
  long_timestamp[18] = timestamp[14];
  return long_timestamp;
}

// Return the timestamp for the given time
// Format: "DDD:HH:MM:SS.UUUUUU"
char * GSL_GEN_FormTimestamp(GSL_DEL_LongTime time) {
  // buffer to hold result
  static char buffer[20] = "DDD:HH:MM:SS.UUUUUU";
  // convert ticks into microseconds, seconds, minutes, hours days
  uint32_t seconds = time.ticks / gsl_del_system_clock;
  uint32_t microseconds =
      (time.ticks % gsl_del_system_clock) /
          (gsl_del_system_clock / 1e6f) + 0.5f;
  if (microseconds >= 1000000) {
    microseconds = 999999;
  }
  // convert seconds into minutes, hours, days
  uint32_t minutes = seconds / 60;
  seconds %= 60;
  uint32_t hours = minutes / 60;
  minutes %= 60;
  uint32_t days = hours / 24;
  hours %= 24;
  // micros
  buffer[18] = '0' + (microseconds % 10);
  microseconds /= 10;
  buffer[17] = '0' + (microseconds % 10);
  microseconds /= 10;
  buffer[16] = '0' + (microseconds % 10);
  microseconds /= 10;
  buffer[15] = '0' + (microseconds % 10);
  microseconds /= 10;
  buffer[14] = '0' + (microseconds % 10);
  microseconds /= 10;
  buffer[13] = '0' + microseconds;
  // seconds
  buffer[11] = '0' + (seconds % 10);
  seconds /= 10;
  buffer[10] = '0' + seconds;
  // minutes
  buffer[8] = '0' + (minutes % 10);
  minutes /= 10;
  buffer[7] = '0' + minutes;
  // hours
  buffer[5] = '0' + (hours % 10);
  hours /= 10;
  buffer[4] = '0' + hours;
  // days
  buffer[2] = '0' + (days % 10);
  buffer[1] = '0' + ((days / 10) % 10);
  buffer[0] = '0' + (days / 100);
  return buffer;
}


// Return the timestamp since reset (using DWT counter)
// Format: "DDD:HH:MM:SS.UUUUUU"
//
// (Native uint64_t modulo and division is not supported on the STM32, so we
// implement something that doesn't require it.  This is preferable to using a
// library which supports uint64_t division operations.)
//
// On my speed tests, this routine took ~221 ticks to run.
//
// To be valid, the ticks32 value must be a value taken recently (within the
// past few seconds)
char * GSL_GEN_GetTimestampSinceReset(uint32_t ticks = GSL_DEL_Ticks()) {
  // get the long time
  GSL_DEL_LongTime time = GSL_DEL_GetLongTime(ticks);
  return GSL_GEN_FormTimestamp(time);
}

// convert value to BCD format
inline uint8_t GSL_GEN_ToBCD(uint8_t value) {
  return value + (value / 10) * 6;
}

// convert value to BCD format
inline uint16_t GSL_GEN_ToBCD(uint16_t value) {
  return GSL_GEN_ToBCD((uint8_t) value) +
      0x0100 * GSL_GEN_ToBCD((uint8_t) (value >> 8));
}

// count the number of nonzero bits in the given value
uint32_t GSL_GEN_CountBits(uint32_t value) {
  uint32_t result = 0;
  while (value) {
    if (value & 1) {
      ++result;
    }
    value >>= 1;
  }
  return result;
}

// swap the endianness of the given 2-byte value
// (STM32 uses native little endian, e.g. (0x10 0x00) = 16)
uint16_t GSL_GEN_SwapEndian(uint16_t value) {
  return __REV16(value);
  //return (value >> 8) | (value << 8);
}

// swap the endianness of the given int16_t
int16_t GSL_GEN_SwapEndian(int16_t value) {
  return __REVSH(value);
}

// swap the endianness of the given 4-byte value
// (STM32 uses native little endian, e.g. (0x10 0x00 0x00 0x00) = 16)
uint32_t GSL_GEN_SwapEndian(uint32_t value) {
  return __REV(value);
  //return GSL_GEN_SwapEndian((uint16_t) (value >> 16)) |
  //    (GSL_GEN_SwapEndian((uint16_t) value) << 16);
}

// return the REV_ID code
// 0x1000 = Revision A
// 0x1001 = Revision Z
// 0x1003 = Revision Y
// 0x1007 = Revision 1
// 0x2001 = Revision 3
uint16_t GSL_GEN_GetRevID(void) {
  return DBGMCU->IDCODE >> 16;
}

// return the IDCODE code
// 0x413 --> STM32F405XX, STM32F407XX, STM32F415XX, STM32F417XX
// 0x419 --> STM32F42XXX and STM32F43XXX
uint16_t GSL_GEN_GetDevID(void) {
  return DBGMCU->IDCODE & 0x0FFF;
}

// return the flash size in kB (1kB = 1024 bytes)
uint16_t GSL_GEN_GetFlashSize(void) {
  return *((uint16_t *) FLASHSIZE_BASE);
}

// return the unique ID pointer (12 bytes)
uint8_t * GSL_GEN_GetUniqueID(void) {
  return (uint8_t *) UID_BASE;
}

// location of the start of the flash memory
uint32_t * const GSL_GEN_MemoryFlashStart = (uint32_t *)(0x08000000);

// return the size of the flash in use in bytes
// this will be a multiple of 4
uint32_t GSL_GEN_GetFlashSizeInUse(void) {

  uint32_t start = (uint32_t) GSL_GEN_MemoryFlashStart;
  uint32_t end = start + 1024 * GSL_GEN_GetFlashSize();
  uint32_t last_used = start;
  uint32_t erased_val = 0xFFFFFFFF;

  asm("mov r0, %[ev]\n"
      "mov r1, %[s]\n"
      "mov r3, %[e]\n"
      "mov r2, r1\n"
      "StartLoop%=:\n"
      "cmp r1, r3\n"
      "beq Exit%=\n"
      "ldr r4, [r1], #4\n"
      "cmp r4, r0\n"
      "beq StartLoop%=\n"
      "mov r2, r1\n"
      "b   StartLoop%=\n"
      "Exit%=:\n"
      "mov %[lu], r2\n"
  :
  [lu] "=r" (last_used)
  :
  [s] "r" (start),
  [e] "r" (end),
  [ev] "r" (erased_val)
  :
  "r0", "r1", "r2", "r3", "r4");

  return last_used - start;

}

// return the size of the flash in use in bytes
// this will be a multiple of 4
uint32_t OBSOLETE_GSL_GEN_GetFlashSizeInUse(void) {

  uint32_t * last_used = GSL_GEN_MemoryFlashStart - 1;
  uint32_t * end =
      GSL_GEN_MemoryFlashStart +
          (GSL_GEN_GetFlashSize() * 1024 / sizeof(*end));
  const uint32_t kErasedValue = 0xFFFFFFFF;

  for (uint32_t * ptr = GSL_GEN_MemoryFlashStart; ptr < end; ++ptr) {
    if (*ptr != kErasedValue) {
      last_used = ptr;
    }
  }

  return (last_used - GSL_GEN_MemoryFlashStart + 1) *
      sizeof(*GSL_GEN_MemoryFlashStart);
}

// returns the CRC32 of the flash in use
uint32_t GSL_GEN_GetFlashCRC(void) {
  return GSL_CRC_Calculate(
      (uint32_t *) GSL_GEN_MemoryFlashStart,
      GSL_GEN_GetFlashSizeInUse() / 4);
}

// forward declarations
float GSL_ADC_GetVRefIntCounts(void);
float GSL_ADC_GetVBatCounts(void);
float GSL_ADC_GetTemperatureC(void);

// this is a helper function for outputting a clock speed in MHz
// 1000000 -> "1 MHz"
// 1234567 -> "1.23 MHz"
void GSL_GEN_OutputClockFreq(uint32_t clock) {
  if (clock % 1000000) {
    LOG(GSL_OUT_FixedFloat((float) clock / 1000000.0f, 1));
  } else {
    LOG(clock / 1000000);
  }
  LOG(" MHz");
}

// output information about the processor state, including clock selection,
// firmware versions, firmware size, voltage and temperature values
void GSL_GEN_LogBanner(bool fast = false) {
  // output separator
  LOG("\n\n\n");
  for (uint16_t line = 0; line < 3; ++line) {
    LOG("\n");
    for (uint16_t i = 0; i < 79; ++i) {
      LOG("*");
    }
  }
  // check reset flags
  {
    bool header = false;
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_LPWRRST)) {
      if (!header) {
        header = true;
        LOG("\n\nSystem reset flags were set:");
      }
      LOG("\n- Low-power reset flag (LPWRRSTF)");
    }
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_WWDGRST)) {
      if (!header) {
        header = true;
        LOG("\n\nSystem reset flags were set:");
      }
      LOG("\n- Window watchdog reset flag (WWDGRSTF)");
    }
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_IWDGRST)) {
      if (!header) {
        header = true;
        LOG("\n\nSystem reset flags were set:");
      }
      LOG("\n- Independent watchdog reset flag (IWDGRSTF)");
    }
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_SFTRST)) {
      if (!header) {
        header = true;
        LOG("\n\nSystem reset flags were set:");
      }
      LOG("\n- Software reset flag (SFTRSTF)");
    }
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PORRST)) {
      if (!header) {
        header = true;
        LOG("\n\nSystem reset flags were set:");
      }
      LOG("\n- Power on/down reset (POR/PDR) flag (PORRSTF)");
    }
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_PINRST)) {
      if (!header) {
        header = true;
        LOG("\n\nSystem reset flags were set:");
      }
      LOG("\n- Pin (NRST) reset flag (PINRSTF)");
    }
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_BORRST)) {
      if (!header) {
        header = true;
        LOG("\n\nSystem reset flags were set:");
      }
      LOG("\n- Brownout reset (BOR) flag (BORRSTF)");
    }
    // clear reset flags
    __HAL_RCC_CLEAR_RESET_FLAGS();
  }
  // physical description
  LOG("\n\nPhysical description:");
  LOG("\n- DEV_ID=", GSL_OUT_Hex(HAL_GetDEVID()));
  LOG(", REV_ID=", GSL_OUT_Hex(HAL_GetREVID()));
  LOG(", U_ID=");
  GSL_OUT_LogHex(GSL_GEN_GetUniqueID(), 4);
  LOG(" ");
  GSL_OUT_LogHex(&GSL_GEN_GetUniqueID()[4], 4);
  LOG(" ");
  GSL_OUT_LogHex(&GSL_GEN_GetUniqueID()[8], 4);
  LOG("\n- Flash size = ",
      GSL_OUT_StorageBytes((uint32_t) 1024 * GSL_GEN_GetFlashSize()));
  // software description
  LOG("\n\nSoftware description:");
#ifdef __GNUC__
  LOG("\n- Compiled with GNU C");
#ifdef __GNUG__
  LOG("++");
#endif
  LOG(" v", __GNUC__, ".", __GNUC_MINOR__);
  LOG(".", __GNUC_PATCHLEVEL__);
#endif
#ifdef __cplusplus
  LOG("\n- C++ standard v", __cplusplus / 100, ".", __cplusplus % 100);
#endif
  LOG("\n- Using STM32F4xx CMSIS v");
  LOG(__STM32F4xx_CMSIS_VERSION_MAIN, ".");
  LOG(__STM32F4xx_CMSIS_VERSION_SUB1, ".");
  LOG(__STM32F4xx_CMSIS_VERSION_SUB2);
  if (__STM32F4xx_CMSIS_VERSION_RC != 0) {
    LOG(" RC", __STM32F4xx_CMSIS_VERSION_RC);
  }
  LOG("\n- Using STM32 HAL v", HAL_GetHalVersion() >> 24);
  LOG(".", (HAL_GetHalVersion() >> 16) % 256);
  LOG(".", (HAL_GetHalVersion() >> 8) % 256);
  if (HAL_GetHalVersion() % 256 != 0) {
    LOG(" RC", HAL_GetHalVersion() % 256);
  }
  // operating description
  LOG("\n\nEnvironment description:");
  float vrefint_counts = GSL_ADC_GetVRefIntCounts();
  float vref_est = 1.21f * 4095 / vrefint_counts;
  float vbat_counts = GSL_ADC_GetVBatCounts();
  float vbat_est = vref_est * vbat_counts / 4095;
  vbat_est *= 2.0f;
  // we guess on the divider here
  if (vbat_est <= vref_est - 0.4f) {
    vbat_est *= 2.0f;
  }
  LOG("\n- VREF=", GSL_OUT_FixedFloat(vref_est, 3)," V");
  LOG(", VBAT=", GSL_OUT_FixedFloat(vbat_est, 3)," V");
  LOG(", TEMP=",
      GSL_OUT_FixedFloat(GSL_ADC_GetTemperatureC(), 1),
      " C");
  // flash operation
  LOG("\n\nFlash operation description:");
  LOG("\n- PREFETCH=");
  LOG((FLASH->ACR & FLASH_ACR_PRFTEN) ? "ON" : "OFF");
  LOG(", LATENCY=", __HAL_FLASH_GET_LATENCY(), " WS");
  LOG("\n- Cache: DATA=");
  LOG((FLASH->ACR & FLASH_ACR_DCEN) ? "ON" : "OFF");
  LOG(", INSTRUCTION=");
  LOG((FLASH->ACR & FLASH_ACR_ICEN) ? "ON" : "OFF");
  // clocks
  LOG("\n\nClock description:");
  RCC_ClkInitTypeDef clock_config;
  uint32_t flash_latency;
  HAL_RCC_GetClockConfig(&clock_config, &flash_latency);
  RCC_OscInitTypeDef osc_config;
  HAL_RCC_GetOscConfig(&osc_config);
  // get prescalers
  // find actual AHB prescaler
  uint32_t ahb_prescaler = 1;
  {
    uint32_t temp = clock_config.AHBCLKDivider >>= 4;
    while (temp) {
      --temp;
      ahb_prescaler *= 2;
    }
  }
  // find actual APB1 prescaler
  uint32_t apb1_prescaler;
  switch (clock_config.APB1CLKDivider) {
    case RCC_HCLK_DIV1:
      apb1_prescaler = 1;
      break;
    case RCC_HCLK_DIV2:
      apb1_prescaler = 2;
      break;
    case RCC_HCLK_DIV4:
      apb1_prescaler = 4;
      break;
    case RCC_HCLK_DIV8:
      apb1_prescaler = 8;
      break;
    case RCC_HCLK_DIV16:
      apb1_prescaler = 16;
      break;
    default:
    HALT("Invalid parameter");
  }
  // find actual APB2 prescaler
  uint32_t apb2_prescaler;
  switch (clock_config.APB2CLKDivider) {
    case RCC_HCLK_DIV1:
      apb2_prescaler = 1;
      break;
    case RCC_HCLK_DIV2:
      apb2_prescaler = 2;
      break;
    case RCC_HCLK_DIV4:
      apb2_prescaler = 4;
      break;
    case RCC_HCLK_DIV8:
      apb2_prescaler = 8;
      break;
    case RCC_HCLK_DIV16:
      apb2_prescaler = 16;
      break;
    default:
    HALT("Invalid parameter");
  }
  LOG("\n- System clock selection: ");
  switch (clock_config.SYSCLKSource) {
    case RCC_SYSCLKSOURCE_HSI:
    case RCC_SYSCLKSOURCE_HSE:
    {
      if (clock_config.SYSCLKSource == RCC_SYSCLKSOURCE_HSI) {
        ASSERT(osc_config.HSIState == RCC_HSI_ON);
        LOG("HSI @ ");
        GSL_GEN_OutputClockFreq(HSI_VALUE);
        LOG(" (cal=", GSL_OUT_Hex(osc_config.HSICalibrationValue, 1), ")");
      } else {
        LOG("HSE");
        if (osc_config.HSEState == RCC_HSE_BYPASS) {
          LOG (" bypass");
        } else {
          ASSERT(osc_config.HSEState == RCC_HSE_ON);
        }
        LOG(" @ assumed ");
        GSL_GEN_OutputClockFreq(HSE_VALUE);
      }
      LOG("\n- Prescalers: ");
      LOG("AHB=/", ahb_prescaler);
      LOG(", APB1=/", apb1_prescaler);
      LOG(", APB2=/", apb2_prescaler);
      LOG("\n- Clocks: ");
      LOG("SYSCLK=");
      GSL_GEN_OutputClockFreq(HAL_RCC_GetSysClockFreq());
      LOG(", HCLK=");
      GSL_GEN_OutputClockFreq(HAL_RCC_GetHCLKFreq());
      break;
    }
    case RCC_SYSCLKSOURCE_PLLCLK:
    {
      LOG("PLLCLK");
      ASSERT(osc_config.PLL.PLLState == RCC_PLL_ON);
      switch (osc_config.PLL.PLLSource) {
        case RCC_PLLSOURCE_HSI:
          LOG(" using HSI @ ");
          GSL_GEN_OutputClockFreq(HSI_VALUE);
          LOG(" (cal=", GSL_OUT_Hex(osc_config.HSICalibrationValue, 1), ")");
          break;
        case RCC_PLLSOURCE_HSE:
          LOG(" using HSE");
          if (osc_config.HSEState == RCC_HSE_BYPASS) {
            LOG (" bypass");
          } else {
            ASSERT(osc_config.HSEState == RCC_HSE_ON);
          }
          LOG(" @ assumed ");
          GSL_GEN_OutputClockFreq(HSE_VALUE);
          break;
        default:
        HALT("Invalid parameter");
      }
      LOG("\n- PLL scalers: ");
      LOG("M=/", osc_config.PLL.PLLM);
      LOG(", N=x", osc_config.PLL.PLLN);
      LOG(", P=/", osc_config.PLL.PLLP);
      LOG(", Q=/", osc_config.PLL.PLLQ);
      LOG("\n- Prescalers: ");
      LOG("AHB=/", ahb_prescaler);
      LOG(", APB1=/", apb1_prescaler);
      LOG(", APB2=/", apb2_prescaler);
      // Clocks: VCO=X MHz, SYSCLK=x MHz, HCLK=X MHz
      uint32_t vco = HAL_RCC_GetSysClockFreq() * osc_config.PLL.PLLP;
      LOG("\n- Clocks: VCO=");
      GSL_GEN_OutputClockFreq(vco);
      // vco must be within a specific range as given in the data sheets
      if (vco < 192000000 || vco > 432000000) {
        LOG(" (OUT OF RANGE)");
      } else {
        LOG(" (in range)");
      }
      LOG(", SYSCLK=");
      GSL_GEN_OutputClockFreq(HAL_RCC_GetSysClockFreq());
      LOG(", HCLK=");
      GSL_GEN_OutputClockFreq(HAL_RCC_GetHCLKFreq());
      break;
    }
    default:
    HALT("Invalid parameter");
  }
  LOG("\n- Clocks: PCLK1=");
  GSL_GEN_OutputClockFreq(HAL_RCC_GetPCLK1Freq());
  if (apb1_prescaler != 1) {
    LOG(" (timers ");
    GSL_GEN_OutputClockFreq(HAL_RCC_GetPCLK1Freq() * 2);
    LOG(")");
  }
  LOG(", PCLK2=");
  GSL_GEN_OutputClockFreq(HAL_RCC_GetPCLK2Freq());
  if (apb2_prescaler != 1) {
    LOG(" (timers ");
    GSL_GEN_OutputClockFreq(HAL_RCC_GetPCLK2Freq() * 2);
    LOG(")");
  }
  LOG("\n- Clocks: 48MHz=");
  GSL_GEN_OutputClockFreq(HAL_RCC_GetSysClockFreq() * osc_config.PLL.PLLP / osc_config.PLL.PLLQ);
  // firmware
  LOG("\n\nFirmware description:");
  LOG("\n- Compile datestamp: ", GSL_GEN_GetFirmwareTimestampLong());
  if (!fast) {
    uint32_t length = GSL_GEN_GetFlashSizeInUse();
    LOG("\n- Flash bytes in use: ", length);
    uint32_t crc =
        GSL_CRC_Calculate((uint32_t *) GSL_GEN_MemoryFlashStart, length / 4);
    LOG("\n- CRC32 of the flash: ", GSL_OUT_Hex(crc));
  }
}

// return true if the code is currently in an interrupt handler
bool GSL_GEN_InInterrupt(void) {
  return (SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) != 0;
}

// return the current IRQ number
uint32_t GSL_GEN_GetInterruptIRQ(void) {
  ASSERT(GSL_GEN_InInterrupt);
  return __get_IPSR() - 16;
}

// return the priorty of the given interrupt
uint32_t GSL_GEN_GetCurrentIRQPriority(void) {
  uint32_t preempt = 0;
  uint32_t subpriority = 0;
  HAL_NVIC_GetPriority(
      (IRQn_Type) (__get_IPSR() - 16),
      HAL_NVIC_GetPriorityGrouping(),
      &preempt,
      &subpriority);
  return preempt;
}

// return the preempt priority of the currently executing interrupt
uint32_t GSL_GEN_GetInterruptPriority(void) {
  ASSERT(GSL_GEN_InInterrupt());
  uint32_t interrupt = SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk;
  uint32_t preempt;
  uint32_t subpriority;
  HAL_NVIC_GetPriority(
      (IRQn_Type) (interrupt - 16),
      HAL_NVIC_GetPriorityGrouping(),
      &preempt,
      &subpriority);
  return preempt;
}

// ensure the float is in the range of [a, b]
void GSL_GEN_ClipFloat(
    float & value,
    float lower_bound = 0.0f,
    float upper_bound = 1.0f) {
  ASSERT(lower_bound <= upper_bound);
  if (value > upper_bound) {
    value = upper_bound;
  } else if (!(value >= lower_bound)) {
    value = lower_bound;
  }
}

// attempt to find the portion of the time the processor is busy doing stuff
// in interrupts
// (this is experimental)
// block for 1s total assuming 100% cpu time and calculate the actual time
// if a negative duration is used, the clocks_per_loop variable is calibrated
// assuming the system is 0% busy apart from this
float GSL_GEN_GetBusyTime(float duration = 0.125f) {
  // number of system clocks in each loop
  // (calibrate this)
  static uint32_t clocks_per_loop = 12;
  // number of ticks from overhead before/after loop
  static uint32_t overhead_ticks = 0;

  bool calibrate = duration < 0.0f;
  if (duration < 0.0f) {
    duration = -duration;
  }
  const uint32_t loop_count =
      HAL_RCC_GetSysClockFreq() * duration / clocks_per_loop;
  //auto start = GSL_DEL_GetLongTime();

  const volatile uint32_t start = GSL_DEL_Ticks();
  for (volatile uint32_t i = 0; i < loop_count; ++i) {
  }
  const volatile uint32_t end = GSL_DEL_Ticks();

  uint32_t elapsed_ticks = end - start;
  float best_fit = 0.0f;
  if (calibrate) {
    best_fit = elapsed_ticks / loop_count;
    clocks_per_loop = best_fit + 0.5f;
  }
  const uint32_t expected_ticks =
      loop_count * clocks_per_loop;
  ASSERT_GE(elapsed_ticks, expected_ticks);
  uint32_t extra_ticks = elapsed_ticks - expected_ticks;
  // calibrate overhead ticks regardless
  if (overhead_ticks == 0 || extra_ticks < overhead_ticks) {
    overhead_ticks = extra_ticks;
  }
  ASSERT_GE(extra_ticks, overhead_ticks);
  extra_ticks -= overhead_ticks;
  //LOG("\nThere were ", extra_ticks, " extra ticks for a duration of ",
  //    expected_ticks, " expected ticks");
  float busy_time = (float) extra_ticks / (extra_ticks + expected_ticks);
  if (false && calibrate) {
    LOG("\nGSL_GEN_GetBusyTime: clocks_per_loop=",
        clocks_per_loop,
        "(", GSL_OUT_FixedFloat(best_fit, 3), "), overhead_ticks=",
        overhead_ticks);
  }
  return busy_time;
}

// swap two values
template <class T>
inline void GSL_GEN_Swap(T & one, T & two) {
  // make sure we only use this for small objects
  static_assert(sizeof(T) <= 32, "Not meant for large objects.");
  T temp = one;
  one = two;
  two = temp;
}

// calculate the mean and/or standard deviation of the given data array
void GSL_GEN_CalculateMeanStdev(
    float * data,
    uint16_t length,
    float * mean_ptr = nullptr,
    float * stdev_ptr = nullptr) {
  float mean = 0.0f;
  for (uint16_t i = 0; i < length; ++i) {
    mean += data[i];
  }
  mean /= length;
  if (stdev_ptr != nullptr) {
    float stdev = 0.0f;
    for (uint16_t i = 0; i < length; ++i) {
      float x = data[i] - mean;
      stdev += x * x;
    }
    stdev /= (length - 1);
    stdev = sqrt(stdev);
    *stdev_ptr = stdev;
  }
  if (mean_ptr != nullptr) {
    *mean_ptr = mean;
  }
}

// this hold the number of "pause interrupts" commands currently in place
volatile uint32_t gsl_gen_interrupt_stack = 0;

// return true if interrupts are enabled
inline bool GSL_GEN_AreInterruptsEnabled(void) {
  return __get_PRIMASK() == 0;
}

// disable interrupts
void GSL_GEN_DisableInterrupts(void) {
  __disable_irq();
  ++gsl_gen_interrupt_stack;
}

// enable interrupts
void GSL_GEN_EnableInterrupts(void) {
  ASSERT_GT(gsl_gen_interrupt_stack, 0U);
  ASSERT(!GSL_GEN_AreInterruptsEnabled());
  --gsl_gen_interrupt_stack;
  if (gsl_gen_interrupt_stack == 0) {
    __enable_irq();
  }
}

// reset this chip
void GSL_GEN_SystemReset(uint32_t delay_ms = 1000) {
  // ensure we're not in an interrupt
  ASSERT(!GSL_GEN_InInterrupt());
  // wait a bit for stuff like the debug log to clear
  GSL_DEL_MS(delay_ms);
  // disable interrupts
  GSL_GEN_DisableInterrupts();
  // reset the chip
  HAL_NVIC_SystemReset();
}

// output a register value
void GSL_GEN_OutputRegister(
    const char * name,
    uint32_t value,
    uint32_t mask,
    uint32_t reset_value) {
  ASSERT_NE(mask, 0u);
  // if value is same as reset value, just return
  if ((value & mask) == (reset_value & mask)) {
    return;
  }
  // else output the information
  LOG(" ", name, "=");
  // get number of bits and offset of mask
  uint8_t bit_offset = 0;
  uint8_t bit_count = 0;
  {
    uint32_t temp_mask = mask;
    while ((temp_mask & 1) == 0) {
      temp_mask /= 2;
      ++bit_offset;
    }
    while ((temp_mask & 1) == 1) {
      temp_mask /= 2;
      ++bit_count;
    }
    ASSERT_EQ(temp_mask, 0u);
  }
  // output register value
  // output in binary unless 16 or 32 bits, in which case we output in
  // hexadecimal
  if (bit_count == 16) {
    LOG(GSL_OUT_Hex((uint16_t) ((value & mask) >> bit_offset)));
  } else if (bit_count == 32) {
    LOG(GSL_OUT_Hex((uint32_t) ((value & mask) >> bit_offset)));
  } else {
    for (uint16_t i = 0; i < bit_count; ++i) {
      bool bit_set = value & (1 << (bit_offset + bit_count - 1 - i));
      LOG((bit_set) ? "1" : "0");
    }
  }
}

#ifdef GSL_CUSTOM_MEMSET

#ifdef __cplusplus
extern "C" {
#endif

// optimized version of memset
// we split up the region into several segments
//
// base_ptr
// * store single bytes
// mid1
// * store words, 4 at a time
// mid2
// * store words, 1 at a time
// mid3
// * store single bytes
// end
//
// For large buffers, most of the time is spent between mid1 and mid2 which is
// highly optimized.
void * memset(void * base_ptr, int x, size_t length) {
  const uint32_t int_size = sizeof(uint32_t);
  static_assert(sizeof(uint32_t) == 4, "only supports 32 bit size");
  // find first word-aligned address
  uint32_t ptr = (uint32_t) base_ptr;
  // get end of memory to set
  uint32_t end = ptr + length;
  // get location of first word-aligned address at/after the start, but not
  // after the end
  uint32_t mid1 = (ptr + int_size - 1) / int_size * int_size;
  if (mid1 > end) {
    mid1 = end;
  }
  // get location of last word-aligned address at/before the end
  uint32_t mid3 = end / int_size * int_size;
  // get end location of optimized section
  uint32_t mid2 = mid1 + (mid3 - mid1) / (8 * int_size) * (8 * int_size);
  // create a word-sized integer
  uint32_t value = (uint8_t) x;
  value |= value << 8;
  value |= value << 16;
  __ASM volatile (
  // store bytes
  "b Compare1%=\n"
  "Store1%=:\n"
  "strb %[value], [%[ptr]], #1\n"
  "Compare1%=:\n"
  "cmp %[ptr], %[mid1]\n"
  "bcc Store1%=\n"
  // store words optimized
  "b Compare2%=\n"
  "Store2%=:\n"
  "str %[value], [%[ptr]], #4\n"
  "str %[value], [%[ptr]], #4\n"
  "str %[value], [%[ptr]], #4\n"
  "str %[value], [%[ptr]], #4\n"
  "str %[value], [%[ptr]], #4\n"
  "str %[value], [%[ptr]], #4\n"
  "str %[value], [%[ptr]], #4\n"
  "str %[value], [%[ptr]], #4\n"
  "Compare2%=:\n"
  "cmp %[ptr], %[mid2]\n"
  "bcc Store2%=\n"
  // store words
  "b Compare3%=\n"
  "Store3%=:\n"
  "str %[value], [%[ptr]], #4\n"
  "Compare3%=:\n"
  "cmp %[ptr], %[mid3]\n"
  "bcc Store3%=\n"
  // store bytes
  "b Compare4%=\n"
  "Store4%=:\n"
  "strb %[value], [%[ptr]], #1\n"
  "Compare4%=:\n"
  "cmp %[ptr], %[end]\n"
  "bcc Store4%=\n"
  : // no outputs
  : [value] "r"(value),
  [ptr] "r"(ptr),
  [mid1] "r"(mid1),
  [mid2] "r"(mid2),
  [mid3] "r"(mid3),
  [end] "r"(end)
  );
  return base_ptr;
}

// do memmove on word-aligned data
void * memmove32_reverse(void * base_dest, const void * base_src, size_t length) {
  // should be byte aligned
  ASSERT((uint32_t) base_dest % 4 == 0);
  ASSERT((uint32_t) base_src % 4 == 0);
  ASSERT((uint32_t) length % 4 == 0);

  uint32_t src = ((uint32_t) base_src) + length - 4;
  uint32_t dest = ((uint32_t) base_dest) + length - 4;
  ASSERT_LT(src, dest);
  // get midway
  uint32_t src_mid = src - (length / 32 * 32);
  // get end
  uint32_t src_end = ((uint32_t) base_src) - 4;
  uint32_t value = 0;
  // run in assembly
  __ASM volatile (
  // store words optimized 8 at a time
  "b Compare1%=\n"
  "Store1%=:\n"
  "ldr %[value], [%[src]], #-4\n"
  "str %[value], [%[dest]], #-4\n"
  "ldr %[value], [%[src]], #-4\n"
  "str %[value], [%[dest]], #-4\n"
  "ldr %[value], [%[src]], #-4\n"
  "str %[value], [%[dest]], #-4\n"
  "ldr %[value], [%[src]], #-4\n"
  "str %[value], [%[dest]], #-4\n"
  "ldr %[value], [%[src]], #-4\n"
  "str %[value], [%[dest]], #-4\n"
  "ldr %[value], [%[src]], #-4\n"
  "str %[value], [%[dest]], #-4\n"
  "ldr %[value], [%[src]], #-4\n"
  "str %[value], [%[dest]], #-4\n"
  "ldr %[value], [%[src]], #-4\n"
  "str %[value], [%[dest]], #-4\n"
  "Compare1%=:\n"
  "cmp %[src], %[src_mid]\n"
  "bgt Store1%=\n"
  // store words one at a time
  "b Compare2%=\n"
  "Store2%=:\n"
  "ldr %[value], [%[src]], #-4\n"
  "str %[value], [%[dest]], #-4\n"
  "Compare2%=:\n"
  "cmp %[src], %[src_end]\n"
  "bgt Store2%=\n"
  : // no outputs
  : [value] "r"(value),
  [src] "r"(src),
  [dest] "r"(dest),
  [src_mid] "r"(src_mid),
  [src_end] "r"(src_end)
  );
}

void * memmove32_forward(void * dest, const void * src, size_t length) {
  // should be byte aligned
  ASSERT((uint32_t) dest % 4 == 0);
  ASSERT((uint32_t) src % 4 == 0);
  ASSERT((uint32_t) length % 4 == 0);
  // src should be after dest
  ASSERT_GT((uint32_t) src, (uint32_t) dest);
  uint32_t value = 0;
  // run in assembly
  __ASM volatile (
  // store words optimized 8 at a time
  "b Compare1%=\n"
  "Store1%=:\n"
  "ldr %[value], [%[src]], #4\n"
  "str %[value], [%[dest]], #4\n"
  "ldr %[value], [%[src]], #4\n"
  "str %[value], [%[dest]], #4\n"
  "ldr %[value], [%[src]], #4\n"
  "str %[value], [%[dest]], #4\n"
  "ldr %[value], [%[src]], #4\n"
  "str %[value], [%[dest]], #4\n"
  "ldr %[value], [%[src]], #4\n"
  "str %[value], [%[dest]], #4\n"
  "ldr %[value], [%[src]], #4\n"
  "str %[value], [%[dest]], #4\n"
  "ldr %[value], [%[src]], #4\n"
  "str %[value], [%[dest]], #4\n"
  "ldr %[value], [%[src]], #4\n"
  "str %[value], [%[dest]], #4\n"
  "Compare1%=:\n"
  "cmp %[src], %[src_mid]\n"
  "bcc Store1%=\n"
  // store words one at a time
  "b Compare2%=\n"
  "Store2%=:\n"
  "ldr %[value], [%[src]], #4\n"
  "str %[value], [%[dest]], #4\n"
  "Compare2%=:\n"
  "cmp %[src], %[src_end]\n"
  "bcc Store2%=\n"
  : // no outputs
  : [value] "r"(value),
  [src] "r"((uint32_t) src),
  [dest] "r"((uint32_t) dest),
  [src_mid] "r"((uint32_t) src + length / 32 * 32),
  [src_end] "r"((uint32_t) src + length)
  );
}

// do memmove on word aligned data
void * memmove32(void * dest, const void * src, size_t length) {
  if (dest > src) {
    memmove32_reverse(dest, src, length);
  } else if (dest < src) {
    memmove32_forward(dest, src, length);
  }
}

#ifdef __cplusplus
}
#endif

// test the memset functions
void GSL_GEN_TestSingleMemSet(
    uint8_t * buffer,
    uint32_t length,
    uint8_t * start,
    uint8_t * end) {
  ASSERT_GE((uint32_t) start, (uint32_t) buffer);
  ASSERT_GE((uint32_t) end, (uint32_t) buffer);
  ASSERT_LE((uint32_t) start, (uint32_t) (buffer + length));
  ASSERT_LE((uint32_t) end, (uint32_t) (buffer + length));
  // set all to zero
  for (uint16_t i = 0; i < length; ++i) {
    buffer[i] = 0;
  }
  // set range to 0xAA
  memset(start, 0xAA, end - start);
  //
  // set range to 0xAA
  /*for (uint8_t * ptr = start; ptr < end; ++ptr) {
    *ptr = 0xAA;
  }*/
  // check all
  //LOG("\nstart=", start - buffer, ", end=", end - buffer);
  for (uint8_t * ptr = buffer; ptr < buffer + length; ++ptr) {
    //LOG("\nptr=", ptr - buffer);
    if (ptr < start) {
      ASSERT_EQ(*ptr, 0);
    } else if (ptr < end) {
      ASSERT_EQ(*ptr, 0xAA);
    } else {
      ASSERT_EQ(*ptr, 0);
    }
  }
}


// test the memset functions
void GSL_GEN_TestMemSet(void) {
  // buffer
  uint8_t buffer[128];
  // size of buffer
  const uint32_t capacity = sizeof(buffer);

  //LOG("\nTesting memset...");

  // test all offsets
  for (uint16_t start = 8; start < 24; ++start) {
    // test all offsets
    for (uint16_t end = capacity - 24; end < capacity - 8; ++end) {
      GSL_GEN_TestSingleMemSet(buffer, capacity, &buffer[start], &buffer[end]);
    }
    // test small sections
    for (uint16_t end = start; end < start + 32; ++end) {
      GSL_GEN_TestSingleMemSet(buffer, capacity, &buffer[start], &buffer[end]);
    }
  }
  //LOG("\nDone");
}

// test the memset functions
void GSL_GEN_TestMemmove32(void) {
  const uint16_t length = 128;
  // buffer
  uint32_t buffer[length];
  for (uint16_t i = 0; i < length; ++i) {
    buffer[i] = i;
  }
  // move back 1 word
  memmove32(&buffer[0], &buffer[1], (length - 1) * 4);
  for (uint16_t i = 0; i < length - 1; ++i) {
    ASSERT_EQ(buffer[i], i + 1);
  }
  // move forward 1 word
  memmove32(&buffer[1], &buffer[0], (length - 1) * 4);
  for (uint16_t i = 1; i < length; ++i) {
    ASSERT_EQ(buffer[i], i);
  }
}

//GSL_INITIALIZER gsl_gen_memset_test(GSL_GEN_TestMemSet);

//GSL_INITIALIZER gsl_gen_memmove32_test(GSL_GEN_TestMemmove32);

#endif
