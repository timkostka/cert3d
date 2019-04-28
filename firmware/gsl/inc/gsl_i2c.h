#pragma once

// This provides a interface to configure and use the I2C peripherals.
//
// Note that all slave addresses are given as 7-bit
// Possible range is 0x00 to 0x7F
//
// Note also that slave addresses given to the HAL library are bit-shifted left
// one bit.  This is transparent to the user.
//
// No DMA functionality is provided, as SPI or USART are better options.

#include "gsl_includes.h"

// convert an I2C pointer to a 0-based number
// e.g. I2C1 -> 0
uint32_t GSL_I2C_GetNumber(I2C_TypeDef * I2Cx) {
  for (uint32_t i = 0; i < GSL_I2C_PeripheralCount; ++i) {
    if (GSL_I2C_Info[i].I2Cx == I2Cx) {
      return i;
    }
  }
  HALT("Invalid parameter");
  return -1;
}

// enable the clock
void GSL_I2C_EnableClock(I2C_TypeDef * I2Cx) {
  if (false) {
#ifdef I2C1
  } else if (I2Cx == I2C1) {
    __HAL_RCC_I2C1_CLK_ENABLE();
#endif
#ifdef I2C2
  } else if (I2Cx == I2C2) {
    __HAL_RCC_I2C2_CLK_ENABLE();
#endif
#ifdef I2C3
  } else if (I2Cx == I2C3) {
    __HAL_RCC_I2C3_CLK_ENABLE();
#endif
  } else {
    HALT("Invalid parameter");
  }
}

// return pointer to the info struct
GSL_I2C_InfoStruct * GSL_I2C_GetInfo(I2C_TypeDef * I2Cx) {
  return &GSL_I2C_Info[GSL_I2C_GetNumber(I2Cx)];
}

// return the EV IRQ for the given I2C peripheral
// e.g. I2C1 -> I2C1_EV_IRQn
IRQn_Type GSL_I2C_GetIRQ_EV(I2C_TypeDef * I2Cx) {
  if (0) {
#ifdef I2C1
  } else if (I2Cx == I2C1) {
    return I2C1_EV_IRQn;
#endif
#ifdef I2C2
  } else if (I2Cx == I2C2) {
    return I2C2_EV_IRQn;
#endif
#ifdef I2C3
  } else if (I2Cx == I2C3) {
    return I2C3_EV_IRQn;
#endif
  } else {
    HALT("Invalid parameter");
  }
}

// return the ER IRQ for the given I2C peripheral
// e.g. I2C1 -> I2C1_ER_IRQn
IRQn_Type GSL_I2C_GetIRQ_ER(I2C_TypeDef * I2Cx) {
  if (0) {
#ifdef I2C1
  } else if (I2Cx == I2C1) {
    return I2C1_ER_IRQn;
#endif
#ifdef I2C2
  } else if (I2Cx == I2C2) {
    return I2C2_ER_IRQn;
#endif
#ifdef I2C3
  } else if (I2Cx == I2C3) {
    return I2C3_ER_IRQn;
#endif
  } else {
    HALT("Invalid parameter");
  }
}

// initialize I2C using #defined values
void GSL_I2C_Initialize(I2C_TypeDef * I2Cx) {
  // get the handle
  GSL_I2C_InfoStruct * info = GSL_I2C_GetInfo(I2Cx);
  // if already initialized, don't do anything
  if (info->handle->State != HAL_I2C_STATE_RESET) {
    return;
  }
  // initialize pins
  GSL_PIN_InitializeAF(
      info->pins->scl,
      GPIO_MODE_AF_OD,
      GPIO_PULLUP,
      info->af_mode);
  GSL_PIN_InitializeAF(
      info->pins->sda,
      GPIO_MODE_AF_OD,
      GPIO_PULLUP,
      info->af_mode);
  // enable the clock
  GSL_I2C_EnableClock(I2Cx);
  // enable interrupts
  {
    IRQn_Type irq = GSL_I2C_GetIRQ_EV(I2Cx);
    HAL_NVIC_SetPriority(irq, 8, 0);
    HAL_NVIC_EnableIRQ(irq);
  }
  {
    IRQn_Type irq = GSL_I2C_GetIRQ_ER(I2Cx);
    HAL_NVIC_SetPriority(irq, 8, 0);
    HAL_NVIC_EnableIRQ(irq);
  }
  //
  // initialize
  HAL_RUN(HAL_I2C_Init(info->handle));
  // link DMA TX
  __HAL_LINKDMA(info->handle, hdmatx, *(info->hdmatx));
  // link DMA RX
  __HAL_LINKDMA(info->handle, hdmarx, *(info->hdmarx));
}

