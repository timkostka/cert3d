# Unified firmware

This file contains info about using the same firmware for both cores on an STM32H745 chip.

## Core detection

A core can be detected by looking at the `SCB->CPUID` register.  Bits [15:4] hold the part number, which will bewhich will be 0xC27 for a CM7 core and 0xC24 for a CM4 core. 

We can test for this immediately within the ResetHandler using assembly language
```
Reset_Handler:
  /* set M4 stack pointer */
  ldr   sp, =_estack_cm4

  /* on M4 core, skip past initialization */
  ldr r2, =_scb_cpuid
  ldr r0, [r2]
  lsr r0, #4
  and r0, #0xF
  cmp r0, #4
  beq CoreM4

CoreM7:
  /* set M7 stack pointer */
  ldr   sp, =_estack_cm7
```

## Implementing printf

It would be nice to have printf implemented so that it can be used as a debug log.  However, there are some issues:
* Increase in code size by ~20kB
  * This is generally not a problem as we have the space
* printf calls malloc
  * This goes against our desire to have no dynamically allocated memory
* Some implementations are not thread-safe
  * For a dual-core device, this can be a problem if they are part of the same compilation unit, which is the goal of the unified firmware.

One option would be to implement printf.  This project does it:
* https://github.com/mpaland/printf

I took this last option which seems to be working well.  To use it, one needs to include `printf.h` prior to using any `printf` calls.
