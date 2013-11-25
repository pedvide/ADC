 /* ADC Library created by Pedro Villanueva
 * It implements all functions of the Teensy 3.0 internal ADC
 * Some of the code was extracted from the file analog.c included in the distribution of Teensduino by PJRC.COM
 *
 */


#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include "core_pins.h"
#include "mk20dx128.h"
// include pow()
#include <math.h>

// include new and delete
#include <Arduino.h>

// include the circular buffer
#include <RingBuffer.h>
// and IntervalTimer
#include <IntervalTimer.h>

// number of interval timers we can have
#define MAX_ANALOG_TIMERS 3

// the alternate clock is connected to OSCERCLK (16 MHz).
// datasheet says ADC clock should be 2 to 12 MHz for 16 bit mode
// datasheet says ADC clock should be 1 to 18 MHz for 8-12 bit mode
// calibration works best when averages are 32 and speed is less than 4 MHz
#if F_BUS == 48000000
  #define ADC0_CFG1_3MHZ   ADC_CFG1_ADIV(3) + ADC_CFG1_ADICLK(1) // Clock divide select: 3=div8 + Input clock: 1=bus/2
  #define ADC0_CFG1_6MHZ   ADC_CFG1_ADIV(2) + ADC_CFG1_ADICLK(1) // Clock divide select: 2=div4 + Input clock: 1=bus/2
  #define ADC0_CFG1_12MHZ  ADC_CFG1_ADIV(1) + ADC_CFG1_ADICLK(1) // Clock divide select: 1=div2 Input clock: 1=bus/2
  #define ADC0_CFG1_24MHZ  ADC_CFG1_ADIV(0) + ADC_CFG1_ADICLK(1) // Clock divide select: 0=direct + Input clock: 1=bus/2
#elif F_BUS == 24000000
  #define ADC0_CFG1_3MHZ   ADC_CFG1_ADIV(3) + ADC_CFG1_ADICLK(1) // Clock divide select: 3=div8 + Input clock: 0=bus
  #define ADC0_CFG1_6MHZ   ADC_CFG1_ADIV(2) + ADC_CFG1_ADICLK(0) // Clock divide select: 2=div4 + Input clock: 0=bus
  #define ADC0_CFG1_12MHZ  ADC_CFG1_ADIV(1) + ADC_CFG1_ADICLK(0) // Clock divide select: 1=div2 + Input clock: 0=bus
  #define ADC0_CFG1_24MHZ  ADC_CFG1_ADIV(0) + ADC_CFG1_ADICLK(0) // Clock divide select: 0=direct + Input clock: 0=bus
#else
#error
#endif


// ADCx_SC2[REFSEL] bit selects the voltage reference sources for ADC.
//   VREFH/VREFL - connected as the primary reference option
//   1.2 V VREF_OUT - connected as the VALT reference option
#define DEFAULT         0
#define INTERNAL        2
#define INTERNAL1V2     2
#define INTERNAL1V1     2
#define EXTERNAL        0


// Error codes for analogRead and analogReadDifferential
#define ADC_ERROR_DIFF_VALUE -70000
#define ADC_ERROR_VALUE ADC_ERROR_DIFF_VALUE

// Error codes for startAnalogTimer
#define ANALOG_TIMER_ERROR -1

/** Class ADC: Implements all functions of the Teensy 3.0 analog to digital converter
*
*/
class ADC
{
    public:

        /** Constructor
        *
        */
        ADC();

        /** Destructor
        *
        */
        ~ADC();


        //! Set the voltage reference you prefer, default is vcc
        /*!
        * \param type can be DEFAULT, EXTERNAL or INTERNAL.
        * It needs to recalibrate.
        */
        void setReference(uint8_t type);


        //! Change the resolution of the measurement.
        /*!
        *  \param bits is the number of bits of resolution.
        *  For single-ended measurements: 8, 10, 12 or 16 bits.
        *  For differential measurements: 9, 11, 13 or 16 bits.
        *  If you want something in between (11 bits single-ended for example) select the inmediate higher
        *  and shift the result one to the right.
        *  If you select, for example, 9 bits and then do a single-ended reading, the resolution will be adjusted to 8 bits
        *  In this case the comparison values will still be correct for analogRead and analogReadDifferential, but not
        *  for startSingle* or startContinous*, so whenever you change the resolution, change also the comparison values.
        */
        static void setResolution(unsigned int bits);

        //! Returns the resolution of the ADC.
        static uint8_t getResolution();

        //! Returns the maximum value for a measurement.
        uint32_t getMaxValue();


        //! Set the number of averages
        /*!
        * \param num can be 0, 4, 8, 16 or 32.
        */
        void setAveraging(uint8_t num);


        //! Enable interrupts
        /** An IRQ_ADC0 Interrupt will be raised when the conversion is completed
        *  (including hardware averages and if the comparison (if any) is true).
        */
        static void enableInterrupts();

        //! Disable interrupts
        static void disableInterrupts();


        //! Enable DMA request
        /** An ADC DMA request will be raised when the conversion is completed
        *  (including hardware averages and if the comparison (if any) is true).
        */
        void enableDMA();

