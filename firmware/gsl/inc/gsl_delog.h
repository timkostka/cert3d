#pragma once

// The serial logger implements a DMA-based outgoing USART stream.  By using
// DMA, control is returned to the CPU immediately after placing data into the
// USART buffer.  The stream continues on in the background, by using
// interrupts, until completed.
//
// Data may be added to the stream by the following command:
// LOG("Hello world!");
//
// The USART peripheral is automatically initialized upon the first instance
// of calling LOG().

// TODO: there is some bug when a block occurs in the KCT_TESTER program that results
// in an invalid message being sent.  not sure the cause of this.
//
// Tester chip on.
// Current draw: 126.5 mA
// Pinging tester processor...
// BAD MESSAGE: unexpected data length (received:65535, expected:3) 0x8D 0x1B 0x01

// If you send too much information and the buffer overflows, there are a few
// configurable possibilities that may occur.


#include "gsl_includes.h"

// the port used by the serial logger
#ifndef GSL_DELOG_PORT
#define GSL_DELOG_PORT USART1
#endif

// serial logger buffer capacity
#ifndef GSL_DELOG_CAPACITY
#define GSL_DELOG_CAPACITY 1024
#endif

// serial logger handler priority
#ifndef GSL_DELOG_PRIORITY
#define GSL_DELOG_PRIORITY 12
#endif

// lock for accessing the UART peripheral or the buffer
GSL_CODELOCK gsl_delog_lock;

// if true, will output in blocking mode
// (this is helpful to turn on while debugging a crash)
bool gsl_delog_block_always = false;

// if true, will convert LF to CR+LF
bool gsl_delog_convert_lf_to_crlf = true;

// if true, serial logger will block on buffer overflow and wait for the buffer
// to clear before sending more characters.
// if false, characters will be dropped
// in either case, characters will be dropped if currently executing interrupt
// is an equal or higher priority as the delog priority.
bool gsl_delog_block_on_overflow = true;

// if true, a timestamp will be added immediately after a \n is encountered
bool gsl_delog_add_timestamps = false;

// if true, strip leading zeroes from timestamps
bool gsl_delog_strip_leading_zeroes_on_timestamps = true;

// total number of characters sent so far
uint32_t gsl_delog_character_sent_count = 0;

// macro to convert integer to a string
#define STRINGIFY(x) #x
#define TO_STRING(x) STRINGIFY(x)

// message to send when we can't send a message due to the buffer being
// locked
const char * gsl_delog_cant_send_message =
    "\n\nGSL_DELOG: Bytes were DISCARDED due to the buffer being locked.  "
    "This message\nwill only display once.\n\n";

// message to send when we block on overflow
const char * gsl_delog_block_event_message =
    "\n\nGSL_DELOG: Execution was BLOCKED to wait for the buffer to empty.  "
    "Consider\nincreasing the buffer size.  The current buffer size is "
    TO_STRING(GSL_DELOG_CAPACITY) ".  This message\nwill only display once."
    "\n\n";

// message to send when we discard bytes due to overflow
const char * gsl_delog_discard_event_message =
    "\n\nGSL_DELOG: Bytes were DISCARDED due to the buffer being full.  "
    "Consider\nincreasing the buffer size.  The current buffer size is "
    TO_STRING(GSL_DELOG_CAPACITY) ".  This message\nwill only display once."
    "\n\n";

// set to true if bytes were discarded due to buffer lock
bool gsl_delog_discard_event_2 = false;

// set to true when message is sent
bool gsl_delog_discard_event_2_message_sent = false;

// set to true if bytes were discarded
bool gsl_delog_discard_event = false;

// set to true when message is sent
bool gsl_delog_discard_event_message_sent = false;

// set to true if execution was blocked
bool gsl_delog_block_event = false;

// set to true when message is sent
bool gsl_delog_block_event_message_sent = false;

// output messages if buffer overrun or blocking occurs
bool gsl_delog_status_messages = true;

// locked when a transfer is ongoing
// unlocked when transfer is complete
GSL_CODELOCK gsl_delog_transfer_lock;