// deinitialize I2C peripheral
void GSL_I2C_Deinitialize(I2C_TypeDef * I2Cx) {
  // get the handle
  GSL_I2C_InfoStruct * info = GSL_I2C_GetInfo(I2Cx);
  // if already deinitialized, don't do anything
  if (info->handle->State == HAL_I2C_STATE_RESET) {
    return;
  }
  // deinitialize pins
  GSL_PIN_Deinitialize(info->pins->scl);
  GSL_PIN_Deinitialize(info->pins->sda);
  // disable interrupts
  HAL_NVIC_DisableIRQ(GSL_I2C_GetIRQ_EV(I2Cx));
  HAL_NVIC_DisableIRQ(GSL_I2C_GetIRQ_ER(I2Cx));
  // deinitialize
  HAL_RUN(HAL_I2C_DeInit(info->handle));
  // deinitialize DMA TX
  if (info->hdmatx) {
    HAL_RUN(HAL_DMA_DeInit(info->hdmatx));
    HAL_NVIC_DisableIRQ(GSL_DMA_GetIRQ(info->hdmatx->Instance));
  }
  // initialize DMA RX
  if (info->hdmarx) {
    HAL_RUN(HAL_DMA_DeInit(info->hdmarx));
    HAL_NVIC_DisableIRQ(GSL_DMA_GetIRQ(info->hdmarx->Instance));
  }
}

// report an I2C error and halt the program
void GSL_I2C_ReportError(HAL_StatusTypeDef status) {
  if (status == HAL_OK) {
    return;
  }
  LOG("\nERROR: Encountered I2C error code ", (uint32_t) status);
  HALT("ERROR");
}

// ensure the slave address is in the range of allowed values
void GSL_I2C_ValidateSlaveAddress(uint16_t slave_address) {
  if ((slave_address & 0x7F) != slave_address) {
    HALT("Invalid slave address");
  }
}

// return true if the given I2C is ready to send data
bool GSL_I2C_Ready(I2C_TypeDef * I2Cx) {
  // get the handle
  I2C_HandleTypeDef * hi2c = GSL_I2C_GetInfo(I2Cx)->handle;
  // initialize if not already done
  if (hi2c->State == HAL_I2C_STATE_RESET) {
    GSL_I2C_Initialize(I2Cx);
  }
  // return the appropriate state
  return (hi2c->State == HAL_I2C_STATE_READY);
}

// wait until the given I2C bus is available or timeout expires
// if not ready at the end of the timeout period, error out
void GSL_I2C_Wait(I2C_TypeDef * I2Cx, uint32_t timeout_ms = 1000) {
  // get the start time
  uint32_t start = GSL_DEL_Ticks();
  // wait until it's ready or the timeout occurs
  while (!GSL_I2C_Ready(I2Cx) &&
      GSL_DEL_ElapsedMS(start) < timeout_ms) {
  }
  // if it's still not ready, there's likely a problem
  if (!GSL_I2C_Ready(I2Cx)) {
    HALT("\nI2C bus timeout");
  }
}

// return true if the given slave is present
bool GSL_I2C_IsSlavePresent(
    I2C_TypeDef * I2Cx,
    uint8_t slave_address) {
  // get handle
  I2C_HandleTypeDef * hi2c = GSL_I2C_GetInfo(I2Cx)->handle;
  // make sure slave address is valid
  GSL_I2C_ValidateSlaveAddress(slave_address);
  // wait for peripheral to be ready
  GSL_I2C_Wait(I2Cx);
  // make the call
  HAL_StatusTypeDef status =
      HAL_I2C_IsDeviceReady(hi2c, slave_address << 1, 1, GSL_I2C_TIMEOUT);
  switch (status) {
    case HAL_OK:
      return true;
    case HAL_ERROR:
      return false;
    default:
      HAL_RUN(status);
      return false;
  }
}

// list all I2C slaves
void GSL_I2C_ListSlaves(I2C_TypeDef * I2Cx) {
  // loop through all slaves
  LOG("\n\nLooking for I2C slaves...");
  for (uint8_t i = 0; i < 0x80; ++i) {
    if (GSL_I2C_IsSlavePresent(I2Cx, i)) {
      LOG("\nSlave at ", GSL_OUT_Hex(i));
    }
  }
}

// read one byte from an 8-bit memory address
uint8_t GSL_I2C_ReadAddress(
    I2C_TypeDef * I2Cx,
    uint16_t slave_address,
    uint8_t memory_address) {
  // get handle
  I2C_HandleTypeDef * hi2c = GSL_I2C_GetInfo(I2Cx)->handle;
  // make sure slave address is valid
  GSL_I2C_ValidateSlaveAddress(slave_address);
  // wait for peripheral to be ready
  GSL_I2C_Wait(I2Cx);
  // transmit and report an error if it occurs
  uint8_t value;
  HAL_RUN(
      HAL_I2C_Mem_Read(
          hi2c,
          slave_address << 1,
          memory_address,
          I2C_MEMADD_SIZE_8BIT,
          &value,
          1,
          GSL_I2C_TIMEOUT));
  return value;
}

