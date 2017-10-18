#ifndef ATOMIC_H
#define ATOMIC_H

//! Atomic set, clear, change, or get bit in a register
namespace atomic
{
    /////// Atomic bit set/clear
    /* Clear bit in address (make it zero), set bit (make it one), or return the value of that bit
    *   We can change this functions depending on the board.
    *   Teensy 3.x use bitband while Teensy LC has a more advanced bit manipulation engine.
    */
    #if defined(KINETISK) // Teensy 3.x
    // bitband
    //#define ADC_BITBAND_ADDR(reg, bit) (((uint32_t)(reg) - 0x40000000) * 32 + (bit) * 4 + 0x42000000)
    __attribute__((always_inline)) inline volatile uint32_t* bitband_address(volatile uint32_t* reg, uint8_t bit) {
        return (volatile uint32_t*)(((uint32_t)reg - 0x40000000)*32 + bit*4 + 0x42000000);
    }

    __attribute__((always_inline)) inline void setBit(volatile uint32_t* reg, uint8_t bit) {
        *bitband_address(reg, bit) = 1;
    }
    __attribute__((always_inline)) inline void clearBit(volatile uint32_t* reg, uint8_t bit) {
        *bitband_address(reg, bit) = 0;
    }
    __attribute__((always_inline)) inline void changeBit(volatile uint32_t* reg, uint8_t bit, bool state) {
        *bitband_address(reg, bit) = state;
    }
    __attribute__((always_inline)) inline volatile bool getBit(volatile uint32_t* reg, uint8_t bit) {
        return (volatile bool)*bitband_address(reg, bit);
    }

    #elif defined(KINETISL) // Teensy LC
    // bit manipulation engine
    //#define ADC_SETBIT_ATOMIC(reg, bit) (*(uint32_t *)(((uint32_t)&(reg) - 0xF8000000) | 0x48000000) = 1 << (bit)) // OR
    //#define ADC_CLRBIT_ATOMIC(reg, bit) (*(uint32_t *)(((uint32_t)&(reg) - 0xF8000000) | 0x44000000) = ~(1 << (bit))) // XOR

    __attribute__((always_inline)) inline void setBit(volatile uint32_t* reg, uint8_t bit) {
        //temp = *(uint32_t *)((uint32_t)(reg) | (1<<26) | (bit<<21)); // LAS
        *(volatile uint32_t*)((uint32_t)(reg) | (1<<27)) = 1<<bit; // OR
    }
    __attribute__((always_inline)) inline void clearBit(volatile uint32_t* reg, uint8_t bit) {
        //temp = *(uint32_t *)((uint32_t)(reg) | (3<<27) | (bit<<21)); // LAC
        *(volatile uint32_t*)((uint32_t)(reg) | (1<<26)) = ~(1<<bit); // AND
    }
    __attribute__((always_inline)) inline void changeBit(volatile uint32_t* reg, uint8_t bit, bool state) {
        //temp = *(uint32_t *)((uint32_t)(reg) | ((3-2*!!state)<<27) | (bit<<21)); // LAS/LAC
        state ? setBit(reg, bit) : clearBit(reg, bit);
    }
    __attribute__((always_inline)) inline volatile bool getBit(volatile uint32_t* reg, uint8_t bit) {
        return (volatile bool)*(uint32_t *)((uint32_t)(reg) | (1<<28) | (bit<<23) ); // UBFX
    }

    #endif

}

#endif // ATOMIC_H
