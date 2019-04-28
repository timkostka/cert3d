/*

GSL_UARTTX_Send(USART1, "text") is the function called to send something out.
We refer to this function as "Send" down below.

Here are the scenarios we need to handle:
- Send is called from a non-interrupt task or an interrupt task with a priority
  less than that of the handler.

*/

#pragma once

#include "gsl_includes.h"

// convert a USART pointer to a 0-based integer
// e.g. USART1 -> 0
uint32_t GSL_UARTTX_GetNumber(USART_TypeDef * USARTx) {
  for (uint32_t i = 0; i < GSL_UARTTX_PeripheralCount; ++i) {
    if (GSL_UARTTX_Info[i]->USARTx == USARTx) {
      return i;
    }
  }
  HALT("Invalid parameter");
  return -1;
}

// return pointer to the info struct
GSL_UARTTX_InfoStruct * GSL_UARTTX_GetInfo(USART_TypeDef * USARTx) {
  return GSL_UARTTX_Info[GSL_UARTTX_GetNumber(USARTx)];
}

// return true if the buffer is empty
bool GSL_UARTTX_IsEmpty(USART_TypeDef * USARTx) {
  return GSL_UARTTX_GetInfo(USARTx)->buffer.GetSizeInUse() == 0;
}

// start a new transfer if data is ready to send
void GSL_UARTTX_SendIfPossible(USART_TypeDef * USARTx) {
  // get the infos
  auto info = GSL_UARTTX_GetInfo(USARTx);
  auto uart_info = GSL_UART_GetInfo(USARTx);
  // to prevent a race condition, disable interrupts briefly
  __disable_irq();
  // if USARTx is free and the buffer isn't frozen, then try to send
  if ((uart_info->handle->gState == HAL_UART_STATE_READY ||
       uart_info->handle->gState == HAL_UART_STATE_BUSY_RX) &&
      info->buffer.GetFrozenBufferSize() == 0) {
    // freeze some portion of the buffer
    info->buffer.Freeze();
    // if frozen region is nonzero, then send it out
    if (info->buffer.GetFrozenBufferSize()) {
      GSL_UART_SendMulti_DMA(USARTx,
                             info->buffer.frozen_buffer_start_,
                             info->buffer.GetFrozenBufferSize());
    }
  }
  // re-enable interrupts
  __enable_irq();
}

// transfer complete handler
void GSL_UARTTX_TransferComplete(USART_TypeDef * USARTx) {
  // get the info
  auto info = GSL_UARTTX_GetInfo(USARTx);
  // mark bytes as sent
  info->processed_bytes += info->buffer.GetFrozenBufferSize();
  // unfreeze the bytes that were sent, if any
  info->buffer.Thaw();
  // send out more data if possible
  GSL_UARTTX_SendIfPossible(USARTx);
}

// initialize
void GSL_UARTTX_Initialize(USART_TypeDef * USARTx) {
  // get the info
  auto info = GSL_UARTTX_GetInfo(USARTx);
  // if already active, do nothing
  if (info->active) {
    return;
  }
  // set up the buffer
  info->buffer.SetCapacity(info->capacity);
  // ensure the mode is TX or TXRX
  ASSERT_NE(GSL_UART_GetInfo(USARTx)->handle->Init.Mode, UART_MODE_RX);
  // set up transfer complete callback
  GSL_UART_SetSendCompleteCallback(
      USARTx,
      GSL_UARTTX_TransferComplete);
  // initialize the UART
  GSL_UART_Initialize(USARTx);
  // mark it as active
  info->active = true;
}

// deinitialize
void GSL_UARTTX_Deinitialize(USART_TypeDef * USARTx,
                             uint16_t timeout_ms = 100) {
  // get the info
  auto info = GSL_UARTTX_GetInfo(USARTx);
  // if inactive, do nothing
  if (!info->active) {
    return;
  }
  // wait until it's inactive
  auto start_time = GSL_DEL_GetLongTime();
  while (!GSL_UARTTX_IsEmpty(USARTx) &&
      GSL_DEL_ElapsedMS(start_time) < timeout_ms) {
  }
  // buffer should be empty
  ASSERT(GSL_UARTTX_IsEmpty(USARTx));
  // deinitialize the peripheral
  GSL_UART_Deinitialize(USARTx);
  // mark it as inactive
  info->active = false;
}

// send out raw bytes
void GSL_UARTTX_Send(USART_TypeDef * USARTx, uint8_t * data, uint16_t length) {
  // if not sending any data, just return
  if (length == 0) {
    return;
  }
  // get the info
  auto info = GSL_UARTTX_GetInfo(USARTx);
  // if not initialized, then initialize
  if (!info->active) {
    GSL_UARTTX_Initialize(USARTx);
  }
  // add to the buffer
  if (info->buffer.GetCapacityRemaining() >= length) {
    info->buffer.AddBytes(data, length);
  } else {
    uint16_t size = info->buffer.GetCapacityRemaining();
    info->buffer.AddBytes(data, size);
    // discard the rest, flag something here
    LOG("\nERROR: Bytes were discarded in GSL_UARTTX_Send");
    LOG("\n- Sent ", size, " out of ", length, " bytes");
    LOG("\n- Capacity: ", size, "/", info->buffer.capacity_);
  }
  // try to send if possible
  GSL_UARTTX_SendIfPossible(USARTx);
}

// send out a null-terminated string
void GSL_UARTTX_Send(USART_TypeDef * USARTx, const char * message) {
  GSL_UARTTX_Send(USARTx, (uint8_t *) message, strlen(message));
}