// write one byte an 8-bit memory address
void GSL_I2C_WriteAddress(
    I2C_TypeDef * I2Cx,
    uint16_t slave_address,
    uint8_t memory_address,
    uint8_t value) {
  // get handle
  I2C_HandleTypeDef * hi2c = GSL_I2C_GetInfo(I2Cx)->handle;
  // make sure slave address is valid
  GSL_I2C_ValidateSlaveAddress(slave_address);
  // wait for peripheral to be ready
  GSL_I2C_Wait(I2Cx);
  // transmit and report an error if it occurs
  HAL_RUN(
      HAL_I2C_Mem_Write(
          hi2c,
          slave_address << 1,
          memory_address,
          I2C_MEMADD_SIZE_8BIT,
          &value,
          1,
          GSL_I2C_TIMEOUT));
}

// ready many bytes starting from an 8-bit memory address
void GSL_I2C_ReadAddressMulti(
    I2C_TypeDef * I2Cx,
    uint16_t slave_address,
    uint8_t memory_address,
    uint8_t * data_in,
    uint8_t length) {
  // get handle
  I2C_HandleTypeDef * hi2c = GSL_I2C_GetInfo(I2Cx)->handle;
  // make sure slave address is valid
  GSL_I2C_ValidateSlaveAddress(slave_address);
  // wait for peripheral to be ready
  GSL_I2C_Wait(I2Cx);
  // transmit and report an error if it occurs
  HAL_RUN(
      HAL_I2C_Mem_Read(
          hi2c,
          slave_address << 1,
          memory_address,
          I2C_MEMADD_SIZE_8BIT,
          data_in,
          length,
          GSL_I2C_TIMEOUT));
}

// write many bytes to an 8-bit memory address
void GSL_I2C_WriteAddressMulti(
    I2C_TypeDef * I2Cx,
    uint16_t slave_address,
    uint8_t memory_address,
    uint8_t * data_out,
    uint16_t length) {
  // get handle
  I2C_HandleTypeDef * hi2c = GSL_I2C_GetInfo(I2Cx)->handle;
  // make sure slave address is valid
  GSL_I2C_ValidateSlaveAddress(slave_address);
  // wait for peripheral to be ready
  GSL_I2C_Wait(I2Cx);
  // transmit and report an error if it occurs
  HAL_RUN(
      HAL_I2C_Mem_Write(
          hi2c,
          slave_address << 1,
          memory_address,
          I2C_MEMADD_SIZE_8BIT,
          data_out,
          length,
          GSL_I2C_TIMEOUT));
}

// read many bytes from an 8-bit memory address using interrupts
void GSL_I2C_ReadAddressMulti_IT(
    I2C_TypeDef * I2Cx,
    uint16_t slave_address,
    uint8_t memory_address,
    uint8_t * data_in,
    uint8_t length) {
  // get handle
  I2C_HandleTypeDef * hi2c = GSL_I2C_GetInfo(I2Cx)->handle;
  // make sure slave address is valid
  GSL_I2C_ValidateSlaveAddress(slave_address);
  // wait for peripheral to be ready
  GSL_I2C_Wait(I2Cx);
  // transmit and report an error if it occurs
  HAL_RUN(
      HAL_I2C_Mem_Read_IT(
          hi2c,
          slave_address << 1,
          memory_address,
          I2C_MEMADD_SIZE_8BIT,
          data_in,
          length));
}

// write many bytes to an 8-bit memory address using interrupts
void GSL_I2C_WriteAddressMulti_IT(
    I2C_TypeDef * I2Cx,
    uint16_t slave_address,
    uint8_t memory_address,
    uint8_t * data_out,
    uint16_t length) {
  // get handle
  I2C_HandleTypeDef * hi2c = GSL_I2C_GetInfo(I2Cx)->handle;
  // make sure slave address is valid
  GSL_I2C_ValidateSlaveAddress(slave_address);
  // wait for peripheral to be ready
  GSL_I2C_Wait(I2Cx);
  // transmit and report an error if it occurs
  HAL_RUN(
      HAL_I2C_Mem_Write_IT(
          hi2c,
          slave_address << 1,
          memory_address,
          I2C_MEMADD_SIZE_8BIT,
          data_out,
          length));
}

