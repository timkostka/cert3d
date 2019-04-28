#pragma once

#include "gsl_includes.h"

// microcontroller state of health stream
struct StateOfHealthStruct {
  // frame sync (0xFA 0xF3 0x20)
  uint8_t frame_sync[3];
  // counter (+1 each embedded stream cycle)
  uint8_t counter;
  // ID_CODE
  uint8_t mcu_id_code[4];
  // flash size (in kB)
  uint8_t mcu_flash_size[2];
  // 12-byte unique ID
  uint8_t mcu_unique_id[12];
  // temperature in Celsius BCD format (27.01 oC = 0x2701, -1 oC = 0x9900)
  uint8_t mcu_temperature[2];
  // estimated VBAT voltage in BCD format (i.e. 3.300V -> 0x3300);
  uint8_t mcu_voltage[2];
  // VREFINT voltage BCD format (i.e. 1.21 V -> 0x1210);
  uint8_t mcu_vrefint[2];
  // firmware date (0xYYYYMMDD 0xHHMMSS)
  uint8_t mcu_firmware_date[7];
  // firmware size in use in bytes
  uint8_t mcu_firmware_size[3];
  // CRC32 of the firmware
  uint8_t mcu_firmware_crc32[4];
  // voltage reading from INA226 chip in BCD format (3.123V -> 0x3123)
  uint8_t ina226_voltage[2];
  // current reading from INA226 chip in BCD format (123.1mA -> 0x1231)
  uint8_t ina226_current[2];
};

// storage for state of health stream
StateOfHealthStruct gsl_sohstream_stream;

// next byte to sent
uint16_t gsl_sohstream_next_byte = 0;

// length of the state of health stream
const uint16_t kSOHStreamLength = sizeof(StateOfHealthStruct);

// unique id location
const uint8_t * const kSOHStreamUniqueIDAddress = (uint8_t *) 0x1FFF7A10;

// flash size address (in kB)
const uint16_t kSOHStreamFlashSize = *((uint16_t *) 0x1FFF7A22);

// return true if INA226 chip is present and responding
// (0x40 slave address on I2C1)
bool gsl_sohstream_ina226_present = false;

// store a uint16_t into the given memory in big endian format
void GSL_SOHSTREAM_Store(uint16_t value, uint8_t * dest) {
  dest[0] = value >> 8;
  dest[1] = value & 0xFF;
}

// store a uint32_t into the given memory in big endian format
void GSL_SOHSTREAM_Store(uint32_t value, uint8_t * dest) {
  dest[0] = value >> 24;
  dest[1] = value >> 16;
  dest[2] = value >> 8;
  dest[3] = value & 0xFF;
}

