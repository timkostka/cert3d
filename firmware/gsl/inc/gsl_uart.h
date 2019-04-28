#pragma once

// This provides a interface to configure and use the U(S)ART peripherals in
// asynchronous mode.

#include "gsl_includes.h"

// convert a USART pointer to a 0-based integer
// e.g. USART1 -> 0
uint32_t GSL_UART_GetNumber(USART_TypeDef * USARTx) {
  for (uint32_t i = 0; i < GSL_UART_PeripheralCount; ++i) {
    if (GSL_UART_Info[i].USARTx == USARTx) {
      return i;
    }
  }
  return -1;
}

// enable the clock
void GSL_UART_EnableClock(USART_TypeDef * USARTx) {
  if (false) {
#ifdef USART1
  } else if (USARTx == USART1) {
    __HAL_RCC_USART1_CLK_ENABLE();
#endif
#ifdef USART2
  } else if (USARTx == USART2) {
    __HAL_RCC_USART2_CLK_ENABLE();
#endif
#ifdef USART3
  } else if (USARTx == USART3) {
    __HAL_RCC_USART3_CLK_ENABLE();
#endif
#ifdef UART4
  } else if (USARTx == UART4) {
    __HAL_RCC_UART4_CLK_ENABLE();
#endif
#ifdef UART5
  } else if (USARTx == UART5) {
    __HAL_RCC_UART5_CLK_ENABLE();
#endif
#ifdef USART6
  } else if (USARTx == USART6) {
    __HAL_RCC_USART6_CLK_ENABLE();
#endif
#ifdef UART7
  } else if (USARTx == UART7) {
    __HAL_RCC_UART7_CLK_ENABLE();
#endif
#ifdef UART8
  } else if (USARTx == UART8) {
    __HAL_RCC_UART8_CLK_ENABLE();
#endif
  } else {
    HALT("Unexpected value");
  }
}

// disable the clock
void GSL_UART_DisableClock(USART_TypeDef * USARTx) {
  if (false) {
#ifdef USART1
  } else if (USARTx == USART1) {
    __HAL_RCC_USART1_CLK_DISABLE();
#endif
#ifdef USART2
  } else if (USARTx == USART2) {
    __HAL_RCC_USART2_CLK_DISABLE();
#endif
#ifdef USART3
  } else if (USARTx == USART3) {
    __HAL_RCC_USART3_CLK_DISABLE();
#endif
#ifdef UART4
  } else if (USARTx == UART4) {
    __HAL_RCC_UART4_CLK_DISABLE();
#endif
#ifdef UART5
  } else if (USARTx == UART5) {
    __HAL_RCC_UART5_CLK_DISABLE();
#endif
#ifdef USART6
  } else if (USARTx == USART6) {
    __HAL_RCC_USART6_CLK_DISABLE();
#endif
#ifdef UART7
  } else if (USARTx == UART7) {
    __HAL_RCC_UART7_CLK_DISABLE();
#endif
#ifdef UART8
  } else if (USARTx == UART8) {
    __HAL_RCC_UART8_CLK_DISABLE();
#endif
  } else {
    HALT("Unexpected value");
  }
}

// return pointer to the info struct
GSL_UART_InfoStruct * GSL_UART_GetInfo(USART_TypeDef * USARTx) {
  return &GSL_UART_Info[GSL_UART_GetNumber(USARTx)];
}

// return the IRQ for the given USART peripheral
// e.g. USART1 -> USART1_IRQn
IRQn_Type GSL_UART_GetIRQ(USART_TypeDef * USARTx) {
  if (0) {
#ifdef USART1
  } else if (USARTx == USART1) {
    return USART1_IRQn;
#endif
#ifdef USART2
  } else if (USARTx == USART2) {
    return USART2_IRQn;
#endif
#ifdef USART3
  } else if (USARTx == USART3) {
    return USART3_IRQn;
#endif
#ifdef UART4
  } else if (USARTx == UART4) {
    return UART4_IRQn;
#endif
#ifdef UART5
  } else if (USARTx == UART5) {
    return UART5_IRQn;
#endif
#ifdef USART6
  } else if (USARTx == USART6) {
    return USART6_IRQn;
#endif
#ifdef UART7
  } else if (USARTx == UART7) {
    return UART7_IRQn;
#endif
#ifdef UART8
  } else if (USARTx == UART8) {
    return UART8_IRQn;
#endif
  } else {
    HALT("Invalid parameter");
  }
}