        //! Disable ADC DMA request
        void disableDMA();


        //! Enable the compare function to a single value
        /** A conversion will be completed only when the ADC value
        *  is >= compValue (greaterThan=1) or < compValue (greaterThan=0)
        *  Call it after changing the resolution
        *  Use with interrupts or poll conversion completion with isComplete()
        */
        void enableCompare(int16_t compValue, int greaterThan);

        //! Enable the compare function to a range
        /** A conversion will be completed only when the ADC value is inside (insideRange=1) or outside (=0)
        *  the range given by (lowerLimit, upperLimit),including (inclusive=1) the limits or not (inclusive=0).
        *  See Table 31-78, p. 617 of the freescale manual.
        *  Call it after changing the resolution
        *  Use with interrupts or poll conversion completion with isComplete()
        */
        void enableCompareRange(int16_t lowerLimit, int16_t upperLimit, int insideRange, int inclusive);

        //! Disable the compare function
        void disableCompare();


        //! Returns the analog value of the pin.
        /** It waits until the value is read and then returns the result.
        * If a comparison has been set up and fails, it will return ADC_ERROR_VALUE.
        * This function is interrupt safe, so it will restore the adc to the state it was before being called
        */
        static int analogRead(uint8_t pin);

        //! Reads the differential analog value of two pins (pinP - pinN).
        /** It waits until the value is read and then returns the result.
        * If a comparison has been set up and fails, it will return ADC_ERROR_DIFF_VALUE.
        * \param pinP must be A10 or A12.
        * \param pinN must be A11 (if pinP=A10) or A13 (if pinP=A12).
        * Other pins will return ADC_ERROR_DIFF_VALUE.
        * This function is interrupt safe, so it will restore the adc to the state it was before being called
        */
        static int analogReadDifferential(uint8_t pinP, uint8_t pinN);

        //! Starts an analog measurement on the pin and enables interrupts.
        /** It returns inmediately, get value with readSingle().
        *   If the pin is incorrect it returns ADC_ERROR_VALUE
        *   This function is interrupt safe. The ADC interrupt will restore the adc to its previous settings and
        *   restart the adc if it stopped a measurement. If you modify the adc_isr then this won't happen.
        */
        static int startSingleRead(uint8_t pin);

        //! Start a differential conversion between two pins (pinP - pinN) and enables interrupts.
        /** It returns inmediately, get value with readSingle().
        *   \param pinP must be A10 or A12.
        *   \param pinN must be A11 (if pinP=A10) or A13 (if pinP=A12).
        *   Other pins will return ADC_ERROR_DIFF_VALUE.
        *   This function is interrupt safe. The ADC interrupt will restore the adc to its previous settings and
        *   restart the adc if it stopped a measurement. If you modify the adc_isr then this won't happen.
        */
        static int startSingleDifferential(uint8_t pinP, uint8_t pinN);

        //! Reads the analog value of a single conversion.
        /** Set the conversion with with startSingleRead(pin) or startSingleDifferential(pinP, pinN).
        *   \return the converted value.
        */
        static int readSingle();


        //! Starts continuous conversion on the pin.
        /** It returns as soon as the ADC is set, use analogReadContinuous() to read the value.
        */
        void startContinuous(uint8_t pin);

        //! Starts continuous conversion between the pins (pinP-pinN).
        /** It returns as soon as the ADC is set, use analogReadContinuous() to read the value.
        * \param pinP must be A10 or A12.
        * \param pinN must be A11 (if pinP=A10) or A13 (if pinP=A12).
        * Other pins will return ADC_ERROR_DIFF_VALUE.
        */
        void startContinuousDifferential(uint8_t pinP, uint8_t pinN);

        //! Reads the analog value of a continuous conversion.
        /** Set the continuous conversion with with analogStartContinuous(pin) or startContinuousDifferential(pinP, pinN).
        *   \return the last converted value.
        */
        static int analogReadContinuous();

        //! Stops continuous conversion
        void stopContinuous();


        //! Starts a periodic measurement using the IntervalTimer library.
        /** The values will be added to a ring buffer of a fixed size.
        *   Read the oldest value with getTimerValue(pin), check if it's the last value with isLastValue(pin).
        *   When the buffer is full, new data will overwrite the oldest values.
        *   \returns ANALOG_TIMER_ERROR if the timer could not be started. Stop other analog timer and retry.
        *   This function is interrupt safe. The ADC interrupt will restore the adc to its previous settings and
        *   restart the adc if it stopped a measurement. If you modify the adc_isr then this won't happen.
        */
        int startAnalogTimer(uint8_t pin, uint32_t period);

        //! Starts a periodic measurement using the IntervalTimer library.
        /** The values will be added to a ring buffer of a fixed size.
        *   Read the oldest value with getTimerValue(pinP), check if it's the last value with isLastValue(pinP).
        *   When the buffer is full, new data will overwrite the oldest values.
        *   \param pinP must be A10 or A12.
        *   \param pinN must be A11 (if pinP=A10) or A13 (if pinP=A12).
        *   \returns ANALOG_TIMER_ERROR if the timer could not be started. Stop other analog timer and retry.
        *   This function is interrupt safe. The ADC interrupt will restore the adc to its previous settings and
        *   restart the adc if it stopped a measurement. If you modify the adc_isr then this won't happen.
        */
        int startAnalogTimerDifferential(uint8_t pinP, uint8_t pinN, uint32_t period);

