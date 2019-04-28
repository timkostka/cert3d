#pragma once

// This file contains files used to flash other STM32 chips.

#include "gsl_includes.h"

// progress update routine to call, if any
void (*gsl_fwf_progress_update_function)(float) = nullptr;

// peripheral family
enum GSL_FWF_PeripheralFamilyEnum {
  kPeripheralUnused = 0,
  kPeripheralUART,
  kPeripheralSPI,
};

// current peripheral family being used
GSL_FWF_PeripheralFamilyEnum gsl_fwf_peripheral_family = kPeripheralUnused;

// current peripheral being used
void * gsl_fwf_peripheral = nullptr;

// ACK (acknowledge/agree) value
const uint8_t gsl_fwf_ack_value = 0x79;

// NACK (not acknowledge) value
const uint8_t gsl_fwf_nack_value = 0x1F;

// send a byte
void GSL_FWF_Send(uint8_t value) {
  if (gsl_fwf_peripheral_family == kPeripheralUART) {
    GSL_UART_Send((USART_TypeDef *) gsl_fwf_peripheral, value);
  } else if (gsl_fwf_peripheral_family == kPeripheralSPI) {
    GSL_SPI_Send((SPI_TypeDef *) gsl_fwf_peripheral, value);
  } else {
    HALT("Unexpected value");
  }
}

// send several bytes
void GSL_FWF_SendMulti(uint8_t * value, uint16_t count) {
  if (gsl_fwf_peripheral_family == kPeripheralUART) {
    GSL_UART_SendMulti((USART_TypeDef *) gsl_fwf_peripheral, value, count);
  } else if (gsl_fwf_peripheral_family == kPeripheralSPI) {
    GSL_SPI_SendMulti((SPI_TypeDef *) gsl_fwf_peripheral, value, count);
  } else {
    HALT("Unexpected value");
  }
}

// receive a byte
uint8_t GSL_FWF_Read(void) {
  if (gsl_fwf_peripheral_family == kPeripheralUART) {
    return GSL_UART_Read((USART_TypeDef *) gsl_fwf_peripheral);
  } else if (gsl_fwf_peripheral_family == kPeripheralSPI) {
    return GSL_SPI_Read((SPI_TypeDef *) gsl_fwf_peripheral);
  } else {
    HALT("Unexpected value");
  }
}

// receive many bytes
void GSL_FWF_ReadMulti(uint8_t * value, uint16_t count) {
  if (gsl_fwf_peripheral_family == kPeripheralUART) {
    GSL_UART_ReadMulti((USART_TypeDef *) gsl_fwf_peripheral, value, count);
  } else if (gsl_fwf_peripheral_family == kPeripheralSPI) {
    GSL_SPI_ReadMulti((SPI_TypeDef *) gsl_fwf_peripheral, value, count);
  } else {
    HALT("Unexpected value");
  }
}

// receive ACK or NACK using UART
bool GSL_FWF_GetACK_UART(uint32_t timeout_ms) {
  ASSERT(gsl_fwf_peripheral_family == kPeripheralUART);
  USART_TypeDef * USARTx = (USART_TypeDef *) gsl_fwf_peripheral;
  uint8_t check = GSL_UART_Read(USARTx, timeout_ms);
  if (check == gsl_fwf_ack_value) {
    return true;
  } else if (check == gsl_fwf_nack_value) {
    return false;
  }
  LOG("\nUnexpected value ", GSL_OUT_Hex(check), " waiting for ACK/NACK");
  return false;
}

// receive ACK or NACK using SPI
bool GSL_FWF_GetACK_SPI(uint32_t timeout_ms) {
  ASSERT(gsl_fwf_peripheral_family == kPeripheralSPI);
  SPI_TypeDef * SPIx = (SPI_TypeDef *) gsl_fwf_peripheral;

  GSL_SPI_Send(SPIx, 0x00);
  GSL_DEL_LongTime start = GSL_DEL_GetLongTime();
  while (GSL_DEL_ElapsedMS(start) < timeout_ms) {
    // initialize bootloader
    uint8_t data = GSL_SPI_SendRead(SPIx, 0x00);
    if (data == 0x79) {
      GSL_SPI_Send(SPIx, 0x79);
      return true;
    } else if (data == 0x1F) {
      GSL_SPI_Send(SPIx, 0x79);
      return false;
    }
    GSL_DEL_MS(1);
  }
  LOG("\nTimeout waiting for ACK/NACK");
  return false;
}