// typedef a callback function
typedef void (*GSL_DELOG_CallbackTypedef)(uint8_t *, uint16_t);

// if non-null, this function will be called after logging the original data
GSL_DELOG_CallbackTypedef gsl_delog_callback = nullptr;

// buffer for holding data to be output
GSL_CBUF gsl_delog_buffer(0);

void GSL_DELOG_SendIfPresent(void);

// Return true if we're not in an interrupt of equal or higher priority than
// the gsl_delog priority.  If this is true, we can block execution to wait
// for the buffer to clear.
bool GSL_DELOG_LowerPriority(void) {
  return !GSL_GEN_InInterrupt() ||
      GSL_GEN_GetInterruptPriority() > GSL_DELOG_PRIORITY;
}

// called via interrupt when a transfer is complete
void GSL_DELOG_TransferComplete(USART_TypeDef * USARTx) {
  ASSERT(gsl_delog_transfer_lock.locked);
  gsl_delog_buffer.Thaw();
  gsl_delog_transfer_lock.Unlock();

  // optimization barrier
  __DMB();
  GSL_DELOG_SendIfPresent();
}

// return true if the buffer is empty and transfers are complete
bool GSL_DELOG_IsEmpty(void) {
  return GSL_TRYCODELOCK(gsl_delog_transfer_lock) &&
      gsl_delog_buffer.free_buffer_start_ == gsl_delog_buffer.frozen_buffer_start_;
}

// wait for the buffer to be empty
void GSL_DELOG_WaitUntilEmpty(uint32_t timeout_ms = 5000) {
  auto start_time = GSL_DEL_GetLongTime();
  while (!GSL_DELOG_IsEmpty() &&
      GSL_DEL_ElapsedMS(start_time) < timeout_ms) {
  }
  if (!GSL_DELOG_IsEmpty()) {
    HALT("Log did not empty");
  }
}

// stop the transfer and deinitialize it
void GSL_DELOG_Deinitialize(void) {
  GSL_UART_Deinitialize(GSL_DELOG_PORT);
}

// if not already done, initialize everything
void GSL_DELOG_Initialize(void) {
  // the first time, set up the callback routine
  static bool initialized = false;
  if (!initialized) {
    // first access to this should work
    ASSERT(gsl_delog_lock.Lock());
    // set rate and protocol to 115200 8-E-1
    auto handle = GSL_UART_GetInfo(GSL_DELOG_PORT)->handle;
    // ensure settings are correct
    // mode must be TX or TX+RX
    //ASSERT_NE(handle->Init.Mode, USART_MODE_RX);
    //ASSERT_EQ(handle->Init.BaudRate, 115200U);
    //ASSERT_EQ(handle->Init.Parity, USART_PARITY_EVEN);
    //ASSERT_EQ(handle->Init.WordLength, USART_WORDLENGTH_9B);
    //ASSERT_EQ(handle->Init.StopBits, USART_STOPBITS_1);
    //handle->Init.Mode = USART_MODE_TX_RX;
    handle->Init.BaudRate = 115200;
    handle->Init.Parity = USART_PARITY_EVEN;
    handle->Init.WordLength = USART_WORDLENGTH_9B;
    handle->Init.StopBits = USART_STOPBITS_1;
    // set up buffer
    gsl_delog_buffer.SetCapacity(GSL_DELOG_CAPACITY);
    // set up complete callback
    GSL_UART_SetSendCompleteCallback(
        GSL_DELOG_PORT,
        GSL_DELOG_TransferComplete);
    // set interrupt priority
    GSL_UART_SetPriority(GSL_DELOG_PORT, GSL_DELOG_PRIORITY);
    ASSERT(!initialized);
    initialized = true;
    gsl_delog_lock.Unlock();
  }
}