// set up the callback routine for UART send completion
void GSL_UART_SetSendCompleteCallback(
    USART_TypeDef * USARTx,
    void (*callback_function)(USART_TypeDef * USARTx)) {
  // get the handle
  GSL_UART_InfoStruct * info = GSL_UART_GetInfo(USARTx);
  // set the routine
  info->SendCompleteCallback = callback_function;
}

// set up the callback routine for UART send half completion
void GSL_UART_SetSendHalfCompleteCallback(
    USART_TypeDef * USARTx,
    void (*callback_function)(USART_TypeDef * USARTx)) {
  // get the handle
  GSL_UART_InfoStruct * info = GSL_UART_GetInfo(USARTx);
  // set the routine
  info->SendHalfCompleteCallback = callback_function;
}

// set up the callback routine for UART read completion
void GSL_UART_SetReadCompleteCallback(
    USART_TypeDef * USARTx,
    void (*callback_function)(USART_TypeDef * USARTx)) {
  // get the handle
  GSL_UART_InfoStruct * info = GSL_UART_GetInfo(USARTx);
  // set the routine
  info->ReadCompleteCallback = callback_function;
}

// set up the callback routine for UART read half completion
void GSL_UART_SetReadHalfCompleteCallback(
    USART_TypeDef * USARTx,
    void (*callback_function)(USART_TypeDef * USARTx)) {
  // get the handle
  GSL_UART_InfoStruct * info = GSL_UART_GetInfo(USARTx);
  // set the routine
  info->ReadHalfCompleteCallback = callback_function;
}

// return the maximum possible clock speed given the current system clock
// and oversampling selection
uint32_t GSL_UART_GetMaxClock(USART_TypeDef * USARTx) {
  // get the info struct
  GSL_UART_InfoStruct * info = GSL_UART_GetInfo(USARTx);
  // get the pclock speed
  uint32_t clock = (USARTx == USART1
#ifdef USART6
      || USARTx == USART6
#endif
      ) ?
      HAL_RCC_GetPCLK2Freq() : HAL_RCC_GetPCLK1Freq();
  // get the OVER8 bit
  bool over8 = info->handle->Init.OverSampling == UART_OVERSAMPLING_8;
  return clock / ((over8) ? 8 : 16);
}

// initialize the given SPI peripheral
void GSL_UART_Initialize(USART_TypeDef * USARTx) {
  // get the handle
  GSL_UART_InfoStruct * info = GSL_UART_GetInfo(USARTx);
  // if already initialized, don't change
  if (info->handle->gState != HAL_UART_STATE_RESET) {
    return;
  }
  // initialize TX pin
  if (info->handle->Init.Mode == UART_MODE_TX ||
      info->handle->Init.Mode == UART_MODE_TX_RX) {
    GSL_PIN_InitializeAF(
        info->pins->tx,
        GPIO_MODE_AF_PP,
        GPIO_PULLUP,
        info->af_mode);
  }
  // initialize RX pin
  if (info->handle->Init.Mode == UART_MODE_RX ||
      info->handle->Init.Mode == UART_MODE_TX_RX) {
    GSL_PIN_InitializeAF(
        info->pins->rx,
        GPIO_MODE_AF_PP,
        GPIO_PULLUP,
        info->af_mode);
  }
  // ensure target clock is reasonable
  {
    uint32_t max_clock = GSL_UART_GetMaxClock(USARTx);
    if (info->handle->Init.BaudRate > max_clock) {
      info->handle->Init.BaudRate = max_clock;
    }
  }
  // enable the clock
  GSL_UART_EnableClock(USARTx);
  // enable interrupt
  {
    IRQn_Type irq = GSL_UART_GetIRQ(USARTx);
    HAL_NVIC_SetPriority(irq, info->irq_priority, 0);
    HAL_NVIC_EnableIRQ(irq);
  }
  // initialize
  HAL_RUN(HAL_UART_Init(info->handle));
  // link DMA TX
  __HAL_LINKDMA(info->handle, hdmatx, *(info->hdmatx));
  // link DMA RX
  __HAL_LINKDMA(info->handle, hdmarx, *(info->hdmarx));
}

