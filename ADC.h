/* Teensy 3.x ADC library
 * https://github.com/pedvide/ADC
 * Copyright (c) 2014 Pedro Villanueva
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* ADC.h: Control for one (Teensy 3.0) or two ADC modules (Teensy 3.1).
 *
 */

/* TODO
* - Function to measure more that 1 pin consecutively (stream?)
*
* bugs:
* - comparison values in 16 bit differential mode are twice what they should be
*/

#ifndef ADC_H
#define ADC_H

#define ADC_0 0
#define ADC_1 1


// include ADC module class
#include "ADC_Module.h"

// include the circular buffer
#include "RingBuffer.h"
#include "RingBufferDMA.h"

// dma assigment
//#include "DMAChannel.h"



#ifdef __cplusplus
extern "C" {
#endif

/** Class ADC: Controls the Teensy 3.x ADC
*
*/
class ADC
{
    public:

        /** Default constructor */
        ADC();


        // create both adc objects

        //! Object to control the ADC0
        //static ADC_Module *adc0; // adc object
        ADC_Module *adc0; // adc object
        #if defined(__MK20DX256__)
        //! Object to control the ADC1
        //static ADC_Module *adc1; // adc object
        ADC_Module *adc1; // adc object
        #endif


        /////////////// METHODS TO SET/GET SETTINGS OF THE ADC ////////////////////

        //! Set the voltage reference you prefer, default is vcc
        /*!
        * \param type can be DEFAULT, EXTERNAL or INTERNAL.
        *
        *  It recalibrates at the end.
        */
        void setReference(uint8_t type, int8_t adc_num = -1);


        //! Change the resolution of the measurement.
        /*!
        *  \param bits is the number of bits of resolution.
        *  For single-ended measurements: 8, 10, 12 or 16 bits.
        *  For differential measurements: 9, 11, 13 or 16 bits.
        *  If you want something in between (11 bits single-ended for example) select the inmediate higher
        *  and shift the result one to the right.
        *
        *  Whenever you change the resolution, change also the comparison values (if you use them).
        */
        void setResolution(uint8_t bits, int8_t adc_num = -1);

        //! Returns the resolution of the ADC_Module.
        uint8_t getResolution(int8_t adc_num = -1);

        //! Returns the maximum value for a measurement.
        uint32_t getMaxValue(int8_t adc_num = -1);


        //! Sets the conversion speed
        /**
        * \param speed can be ADC_VERY_LOW_SPEED, ADC_LOW_SPEED, ADC_MED_SPEED, ADC_HIGH_SPEED_16BITS, ADC_HIGH_SPEED or ADC_VERY_HIGH_SPEED.
        *
        * ADC_VERY_LOW_SPEED is guaranteed to be the lowest possible speed within specs for resolutions less than 16 bits (higher than 1 MHz),
        * it's different from ADC_LOW_SPEED only for 24, 4 or 2 MHz.
        * ADC_LOW_SPEED is guaranteed to be the lowest possible speed within specs for all resolutions (higher than 2 MHz).
        * ADC_MED_SPEED is always >= ADC_LOW_SPEED and <= ADC_HIGH_SPEED.
        * ADC_HIGH_SPEED_16BITS is guaranteed to be the highest possible speed within specs for all resolutions (lower or eq than 12 MHz).
        * ADC_HIGH_SPEED is guaranteed to be the highest possible speed within specs for resolutions less than 16 bits (lower or eq than 18 MHz).
        * ADC_VERY_HIGH_SPEED may be out of specs, it's different from ADC_HIGH_SPEED only for 48, 40 or 24 MHz.
        */
        void setConversionSpeed(uint8_t speed, int8_t adc_num = -1);


        //! Sets the sampling speed
        /** Increase the sampling speed for low impedance sources, decrease it for higher impedance ones.
        * \param speed can be ADC_VERY_LOW_SPEED, ADC_LOW_SPEED, ADC_MED_SPEED, ADC_HIGH_SPEED or ADC_VERY_HIGH_SPEED.
        *
        * ADC_VERY_LOW_SPEED is the lowest possible sampling speed (+24 ADCK).
        * ADC_LOW_SPEED adds +16 ADCK.
        * ADC_MED_SPEED adds +10 ADCK.
        * ADC_HIGH_SPEED (or ADC_HIGH_SPEED_16BITS) adds +6 ADCK.
        * ADC_VERY_HIGH_SPEED is the highest possible sampling speed (0 ADCK added).
        */
        void setSamplingSpeed(uint8_t speed, int8_t adc_num = -1);


        //! Set the number of averages
        /*!
        * \param num can be 0, 4, 8, 16 or 32..
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
        void enableCompare(int16_t compValue, bool greaterThan, int8_t adc_num = -1);

        //! Enable the compare function to a range
        /** A conversion will be completed only when the ADC value is inside (insideRange=1) or outside (=0)
        *  the range given by (lowerLimit, upperLimit),including (inclusive=1) the limits or not (inclusive=0).
        *  See Table 31-78, p. 617 of the freescale manual.
        *  Call it after changing the resolution
        *  Use with interrupts or poll conversion completion with isComplete()
        */
        void enableCompareRange(int16_t lowerLimit, int16_t upperLimit, bool insideRange, bool inclusive, int8_t adc_num = -1);

        //! Disable the compare function
        void disableCompare(int8_t adc_num = -1);


        //! Enable and set PGA
        /** Enables the PGA and sets the gain
        *   Use only for signals lower than 1.2 V
        *   \param gain can be 1 (disabled), 2, 4, 8, 16, 32 or 64
        *
        */
        void enablePGA(uint8_t gain, int8_t adc_num = -1);

        //! Returns the PGA level
        /** PGA level = from 1 to 64
        */
        uint8_t getPGA(int8_t adc_num = -1);

        //! Disable PGA
        void disablePGA(int8_t adc_num = -1);



        ////////////// INFORMATION ABOUT THE STATE OF THE ADC /////////////////

        //! Is the ADC converting at the moment?
        bool isConverting(int8_t adc_num = -1);

        //! Is an ADC conversion ready?
        /**
        *  \return 1 if yes, 0 if not.
        *  When a value is read this function returns 0 until a new value exists
        *  So it only makes sense to call it with continuous or non-blocking methods
        */
        bool isComplete(int8_t adc_num = -1);

        //! Is the ADC in differential mode?
        bool isDifferential(int8_t adc_num = -1);

        //! Is the ADC in continuous mode?
        bool isContinuous(int8_t adc_num = -1);



        //////////////// BLOCKING CONVERSION METHODS //////////////////

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
        *
        * This function is interrupt safe, so it will restore the adc to the state it was before being called
        * If more than one ADC exists, it will select the module with less workload, you can force a selection using
        * adc_num
        */
        int analogReadDifferential(uint8_t pinP, uint8_t pinN, int8_t adc_num = -1);


        /////////////// NON-BLOCKING CONVERSION METHODS //////////////

        //! Starts an analog measurement on the pin and enables interrupts.
        /** It returns inmediately, get value with readSingle().
        *   If the pin is incorrect it returns ADC_ERROR_VALUE
        *   If this function interrupts a measurement, it stores the settings in adc_config
        */
        int startSingleRead(uint8_t pin, int8_t adc_num = -1);

        //! Start a differential conversion between two pins (pinP - pinN) and enables interrupts.
        /** It returns inmediately, get value with readSingle().
        *   \param pinP must be A10 or A12.
        *   \param pinN must be A11 (if pinP=A10) or A13 (if pinP=A12).
        *
        *   Other pins will return ADC_ERROR_DIFF_VALUE.
        *   If this function interrupts a measurement, it stores the settings in adc_config
        */
        int startSingleDifferential(uint8_t pinP, uint8_t pinN, int8_t adc_num = -1);

        //! Reads the analog value of a single conversion.
        /** Set the conversion with with startSingleRead(pin) or startSingleDifferential(pinP, pinN).
        *   \return the converted value.
        */
        int readSingle(int8_t adc_num = -1);



        ///////////// CONTINUOUS CONVERSION METHODS ////////////

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
        *   If single-ended and 16 bits it's necessary to typecast it to an unsigned type (like uint16_t),
        *   otherwise values larger than 3.3/2 V are interpreted as negative!
        */
        inline int analogReadContinuous(int8_t adc_num = -1) { // make it inline so it's a bit faster
            if(adc_num==1){ // user wants ADC 1, do nothing if it's a Teensy 3.0
                #if defined(__MK20DX256__)
                return adc1->analogReadContinuous();
                #else
                adc0->fail_flag |= ADC_ERROR_WRONG_ADC;
                #endif
                return false;
            }
            return adc0->analogReadContinuous();
        }

        //! Stops continuous conversion
        void stopContinuous(int8_t adc_num = -1);



        /////////// SYNCHRONIZED METHODS ///////////////

        //! Struct for synchronous measurements
        /** result_adc0 has the result from ADC0 and result_adc1 from ADC1.
        */
        typedef struct SYNC_RESULT{
            int32_t result_adc0, result_adc1;
        } Sync_result;

        //////////////// SYNCHRONIZED BLOCKING METHODS //////////////////

        //! Returns the analog values of both pins, measured at the same time by the two ADC modules.
        /** It waits until the values are read and then returns the result as a struct Sync_result,
        *   use Sync_result.result_adc0 and Sync_result.result_adc1.
        * If a comparison has been set up and fails, it will return ADC_ERROR_VALUE in both fields of the struct.
        * This function is interrupt safe, so it will restore the adc to the state it was before being called
        */
        Sync_result analogSynchronizedRead(uint8_t pin0, uint8_t pin1);
        inline Sync_result analogSyncRead(uint8_t pin0, uint8_t pin1) {return analogSynchronizedRead(pin0, pin1);}

        //! Returns the differential analog values of both sets of pins, measured at the same time by the two ADC modules.
        /** It waits until the values are read and then returns the result as a struct Sync_result,
        *   use Sync_result.result_adc0 and Sync_result.result_adc1.
        * If a comparison has been set up and fails, it will return ADC_ERROR_VALUE in both fields of the struct.
        * This function is interrupt safe, so it will restore the adc to the state it was before being called
        */
        Sync_result analogSynchronizedReadDifferential(uint8_t pin0P, uint8_t pin0N, uint8_t pin1P, uint8_t pin1N);
        inline Sync_result analogSyncReadDifferential(uint8_t pin0P, uint8_t pin0N, uint8_t pin1P, uint8_t pin1N) {
            return analogSynchronizedReadDifferential(pin0P, pin0N, pin1P, pin1N);
        }

        /////////////// SYNCHRONIZED NON-BLOCKING METHODS //////////////

        //! Starts an analog measurement at the same time on the two ADC modules
        /** It returns inmediately, get value with readSynchronizedSingle().
        *   If the pin is incorrect it returns ADC_ERROR_VALUE
        *   If this function interrupts a measurement, it stores the settings in adc_config
        */
        int startSynchronizedSingleRead(uint8_t pin0, uint8_t pin1);

        //! Start a differential conversion between two pins (pin0P - pin0N) and (pin1P - pin1N)
        /** It returns inmediately, get value with readSynchronizedSingle().
        *   \param pinP must be A10 or A12.
        *   \param pinN must be A11 (if pinP=A10) or A13 (if pinP=A12).
        *   Other pins will return ADC_ERROR_DIFF_VALUE.
        *   If this function interrupts a measurement, it stores the settings in adc_config
        */
        int startSynchronizedSingleDifferential(uint8_t pin0P, uint8_t pin0N, uint8_t pin1P, uint8_t pin1N);

        //! Reads the analog value of a single conversion.
        /**
        *   \return the converted value.
        */
        Sync_result readSynchronizedSingle();


        ///////////// SYNCHRONIZED CONTINUOUS CONVERSION METHODS ////////////

        //! Starts a continuous conversion in both ADCs simultaneously
        /** Use readSynchronizedContinuous to get the values
        *
        */
        void startSynchronizedContinuous(uint8_t pin0, uint8_t pin1);

        //! Starts a continuous differential conversion in both ADCs simultaneously
        /** Use readSynchronizedContinuous to get the values
        *
        */
        void startSynchronizedContinuousDifferential(uint8_t pin0P, uint8_t pin0N, uint8_t pin1P, uint8_t pin1N);

        //! Returns the values of both ADCs.
        Sync_result readSynchronizedContinuous();

        //! Stops synchronous continuous conversion
        void stopSynchronizedContinuous();


        /////////// DMA METHODS ///////////////

        // DMA stuff
        static uint8_t dma_Ch0, dma_Ch1;

        static void dma_isr_0(void);
        static void dma_isr_1(void);

        #if defined(__MK20DX128__)
        //! DMA buffer to store all converted values
        RingBufferDMA *buffer0;
        #elif defined(__MK20DX256__)
        //! DMA buffer to store all converted values
        RingBufferDMA *buffer0;
        RingBufferDMA *buffer1;
        #endif

        void useDMA(uint8_t ch0=0, uint8_t ch1=1);


        // PDB stuff
        //void startPDB(double period);
        //double adc_pdb_period;

        //! Translate pin number to SC1A nomenclature and viceversa
        static const uint8_t channel2sc1aADC0[44];
        static const uint8_t sc1a2channelADC0[31];
        static const uint8_t channel2sc1aADC1[44];
        static const uint8_t sc1a2channelADC1[31];


    protected:
    private:


        #if defined(__MK20DX128__)
        const uint8_t num_ADCs = 1;
        #elif defined(__MK20DX256__)
        const uint8_t num_ADCs = 2;
        #endif


};


#ifdef __cplusplus
}
#endif


#endif // ADC_H
