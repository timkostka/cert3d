#pragma once

#include "gsl_includes.h"

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

// temporary
//#include "fatfs_sd_sdio.h"


// hold each possible value for a dma stream
DMA_Stream_TypeDef * const gsl_dma_stream_list[] = {
    DMA1_Stream0, DMA1_Stream1, DMA1_Stream2, DMA1_Stream3,
    DMA1_Stream4, DMA1_Stream5, DMA1_Stream6, DMA1_Stream7,
    DMA2_Stream0, DMA2_Stream1, DMA2_Stream2, DMA2_Stream3,
    DMA2_Stream4, DMA2_Stream5, DMA2_Stream6, DMA2_Stream7};

// total number of streams
const uint16_t gsl_dma_stream_count =
    sizeof(gsl_dma_stream_list) / sizeof(*gsl_dma_stream_list);

// return the index of the given stream
uint16_t GSL_DMA_GetIndex(DMA_Stream_TypeDef * DMAx_Streamy) {
  for (uint16_t i = 0; i < gsl_dma_stream_count; ++i) {
    if (gsl_dma_stream_list[i] == DMAx_Streamy) {
      return i;
    }
  }
  HALT("Unexpected value");
}

// given a stream, return the DMA base
// e.g. DMA2_Stream5 -> DMA2
DMA_TypeDef * GSL_DMA_GetBase(DMA_Stream_TypeDef * DMAx_Streamy) {
  if ((void*) DMAx_Streamy >= (void*) DMA2) {
    return DMA2;
  } else {
    return DMA1;
  }
}

// DMA handles
DMA_HandleTypeDef gsl_hdma[gsl_dma_stream_count] = {0};

// return the handle for the given stream
DMA_HandleTypeDef * GSL_DMA_GetHandle(DMA_Stream_TypeDef * DMAx_Streamy) {
  return &gsl_hdma[GSL_DMA_GetIndex(DMAx_Streamy)];
}

// enable the DMA clock
void GSL_DMA_EnableClock(DMA_TypeDef * DMAx) {
  if (DMAx == DMA1) {
    __HAL_RCC_DMA1_CLK_ENABLE();
  } else if (DMAx == DMA2) {
    __HAL_RCC_DMA2_CLK_ENABLE();
  } else {
    LOG("unknown value");
  }
}

// IRQ handlers for various libraries

// These are awkward to define.  On the one hand, we need them to be defined to
// handle requests when they are needed, such as when using SPI in DMA mode,
// however we would prefer not to define them if they are not necessary.

// if the stream matches up and is busy, call the given DMA IRQ handler
void GSL_DMA_HandleIfMatch(
    DMA_Stream_TypeDef * DMAx_Streamy,
    DMA_HandleTypeDef * hdma) {
  if (hdma &&
      DMAx_Streamy == hdma->Instance &&
      hdma->State != HAL_DMA_STATE_RESET) {
    HAL_DMA_IRQHandler(hdma);
  }
}

// these are super awkward!
extern "C" {

#define GSL_DMA_LocalStream DMA1_Stream0
void DMA1_Stream0_IRQHandler(void) {
#ifdef GSL_LOG_IRQ_ONCE
  LOG_ONCE("\nIn ", __FUNCTION__);
#endif
#ifdef SPI3
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_spi3_hspi.hdmarx);
#endif
#ifdef I2C1
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_i2c1_hi2c.hdmarx);
#endif
#ifdef TIM4
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim4_htim.hdma[TIM_DMA_ID_CC1]);
#endif
#ifdef UART5
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_uart5_huart.hdmarx);
#endif
#ifdef UART8
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_uart8_huart.hdmatx);
#endif
#ifdef TIM5
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim5_htim.hdma[TIM_DMA_ID_CC3]);
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim5_htim.hdma[TIM_DMA_ID_UPDATE]);
#endif
}
#undef GSL_DMA_LocalStream

#define GSL_DMA_LocalStream DMA1_Stream1
void DMA1_Stream1_IRQHandler(void) {
#ifdef GSL_LOG_IRQ_ONCE
  LOG_ONCE("\nIn ", __FUNCTION__);
#endif
#ifdef TIM2
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim2_htim.hdma[TIM_DMA_ID_UPDATE]);
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim2_htim.hdma[TIM_DMA_ID_CC3]);
#endif
#ifdef USART3
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_usart3_huart.hdmarx);
#endif
#ifdef UART7
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_uart7_huart.hdmatx);
#endif
#ifdef TIM5
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim5_htim.hdma[TIM_DMA_ID_CC4]);
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim5_htim.hdma[TIM_DMA_ID_TRIGGER]);
#endif
#ifdef TIM6
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim6_htim.hdma[TIM_DMA_ID_UPDATE]);
#endif
}
#undef GSL_DMA_LocalStream