        //! Stops the periodic measurement
        /** As soon as it is stopped, you won't be able to access the buffer anymore
        *   It also disables the adc interrupt
        */
        void stopAnalogTimer(uint8_t pin);

        //! Returns the oldest value of the ring buffer where the periodic measurements go
        int getTimerValue(uint8_t pin);

        //! Is the oldest value also the last one?
        bool isTimerLastValue(uint8_t pin);


        //! Is the ADC converting at the moment?
        static bool isConverting();

        //! Is an ADC conversion ready?
        /**
        *  \return 1 if yes, 0 if not.
        *  When a value is read this function returns 0 until a new value exists
        *  So it only makes sense to call it before analogReadContinuous() or readSingle()
        */
        static bool isComplete();

        //! Is the ADC in differential mode?
        static bool isDifferential();

        //! Is the ADC in continuous mode?
        static bool isContinuous();


        //! Points to the function that takes care of the analogTimers
        /** When there are no analogTimers this points to an empty function
        *   if you want to modify the adc_isr but still take care of the analogTimers, call this function
        *   in the code of you adc_isr
        */
        static void (*analogTimer_ADC_Callback)(void);


    protected:
    private:

        static const uint8_t ledPin;

        static uint8_t calibrating;

        static uint8_t analog_config_bits;

        static uint32_t analog_max_val;

        static uint8_t analog_num_average;

        static uint8_t analog_reference_internal;

        static uint8_t var_enableInterrupts;

        /** Sets up all initial configurations and starts calibration
        *
        */
        static void analog_init(uint32_t config=0);

        /** Waits until calibration is finished and writes the corresponding registers
        *
        */
        static void wait_for_cal(void);

        /** ADC interrupt callback for the timers
        *
        */
        static void ADC_callback();

        /** Empty function
        *
        */
        static void voidFunction();

        // translate pin number to SC1A nomenclature and viceversa
        static const uint8_t channel2sc1a[16];
        static const uint8_t sc1a2channel[27];


        // struct to store the config of the adc
        typedef struct ADC_CONFIG{
            uint32_t savedSC1A, savedSC2, savedSC3, savedCFG1, savedCFG2, savedRes;
            uint8_t diffRes; // is the new resolution different from the old one?
        } ADC_Config;
        static ADC_Config adc_config;
        static uint8_t adcWasInUse; // was the adc in use before an analog timer call?


        // struct with the analog timer pin, data and timer
        typedef struct ANALOG_TIMER{
            int8_t pinNumber; // the timer's pin
            IntervalTimer *timer; // timer
            RingBuffer *buffer; // circular buffer to store analog values

            uint32_t period; // timer's period

            bool isDiff; // is this a differential measurement

            ANALOG_TIMER() { pinNumber=-1; isDiff = false; } // ctor

            ADC_Config adc_config; // the timer will interrupt the adc, store its settings to restore them later

            /* this doesn't work b/c it'd need to be static and pinNumber as well,
               but then all instances of the struct would share the same pinNumber
            void analogTimerCallback() {
                //digitalWriteFast(ledPin, HIGH);

                if(pinNumber == A10) {
                    startSingleDifferential(A10, A11);
                } else if(pinNumber == A12) {
                    startSingleDifferential(A12, A13);
                } else {
                    startSingleRead(pinNumber);
                }

                //digitalWriteFast(ledPin, LOW);
            }
            */

        } AnalogTimer;
        static AnalogTimer *analogTimer[MAX_ANALOG_TIMERS];

        // function callback for the analog timers
        typedef void (*ISR)(void);
        static ISR analogTimerCallback[MAX_ANALOG_TIMERS];
        static void analogTimerCallback0();
        static void analogTimerCallback1();
        static void analogTimerCallback2();


};


#endif // ADC_H


/* PDB SKETCH FROM SERVO.CPP

if (!(SIM_SCGC6 & SIM_SCGC6_PDB)) {
    SIM_SCGC6 |= SIM_SCGC6_PDB; // TODO: use bitband for atomic bitset // enable pdb clock
    PDB0_MOD = 0xFFFF; // max period of counter, max 65535.
    PDB0_CNT = 0; // current value of the counter, is supposed to be read-only....
    PDB0_IDLY = 0; // the pdb interrupt happens when IDLY is equal to CNT
    PDB0_SC = PDB_CONFIG; // software trigger, pdb and interrupts enabled, continuous mode, clock = bus/4
    // TODO: maybe this should be a higher priority than most
    // other interrupts (init all to some default?)
    PDB0_SC = PDB_CONFIG | PDB_SC_SWTRIG; // start the counter!
    // i think the pdb interrupt goes off at 183.11 Hz
}
NVIC_ENABLE_IRQ(IRQ_PDB);


*/
