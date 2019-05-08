#pragma once

// This file implements a DMA-based UART receivers.

#include "gsl_includes.h"

// convert a USART pointer to a 0-based integer
// e.g. USART1 -> 0
uint32_t GSL_UARTRX_GetNumber(USART_TypeDef * USARTx) {
  for (uint32_t i = 0; i < GSL_UARTRX_PeripheralCount; ++i) {
    if (GSL_UARTRX_Info[i]->USARTx == USARTx) {
      return i;
    }
  }
  HALT("Invalid parameter");
  return -1;
}

// return pointer to the info struct
GSL_UARTRX_InfoStruct * GSL_UARTRX_GetInfo(USART_TypeDef * USARTx) {
  return GSL_UARTRX_Info[GSL_UARTRX_GetNumber(USARTx)];
}

// return a pointer to the next byte to be stored
volatile uint8_t * GSL_UARTRX_NextStoredByte(GSL_UARTRX_InfoStruct * info) {
  // get info
  ASSERT(info->hdmarx->Instance->NDTR <= info->capacity);
  ASSERT_GT(info->hdmarx->Instance->NDTR, (uint32_t) 0);
  return info->buffer + (info->capacity - info->hdmarx->Instance->NDTR);
}

// return a pointer to the next byte to be stored
volatile uint8_t * GSL_UARTRX_NextStoredByte(USART_TypeDef * USARTx) {
  // get info
  GSL_UARTRX_InfoStruct * info = GSL_UARTRX_GetInfo(USARTx);
  return GSL_UARTRX_NextStoredByte(info);
}

// return the number of bytes available
uint16_t GSL_UARTRX_Available(GSL_UARTRX_InfoStruct * info) {
  uint16_t count = info->capacity;
  count += GSL_UARTRX_NextStoredByte(info) - info->next_data;
  count %= info->capacity;
  // check for an overflow
  if (info->processed_bytes + count < info->received_bytes) {
    HALT("\nOverflow on UARTRX buffer!");
  } else {
    info->received_bytes = info->processed_bytes + count;
  }
  return count;
}

// return the number of bytes available
uint16_t GSL_UARTRX_Available(USART_TypeDef * USARTx) {
  // get info
  GSL_UARTRX_InfoStruct * info = GSL_UARTRX_GetInfo(USARTx);
  return GSL_UARTRX_Available(info);
}

// return true if the buffer is empty
bool GSL_UARTRX_IsEmpty(USART_TypeDef * USARTx) {
  return GSL_UARTRX_Available(USARTx) == 0;
}

// pop and return a uint8_t value
uint8_t GSL_UARTRX_Pop(GSL_UARTRX_InfoStruct * info) {
  uint8_t value = 0;
  static uint32_t last_ndtr = 0;
  uint32_t this_ndtr = info->hdmarx->Instance->NDTR;
  if (!GSL_UARTRX_Available(info)) {
    LOG("\nLAST NDTR = ", last_ndtr);
    LOG("\n     NDTR = ", this_ndtr);
    LOG("\nnext_data = ", GSL_OUT_Hex((uint32_t) info->next_data));
    LOG("\nbuffer = ", GSL_OUT_Hex((uint32_t) info->buffer));
    LOG("\nGSL_UARTRX_NextStoredByte = ", GSL_OUT_Hex((uint32_t) GSL_UARTRX_NextStoredByte(info)));
    ASSERT(GSL_UARTRX_Available(info));
  }
  last_ndtr = this_ndtr;
  value = *info->next_data;
  ++info->next_data;
  if (info->next_data == info->buffer_end) {
    info->next_data = info->buffer;
  }
  ++info->processed_bytes;
  return value;
}

// pop and return a uint8_t value
uint8_t GSL_UARTRX_Pop(USART_TypeDef * USARTx) {
  GSL_UARTRX_InfoStruct * info = GSL_UARTRX_GetInfo(USARTx);
  return GSL_UARTRX_Pop(info);
}

// copy the next X bytes to the given buffer
// passing a nullptr as the buffer will just ignore the bytes
void GSL_UARTRX_Transfer(
    USART_TypeDef * USARTx,
    uint8_t * destination,
    uint16_t count) {
  // get info
  GSL_UARTRX_InfoStruct * info = GSL_UARTRX_GetInfo(USARTx);
  while (count > 0) {
    if (destination) {
      *destination = GSL_UARTRX_Pop(info);
      ++destination;
    }
    --count;
  }
}

// pop the next X bytes
// return the number of bytes ignored
void GSL_UARTRX_Ignore(
    USART_TypeDef * USARTx,
    uint16_t count) {
  GSL_UARTRX_Transfer(USARTx, nullptr, count);
  // get info
  /*
  GSL_UARTRX_InfoStruct * info = GSL_UARTRX_GetInfo(USARTx);
  uint16_t target = count;
  uint8_t * last_byte = GSL_UARTRX_NextStoredByte(USARTx);
  while (count && info->next_data != last_byte) {
    ++info->next_data;
    if (info->next_data == info->buffer + info->capacity) {
      info->next_data = info->buffer;
    }
    --count;
  }
  info->processed_bytes += target - count;
  return target - count;
  */
}