// read many bytes from an 8-bit memory address using DMA
void GSL_I2C_ReadAddressMulti_DMA(
    I2C_TypeDef * I2Cx,
    uint16_t slave_address,
    uint8_t memory_address,
    uint8_t * data_in,
    uint8_t length) {
  // get handle
  I2C_HandleTypeDef * hi2c = GSL_I2C_GetInfo(I2Cx)->handle;
  // make sure slave address is valid
  GSL_I2C_ValidateSlaveAddress(slave_address);
  // wait for peripheral to be ready
  GSL_I2C_Wait(I2Cx);
  // initialize DMA
  GSL_DMA_Initialize(hi2c->hdmarx);
  // transmit and report an error if it occurs
  HAL_RUN(
      HAL_I2C_Mem_Read_DMA(
          hi2c,
          slave_address << 1,
          memory_address,
          I2C_MEMADD_SIZE_8BIT,
          data_in,
          length));
}

// write many bytes to an 8-bit memory address using DMA
void GSL_I2C_WriteAddressMulti_DMA(
    I2C_TypeDef * I2Cx,
    uint16_t slave_address,
    uint8_t memory_address,
    uint8_t * data_out,
    uint16_t length) {
  // get handle
  I2C_HandleTypeDef * hi2c = GSL_I2C_GetInfo(I2Cx)->handle;
  // make sure slave address is valid
  GSL_I2C_ValidateSlaveAddress(slave_address);
  // wait for peripheral to be ready
  GSL_I2C_Wait(I2Cx);
  // initialize DMA
  GSL_DMA_Initialize(hi2c->hdmatx);
  // transmit and report an error if it occurs
  HAL_RUN(
      HAL_I2C_Mem_Write_DMA(
          hi2c,
          slave_address << 1,
          memory_address,
          I2C_MEMADD_SIZE_8BIT,
          data_out,
          length));
}

// I2C IRQ handlers
extern "C" {

// error handler
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c) {
  LOG("\nAn error occurred on I2C ",
      GSL_OUT_Hex((uint32_t) hi2c->Instance));
  LOG("\nError code: ", GSL_OUT_Hex(hi2c->ErrorCode));
}

// callbacks
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c) {
  // get the info
  GSL_I2C_InfoStruct * info = GSL_I2C_GetInfo(hi2c->Instance);
  // call the callback routine if one is defined
  if (info->MasterTxCpltCallback) {
    info->MasterTxCpltCallback();
  }
}
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c) {
  // get the info
  GSL_I2C_InfoStruct * info = GSL_I2C_GetInfo(hi2c->Instance);
  // call the callback routine if one is defined
  if (info->MasterRxCpltCallback) {
    info->MasterRxCpltCallback();
  }
}
void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c) {
  // get the info
  GSL_I2C_InfoStruct * info = GSL_I2C_GetInfo(hi2c->Instance);
  // call the callback routine if one is defined
  if (info->SlaveTxCpltCallback) {
    info->SlaveTxCpltCallback();
  }
}
void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c) {
  // get the info
  GSL_I2C_InfoStruct * info = GSL_I2C_GetInfo(hi2c->Instance);
  // call the callback routine if one is defined
  if (info->SlaveRxCpltCallback) {
    info->SlaveRxCpltCallback();
  }
}
void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c) {
  // get the info
  GSL_I2C_InfoStruct * info = GSL_I2C_GetInfo(hi2c->Instance);
  // call the callback routine if one is defined
  if (info->MemTxCpltCallback) {
    info->MemTxCpltCallback();
  }
}
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c) {
  // get the info
  GSL_I2C_InfoStruct * info = GSL_I2C_GetInfo(hi2c->Instance);
  // call the callback routine if one is defined
  if (info->MemRxCpltCallback) {
    info->MemRxCpltCallback();
  }
}

#ifdef I2C1
// I2C1 event IRQ handler
void I2C1_EV_IRQHandler(void) {
  HAL_I2C_EV_IRQHandler(&gsl_i2c1_hi2c);
}
// I2C1 error IRQ handler
void I2C1_ER_IRQHandler(void) {
  HAL_I2C_ER_IRQHandler(&gsl_i2c1_hi2c);
}
#endif

#ifdef I2C2
// I2C2 event IRQ handler
void I2C2_EV_IRQHandler(void) {
  HAL_I2C_EV_IRQHandler(&gsl_i2c2_hi2c);
}
// I2C2 error IRQ handler
void I2C2_ER_IRQHandler(void) {
  HAL_I2C_ER_IRQHandler(&gsl_i2c2_hi2c);
}
#endif

#ifdef I2C3
// I2C3 event IRQ handler
void I2C3_EV_IRQHandler(void) {
  HAL_I2C_EV_IRQHandler(&gsl_i2c3_hi2c);
}
// I2C3 error IRQ handler
void I2C3_ER_IRQHandler(void) {
  HAL_I2C_ER_IRQHandler(&gsl_i2c3_hi2c);
}
#endif

} // extern "C"