// return true for ACK, false for NACK
bool GSL_FWF_GetACK(uint32_t timeout_ms = 100) {
  if (gsl_fwf_peripheral_family == kPeripheralSPI) {
    return GSL_FWF_GetACK_SPI(timeout_ms);
  } else if (gsl_fwf_peripheral_family == kPeripheralUART) {
    return GSL_FWF_GetACK_UART(timeout_ms);
  } else {
    HALT("Unexpected value");
  }
}

// define this as a macro so we get line information output when it
// fails
#define GSL_FWF_ForceACK(...) \
  if (!GSL_FWF_GetACK(__VA_ARGS__)) { \
    HALT("Received NACK when expecting ACK."); \
  }
/*
// wait for an ACK, halt if we get a NACK
void GSL_FWF_ForceACK(uint32_t timeout_ms = 100) {
  if (!GSL_FWF_GetACK(timeout_ms)) {
    HALT("Did not receive ACK when needed.");
  }
}*/

// wait for an ACK signal and report progress in seconds
bool GSL_FWF_LongWaitForACK(uint32_t timeout_ms) {
  auto start_time = GSL_DEL_GetLongTime();
  SPI_TypeDef * SPIx = (SPI_TypeDef *) gsl_fwf_peripheral;
  // wait for a byte to be ready
  if (gsl_fwf_peripheral_family == kPeripheralSPI) {
    GSL_SPI_Send(SPIx, 0x00);
  }
  while (GSL_DEL_ElapsedS(start_time) < timeout_ms * 0.001f) {
    // update progress
    if (gsl_fwf_progress_update_function) {
      gsl_fwf_progress_update_function(GSL_DEL_ElapsedSFloat(start_time));
    }
    if (gsl_fwf_peripheral_family == kPeripheralSPI) {
      GSL_SPI_Send(SPIx, 0x79);
      // attempt to read a response
      uint8_t data = GSL_SPI_SendRead(SPIx, 0);
      // if response was ACK or NACK, then it is probably valid
      if (data == gsl_fwf_ack_value) {
        GSL_SPI_Send(SPIx, gsl_fwf_ack_value);
        return true;
      } else if (data == gsl_fwf_nack_value) {
        GSL_SPI_Send(SPIx, gsl_fwf_ack_value);
        return false;
      }
    } else if (gsl_fwf_peripheral_family == kPeripheralUART) {
      // if a response is waiting, then exit
      if (((USART_TypeDef *) gsl_fwf_peripheral)->SR & UART_FLAG_RXNE) {
        uint8_t data = GSL_UART_Read((USART_TypeDef *) gsl_fwf_peripheral);
        if (data == gsl_fwf_ack_value) {
          return true;
        } else if (data == gsl_fwf_nack_value) {
          return false;
        }
      }
    } else {
      HALT("Unexpected value");
    }
  }
  LOG("\nTimeout waiting for ACK/NACK");
  return false;
}

// list bootloader info returned from get command using the SPI peripheral
void GSL_FWF_LogInformation_SPI(void) {
  // verify correct peripheral
  ASSERT(gsl_fwf_peripheral_family == kPeripheralSPI);
  //SPI_TypeDef * SPIx = (SPI_TypeDef *) gsl_fwf_peripheral;
  HALT("");
}