#define GSL_DMA_LocalStream DMA1_Stream2
void DMA1_Stream2_IRQHandler(void) {
#ifdef GSL_LOG_IRQ_ONCE
  LOG_ONCE("\nIn ", __FUNCTION__);
#endif
#ifdef SPI3
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_spi3_hspi.hdmarx);
#endif
#ifdef TIM7
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim7_htim.hdma[TIM_DMA_ID_UPDATE]);
#endif
#ifdef I2C3
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_i2c3_hi2c.hdmarx);
#endif
#ifdef UART4
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_uart4_huart.hdmarx);
#endif
#ifdef TIM3
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim3_htim.hdma[TIM_DMA_ID_CC4]);
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim3_htim.hdma[TIM_DMA_ID_UPDATE]);
#endif
#ifdef TIM5
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim5_htim.hdma[TIM_DMA_ID_CC1]);
#endif
#ifdef I2C2
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_i2c2_hi2c.hdmarx);
#endif
}
#undef GSL_DMA_LocalStream

#define GSL_DMA_LocalStream DMA1_Stream3
void DMA1_Stream3_IRQHandler(void) {
#ifdef GSL_LOG_IRQ_ONCE
  LOG_ONCE("\nIn ", __FUNCTION__);
#endif
#ifdef SPI2
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_spi2_hspi.hdmarx);
#endif
#ifdef TIM4
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim4_htim.hdma[TIM_DMA_ID_CC2]);
#endif
#ifdef USART3
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_usart3_huart.hdmatx);
#endif
#ifdef UART7
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_uart7_huart.hdmarx);
#endif
#ifdef TIM5
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim5_htim.hdma[TIM_DMA_ID_CC4]);
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim5_htim.hdma[TIM_DMA_ID_TRIGGER]);
#endif
#ifdef I2C2
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_i2c2_hi2c.hdmarx);
#endif
}
#undef GSL_DMA_LocalStream

#define GSL_DMA_LocalStream DMA1_Stream4
void DMA1_Stream4_IRQHandler(void) {
#ifdef GSL_LOG_IRQ_ONCE
  LOG_ONCE("\nIn ", __FUNCTION__);
#endif
#ifdef SPI2
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_spi2_hspi.hdmatx);
#endif
#ifdef TIM7
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim7_htim.hdma[TIM_DMA_ID_UPDATE]);
#endif
#ifdef I2C3
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_i2c3_hi2c.hdmatx);
#endif
#ifdef UART4
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_uart4_huart.hdmatx);
#endif
#ifdef TIM3
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim3_htim.hdma[TIM_DMA_ID_CC1]);
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim3_htim.hdma[TIM_DMA_ID_TRIGGER]);
#endif
#ifdef TIM5
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim5_htim.hdma[TIM_DMA_ID_CC2]);
#endif
#ifdef USART3
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_usart3_huart.hdmatx);
#endif
}
#undef GSL_DMA_LocalStream

#define GSL_DMA_LocalStream DMA1_Stream5
void DMA1_Stream5_IRQHandler(void) {
#ifdef GSL_LOG_IRQ_ONCE
  LOG_ONCE("\nIn ", __FUNCTION__);
#endif
#ifdef SPI3
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_spi3_hspi.hdmatx);
#endif
#ifdef I2C1
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_i2c1_hi2c.hdmarx);
#endif
#ifdef TIM2
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim2_htim.hdma[TIM_DMA_ID_CC1]);
#endif
#ifdef USART2
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_usart2_huart.hdmarx);
#endif
#ifdef TIM3
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim3_htim.hdma[TIM_DMA_ID_CC2]);
#endif
#ifdef DAC
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_dac_hdac.DMA_Handle1);
#endif
}
#undef GSL_DMA_LocalStream

