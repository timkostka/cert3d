# Migration to STM32H7

I'd like to port this to the STM32H745/747 because it's faster (480MHz vs 168 MHz) and about as expensive.  The dual core nature also has interesting possibilities.

## ST-Link VCP connections

First thing to get up and running is a UART debug log functionality.

It would be nice to implement this so that `printf` functions get redirected to the debug log.  A lot of external libraries use `printf` for debug.

I have a STM32H747-DISCO board which I will use to prototype.  There is a nucleo board planned but it is not yet available on Mouser or Digikey.

On the board the PA9 (USART1_TX) and PA10 (USART1_RX) pins are connected to the ST-Link VCP.

I'll stick with 115200 8-E-1 protocol as this is supported by the UART bootloader and it's what I've been using on the STM32F4 line.

### Power supply

This board gets 3.3V power from the ST-Link USB connection via the JP3 jumper.  Voltage was measured and verified.

### Peripheral clocks

Per "9.5.10 Peripheral allocation" of the RM0399 reference manual, each core can allocate a peripheral for itself or for the other core.  I believe a core must activate the clock for itself before accessing it.

Accessing the `__HAL_RCC_USART1_CLK_ENABLE_()` macro which sets the `RCC_APB2ENR_USART1EN` bit will allocate the peripheral for the core that is currently executing that instruction.  The memory is remapped to make this possible.

### Implementing printf

It seems like to implement printf, I can implement the lower-level call `_write`.
* https://electronics.stackexchange.com/questions/206113/how-do-i-use-the-printf-function-on-stm32

### Input capture

It may be possible to use the HRTIM peripheral (480 MHz) to capture up to 10 input channels.

It looks like the CPT1DE/CPT2DE bits control the DMA request enable for each channel.  These use the hrtim_dmaX (X=2 to 6) channel.