// list bootloader info returned from get command using the UART peripheral
void GSL_FWF_LogInformation_UART(void) {
  // verify correct peripheral
  ASSERT(gsl_fwf_peripheral_family == kPeripheralUART);
  USART_TypeDef * USARTx = (USART_TypeDef *) gsl_fwf_peripheral;
  // send command
  GSL_UART_Send(USARTx, 0x00);
  GSL_UART_Send(USARTx, 0xFF);
  // read ACK
  GSL_FWF_ForceACK();
  // read number of bytes
  uint8_t bytes = GSL_UART_Read(USARTx);
  // read version
  uint8_t version = GSL_UART_Read(USARTx);
  LOG("\nBootloader version v", version / 16, ".", version % 16);
  LOG("\nCommands: ");
  for (uint16_t i = 0; i < bytes; ++i) {
    uint8_t command = GSL_UART_Read(USARTx);
    if (i > 0) {
      LOG(", ");
    }
    LOG(GSL_OUT_Hex(command));
  }
  // read ACK
  GSL_FWF_ForceACK();
}

// list bootloader info returned from get ID command
void GSL_FWF_LogID(void) {
  // send command
  if (gsl_fwf_peripheral_family == kPeripheralSPI) {
    GSL_FWF_Send(0x5A);
  }
  GSL_FWF_Send(0x02);
  GSL_FWF_Send(0xFD);
  // read ACK
  GSL_FWF_ForceACK();
  // read number of bytes
  uint8_t bytes = GSL_FWF_Read();
  ASSERT_EQ(bytes, 1);
  // read version
  uint8_t id[2];
  GSL_FWF_ReadMulti(id, 2);
  LOG("\nChip ID: ", GSL_OUT_Hex((uint16_t) (256 * ((uint16_t) id[0]) + id[1])));
  // read ACK
  GSL_FWF_ForceACK();
}

// list bootloader info returned from get command
void GSL_FWF_LogInformation(void) {
  if (gsl_fwf_peripheral_family == kPeripheralSPI) {
    return GSL_FWF_LogInformation_SPI();
  } else if (gsl_fwf_peripheral_family == kPeripheralUART) {
    return GSL_FWF_LogInformation_UART();
  } else {
    HALT("Unexpected value");
  }
}

// enter the bootloader with UART peripheral
void GSL_FWF_EnterBootloader_UART(void) {
  ASSERT_EQ(gsl_fwf_peripheral_family, kPeripheralUART);
  USART_TypeDef * USARTx = (USART_TypeDef *) gsl_fwf_peripheral;
  GSL_UART_Send(USARTx, 0x7F);
  GSL_FWF_ForceACK();
}

// enter the bootloader with SPI peripheral
void GSL_FWF_EnterBootloader_SPI(void) {
  ASSERT(gsl_fwf_peripheral_family == kPeripheralSPI);
  SPI_TypeDef * SPIx = (SPI_TypeDef *) gsl_fwf_peripheral;
  uint8_t check = GSL_SPI_SendRead(SPIx, 0x5A);
  if (check != 0xA5) {
    LOG("\nReceived ", GSL_OUT_Hex(check), " when expecting 0xA5");
    HALT("Unexpected value");
  }
  if (!GSL_FWF_GetACK()) {
    LOG("\nReceived NACK when expecting ACK");
    HALT("Unexpected value");
  }
}

// perform a full erase using an SPI peripheral
void GSL_FWF_MassErase(void) {
  // start time
  auto start_time = GSL_DEL_GetLongTime();
  LOG("\nPerforming mass erase");
  //ASSERT(gsl_fwf_peripheral_family == kPeripheralSPI);
  //SPI_TypeDef * SPIx = (SPI_TypeDef *) gsl_fwf_peripheral;
  if (gsl_fwf_peripheral_family == kPeripheralSPI) {
    GSL_FWF_Send(0x5A);
  }
  GSL_FWF_Send(0x44);
  GSL_FWF_Send(0xBB);
  GSL_FWF_ForceACK();
  GSL_FWF_Send(0xFF);
  GSL_FWF_Send(0xFF);
  GSL_FWF_Send(0x00);
  // wait up to 30s for this to finish
  GSL_FWF_LongWaitForACK(30000);
  float elapsed_s = GSL_DEL_ElapsedS(start_time);
  LOG("\n- Operation took ", GSL_OUT_FixedFloat(elapsed_s, 1), " s");
}