#define GSL_DMA_LocalStream DMA1_Stream6
void DMA1_Stream6_IRQHandler(void) {
#ifdef GSL_LOG_IRQ_ONCE
  LOG_ONCE("\nIn ", __FUNCTION__);
#endif
#ifdef I2C1
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_i2c1_hi2c.hdmatx);
#endif
#ifdef TIM4
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim4_htim.hdma[TIM_DMA_ID_UPDATE]);
#endif
#ifdef TIM2
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim2_htim.hdma[TIM_DMA_ID_CC2]);
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim2_htim.hdma[TIM_DMA_ID_CC4]);
#endif
#ifdef USART2
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_usart2_huart.hdmatx);
#endif
#ifdef UART8
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_uart8_huart.hdmarx);
#endif
#ifdef TIM5
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim5_htim.hdma[TIM_DMA_ID_UPDATE]);
#endif
#ifdef DAC
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_dac_hdac.DMA_Handle2);
#endif
}
#undef GSL_DMA_LocalStream

#define GSL_DMA_LocalStream DMA1_Stream7
void DMA1_Stream7_IRQHandler(void) {
#ifdef GSL_LOG_IRQ_ONCE
  LOG_ONCE("\nIn " TOSTRING(GSL_DMA_LocalStream) "_IRQHandler");
#endif
#ifdef SPI3
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_spi3_hspi.hdmatx);
#endif
#ifdef I2C1
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_i2c1_hi2c.hdmatx);
#endif
#ifdef TIM4
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim4_htim.hdma[TIM_DMA_ID_CC3]);
#endif
#ifdef TIM2
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim2_htim.hdma[TIM_DMA_ID_UPDATE]);
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim2_htim.hdma[TIM_DMA_ID_CC4]);
#endif
#ifdef UART5
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_uart5_huart.hdmatx);
#endif
#ifdef TIM3
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim3_htim.hdma[TIM_DMA_ID_CC3]);
#endif
#ifdef I2C2
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_i2c2_hi2c.hdmatx);
#endif
}
#undef GSL_DMA_LocalStream


#define GSL_DMA_LocalStream DMA2_Stream0
void DMA2_Stream0_IRQHandler(void) {
#ifdef GSL_LOG_IRQ_ONCE
  LOG_ONCE("\nIn ", __FUNCTION__);
#endif
#ifdef ADC1
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_adc1_hadc.DMA_Handle);
#endif
#ifdef ADC3
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_adc3_hadc.DMA_Handle);
#endif
#ifdef SPI1
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_spi1_hspi.hdmarx);
#endif
#ifdef SPI4
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_spi4_hspi.hdmarx);
#endif
#ifdef TIM1
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim1_htim.hdma[TIM_DMA_ID_TRIGGER]);
#endif
}
#undef GSL_DMA_LocalStream


#define GSL_DMA_LocalStream DMA2_Stream1
void DMA2_Stream1_IRQHandler(void) {
#ifdef GSL_LOG_IRQ_ONCE
  LOG_ONCE("\nIn ", __FUNCTION__);
#endif
#ifdef ADC3
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_adc3_hadc.DMA_Handle);
#endif
#ifdef SPI4
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_spi4_hspi.hdmatx);
#endif
#ifdef USART6
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_usart6_huart.hdmarx);
#endif
#ifdef TIM1
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim1_htim.hdma[TIM_DMA_ID_CC1]);
#endif
#ifdef TIM8
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim8_htim.hdma[TIM_DMA_ID_UPDATE]);
#endif
}
#undef GSL_DMA_LocalStream

#define GSL_DMA_LocalStream DMA2_Stream2
void DMA2_Stream2_IRQHandler(void) {
#ifdef GSL_LOG_IRQ_ONCE
  LOG_ONCE("\nIn ", __FUNCTION__);
#endif
#ifdef TIM8
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim8_htim.hdma[TIM_DMA_ID_CC1]);
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim8_htim.hdma[TIM_DMA_ID_CC2]);
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim8_htim.hdma[TIM_DMA_ID_CC3]);
#endif
#ifdef ADC2
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_adc2_hadc.DMA_Handle);
#endif
#ifdef SPI1
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_spi1_hspi.hdmarx);
#endif
#ifdef USART1
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_usart1_huart.hdmarx);
#endif
#ifdef USART6
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_usart6_huart.hdmarx);
#endif
#ifdef TIM1
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim1_htim.hdma[TIM_DMA_ID_CC2]);
#endif
  // Note: channel 7 TIM8_CH1 handled above
}
#undef GSL_DMA_LocalStream

// temporarily defined here
// DEBUG
extern "C" {
//extern SD_HandleTypeDef uSdHandle;
}