// begin DMA-based receiving on the given USART channel
void GSL_UARTRX_Initialize(USART_TypeDef * USARTx) {
  // get info
  GSL_UARTRX_InfoStruct * info = GSL_UARTRX_GetInfo(USARTx);
  GSL_UART_InfoStruct * uart_info = GSL_UART_GetInfo(USARTx);
  // should not be active
  ASSERT(!info->active);
  // assign buffer if it's not yet created
  if (info->buffer == nullptr) {
    info->buffer = (uint8_t *) GSL_BUF_Create(info->capacity);
    info->buffer_end = info->buffer + info->capacity;
  }
  // ensure USART settings are correct
  ASSERT_NE(uart_info->handle->Init.Mode, UART_MODE_TX);
  // initialize the USART
  GSL_UART_Initialize(USARTx);
  // set it to active
  info->active = true;
  info->next_data = info->buffer;
  info->hdmarx = uart_info->hdmarx;
  // start the receiving
  GSL_UART_ReadMulti_DMA_Circular(USARTx,
                                  (uint8_t *) info->buffer,
                                  info->capacity);
  // disable the DMA stream interrupts
  info->hdmarx->Instance->CR &= ~(DMA_IT_TC | DMA_IT_HT | DMA_IT_TE | DMA_IT_DME);
  info->hdmarx->Instance->FCR &= ~(DMA_IT_FE);
}

/*
// stop the DMA-based receiving
void GSL_UARTRX_Stop(USART_TypeDef * USARTx) {
  // get info
  GSL_UARTRX_InfoStruct * info = GSL_UARTRX_GetInfo(USARTx);
  GSL_UART_InfoStruct * uart_info = GSL_UART_GetInfo(USARTx);

  // Stop UART DMA Rx request if ongoing
  bool dmarequest = HAL_IS_BIT_SET(USARTx->CR3, USART_CR3_DMAR);
  if((uart_info->handle->RxState == HAL_UART_STATE_BUSY_RX) && dmarequest) {
    CLEAR_BIT(USARTx->CR3, USART_CR3_DMAR);
    // Abort the UART DMA Rx channel
    if (info->hdmarx != NULL) {
      HAL_DMA_Abort(info->hdmarx);
    }
    // Disable RXNE, PE and ERR (Frame error, noise error, overrun error)
    // interrupts
    CLEAR_BIT(uart_info->handle->Instance->CR1,
              (USART_CR1_RXNEIE | USART_CR1_PEIE));
    CLEAR_BIT(uart_info->handle->Instance->CR3,
              USART_CR3_EIE);
    // At end of Rx process, restore huart->RxState to Ready
    uart_info->handle->RxState = HAL_UART_STATE_READY;
  }
}*/

// deinitialize
/*
void GSL_UARTRX_Deinitialize(USART_TypeDef * USARTx,
                             uint16_t timeout_ms = 100) {
  // get the info
  auto info = GSL_UARTRX_GetInfo(USARTx);
  // if active, mark as inactive
  if (info->active) {
    // wait until it's inactive
    uint32_t start_time = GSL_DEL_GetLongTime();
    while (!GSL_UARTTX_IsEmpty(USARTx) &&
        GSL_DEL_ElapsedMS(start_time) < timeout_ms) {
    }
    // buffer should be empty
    ASSERT(GSL_UARTTX_IsEmpty(USARTx));
    return;
    // mark it as inactive
    info->active = false;
  }
  // deinitialize the peripheral
  GSL_UART_Deinitialize(USARTx);
}
*/

// return the given byte number without removing it from the buffer
uint8_t GSL_UARTRX_Peek(USART_TypeDef * USARTx, uint16_t offset = 0) {
  // if nothing available, return 0
  ASSERT_GT(GSL_UARTRX_Available(USARTx), offset);
  // get info
  GSL_UARTRX_InfoStruct * info = GSL_UARTRX_GetInfo(USARTx);
  // find pointer to next data
  volatile uint8_t * ptr = info->next_data;
  ptr += offset;
  if (ptr >= info->buffer_end) {
    ptr -= info->capacity;
  }
  return *ptr;
}

// return the given byte number without removing it from the buffer
uint8_t GSL_UARTRX_PeekTo(USART_TypeDef * USARTx, uint16_t offset = 0) {
  // if nothing available, return 0
  ASSERT_GT(GSL_UARTRX_Available(USARTx), offset);
  // get info
  GSL_UARTRX_InfoStruct * info = GSL_UARTRX_GetInfo(USARTx);
  // find pointer to next data
  volatile uint8_t * ptr = info->next_data;
  ptr += offset;
  if (ptr >= info->buffer_end) {
    ptr -= info->capacity;
  }
  return *ptr;
}

// peek a value of the given type with the given offset
template <class T>
void GSL_UARTRX_PeekTo(USART_TypeDef * USARTx,
                      T & object,
                      uint16_t offset = 0) {
  ASSERT_GT(GSL_UARTRX_Available(USARTx), offset + sizeof(object) - 1);
  uint16_t count = sizeof(object);
  for (uint16_t i = 0; i < count; ++i) {
    ((uint8_t *) &object)[i] = GSL_UARTRX_Peek(USARTx, offset + i);
  }
}

// pop a value of the given type
template <class T>
void GSL_UARTRX_PopTo(USART_TypeDef * USARTx, T & object) {
  GSL_UARTRX_Transfer(USARTx, (uint8_t *) &object, sizeof(object));
}

// empty the buffer
void GSL_UARTRX_Purge(USART_TypeDef * USARTx) {
  GSL_UARTRX_Ignore(USARTx, GSL_UARTRX_Available(USARTx));
  /*// get info
  GSL_UARTRX_InfoStruct * info = GSL_UARTRX_GetInfo(USARTx);
  // set the new pointer
  uint8_t * new_ptr = GSL_UARTRX_NextStoredByte(USARTx);
  // calculate the number of bytes ignored
  uint16_t bytes_ignored =
      (info->capacity + (new_ptr - info->next_data)) % info->capacity;
  // set the new data pointer
  info->next_data = new_ptr;
  info->processed_bytes += bytes_ignored;
  return bytes_ignored;*/
}