// if we have bytes to send, then send them
void GSL_DELOG_SendIfPresent(void) {
  // if we can't get a lock, a transfer is in progress so just return
  if (!gsl_delog_transfer_lock.Lock()) {
    return;
  }
  // freeze as much as possible in the buffer
  gsl_delog_buffer.Freeze();
  // send the frozen portion
  uint32_t size_to_send = gsl_delog_buffer.GetFrozenBufferSize();
  if (size_to_send) {
    //ASSERT(!gsl_delog_transfer_in_progress);
    //gsl_delog_transfer_in_progress = true;
    gsl_delog_character_sent_count += size_to_send;
    // call the additional function, if defined
    if (gsl_delog_callback) {
      (*gsl_delog_callback)((uint8_t *) gsl_delog_buffer.frozen_buffer_start_,
                            size_to_send);
    }
    // send out message via DMA UART
    GSL_UART_SendMulti_DMA(
        GSL_DELOG_PORT,
        (uint8_t *) gsl_delog_buffer.frozen_buffer_start_,
        size_to_send);
  } else {
    // if we didn't actually start a transfer, unlock the buffer
    gsl_delog_transfer_lock.Unlock();
  }
}

// return true if a transmission is in progress
bool GSL_DELOG_TransferInProgress(void) {
  return gsl_delog_buffer.GetFrozenBufferSize() != 0;
}

void GSL_DELOG_AddMessage(const char * message);

// add bytes to the buffer and begin transmission if possible
void GSL_DELOG_AddRawBytes(const uint8_t * message, uint16_t size) {
  // initialize if not already done
  GSL_DELOG_Initialize();
  // lock the peripheral
  GSL_TRYCODELOCK locked(gsl_delog_lock);
  // if it's already locked, and we can't wait for buffer to empty
  if (!locked) {
    if (GSL_GEN_InInterrupt()) {
      gsl_delog_discard_event_2 = true;
      return;
    }
    // if we're in the main thread, wait for lock to clear
    gsl_delog_lock.WaitAndLock();
    locked.locked = true;
  }
  // for blocking mode, just send it out
  if (gsl_delog_block_always) {
    // send it out
    GSL_UART_SendMulti(GSL_DELOG_PORT, (uint8_t *) message, size);
    return;
  }
  // ensure capacity is sufficient
  if (size > GSL_DELOG_CAPACITY - 1) {
    HALT("Insufficient buffer");
  }
  // see if we have enough space to add the message
  uint32_t available = gsl_delog_buffer.GetCapacityRemaining();
  if (available < size + 1U) {
    if (!gsl_delog_block_on_overflow) {
      // flag a discard event
      gsl_delog_discard_event = true;
      if (available == 0) {
        return;
      }
      // send out what we can
      size = available - 1;
    } else {
      if (GSL_GEN_InInterrupt()) {
        gsl_delog_discard_event_2 = true;
        return;
      }
      // wait until space is free
      locked.lock->Unlock();
      while (available < size + 1U) {
        available = gsl_delog_buffer.GetCapacityRemaining();
      }
      locked.lock->WaitAndLock();
    }
  }
  // add bytes to buffer
  gsl_delog_buffer.AddBytes((const uint8_t *) message, size);
}

// add text to the buffer and begin transmission if possible
void GSL_DELOG_AddRawText(const char * message) {
  GSL_DELOG_AddRawBytes((const uint8_t *) message, strlen(message));
}