#define GSL_DMA_LocalStream DMA2_Stream3
void DMA2_Stream3_IRQHandler(void) {
#ifdef GSL_LOG_IRQ_ONCE
  LOG_ONCE("\nIn ", __FUNCTION__);
#endif
#ifdef ADC2
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_adc2_hadc.DMA_Handle);
#endif
#ifdef SPI5
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_spi5_hspi.hdmarx);
#endif
#ifdef SPI1
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_spi1_hspi.hdmatx);
#endif
#ifdef SPI4
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_spi4_hspi.hdmarx);
#endif
#ifdef TIM1
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim1_htim.hdma[TIM_DMA_ID_CC1]);
#endif
#ifdef TIM8
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim8_htim.hdma[TIM_DMA_ID_CC2]);
#endif
}
#undef GSL_DMA_LocalStream


#define GSL_DMA_LocalStream DMA2_Stream4
void DMA2_Stream4_IRQHandler(void) {
#ifdef GSL_LOG_IRQ_ONCE
  LOG_ONCE("\nIn ", __FUNCTION__);
#endif
#ifdef ADC1
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_adc1_hadc.DMA_Handle);
#endif
#ifdef SPI5
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_spi5_hspi.hdmatx);
#endif
#ifdef SPI4
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_spi4_hspi.hdmatx);
#endif
#ifdef TIM1
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim1_htim.hdma[TIM_DMA_ID_CC4]);
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim1_htim.hdma[TIM_DMA_ID_TRIGGER]);
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim1_htim.hdma[TIM_DMA_ID_COMMUTATION]);
#endif
#ifdef TIM8
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim8_htim.hdma[TIM_DMA_ID_CC3]);
#endif
}
#undef GSL_DMA_LocalStream

#define GSL_DMA_LocalStream DMA2_Stream5
void DMA2_Stream5_IRQHandler(void) {
#ifdef GSL_LOG_IRQ_ONCE
  LOG_ONCE("\nIn ", __FUNCTION__);
#endif
#ifdef SPI6
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_spi6_hspi.hdmatx);
#endif
#ifdef SPI1
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_spi1_hspi.hdmatx);
#endif
#ifdef USART1
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_usart1_huart.hdmarx);
#endif
#ifdef TIM1
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim1_htim.hdma[TIM_DMA_ID_UPDATE]);
#endif
#ifdef SPI5
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_spi5_hspi.hdmarx);
#endif
}
#undef GSL_DMA_LocalStream

// defined in FATFS library
#define GSL_DMA_LocalStream DMA2_Stream6
void DMA2_Stream6_IRQHandler(void) {
#ifdef GSL_LOG_IRQ_ONCE
  LOG_ONCE("\nIn ", __FUNCTION__);
#endif
#ifdef TIM1
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim1_htim.hdma[TIM_DMA_ID_CC1]);
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim1_htim.hdma[TIM_DMA_ID_CC2]);
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim1_htim.hdma[TIM_DMA_ID_CC3]);
#endif
#ifdef SPI6
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_spi6_hspi.hdmarx);
#endif
#ifdef USART6
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_usart6_huart.hdmatx);
#endif
#ifdef SPI5
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_spi5_hspi.hdmatx);
#endif
  // in the FatFS library, we're including here for the moment
  //HAL_DMA_IRQHandler(uSdHandle.hdmatx);
}
#undef GSL_DMA_LocalStream

#define GSL_DMA_LocalStream DMA2_Stream7
void DMA2_Stream7_IRQHandler(void) {
#ifdef GSL_LOG_IRQ_ONCE
  LOG_ONCE("\nIn ", __FUNCTION__);
#endif
#ifdef USART1
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_usart1_huart.hdmatx);
#endif
#ifdef USART6
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_usart6_huart.hdmatx);
#endif
#ifdef TIM8
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim8_htim.hdma[TIM_DMA_ID_CC4]);
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim8_htim.hdma[TIM_DMA_ID_TRIGGER]);
  GSL_DMA_HandleIfMatch(GSL_DMA_LocalStream, gsl_tim8_htim.hdma[TIM_DMA_ID_COMMUTATION]);
#endif
}
#undef GSL_DMA_LocalStream

}