// deinitialize the given SPI peripheral
void GSL_UART_Deinitialize(USART_TypeDef * USARTx) {
  // get the handle
  GSL_UART_InfoStruct * info = GSL_UART_GetInfo(USARTx);
  // if already deinitialized, don't change
  if (info->handle->gState == HAL_UART_STATE_RESET) {
    return;
  }
  // deinitialize TX pin
  if (info->handle->Init.Mode == UART_MODE_TX ||
      info->handle->Init.Mode == UART_MODE_TX_RX) {
    GSL_PIN_Deinitialize(info->pins->tx);
  }
  // initialize RX pin
  if (info->handle->Init.Mode == UART_MODE_RX ||
      info->handle->Init.Mode == UART_MODE_TX_RX) {
    GSL_PIN_Deinitialize(info->pins->rx);
  }
  // disable interrupt
  HAL_NVIC_DisableIRQ(GSL_UART_GetIRQ(USARTx));
  // deinitialize
  HAL_RUN(HAL_UART_DeInit(info->handle));
  // if pointers are not null, then initialize DMA functions
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

// set the interrupt priority
void GSL_UART_SetPriority(USART_TypeDef * USARTx, uint8_t priority) {
  // get the info struct
  GSL_UART_InfoStruct * info = GSL_UART_GetInfo(USARTx);
  info->irq_priority = priority;
  // initialize first
  GSL_UART_Initialize(USARTx);
  // now (re)set the priority
  IRQn_Type irq = GSL_UART_GetIRQ(USARTx);
  HAL_NVIC_SetPriority(irq, priority, 0);
}

// return the clock speed
uint32_t GSL_UART_GetClock(USART_TypeDef * USARTx) {
  // get the info struct
  GSL_UART_InfoStruct * info = GSL_UART_GetInfo(USARTx);
  // initialize it
  GSL_UART_Initialize(USARTx);
  // get the pclock speed
  uint32_t clock = (USARTx == USART1 || USARTx == USART6) ?
      HAL_RCC_GetPCLK2Freq() : HAL_RCC_GetPCLK1Freq();
  // get the OVER8 bit
  bool over8 = info->handle->Init.OverSampling == UART_OVERSAMPLING_8;
  // get the fraction from the brr register
  uint32_t brr = info->handle->Instance->BRR;
  uint32_t mantissa = brr >> 4;
  uint32_t fraction = brr & 0b1111;
  if (over8) {
    fraction &= 0b0111;
    fraction <<= 1;
  }
  float usart_div = (mantissa + fraction / 16.0f);
  uint32_t usart_clock = clock / (8.0f * ((over8) ? 1 : 2) * usart_div) + 0.5f;
  return usart_clock;
}

// set the clock speed as close as possible to the target and return the actual
// speed
uint32_t GSL_UART_SetClock(USART_TypeDef * USARTx, uint32_t target_baudrate) {
  // get the info struct
  GSL_UART_InfoStruct * info = GSL_UART_GetInfo(USARTx);
  // deinitialize
  GSL_UART_Deinitialize(USARTx);
  // set the target speed
  info->handle->Init.BaudRate = target_baudrate;
  // ensure target clock is reasonable
  {
    uint32_t max_clock = GSL_UART_GetMaxClock(USARTx);
    if (info->handle->Init.BaudRate > max_clock) {
      info->handle->Init.BaudRate = max_clock;
    }
  }
  // initialize it
  GSL_UART_Initialize(USARTx);
  // return the actual clock
  return GSL_UART_GetClock(USARTx);
}

// ensure the given SPI is enabled
// if not, initialize it
void GSL_UART_CheckEnabled(UART_HandleTypeDef * huart) {
  if (huart->gState == HAL_UART_STATE_RESET) {
    // not initialized, go initialize it
    GSL_UART_Initialize(huart->Instance);
  }
}

// report a UART error and halt the program
/*void GSL_UART_ReportError(HAL_StatusTypeDef status) {
  if (status == HAL_OK) {
    return;
  }
  LOG("\nERROR: Encountered UART error code ", (uint32_t) status);
  HALT("ERROR");
}*/

// return true if the given UART is ready to send data
bool GSL_UART_ReadyToSend(USART_TypeDef * USARTx) {
  // get the handle
  UART_HandleTypeDef * huart = GSL_UART_GetInfo(USARTx)->handle;
  // initialize if not already done
  if (huart->gState == HAL_UART_STATE_RESET) {
    GSL_UART_Initialize(USARTx);
  }
  // return the appropriate state
  return (huart->gState == HAL_UART_STATE_READY ||
          huart->gState == HAL_UART_STATE_BUSY_RX);
}

// return true if the given UART is ready to read data
bool GSL_UART_ReadyToRead(USART_TypeDef * USARTx) {
  // get the handle
  UART_HandleTypeDef * huart = GSL_UART_GetInfo(USARTx)->handle;
  // initialize if not already done
  if (huart->gState == HAL_UART_STATE_RESET) {
    GSL_UART_Initialize(USARTx);
  }
  // return the appropriate state
  return (huart->gState == HAL_UART_STATE_READY ||
          huart->gState == HAL_UART_STATE_BUSY_TX);
}

// wait until the given UART TX bus is available or timeout expires
// if not ready at the end of the timeout period, error out
void GSL_UART_WaitSend(USART_TypeDef * USARTx, uint32_t timeout_ms = 1000) {
  // get the start time
  uint32_t start = GSL_DEL_Ticks();
  // wait until it's ready or the timeout occurs
  while (!GSL_UART_ReadyToSend(USARTx) &&
      GSL_DEL_ElapsedMS(start) < timeout_ms) {
  }
  // if it's still not ready, there's likely a problem
  if (!GSL_UART_ReadyToSend(USARTx)) {
    HALT("\nUART bus timeout");
  }
}

// wait until the given UART RX bus is available or timeout expires
// if not ready at the end of the timeout period, error out
void GSL_UART_WaitRead(USART_TypeDef * USARTx, uint32_t timeout_ms = 1000) {
  // get the start time
  uint32_t start = GSL_DEL_Ticks();
  // wait until it's ready or the timeout occurs
  while (!GSL_UART_ReadyToRead(USARTx) &&
      GSL_DEL_ElapsedMS(start) < timeout_ms) {
  }
  // if it's still not ready, there's likely a problem
  if (!GSL_UART_ReadyToRead(USARTx)) {
    HALT("\nUART bus timeout");
  }
}

// send an 8-bit value
void GSL_UART_Send(
    USART_TypeDef * USARTx,
    uint8_t value) {
  UART_HandleTypeDef * huart = GSL_UART_GetInfo(USARTx)->handle;
  GSL_UART_WaitSend(USARTx);
  HAL_RUN(HAL_UART_Transmit(huart, &value, 1, GSL_UART_TIMEOUT));
}

// send many 8-bit values
void GSL_UART_SendMulti(
    USART_TypeDef * USARTx,
    uint8_t * data_out,
    uint16_t count) {
  UART_HandleTypeDef * huart = GSL_UART_GetInfo(USARTx)->handle;
  GSL_UART_WaitSend(USARTx);
  HAL_RUN(HAL_UART_Transmit(huart, data_out, count, GSL_UART_TIMEOUT));
}

// send a null terminated string, not including the null termination
void GSL_UART_SendString(
    USART_TypeDef * USARTx,
    const char * string) {
  GSL_UART_SendMulti(USARTx, (uint8_t *) string, strlen(string));
}

// send many 8-bit values via interrupt
void GSL_UART_SendMulti_IT(
    USART_TypeDef * USARTx,
    uint8_t * data_out,
    uint16_t count) {
  UART_HandleTypeDef * huart = GSL_UART_GetInfo(USARTx)->handle;
  GSL_UART_WaitSend(USARTx);
  HAL_RUN(HAL_UART_Transmit_IT(huart, data_out, count));
}

// read an 8-bit value
uint8_t GSL_UART_Read(
    USART_TypeDef * USARTx,
    uint32_t timeout_ms = GSL_UART_TIMEOUT) {
  UART_HandleTypeDef * huart = GSL_UART_GetInfo(USARTx)->handle;
  uint8_t value[2];
  GSL_UART_WaitRead(USARTx);
  HAL_RUN(HAL_UART_Receive(huart, value, 1, timeout_ms));
  return value[0];
}

// read an 8-bit value
void GSL_UART_Read(
    USART_TypeDef * USARTx,
    uint8_t * value) {
  UART_HandleTypeDef * huart = GSL_UART_GetInfo(USARTx)->handle;
  GSL_UART_WaitRead(USARTx);
  HAL_RUN(HAL_UART_Receive(huart, value, 1, GSL_UART_TIMEOUT));
}

// read many 8-bit values
void GSL_UART_ReadMulti(
    USART_TypeDef * USARTx,
    uint8_t * data_in,
    uint16_t count) {
  UART_HandleTypeDef * huart = GSL_UART_GetInfo(USARTx)->handle;
  GSL_UART_WaitRead(USARTx);
  HAL_RUN(HAL_UART_Receive(huart, data_in, count, GSL_UART_TIMEOUT));
}

// read many 8-bit values via interrupt
void GSL_UART_ReadMulti_IT(
    USART_TypeDef * USARTx,
    uint8_t * data_in,
    uint16_t count) {
  UART_HandleTypeDef * huart = GSL_UART_GetInfo(USARTx)->handle;
  GSL_UART_WaitRead(USARTx);
  HAL_RUN(HAL_UART_Receive_IT(huart, data_in, count));
}

// start send via DMA
void GSL_UART_SendMulti_DMA(
    USART_TypeDef * USARTx,
    uint8_t * data_out,
    uint16_t count) {
  // get the info struct
  GSL_UART_InfoStruct * info = GSL_UART_GetInfo(USARTx);
  // wait for UART to be ready
  GSL_UART_WaitSend(USARTx);
  // initialize DMA
  GSL_DMA_Initialize(info->handle->hdmatx, info->irq_priority);
  // send it
  HAL_RUN(HAL_UART_Transmit_DMA(info->handle, data_out, count));
}

// start receive via DMA
void GSL_UART_ReadMulti_DMA(
    USART_TypeDef * USARTx,
    uint8_t * data_in,
    uint16_t count) {
  // get the info struct
  GSL_UART_InfoStruct * info = GSL_UART_GetInfo(USARTx);
  // wait for UART to be ready
  GSL_UART_WaitRead(USARTx);
  // set normal mode
  info->handle->hdmarx->Init.Mode = DMA_NORMAL;
  // initialize DMA
  GSL_DMA_Initialize(info->handle->hdmarx, info->irq_priority);
  // receive data
  HAL_RUN(HAL_UART_Receive_DMA(info->handle, data_in, count));
}

// start receive via DMA into a circular buffer
void GSL_UART_ReadMulti_DMA_Circular(
    USART_TypeDef * USARTx,
    uint8_t * data_in,
    uint16_t count) {
  // get the info struct
  GSL_UART_InfoStruct * info = GSL_UART_GetInfo(USARTx);
  // wait for UART to be ready
  GSL_UART_WaitRead(USARTx);
  // set circular mode
  info->handle->hdmarx->Init.Mode = DMA_CIRCULAR;
  // initialize DMA
  GSL_DMA_Initialize(info->handle->hdmarx, info->irq_priority);
  // receive data
  HAL_RUN(HAL_UART_Receive_DMA(info->handle, data_in, count));
}

extern "C" {

#ifdef USART1
void USART1_IRQHandler(void) {
  HAL_UART_IRQHandler(&gsl_usart1_huart);
}
#endif
#ifdef USART2
void USART2_IRQHandler(void) {
  HAL_UART_IRQHandler(&gsl_usart2_huart);
}
#endif
#ifdef USART3
void USART3_IRQHandler(void) {
  HAL_UART_IRQHandler(&gsl_usart3_huart);
}
#endif
#ifdef UART4
void UART4_IRQHandler(void) {
  HAL_UART_IRQHandler(&gsl_uart4_huart);
}
#endif
#ifdef UART5
void UART5_IRQHandler(void) {
  HAL_UART_IRQHandler(&gsl_uart5_huart);
}
#endif
#ifdef USART6
void USART6_IRQHandler(void) {
  HAL_UART_IRQHandler(&gsl_usart6_huart);
}
#endif
#ifdef UART7
void UART7_IRQHandler(void) {
  HAL_UART_IRQHandler(&gsl_uart7_huart);
}
#endif
#ifdef UART8
void UART8_IRQHandler(void) {
  HAL_UART_IRQHandler(&gsl_uart8_huart);
}
#endif


// an error occurred
// error codes are like HAL_UART_ERROR_NONE
void HAL_UART_ErrorCallback(UART_HandleTypeDef * huart) {
  LOG("\nAn error occurred on UART ",
      GSL_OUT_Hex((uint32_t) huart->Instance));
  LOG("\nError code: ",
      GSL_OUT_Hex(huart->ErrorCode));
  if (huart->ErrorCode == HAL_UART_ERROR_DMA) {
    if (huart->hdmatx->State != HAL_DMA_STATE_RESET) {
      // such as HAL_DMA_ERROR_NONE
      LOG("\nDMA TX error code: ", GSL_OUT_Hex((uint32_t) HAL_DMA_GetError(huart->hdmatx)));
    }
    if (huart->hdmarx->State != HAL_DMA_STATE_RESET) {
      LOG("\nDMA RX error code: ", GSL_OUT_Hex((uint32_t) HAL_DMA_GetError(huart->hdmarx)));
    }
  }
}

// transfer complete
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
  // get the info
  GSL_UART_InfoStruct * info = GSL_UART_GetInfo(huart->Instance);
  // call the callback routine if one is defined
  if (info->SendCompleteCallback) {
    info->SendCompleteCallback(huart->Instance);
  }
}

// receive complete
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  // get the info
  GSL_UART_InfoStruct * info = GSL_UART_GetInfo(huart->Instance);
  // call the callback routine if one is defined
  if (info->ReadCompleteCallback) {
    info->ReadCompleteCallback(huart->Instance);
  }
}

// send half complete
void HAL_UART_TxHalfCpltCallback(UART_HandleTypeDef *huart) {
  // get the info
  GSL_UART_InfoStruct * info = GSL_UART_GetInfo(huart->Instance);
  // call the callback routine if one is defined
  if (info->SendHalfCompleteCallback) {
    info->SendHalfCompleteCallback(huart->Instance);
  }
}

// read half complete
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart) {
  // get the info
  GSL_UART_InfoStruct * info = GSL_UART_GetInfo(huart->Instance);
  // call the callback routine if one is defined
  if (info->ReadHalfCompleteCallback) {
    info->ReadHalfCompleteCallback(huart->Instance);
  }
}

} // extern "C"
