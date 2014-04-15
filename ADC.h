#ifndef ADC_H
#define ADC_H


// number of interval timers we can have
#define MAX_ANALOG_TIMERS 3
// Error codes for startAnalogTimer
#define ANALOG_TIMER_ERROR -1

#define ADC_0 0
#define ADC_1 1


// include ADC module class
#include "ADC_Module.h"
// include the circular buffer
#include "RingBuffer.h"
// and IntervalTimer
#include <IntervalTimer.h>


#ifdef __cplusplus
extern "C" {
#endif


class ADC
{
    public:
        /** Default constructor */
        ADC();
        /** Default destructor */
        virtual ~ADC();

        // create both adc objects
        static ADC_Module *adc0; // adc object
        #if defined(__MK20DX256__)
        static ADC_Module *adc1; // adc object
        #endif

        //! Points to the function that takes care of the analogTimers
        /** When there are no analogTimers this points to an empty function
        *   if you want to modify the adc_isr but still take care of the analogTimers, call this function
        *   in the code of you adc_isr
        */
        static void (*analogTimer_ADC0_Callback)(void);
        #if defined(__MK20DX256__)
        static void (*analogTimer_ADC1_Callback)(void);
        #endif


        //! Set the voltage reference you prefer, default is vcc
        /*!
        * \param type can be DEFAULT, EXTERNAL or INTERNAL.
        * It needs to recalibrate.
        */
        void setReference(uint8_t type, int8_t adc_num = -1);


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
        void setResolution(uint8_t bits, int8_t adc_num = -1);

        //! Returns the resolution of the ADC_Module.
        uint8_t getResolution(int8_t adc_num = -1);

        //! Returns the maximum value for a measurement.
        uint32_t getMaxValue(int8_t adc_num = -1);


        //! Set the number of averages
        /*!
        * \param num can be 0, 4, 8, 16 or 32.
        */
        void setAveraging(uint8_t num, int8_t adc_num = -1);


        //! Enable interrupts
        /** An IRQ_ADC0 Interrupt will be raised when the conversion is completed
        *  (including hardware averages and if the comparison (if any) is true).
        */
        void enableInterrupts(int8_t adc_num = -1);

        //! Disable interrupts
        void disableInterrupts(int8_t adc_num = -1);


        //! Enable DMA request
        /** An ADC DMA request will be raised when the conversion is completed
        *  (including hardware averages and if the comparison (if any) is true).
        */
        void enableDMA(int8_t adc_num = -1);

        //! Disable ADC DMA request
        void disableDMA(int8_t adc_num = -1);


        //! Enable the compare function to a single value
        /** A conversion will be completed only when the ADC value
        *  is >= compValue (greaterThan=1) or < compValue (greaterThan=0)
        *  Call it after changing the resolution
        *  Use with interrupts or poll conversion completion with isComplete()
        */
        void enableCompare(int16_t compValue, int greaterThan, int8_t adc_num = -1);

        //! Enable the compare function to a range
        /** A conversion will be completed only when the ADC value is inside (insideRange=1) or outside (=0)
        *  the range given by (lowerLimit, upperLimit),including (inclusive=1) the limits or not (inclusive=0).
        *  See Table 31-78, p. 617 of the freescale manual.
        *  Call it after changing the resolution
        *  Use with interrupts or poll conversion completion with isComplete()
        */
        void enableCompareRange(int16_t lowerLimit, int16_t upperLimit, int insideRange, int inclusive, int8_t adc_num = -1);

        //! Disable the compare function
        void disableCompare(int8_t adc_num = -1);


        //! Enable and set PGA
        /** Enables the PGA and sets the gain
        *   Use only for signals lower than 1.2 V
        *   \param gain From 0 to 6, set PGA to 2^gain
        *
        */
        void enablePGA(uint8_t gain, int8_t adc_num = -1);
        void setPGA(uint8_t gain, int8_t adc_num = -1) {enablePGA(gain, adc_num);}

        //! Returns the PGA level
        /** PGA level = 2^gain, from 0 to 64
        */
        uint8_t getPGA(int8_t adc_num = -1);

        //! Disable PGA
        void disablePGA(int8_t adc_num = -1);


        //! Is the ADC converting at the moment?
        bool isConverting(int8_t adc_num = -1);

        //! Is an ADC conversion ready?
        /**
        *  \return 1 if yes, 0 if not.
        *  When a value is read this function returns 0 until a new value exists
        *  So it only makes sense to call it before analogReadContinuous() or readSingle()
        */
        bool isComplete(int8_t adc_num = -1);

        //! Is the ADC in differential mode?
        bool isDifferential(int8_t adc_num = -1);

        //! Is the ADC in continuous mode?
        bool isContinuous(int8_t adc_num = -1);


        //! Returns the analog value of the pin.
        /** It waits until the value is read and then returns the result.
        * If a comparison has been set up and fails, it will return ADC_ERROR_VALUE.
        * This function is interrupt safe, so it will restore the adc to the state it was before being called
        * If more than one ADC exists, it will select the module with less workload, you can force a selection using
        * adc_num. If you select ADC1 in Teensy 3.0 it will return ADC_ERROR_VALUE.
        */
        int analogRead(uint8_t pin, int8_t adc_num = -1);

        //! Reads the differential analog value of two pins (pinP - pinN).
        /** It waits until the value is read and then returns the result.
        * If a comparison has been set up and fails, it will return ADC_ERROR_VALUE.
        * \param pinP must be A10 or A12.
        * \param pinN must be A11 (if pinP=A10) or A13 (if pinP=A12).
        * Other pins will return ADC_ERROR_VALUE.
        * This function is interrupt safe, so it will restore the adc to the state it was before being called
        * If more than one ADC exists, it will select the module with less workload, you can force a selection using
        * adc_num
        */
        int analogReadDifferential(uint8_t pinP, uint8_t pinN, int8_t adc_num = -1);


        //! Starts an analog measurement on the pin and enables interrupts.
        /** It returns inmediately, get value with readSingle().
        *   If the pin is incorrect it returns ADC_ERROR_VALUE
        *   This function is interrupt safe. The ADC interrupt will restore the adc to its previous settings and
        *   restart the adc if it stopped a measurement. If you modify the adc_isr then this won't happen.
        */
        static int startSingleRead(uint8_t pin, int8_t adc_num = -1);

        //! Start a differential conversion between two pins (pinP - pinN) and enables interrupts.
        /** It returns inmediately, get value with readSingle().
        *   \param pinP must be A10 or A12.
        *   \param pinN must be A11 (if pinP=A10) or A13 (if pinP=A12).
        *   Other pins will return ADC_ERROR_DIFF_VALUE.
        *   This function is interrupt safe. The ADC interrupt will restore the adc to its previous settings and
        *   restart the adc if it stopped a measurement. If you modify the adc_isr then this won't happen.
        */
        static int startSingleDifferential(uint8_t pinP, uint8_t pinN, int8_t adc_num = -1);

        //! Reads the analog value of a single conversion.
        /** Set the conversion with with startSingleRead(pin) or startSingleDifferential(pinP, pinN).
        *   \return the converted value.
        */
        int readSingle(int8_t adc_num = -1);


        //! Starts continuous conversion on the pin.
        /** It returns as soon as the ADC is set, use analogReadContinuous() to read the value.
        */
        void startContinuous(uint8_t pin, int8_t adc_num = -1);

        //! Starts continuous conversion between the pins (pinP-pinN).
        /** It returns as soon as the ADC is set, use analogReadContinuous() to read the value.
        * \param pinP must be A10 or A12.
        * \param pinN must be A11 (if pinP=A10) or A13 (if pinP=A12).
        * Other pins will return ADC_ERROR_DIFF_VALUE.
        */
        void startContinuousDifferential(uint8_t pinP, uint8_t pinN, int8_t adc_num = -1);

        //! Reads the analog value of a continuous conversion.
        /** Set the continuous conversion with with analogStartContinuous(pin) or startContinuousDifferential(pinP, pinN).
        *   \return the last converted value.
        */
        int analogReadContinuous(int8_t adc_num = -1);

        //! Stops continuous conversion
        void stopContinuous(int8_t adc_num = -1);


         //! Starts a periodic measurement using the IntervalTimer library.
        /** The values will be added to a ring buffer of a fixed size.
        *   Read the oldest value with getTimerValue(pin), check if it's the last value with isLastValue(pin).
        *   When the buffer is full, new data will overwrite the oldest values.
        *   \returns ANALOG_TIMER_ERROR if the timer could not be started. Stop other analog timer and retry.
        *   This function is interrupt safe. The ADC interrupt will restore the adc to its previous settings and
        *   restart the adc if it stopped a measurement. If you modify the adc_isr then this won't happen.
        */
        int startAnalogTimer(uint8_t pin, uint32_t period, int8_t adc_num = -1);

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
        int startAnalogTimerDifferential(uint8_t pinP, uint8_t pinN, uint32_t period, int8_t adc_num = -1);

        //! Stops the periodic measurement
        /** As soon as it is stopped, you won't be able to access the buffer anymore
        *   It also disables the adc interrupt
        */
        void stopAnalogTimer(uint8_t pin);

        //! Returns the oldest value of the ring buffer where the periodic measurements go
        int getTimerValue(uint8_t pin);

        //! Is the oldest value also the last one?
        bool isTimerLastValue(uint8_t pin);


        typedef struct SYNC_RESULT{
            int result_adc0, result_adc1;
        } Sync_result;
        //! Returns the analog values of both pins, measured at the same time by the two ADC modules.
        /** It waits until the value is read and then returns the result as a struct Sync_result,
        *   use Sync_result.result_adc0 and Sync_result.result_adc1.
        * If a comparison has been set up and fails, it will return ADC_ERROR_VALUE in both fields of the struct.
        * This function is interrupt safe, so it will restore the adc to the state it was before being called
        * If more than one ADC exists, it will select the module with less workload
        */
        Sync_result analogSynchronizedRead(uint8_t pin0, uint8_t pin1);
        Sync_result analogSyncRead(uint8_t pin0, uint8_t pin1) {return analogSynchronizedRead(pin0, pin1);}


        //uint8_t channels_ADC0[20] = {14,15,16,17,18,19,20,21,22,23,34,35,36,37,40, ADC_TEMP_SENSOR, ADC_VREF_OUT, ADC_BANDGAP, ADC_VREFH, ADC_VREFL};
        //uint8_t channels_ADC1[17] = {16,17,26,27,28,29,30,31,34,35,36,37, ADC_TEMP_SENSOR, ADC_VREF_OUT, ADC_BANDGAP, ADC_VREFH, ADC_VREFL};


    protected:
    private:

         /** ADC interrupt callback for the timers
        *
        */
        static void ADC0_callback();
        static void ADC1_callback();

        /** Empty function
        *
        */
        static void voidFunction();


        // struct to store the config of the adc
        typedef struct ADC_CONFIG{
            uint32_t savedSC1A, savedSC2, savedSC3, savedCFG1, savedCFG2, savedRes;
            uint8_t diffRes; // is the new resolution different from the old one?
        } ADC_Config;


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

        #if defined(__MK20DX128__)
        uint8_t num_ADCs = 1;
        #elif defined(__MK20DX256__)
        uint8_t num_ADCs = 2;
        #endif


};


#ifdef __cplusplus
}
#endif


#endif // ADC_H