// given a stream, return the interrupt for that stream
IRQn_Type GSL_DMA_GetIRQ(DMA_Stream_TypeDef * DMAx_Streamy) {
  if (DMAx_Streamy == DMA1_Stream0) {
    return DMA1_Stream0_IRQn;
  } else if (DMAx_Streamy == DMA1_Stream1) {
    return DMA1_Stream1_IRQn;
  } else if (DMAx_Streamy == DMA1_Stream2) {
    return DMA1_Stream2_IRQn;
  } else if (DMAx_Streamy == DMA1_Stream3) {
    return DMA1_Stream3_IRQn;
  } else if (DMAx_Streamy == DMA1_Stream4) {
    return DMA1_Stream4_IRQn;
  } else if (DMAx_Streamy == DMA1_Stream5) {
    return DMA1_Stream5_IRQn;
  } else if (DMAx_Streamy == DMA1_Stream6) {
    return DMA1_Stream6_IRQn;
  } else if (DMAx_Streamy == DMA1_Stream7) {
    return DMA1_Stream7_IRQn;
  } else if (DMAx_Streamy == DMA2_Stream0) {
    return DMA2_Stream0_IRQn;
  } else if (DMAx_Streamy == DMA2_Stream1) {
    return DMA2_Stream1_IRQn;
  } else if (DMAx_Streamy == DMA2_Stream2) {
    return DMA2_Stream2_IRQn;
  } else if (DMAx_Streamy == DMA2_Stream3) {
    return DMA2_Stream3_IRQn;
  } else if (DMAx_Streamy == DMA2_Stream4) {
    return DMA2_Stream4_IRQn;
  } else if (DMAx_Streamy == DMA2_Stream5) {
    return DMA2_Stream5_IRQn;
  } else if (DMAx_Streamy == DMA2_Stream6) {
    return DMA2_Stream6_IRQn;
  } else if (DMAx_Streamy == DMA2_Stream7) {
    return DMA2_Stream7_IRQn;
  } else {
    HALT("Unknown parameter");
    return NonMaskableInt_IRQn;
  }
}

// convert the given stream into an integer
// e.g. DMA1_Stream0 -> 0
// e.g. DMA1_Stream7 -> 7
// e.g. DMA2_Stream0 -> 8
// e.g. DMA2_Stream7 -> 15
uint16_t GSL_DMA_ToInteger(DMA_Stream_TypeDef * DMAx_Streamy) {
  // get offset from base
  for (uint16_t i = 0; i < gsl_dma_stream_count; ++i) {
    if (DMAx_Streamy == gsl_dma_stream_list[i])
      return i;
  }
  HALT("Invalid stream");
  return 0;
}

// set bit to 1 when reserved
uint16_t GSL_DMA_Reserved = 0;

// reserve a given dma stream
void GSL_DMA_ReserveStream(DMA_Stream_TypeDef * DMAx_Streamy) {
  uint16_t bitmask = ((uint16_t) 1) << GSL_DMA_ToInteger(DMAx_Streamy);
  if (GSL_DMA_Reserved & bitmask) {
    HALT("DMA stream already reserved");
  }
  GSL_DMA_Reserved |= bitmask;
}

// unreserve a given dma stream
void GSL_DMA_UnreserveStream(DMA_Stream_TypeDef * DMAx_Streamy) {
  uint16_t bitmask = ((uint16_t) 1) << GSL_DMA_ToInteger(DMAx_Streamy);
  GSL_DMA_Reserved &= ~bitmask;
}

// initialize the given DMA stream
void GSL_DMA_Initialize(DMA_HandleTypeDef * hdma, uint8_t priority = 8) {
  // if we're not reset, don't initialize
  if (hdma->State != HAL_DMA_STATE_RESET) {
    return;
  }
  GSL_DMA_ReserveStream(hdma->Instance);
  GSL_DMA_EnableClock(GSL_DMA_GetBase(hdma->Instance));
  HAL_RUN(HAL_DMA_Init(hdma));
  IRQn_Type irq = GSL_DMA_GetIRQ(hdma->Instance);
  HAL_NVIC_SetPriority(irq, priority, 0);
  HAL_NVIC_EnableIRQ(irq);
  // DEBUG
  //LOG("\nInitialized the ", GSL_DMA_ToInteger(hdma->Instance), " DMA stream.");
}

// deinitialize the given DMA stream
void GSL_DMA_Deinitialize(DMA_HandleTypeDef * hdma) {
  // if not initialized, don't do anything
  if (hdma->State == HAL_DMA_STATE_RESET) {
    return;
  }
  GSL_DMA_UnreserveStream(hdma->Instance);
  HAL_RUN(HAL_DMA_DeInit(hdma));
  IRQn_Type irq = GSL_DMA_GetIRQ(hdma->Instance);
  HAL_NVIC_DisableIRQ(irq);
}