// duplicate the current chip's firmware onto the target chip
/*void GSL_FWF_DuplicateFirmware_UART(void) {
  ASSERT(gsl_fwf_peripheral_family == kPeripheralUART);
  USART_TypeDef * USARTx = (USART_TypeDef *) gsl_fwf_peripheral;
  const uint32_t size = GSL_GEN_GetFlashSizeInUse();
  uint8_t * start = (uint8_t *) GSL_GEN_MemoryFlashStart;
  uint8_t * end = start + size;
  // program data

  while (start != end) {
    // find out length to send
    uint32_t this_length = 256;
    if (start + this_length > end) {
      this_length = end - start;
    }
    // send the command
    GSL_SPI_Send(SPIx, 0x31);
    GSL_SPI_Send(SPIx, 0xCE);
    GSL_FWF_ForceACK();
    uint8_t data[5];
    data[0] = ((uint32_t) start) >> 24;
    data[1] = ((uint32_t) start) >> 16;
    data[2] = ((uint32_t) start) >> 8;
    data[3] = ((uint32_t) start) >> 0;
    data[4] = 0x00 ^ data[0] ^ data[1] ^ data[2] ^ data[3];
    GSL_SPI_SendMulti(SPIx, data, 5);
    GSL_FWF_ForceACK();
    // send the data
    uint8_t crc = 0;
    GSL_SPI_Send(SPIx, this_length - 1);
    crc ^= this_length - 1;
    for (uint32_t i = 0; i < this_length; ++i) {
      GSL_SPI_Send(SPIx, *start);
      crc ^= *start;
      ++start;
    }
    GSL_SPI_Send(SPIx, crc);
    GSL_FWF_ForceACK();
    // update progress
    if (gsl_fwf_progress_update_function) {
      float progress =
          (float) (start - (uint8_t *) GSL_GEN_MemoryFlashStart) / size;
      gsl_fwf_progress_update_function(progress);
    }
  }
}

// duplicate the current chip's firmware onto the target chip
void GSL_FWF_DuplicateFirmware_SPI(void) {
  ASSERT(gsl_fwf_peripheral_family == kPeripheralSPI);
  SPI_TypeDef * SPIx = (SPI_TypeDef *) gsl_fwf_peripheral;
  const uint32_t size = GSL_GEN_GetFlashSizeInUse();
  uint8_t * start = (uint8_t *) GSL_GEN_MemoryFlashStart;
  uint8_t * end = start + size;
  // program data

  while (start != end) {
    // find out length to send
    uint32_t this_length = 256;
    if (start + this_length > end) {
      this_length = end - start;
    }
    // send the command
    GSL_SPI_Send(SPIx, 0x5A);
    GSL_SPI_Send(SPIx, 0x31);
    GSL_SPI_Send(SPIx, 0xCE);
    GSL_FWF_ForceACK();
    uint8_t data[5];
    data[0] = ((uint32_t) start) >> 24;
    data[1] = ((uint32_t) start) >> 16;
    data[2] = ((uint32_t) start) >> 8;
    data[3] = ((uint32_t) start) >> 0;
    data[4] = 0x00 ^ data[0] ^ data[1] ^ data[2] ^ data[3];
    GSL_SPI_SendMulti(SPIx, data, 5);
    GSL_FWF_ForceACK();
    // send the data
    uint8_t crc = 0;
    GSL_SPI_Send(SPIx, this_length - 1);
    crc ^= this_length - 1;
    for (uint32_t i = 0; i < this_length; ++i) {
      GSL_SPI_Send(SPIx, *start);
      crc ^= *start;
      ++start;
    }
    GSL_SPI_Send(SPIx, crc);
    GSL_FWF_ForceACK();
    // update progress
    if (gsl_fwf_progress_update_function) {
      float progress =
          (float) (start - (uint8_t *) GSL_GEN_MemoryFlashStart) / size;
      gsl_fwf_progress_update_function(progress);
    }
  }
}*/