// routine for getting new ADC values
// the first time this is called, we set up the VREFINT conversion
// the second time, we store VREFINT and set up VTEMP
// the third time, we store VTEMP and set up VBAT
// the fourth time, we store VBAT
void GSL_SOHSTREAM_RefreshADCValue(void) {
  // store the call number
  static uint8_t call = 0;
  uint8_t this_call = call;
  // increase call number for the next time
  ++call;
  call %= 4;
  // initialize on the first call
  static bool initialized = false;
  if (!initialized) {
    GSL_ADC_SetCompleteCallback(ADC1, GSL_SOHSTREAM_RefreshADCValue);
    gsl_adc1_hadc.Init.ContinuousConvMode = DISABLE;
    initialized = true;
  }
  if (this_call == 0) {
    // zero out old values values
    gsl_sohstream_stream.mcu_voltage[0] = 0x00;
    gsl_sohstream_stream.mcu_voltage[1] = 0x00;
    gsl_sohstream_stream.mcu_temperature[0] = 0x00;
    gsl_sohstream_stream.mcu_temperature[1] = 0x00;
    gsl_sohstream_stream.mcu_vrefint[0] = 0x00;
    gsl_sohstream_stream.mcu_vrefint[1] = 0x00;
    // set up VREFINT conversion
    GSL_ADC_ReadChannel_IT(ADC1, ADC_CHANNEL_VREFINT, ADC_SAMPLETIME_480CYCLES);
  } else if (this_call == 1) {
    // disable VREFINT channel
    ADC->CCR &= ~ADC_CCR_TSVREFE;
    // store VREFINT in BCD format
    uint16_t value = HAL_ADC_GetValue(GSL_ADC_GetInfo(ADC1)->handle);
    //LOG("\nVREFINT = ", value);
    float voltage = value / 4095.0f * GSL_ADC_VREF;
    value = voltage * 1000.0f + 0.5f;
    if (value > 9999) {
      value = 9999;
    }
    gsl_sohstream_stream.mcu_vrefint[0] =
        GSL_GEN_ToBCD((uint8_t) (value / 100));
    gsl_sohstream_stream.mcu_vrefint[1] =
        GSL_GEN_ToBCD((uint8_t) (value % 100));
    // set up TEMPSENSOR
    GSL_ADC_ReadChannel_IT(ADC1, ADC_CHANNEL_TEMPSENSOR, ADC_SAMPLETIME_480CYCLES);
  } else if (this_call == 2) {
    // store TEMPSENSOR
    uint16_t value = HAL_ADC_GetValue(GSL_ADC_GetInfo(ADC1)->handle);
    //LOG("\nTEMPSENSOR = ", value);
    float temp_c = GSL_ADC_ConvertTemperature(value);
    temp_c *= 10.0f;
    if (temp_c < 0) {
      temp_c = 10000.0f + temp_c;
    }
    uint16_t temp_c_int = temp_c;
    gsl_sohstream_stream.mcu_temperature[0] =
        GSL_GEN_ToBCD((uint8_t) (temp_c_int / 100));
    gsl_sohstream_stream.mcu_temperature[1] =
        GSL_GEN_ToBCD((uint8_t) (temp_c_int % 100));
    // set up VBAT
    GSL_ADC_ReadChannel_IT(ADC1, ADC_CHANNEL_VBAT, ADC_SAMPLETIME_480CYCLES);
  } else if (this_call == 3) {
    // Disable the VBAT channel
    ADC->CCR &= ~ADC_CCR_VBATE;
    // store VBAT
    uint16_t value = HAL_ADC_GetValue(GSL_ADC_GetInfo(ADC1)->handle);
    // adjust
    //LOG("\nVBAT = ", value);
    float voltage = value / 4095.0f * GSL_ADC_VREF;
    voltage *= 2;
#ifdef STM32F429xx
    voltage *= 2;
#endif
    value = voltage * 1000.0f + 0.5f;
    if (value > 9999) {
      value = 9999;
    }
    gsl_sohstream_stream.mcu_voltage[0] =
        GSL_GEN_ToBCD((uint8_t) (value / 100));
    gsl_sohstream_stream.mcu_voltage[1] =
        GSL_GEN_ToBCD((uint8_t) (value % 100));
  }
}

// called to initiate new voltage and current readings from the INA226 chip
void GSL_SOHSTREAM_RefreshMonitor(void) {
  // initialize the first time we're here
  static bool initialized = false;
  if (!initialized) {
    GSL_I2C_Initialize(I2C1);
    GSL_I2C_GetInfo(I2C1)->MemRxCpltCallback = GSL_SOHSTREAM_RefreshMonitor;
    initialized = true;
  }
  // hold the call number
  static uint8_t call = 0;
  uint8_t this_call = call;
  ++call;
  call %= 3;
  // storage space for the readings
  static uint8_t reading[2];
  if (this_call == 0) {
    // zero out previous values
    gsl_sohstream_stream.ina226_current[0] = 0;
    gsl_sohstream_stream.ina226_current[1] = 0;
    gsl_sohstream_stream.ina226_voltage[0] = 0;
    gsl_sohstream_stream.ina226_voltage[1] = 0;
    // begin voltage reading
    if (gsl_sohstream_ina226_present) {
      GSL_I2C_ReadAddressMulti_IT(
          I2C1,
          0x40,
          GSL_INA226::kRegisterBusVoltage,
          reading,
          2);
    }
  } else if (this_call == 1) {
    // store voltage reading
    float voltage = reading[0] * 256.0f + reading[1];
    voltage *= 1.25e-3f;
    uint16_t voltage_counts = voltage * 1000.0f + 0.5f;
    if (voltage_counts > 9999) {
      voltage_counts = 9999;
    }
    gsl_sohstream_stream.ina226_voltage[0] =
        GSL_GEN_ToBCD((uint8_t) (voltage_counts / 100));
    gsl_sohstream_stream.ina226_voltage[1] =
        GSL_GEN_ToBCD((uint8_t) (voltage_counts % 100));
    // begin current reading
    if (gsl_sohstream_ina226_present) {
      GSL_I2C_ReadAddressMulti_IT(
          I2C1,
          0x40,
          GSL_INA226::kRegisterShuntVoltage,
          reading,
          2);
    }
  } else if (this_call == 2) {
    // store current reading
    float current = reading[0] * 256.0f + reading[1];
    // convert counts to shunt voltage
    current *= 2.5e-6f;
    // convert shunt voltage to current
    current /= 0.1f;
    // convert amps to milliamps
    current *= 1000.0f;
    uint16_t current_counts = current * 10.0f + 0.5f;
    if (current_counts > 9999) {
      current_counts = 9999;
    }
    gsl_sohstream_stream.ina226_current[0] =
        GSL_GEN_ToBCD((uint8_t) (current_counts / 100));
    gsl_sohstream_stream.ina226_current[1] =
        GSL_GEN_ToBCD((uint8_t) (current_counts % 100));
  }
}