// add bytes to the buffer and begin transmission if possible
// this will do the LF -> CRLF conversion if selected
// this will add timestamps if selected (gsl_delog_add_timestamps)
void GSL_DELOG_AddBytes(const uint8_t * message, uint16_t size) {
  // send pending overflow and discard messages
  if (gsl_delog_status_messages && GSL_DELOG_LowerPriority()) {
    // message about blocking on overflow
    if (gsl_delog_block_event && !gsl_delog_block_event_message_sent) {
      const char * text = gsl_delog_block_event_message;
      if (gsl_delog_buffer.GetCapacityRemaining() > strlen(text)) {
        gsl_delog_block_event_message_sent = true;
        GSL_DELOG_AddMessage(text);
      }
    }
    // send discard on overflow message
    if (gsl_delog_discard_event && !gsl_delog_discard_event_message_sent) {
      const char * text = gsl_delog_discard_event_message;
      // only send if we have the capacity to send it without it being
      // discarded
      if (gsl_delog_buffer.GetCapacityRemaining() > strlen(text)) {
        gsl_delog_discard_event_message_sent = true;
        GSL_DELOG_AddMessage(text);
      }
    }
    // send discard on lock message
    if (gsl_delog_discard_event_2 && !gsl_delog_discard_event_2_message_sent) {
      const char * text = gsl_delog_cant_send_message;
      // only send if we have the capacity to send it without it being
      // discarded
      if (gsl_delog_buffer.GetCapacityRemaining() > strlen(text)) {
        gsl_delog_discard_event_2_message_sent = true;
        GSL_DELOG_AddMessage(text);
      }
    }
  }
  // if we're not converting, just add the bytes
  if (!gsl_delog_convert_lf_to_crlf) {
    GSL_DELOG_AddRawBytes(message, size);
    return;
  }
  // true if CR was the last thing sent
  static bool sent_cr_last = false;
  // loop until all bytes are sent
  while (size) {
    // look for first LF
    const uint8_t * match = (const uint8_t *) memchr(message, '\n', size);
    // if no matches, then send all bytes
    if (match == nullptr) {
      GSL_DELOG_AddRawBytes(message, size);
      size -= size;
      sent_cr_last = message[size - 1] == '\r';
    } else {
      uint32_t length = match - message;
      // send bytes before the LF
      if (length) {
        GSL_DELOG_AddRawBytes(message, length);
        sent_cr_last = message[length - 1] == '\r';
        size -= length;
        message += length;
      }
      // now send an LF or CRLF as necessary
      if (!sent_cr_last) {
        GSL_DELOG_AddRawBytes((const uint8_t *) "\r\n", 2);
      } else {
        GSL_DELOG_AddRawBytes((const uint8_t *) "\n", 1);
      }
      // send timestamp as necessary
      if (gsl_delog_add_timestamps) {
        GSL_DELOG_AddRawText("[");
        const char * text = GSL_GEN_GetTimestampSinceReset();
        if (gsl_delog_strip_leading_zeroes_on_timestamps) {
          while (strncmp("000:", text, 4) == 0) {
            text = &text[4];
          }
          while (strncmp("00:", text, 3) == 0) {
            text = &text[3];
          }
        }
        GSL_DELOG_AddRawText(text);
        GSL_DELOG_AddRawText("] ");
      }
      // skip the LF in the string
      ASSERT_EQ(*message, '\n');
      --size;
      ++message;
      sent_cr_last = false;
    }
  }
}

// add a message to the buffer and begin transmission if possible
void GSL_DELOG_AddMessage(const char * message) {
  GSL_DELOG_AddBytes((const uint8_t *) message, strlen(message));
  GSL_DELOG_SendIfPresent();
}

// set a callback function to be called whenever LOG(x) is called
// set argument to nullptr to clear
// by default, this will wait until the buffer is empty before continuing
void GSL_DELOG_SetCallback(
    GSL_DELOG_CallbackTypedef function,
    bool wait_for_empty = true) {
  while (wait_for_empty && GSL_DELOG_LowerPriority() && !GSL_DELOG_IsEmpty()) {
  }
  gsl_delog_callback = function;
}

// output a message to the log
void LOG(const char * message) {
  GSL_DELOG_AddMessage((char *) message);
}

void GSL_DELOG_UnitTestHelper(void) {
  LOG("\nThis message should never be sent.");
}

/*
// test out some functionality
void GSL_DELOG_UnitTests(void) {
  ASSERT(!GSL_GEN_InInterrupt());
  GSL_DELOG_WaitUntilEmpty();

  // check to see if bytes are discarded
  //gsl_delog_block_always = true;
  GSL_TIM_FutureCallback(TIM1, GSL_DELOG_UnitTestHelper, 0.001);
  LOG("\nThis is a test message.");
  LOG("");
  GSL_DEL_MS(1000);

}
*/