// duplicate the current chip's firmware onto the target chip
void GSL_FWF_DuplicateFirmware(void) {
  LOG("\nDuplicating firmware:");
  // start time
  auto start_time = GSL_DEL_GetLongTime();
  // get length of current firmware
  const uint32_t size = GSL_GEN_GetFlashSizeInUse();
  uint8_t * start = (uint8_t *) GSL_GEN_MemoryFlashStart;
  uint8_t * end = start + size;
  // program data
  LOG("\n- Firmware is ", size, " bytes");
  while (start != end) {
    // find out length to send
    uint32_t this_length = 256;
    if (start + this_length > end) {
      this_length = end - start;
    }
    // send start of frame for SPI only
    if (gsl_fwf_peripheral_family == kPeripheralSPI) {
      GSL_FWF_Send(0x5A);
    }
    // send the write memory command
    GSL_FWF_Send(0x31);
    GSL_FWF_Send(0xCE);
    // read an ACK
    GSL_FWF_ForceACK();
    // send the memory address and crc
    uint8_t data[5];
    data[0] = ((uint32_t) start) >> 24;
    data[1] = ((uint32_t) start) >> 16;
    data[2] = ((uint32_t) start) >> 8;
    data[3] = ((uint32_t) start) >> 0;
    data[4] = 0x00 ^ data[0] ^ data[1] ^ data[2] ^ data[3];
    GSL_FWF_SendMulti(data, 5);
    // read an ACK
    GSL_FWF_ForceACK();
    // send the data length,  data and crc
    uint8_t crc = 0;
    GSL_FWF_Send(this_length - 1);
    crc ^= this_length - 1;
    for (uint32_t i = 0; i < this_length; ++i) {
      GSL_FWF_Send(*start);
      crc ^= *start;
      ++start;
    }
    GSL_FWF_Send(crc);
    // read an ACK
    GSL_FWF_ForceACK();
    // update progress
    if (gsl_fwf_progress_update_function) {
      float progress =
          (float) (start - (uint8_t *) GSL_GEN_MemoryFlashStart) / size;
      gsl_fwf_progress_update_function(progress);
    }
  }
  // report how long it took
  uint32_t elapsed_ms = GSL_DEL_ElapsedMS(start_time);
  LOG("\n- Operation took ", elapsed_ms, " ms");
  float kb_per_sec = size * 1000.0f / elapsed_ms;
  LOG("\n- Speed of ", GSL_OUT_FixedFloat(kb_per_sec, 1), " kB/sec");
}

// duplicate the current chip's firmware onto the target chip
/*
void GSL_FWF_DuplicateFirmware(void) {
  switch (gsl_fwf_peripheral_family) {
    case kPeripheralSPI:
      GSL_FWF_DuplicateFirmware_UART();
      break;
    case kPeripheralSPI:
      GSL_FWF_DuplicateFirmware_SPI();
      break;
    default:
      HALT("Unexpected value");
  }
}
*/