// initialize the state of health stream
void GSL_SOHSTREAM_Initialize(void) {
  // initialize all to zero first
  memset(&gsl_sohstream_stream, 0, sizeof(gsl_sohstream_stream));
  // start the automatic conversion of ADC channels
  GSL_SOHSTREAM_RefreshADCValue();
  // set low priority IRQ
  HAL_NVIC_SetPriority(GSL_ADC_GetIRQ(ADC1), 14, 0);
  // check for ina226 chip
  gsl_sohstream_ina226_present = GSL_I2C_IsSlavePresent(I2C1, 0x40);
  if (!gsl_sohstream_ina226_present) {
    LOG("\nWARNING: INA226 chip not present.  These values will be 0.");
  }
  // begin reading of INA226 values
  GSL_SOHSTREAM_RefreshMonitor();
  // set low priority IRQ
  HAL_NVIC_SetPriority(GSL_I2C_GetIRQ_EV(I2C1), 14, 0);
  HAL_NVIC_SetPriority(GSL_I2C_GetIRQ_ER(I2C1), 14, 0);
  // set up the ones we know about
  gsl_sohstream_stream.frame_sync[0] = 0xFA;
  gsl_sohstream_stream.frame_sync[1] = 0xF3;
  gsl_sohstream_stream.frame_sync[2] = 0x20;
  gsl_sohstream_stream.counter = 0;
  GSL_SOHSTREAM_Store(DBGMCU->IDCODE, gsl_sohstream_stream.mcu_id_code);
  GSL_SOHSTREAM_Store(
      kSOHStreamFlashSize,
      gsl_sohstream_stream.mcu_flash_size);
  memcpy(gsl_sohstream_stream.mcu_unique_id, kSOHStreamUniqueIDAddress, 12);
  memcpy(
      gsl_sohstream_stream.mcu_firmware_date,
      GSL_GEN_GetFirmwareTimestampHex(),
      7);
  uint32_t in_use = GSL_GEN_GetFlashSizeInUse();
  gsl_sohstream_stream.mcu_firmware_size[0] = in_use >> 16;
  GSL_SOHSTREAM_Store(
      (uint16_t) in_use,
      &gsl_sohstream_stream.mcu_firmware_size[1]);
  GSL_SOHSTREAM_Store(
      GSL_GEN_GetFlashCRC(),
      gsl_sohstream_stream.mcu_firmware_crc32);
}

// return the next byte in the state of health stream
uint8_t GSL_SOHSTREAM_NextByte(void) {
  uint8_t value = ((uint8_t *) &gsl_sohstream_stream)[gsl_sohstream_next_byte];
  ++gsl_sohstream_next_byte;
  gsl_sohstream_next_byte %= kSOHStreamLength;
  // once per stream, take updated values for vrefint and temperature
  if (gsl_sohstream_next_byte == 0) {
    // write out substream
    /*static uint8_t counter_z = 0;
    ++counter_z;
    counter_z %= 10;
    if (counter_z == 7 || true) {
      LOG("\n");
      for (uint16_t i = 0; i < sizeof(StateOfHealthStruct); ++i) {
        LOG(&OutputFormHex(((uint8_t *) &gsl_sohstream_stream)[i])[2]);
      }
    }*/
    // start new ADC conversion sequence
    GSL_SOHSTREAM_RefreshADCValue();
    // start new INA226 conversion sequence
    GSL_SOHSTREAM_RefreshMonitor();
    // increment counter
    ++gsl_sohstream_stream.counter;
  }
  return value;
}
