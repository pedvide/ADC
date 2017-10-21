#ifndef VREF_H
#define VREF_H

#include <Arduino.h>

#include <atomic.h>

//! Controls the Teensy internal voltage reference module (VREFV1)
namespace VREF
{

    //! Start the 1.2V internal reference (if present)
    /** This is called automatically by ADC_Module::setReference(ADC_REFERENCE::REF_1V2)
    *   Use it to switch on the internal reference on the VREF_OUT pin.
    *   You can measure it with adc->analogRead(ADC_INTERNAL_SOURCE::VREF_OUT).
    *   \param mode can be (these are defined in kinetis.h)
    *    VREF_SC_MODE_LV_BANDGAPONLY (0) for stand-by
    *    VREF_SC_MODE_LV_HIGHPOWERBUF (1) for high power buffer and
    *    VREF_SC_MODE_LV_LOWPOWERBUF (2) for low power buffer.
    *   \param trim adjusts the reference value, from 0 to 0x3F (63). Default is 32.
    *
    */
    inline void start(uint8_t mode = VREF_SC_MODE_LV_HIGHPOWERBUF, uint8_t trim = 0x20) {
        VREF_TRM = VREF_TRM_CHOPEN | (trim&0x3F); // enable module and set the trimmer to medium (max=0x3F=63)
        // enable 1.2 volt ref with all compensations in high power mode
        VREF_SC = VREF_SC_VREFEN | VREF_SC_REGEN | VREF_SC_ICOMPEN | VREF_SC_MODE_LV(mode);

        // "PMC_REGSC[BGEN] bit must be set if the VREF regulator is
        // required to remain operating in VLPx modes."
        // Also "If the chop oscillator is to be used in very low power modes,
        // the system (bandgap) voltage reference must also be enabled."
        // enable bandgap, can be read directly with ADC_INTERNAL_SOURCE::BANDGAP
        atomic::setBitFlag(PMC_REGSC, PMC_REGSC_BGBE);
    }

    //! Set the trim
    /** Set the trim, the change in the reference is about 0.5 mV per step.
    *   \param trim adjusts the reference value, from 0 to 0x3F (63).
    */
    inline void trim(uint8_t trim) {
        bool chopen = atomic::getBitFlag(VREF_TRM, VREF_TRM_CHOPEN);
        VREF_TRM = (chopen ? VREF_TRM_CHOPEN : 0) | (trim&0x3F);
    }

    //! Stops the internal reference
    /** This is called automatically by ADC_Module::setReference(ref) when ref is any other than REF_1V2
    */
    __attribute__((always_inline)) inline void stop(){
        VREF_SC = 0;
        atomic::clearBitFlag(PMC_REGSC, PMC_REGSC_BGBE);
    }

    //! Check if the internal reference has stabilized.
    /** NOTE: This is valid only when the chop oscillator is not being used.
    *   By default the chop oscillator IS used, so wait the maximum start-up time of 35 ms (as per datasheet).
    *   waitUntilStable waits 35 us.
    *   This should be polled after enabling the reference after reset, after changing
    *   its buffer mode from VREF_SC_MODE_LV_BANDGAPONLY to any of the buffered modes, or
    *   after changing the trim.
    *
    *   \return true if the VREF module is already in a stable condition and can be used.
    */
    __attribute__((always_inline)) inline volatile bool isStable() {
        return atomic::getBitFlag(VREF_SC, VREF_SC_VREFST);
    }

    //! Check if the internal reference is on.
    /**
    *   \return true if the VREF module is switched on.
    */
    __attribute__((always_inline)) inline volatile bool isOn() {
        return atomic::getBitFlag(VREF_SC, VREF_SC_VREFEN);
    }

    //! Wait for the internal reference to stabilize.
    /** This function can be called to wait for the internal reference to stabilize.
    *   It will block until the reference has stabilized, or return immediately if the
    *   reference is not enabled in the first place.
    */
    inline void waitUntilStable() {
        delay(35); // see note in isStable()
        while(isOn() && !isStable()) {
            yield();
        }
    }

}

#endif // VREF_H