// verify the chip matches this chip's firmware
void GSL_FWF_VerifyDuplicateFirmware(bool full = false) {
  LOG("\nVerifying duplicate firmware:");
  // start time
  auto start_time = GSL_DEL_GetLongTime();
  uint8_t * start = (uint8_t *) GSL_GEN_MemoryFlashStart;
  const uint32_t size =
      (full) ? GSL_GEN_GetFlashSize() * 1024 : GSL_GEN_GetFlashSizeInUse();
  uint8_t * end = start + size;
  // program data
  LOG("\n- Firmware is ", size, " bytes");
  // program data
  uint32_t total = 0;
  while (start != end) {
    // find out length to send
    uint32_t this_length = 256;
    if (start + this_length > end) {
      this_length = end - start;
    }
    total += this_length;
    // send the command
    // (once in a blue moon we would receive a NACK, so
    // we simply retry a few times)
    const uint8_t tries = 5;
    for (uint8_t i = 0; i < tries; ++i) {
      if (gsl_fwf_peripheral_family == kPeripheralSPI) {
        GSL_FWF_Send(0x5A);
      }
      GSL_FWF_Send(0x11);
      GSL_FWF_Send(0xEE);
      if (GSL_FWF_GetACK()) {
        break;
      }
      if (i == tries - 1) {
        HALT("ERROR");
      }
    }
    uint8_t data[5];
    data[0] = ((uint32_t) start) >> 24;
    data[1] = ((uint32_t) start) >> 16;
    data[2] = ((uint32_t) start) >> 8;
    data[3] = ((uint32_t) start) >> 0;
    data[4] = 0x00 ^ data[0] ^ data[1] ^ data[2] ^ data[3];
    GSL_FWF_SendMulti(data, 5);
    GSL_FWF_ForceACK();
    // read and verify the data
    uint8_t crc = 0;
    GSL_FWF_Send(this_length - 1);
    crc = 0xFF ^ (this_length - 1);
    GSL_FWF_Send(crc);
    GSL_FWF_ForceACK();
    // the SPI peripheral sends ACK twice here
    if (gsl_fwf_peripheral_family == kPeripheralSPI) {
      ASSERT_EQ(GSL_FWF_Read(), gsl_fwf_ack_value);
    }
    for (uint32_t i = 0; i < this_length; ++i) {
      uint8_t data = GSL_FWF_Read();
      if (data != *start) {
        LOG("\nFirmware verification failed at address ");
        LOG(GSL_OUT_Hex((uint32_t) start));
        LOG("\nValue ", GSL_OUT_Hex(data), " vs. ", GSL_OUT_Hex(*start));
        HALT("ERROR");
      }
      ++start;
    }
    // update progress
    if (gsl_fwf_progress_update_function) {
      float progress =
          (float) (start - (uint8_t *) GSL_GEN_MemoryFlashStart) / size;
      gsl_fwf_progress_update_function(progress);
    }
  }
  // report how long it took
  uint32_t elapsed_ms = GSL_DEL_ElapsedMS(start_time);
  LOG("\n- Operation took ", elapsed_ms, " ms");
  float kb_per_sec = size * 1000.0f / elapsed_ms;
  LOG("\n- Speed of ", GSL_OUT_FixedFloat(kb_per_sec, 1), " kB/sec");
}
/*
// verify the chip matches this chip's firmware
void GSL_FWF_VerifyDuplicateFirmware_UART(bool full) {
  ASSERT(gsl_fwf_peripheral_family == kPeripheralUART);
  USART_TypeDef * USARTx = (USART_TypeDef *) gsl_fwf_peripheral;
  uint8_t * start = (uint8_t *) GSL_GEN_MemoryFlashStart;
  const uint32_t size =
      (full) ? GSL_GEN_GetFlashSize() * 1024 : GSL_GEN_GetFlashSizeInUse();
  uint8_t * end = start + size;
  // program data
  volatile uint32_t total = 0;
  while (start != end) {
    // find out length to send
    uint32_t this_length = 256;
    if (start + this_length > end) {
      this_length = end - start;
    }
    total += this_length;
    // send the command
    // (once in a blue moon we would receive a NACK, so
    // we simply retry a few times)
    const uint8_t tries = 5;
    for (uint8_t i = 0; i < tries; ++i) {
      //GSL_UART_Send(USARTx, 0x5A);
      GSL_UART_Send(USARTx, 0x11);
      GSL_UART_Send(USARTx, 0xEE);
      if (GSL_FWF_GetACK()) {
        break;
      }
      if (i == tries - 1) {
        HALT("ERROR");
      }
    }
    uint8_t data[5];
    data[0] = ((uint32_t) start) >> 24;
    data[1] = ((uint32_t) start) >> 16;
    data[2] = ((uint32_t) start) >> 8;
    data[3] = ((uint32_t) start) >> 0;
    data[4] = 0x00 ^ data[0] ^ data[1] ^ data[2] ^ data[3];
    GSL_UART_SendMulti(USARTx, data, 5);
    GSL_FWF_ForceACK();
    // read and verify the data
    uint8_t crc = 0;
    GSL_UART_Send(USARTx, this_length - 1);
    crc = 0xFF ^ (this_length - 1);
    GSL_UART_Send(USARTx, crc);
    GSL_FWF_ForceACK();
    //GSL_DEL_MS(1);
    //ASSERT(GSL_UART_Read(USARTx) == 0x79);
    for (uint32_t i = 0; i < this_length; ++i) {
      uint8_t data = GSL_UART_Read(USARTx);
      if (data != *start) {
        LOG("\nFirmware verification failed at address ");
        LOG(GSL_OUT_Hex((uint32_t) start));
        LOG("\nValue ", GSL_OUT_Hex(data), " vs. ", GSL_OUT_Hex(*start));
        HALT("ERROR");
      }
      ++start;
    }
    // update progress
    if (gsl_fwf_progress_update_function) {
      float progress =
          (float) (start - (uint8_t *) GSL_GEN_MemoryFlashStart) / size;
      gsl_fwf_progress_update_function(progress);
    }
  }
}

// verify the chip matches this chip's firmware
void GSL_FWF_VerifyDuplicateFirmware_SPI(bool full) {
  ASSERT(gsl_fwf_peripheral_family == kPeripheralSPI);
  SPI_TypeDef * SPIx = (SPI_TypeDef *) gsl_fwf_peripheral;
  uint8_t * start = (uint8_t *) GSL_GEN_MemoryFlashStart;
  const uint32_t size =
      (full) ? GSL_GEN_GetFlashSize() * 1024 : GSL_GEN_GetFlashSizeInUse();
  uint8_t * end = start + size;
  // program data
  volatile uint32_t total = 0;
  while (start != end) {
    // find out length to send
    uint32_t this_length = 256;
    if (start + this_length > end) {
      this_length = end - start;
    }
    total += this_length;
    // send the command
    // (once in a blue moon we would receive a NACK, so
    // we simply retry a few times)
    const uint8_t tries = 5;
    for (uint8_t i = 0; i < tries; ++i) {
      GSL_SPI_Send(SPIx, 0x5A);
      GSL_SPI_Send(SPIx, 0x11);
      GSL_SPI_Send(SPIx, 0xEE);
      if (GSL_FWF_GetACK()) {
        break;
      }
      if (i == tries - 1) {
        HALT("ERROR");
      }
    }
    uint8_t data[5];
    data[0] = ((uint32_t) start) >> 24;
    data[1] = ((uint32_t) start) >> 16;
    data[2] = ((uint32_t) start) >> 8;
    data[3] = ((uint32_t) start) >> 0;
    data[4] = 0x00 ^ data[0] ^ data[1] ^ data[2] ^ data[3];
    GSL_SPI_SendMulti(SPIx, data, 5);
    GSL_FWF_ForceACK();
    // read and verify the data
    uint8_t crc = 0;
    GSL_SPI_Send(SPIx, this_length - 1);
    crc = 0xFF ^ (this_length - 1);
    GSL_SPI_Send(SPIx, crc);
    GSL_FWF_ForceACK();
    //GSL_DEL_MS(1);
    ASSERT(GSL_SPI_Read(SPIx) == 0x79);
    for (uint32_t i = 0; i < this_length; ++i) {
      uint8_t data = GSL_SPI_Read(SPIx);
      if (data != *start) {
        LOG("\nFirmware verification failed at address ");
        LOG(GSL_OUT_Hex((uint32_t) start));
        LOG("\nValue ", GSL_OUT_Hex(data), " vs. ", GSL_OUT_Hex(*start));
        HALT("ERROR");
      }
      ++start;
    }
    // update progress
    if (gsl_fwf_progress_update_function) {
      float progress =
          (float) (start - (uint8_t *) GSL_GEN_MemoryFlashStart) / size;
      gsl_fwf_progress_update_function(progress);
    }
  }
}
*/

