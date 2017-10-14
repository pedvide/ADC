#ifndef VREF_H
#define VREF_H

#include <Arduino.h>

/** Class VREF: Controls the Teensy internal voltage reference module (VREFV1)
*
*/
class VREF
{
    public:
        VREF() {}

        //! Start the 1.2V internal reference (if present)
        /** This is called automatically by setReference(ADC_REFERENCE::REF_1V2)
        *   Use it to switch on the internal reference on the VREF_OUT pin.
        *   \param mode can be (these are defined in kinetis.h)
        *    VREF_SC_MODE_LV_BANDGAPONLY (0) for stand-by
        *    VREF_SC_MODE_LV_HIGHPOWERBUF (1) for high power buffer and
        *    VREF_SC_MODE_LV_LOWPOWERBUF (2) for low power buffer.
        *   \param trim adjusts the reference value, from 0 to 0x3F (63)
        *
        */
        void start(uint8_t mode = VREF_SC_MODE_LV_HIGHPOWERBUF, uint8_t trim = 0x20) {
            VREF_TRM = VREF_TRM_CHOPEN | trim; // enable module and set the trimmer to medium (max=0x3F=63)
            // enable 1.2 volt ref with all compensations in high power mode
            VREF_SC = VREF_SC_VREFEN | VREF_SC_REGEN | VREF_SC_ICOMPEN | VREF_SC_MODE_LV(mode);
        }

        //! Stops the internal reference
        /** This is called automatically by setReference(ref) when ref is any other than REF_1V2
        */
        void stop() __attribute__((always_inline)) {
            VREF_SC = 0;
        }

         //! Check if the internal reference has stabilized.
        /** This should be polled after enabling the reference after reset, or after changing
        *   its buffer mode from VREF_SC_MODE_LV_BANDGAPONLY to any of the buffered modes.
        *   Typical start-up time is 35 ms (as per datasheet).
        */
        volatile bool isStable() __attribute__((always_inline)) {
            return VREF_SC & VREF_SC_VREFST;
        }

        //! Wait for the internal reference to stabilize.
        /** This function can be called to wait for the internal reference to stabilize.
        *   It will block until the reference has stabilized, or return immediately if the
        *   reference is not enabled in the first place.
        */
        void waitUntilStable() {
            while((VREF_SC & VREF_SC_VREFEN) && (!isStable())) {
                yield();
            }
        }

    protected:

    private:
};

#endif // VREF_H
