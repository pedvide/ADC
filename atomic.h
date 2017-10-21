#ifndef ATOMIC_H
#define ATOMIC_H

// kinetis.h has the following types for addresses: uint32_t, uint16_t, uint8_t, int32_t, int16_t

//! Atomic set, clear, change, or get bit in a register
namespace atomic
{
    /////// Atomic bit set/clear
    /* Clear bit in address (make it zero), set bit (make it one), or return the value of that bit
    *   We can change this functions depending on the board.
    *   Teensy 3.x use bitband while Teensy LC has a more advanced bit manipulation engine.
    */
    #if defined(KINETISK) // Teensy 3.x
    //! Bitband address
    /** Gets the aliased address of the bit-band register
    *   \param reg  Register in the bit-band area
    *   \param bit  Bit number of reg to read/modify
    *   \return A pointer to the aliased address of the bit of reg
    */
    template<typename T>
    __attribute__((always_inline)) inline volatile T& bitband_address(volatile T& reg, uint8_t bit) {
        return (*(volatile T*)(((uint32_t)&reg - 0x40000000)*32 + bit*4 + 0x42000000));
    }

    template<typename T>
    __attribute__((always_inline)) inline void setBit(volatile T& reg, uint8_t bit) {
        bitband_address(reg, bit) = 1;
    }
    template<typename T>
    __attribute__((always_inline)) inline void setBitFlag(volatile T& reg, uint32_t flag) {
        // 31-__builtin_clzl(flag) = gets bit number in flag
        // __builtin_clzl works for long ints, which are guaranteed by standard to be at least 32 bit wide.
        // there's no difference in the asm emitted.
        bitband_address(reg, 31-__builtin_clzl(flag)) = 1;
    }

    template<typename T>
    __attribute__((always_inline)) inline void clearBit(volatile T& reg, uint8_t bit) {
        bitband_address(reg, bit) = 0;
    }
    template<typename T>
    __attribute__((always_inline)) inline void clearBitFlag(volatile T& reg, uint32_t flag) {
        bitband_address(reg, 31-__builtin_clzl(flag)) = 0;
    }

    template<typename T>
    __attribute__((always_inline)) inline void changeBit(volatile T& reg, uint8_t bit, bool state) {
        bitband_address(reg, bit) = state;
    }
    template<typename T>
    __attribute__((always_inline)) inline void changeBitFlag(volatile T& reg, uint32_t flag, bool state) {
        bitband_address(reg, 31-__builtin_clzl(flag)) = state;
    }

    template<typename T>
    __attribute__((always_inline)) inline volatile bool getBit(volatile T& reg, uint8_t bit) {
        return (volatile bool)bitband_address(reg, bit);
    }
    template<typename T>
    __attribute__((always_inline)) inline volatile bool getBitFlag(volatile T& reg, uint32_t flag) {
        return (volatile bool)bitband_address(reg, 31-__builtin_clzl(flag));
    }

    #elif defined(KINETISL) // Teensy LC
    // bit manipulation engine
    //#define ADC_SETBIT_ATOMIC(reg, bit) (*(uint32_t *)(((uint32_t)&(reg) - 0xF8000000) | 0x48000000) = 1 << (bit)) // OR
    //#define ADC_CLRBIT_ATOMIC(reg, bit) (*(uint32_t *)(((uint32_t)&(reg) - 0xF8000000) | 0x44000000) = ~(1 << (bit))) // XOR

    template<typename T>
    __attribute__((always_inline)) inline void setBit(volatile T& reg, uint8_t bit) {
        //temp = *(uint32_t *)((uint32_t)(reg) | (1<<26) | (bit<<21)); // LAS
        *(volatile T*)((uint32_t)(&reg) | (1<<27)) = 1<<bit; // OR
    }
    template<typename T>
    __attribute__((always_inline)) inline void setBitFlag(volatile T& reg, uint32_t flag) {
        //temp = *(uint32_t *)((uint32_t)(reg) | (1<<26) | (bit<<21)); // LAS
        *(volatile T*)((uint32_t)(&reg) | (1<<27)) = flag; // OR
    }

    template<typename T>
    __attribute__((always_inline)) inline void clearBit(volatile T& reg, uint8_t bit) {
        //temp = *(uint32_t *)((uint32_t)(reg) | (3<<27) | (bit<<21)); // LAC
        *(volatile T*)((uint32_t)(&reg) | (1<<26)) = ~(1<<bit); // AND
    }
    template<typename T>
    __attribute__((always_inline)) inline void clearBitFlag(volatile T& reg, uint32_t flag) {
        //temp = *(uint32_t *)((uint32_t)(reg) | (3<<27) | (bit<<21)); // LAC
        *(volatile T*)((uint32_t)(&reg) | (1<<26)) = ~flag; // AND
    }

    template<typename T>
    __attribute__((always_inline)) inline void changeBit(volatile T& reg, uint8_t bit, bool state) {
        //temp = *(uint32_t *)((uint32_t)(reg) | ((3-2*!!state)<<27) | (bit<<21)); // LAS/LAC
        state ? setBit(reg, bit) : clearBit(reg, bit);
    }
    template<typename T>
    __attribute__((always_inline)) inline void changeBitFlag(volatile T& reg, uint32_t flag, bool state) {
        //temp = *(uint32_t *)((uint32_t)(reg) | ((3-2*!!state)<<27) | (bit<<21)); // LAS/LAC
        state ? setBitFlag(reg, flag) : clearBitFlag(reg, flag);
    }

    template<typename T>
    __attribute__((always_inline)) inline volatile bool getBit(volatile T& reg, uint8_t bit) {
        return (volatile bool)*(volatile T *)((uint32_t)(&reg) | (1<<28) | (bit<<23) ); // UBFX
    }
    template<typename T>
    __attribute__((always_inline)) inline volatile bool getBitFlag(volatile T& reg, uint32_t flag) {
        return (volatile bool)*(volatile T *)((uint32_t)(&reg) | (1<<28) | ((31-__builtin_clzl(flag))<<23) ); // UBFX
    }

    #endif

}

#endif // ATOMIC_H