// reset the chip and start the code stored in flash
void GSL_FWF_ResetToFlash(void) {
  LOG("\nResetting to flash memory");
  if (gsl_fwf_peripheral_family == kPeripheralSPI) {
    GSL_FWF_Send(0x5A);
  }
  GSL_FWF_Send(0x21);
  GSL_FWF_Send(0xDE);
  // wait for ACK
  GSL_FWF_ForceACK();
  // address
  GSL_FWF_Send(0x08);
  GSL_FWF_Send(0x00);
  GSL_FWF_Send(0x00);
  GSL_FWF_Send(0x00);
  // checksum of address
  GSL_FWF_Send(0x08);
  // wait for ACK
  GSL_FWF_ForceACK();
  LOG("\n- done");
}

// verify the chip's firmware
/*void GSL_FWF_VerifyDuplicateFirmware(bool full = false) {
  switch (gsl_fwf_peripheral_family) {
    case kPeripheralUART:
      GSL_FWF_VerifyDuplicateFirmware_UART(full);
      break;
    case kPeripheralSPI:
      GSL_FWF_VerifyDuplicateFirmware_SPI(full);
      break;
  default:
    HALT("Unexpected value");
  }
}*/

// enter the bootloader
void GSL_FWF_EnterBootloader(void) {
  switch (gsl_fwf_peripheral_family) {
    case kPeripheralUART:
      GSL_FWF_EnterBootloader_UART();
      break;
    case kPeripheralSPI:
      GSL_FWF_EnterBootloader_SPI();
      break;
    default:
      HALT("Unexpected value");
  }
  LOG("\nBootloader entered successfully.");
}

// perform a full chip erase
/*void GSL_FWF_MassErase(void) {
  if (gsl_fwf_peripheral_family == kPeripheralSPI) {
    GSL_FWF_MassErase_SPI();
  } else {
    HALT("Unexpected value");
  }
}*/

// set the peripheral to be used in future routines
void GSL_FWF_SetPeripheral(void * ptr) {
  // look for UART peripherals
  for (uint16_t i = 0; i < GSL_UART_PeripheralCount; ++i) {
    if (ptr == GSL_UART_Info[i].USARTx) {
      gsl_fwf_peripheral_family = kPeripheralUART;
      gsl_fwf_peripheral = ptr;
      return;
    }
  }
  // look for SPI peripherals
  for (uint16_t i = 0; i < GSL_SPI_PeripheralCount; ++i) {
    if (ptr == GSL_SPI_Info[i].SPIx) {
      gsl_fwf_peripheral_family = kPeripheralSPI;
      gsl_fwf_peripheral = ptr;
      return;
    }
  }
  // look through SPI peripherals
  HALT("Unexpected value");
  // initialize values
  gsl_fwf_peripheral_family = kPeripheralUnused;
  gsl_fwf_peripheral = nullptr;
}

// puts THIS chip into bootloader mode
// this requires a bit of setup as detailed in AB2606
// * disable all peripheral clocks
// * disable used PLL
// * disable interrupts
// * clear pending interrupts
void GSL_FWF_JumpToBootloader(void) {
  LOG("\nEntering bootloader mode!");
  GSL_DELOG_WaitUntilEmpty();
  /*
  // deinitialize I2C peripherals
  LOG("\nDeinitializing I2C peripherals...");
  GSL_DELOG_WaitUntilEmpty();
  for (uint16_t i = 0; i < GSL_I2C_PeripheralCount; ++i) {
    GSL_I2C_Deinitialize(GSL_I2C_Info[i].I2Cx);
  }
  // deinitialize SPI peripherals
  LOG("\nDeinitializing SPI peripherals...");
  GSL_DELOG_WaitUntilEmpty();
  for (uint16_t i = 0; i < GSL_SPI_PeripheralCount; ++i) {
    GSL_SPI_Deinitialize(GSL_SPI_Info[i].SPIx);
  }
  // deinitialize UART peripherals
  LOG("\nDeinitializing UART peripherals...");
  GSL_DELOG_WaitUntilEmpty();
  for (uint16_t i = 0; i < GSL_UART_PeripheralCount; ++i) {
    GSL_UART_Deinitialize(GSL_UART_Info[i].USARTx);
  }
  */
  // disable interrupts
  GSL_GEN_DisableInterrupts();
  // switch clock
  //GSL_CLK_SwitchToSafeHSI();
  // deinitialize clock
  HAL_RCC_DeInit();
  SysTick->CTRL = 0;
  SysTick->LOAD = 0;
  SysTick->VAL = 0;
  volatile uint32_t addr = 0x1FFF0000;
  void (*SysMemBootJump)(void);
  __disable_irq();
  __HAL_SYSCFG_REMAPMEMORY_SYSTEMFLASH();
  SysMemBootJump = (void (*)(void)) (*((uint32_t *)(addr + 4)));
  __set_MSP(*(uint32_t *)addr);
  SysMemBootJump();
}
