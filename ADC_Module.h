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

/* ADC_Module.h: Declarations of the fuctions of a Teensy 3.x ADC module
 *
 */


#ifndef ADC_MODULE_H
#define ADC_MODULE_H

//#include "mk20dx128.h"

#include <Arduino.h>

/* MK20DX256 Datasheet:
The 16-bit accuracy specifications listed in Table 24 and Table 25 are achievable on the
differential pins ADCx_DP0, ADCx_DM0
All other ADC channels meet the 13-bit differential/12-bit single-ended accuracy
specifications.

The results in this data sheet were derived from a system which has < 8 Ohm analog source resistance. The RAS/CAS
time constant should be kept to < 1ns.

ADC clock should be 2 to 12 MHz for 16 bit mode
ADC clock should be 1 to 18 MHz for 8-12 bit mode
To use the maximum ADC conversion clock frequency, the ADHSC bit must be set and the ADLPC bit must be clear

The ADHSC bit is used to configure a higher clock input frequency. This will allow
faster overall conversion times. To meet internal ADC timing requirements, the ADHSC
bit adds additional ADCK cycles. Conversions with ADHSC = 1 take two more ADCK
cycles. ADHSC should be used when the ADCLK exceeds the limit for ADHSC = 0.

*/


// the alternate clock is connected to OSCERCLK (16 MHz).
// datasheet says ADC clock should be 2 to 12 MHz for 16 bit mode
// datasheet says ADC clock should be 1 to 18 MHz for 8-12 bit mode
// calibration works best when averages are 32 and speed is less than 4 MHz
// ADC_CFG1_ADICLK: 0=bus, 1=bus/2, 2=(alternative clk) altclk, 3=(async. clk) adack
// See below for an explanation of VERY_LOW_SPEED, LOW_SPEED, etc.
#if F_BUS == 60000000
  #define ADC_CFG1_3_75MHZ      (ADC_CFG1_ADIV(3) + ADC_CFG1_ADICLK(1))
  #define ADC_CFG1_7_5MHZ       (ADC_CFG1_ADIV(2) + ADC_CFG1_ADICLK(1))
  #define ADC_CFG1_15MHZ        (ADC_CFG1_ADIV(1) + ADC_CFG1_ADICLK(1))

  #define ADC_CFG1_VERY_LOW_SPEED ADC_CFG1_LOW_SPEED
  #define ADC_CFG1_LOW_SPEED (ADC_CFG1_3_75MHZ)
  #define ADC_CFG1_MED_SPEED (ADC_CFG1_7_5MHZ)
  #define ADC_CFG1_HI_SPEED_16_BITS (ADC_CFG1_7_5MHZ)
  #define ADC_CFG1_HI_SPEED (ADC_CFG1_15MHZ)
  #define ADC_CFG1_VERY_HIGH_SPEED ADC_CFG1_HI_SPEED

#elif F_BUS == 56000000
  #define ADC_CFG1_3_5MHZ   (ADC_CFG1_ADIV(3) + ADC_CFG1_ADICLK(1))
  #define ADC_CFG1_7MHZ     (ADC_CFG1_ADIV(2) + ADC_CFG1_ADICLK(1))
  #define ADC_CFG1_14MHZ    (ADC_CFG1_ADIV(1) + ADC_CFG1_ADICLK(1))

  #define ADC_CFG1_VERY_LOW_SPEED ADC_CFG1_LOW_SPEED
  #define ADC_CFG1_LOW_SPEED (ADC_CFG1_3_5MHZ)
  #define ADC_CFG1_MED_SPEED (ADC_CFG1_7MHZ)
  #define ADC_CFG1_HI_SPEED_16_BITS (ADC_CFG1_7MHZ)
  #define ADC_CFG1_HI_SPEED (ADC_CFG1_14MHZ)
  #define ADC_CFG1_VERY_HIGH_SPEED ADC_CFG1_HI_SPEED

#elif F_BUS == 48000000
  #define ADC_CFG1_3MHZ   (ADC_CFG1_ADIV(3) + ADC_CFG1_ADICLK(1)) // Clock divide select: 3=div8 + Input clock: 1=bus/2
  #define ADC_CFG1_6MHZ   (ADC_CFG1_ADIV(2) + ADC_CFG1_ADICLK(1)) // Clock divide select: 2=div4 + Input clock: 1=bus/2
  #define ADC_CFG1_12MHZ  (ADC_CFG1_ADIV(1) + ADC_CFG1_ADICLK(1)) // Clock divide select: 1=div2 Input clock: 1=bus/2
  #define ADC_CFG1_24MHZ  (ADC_CFG1_ADIV(0) + ADC_CFG1_ADICLK(1)) // this is way too fast, so accurancy is not guaranteed

  #define ADC_CFG1_VERY_LOW_SPEED ADC_CFG1_LOW_SPEED
  #define ADC_CFG1_LOW_SPEED (ADC_CFG1_3MHZ)
  #define ADC_CFG1_MED_SPEED (ADC_CFG1_6MHZ)
  #define ADC_CFG1_HI_SPEED_16_BITS (ADC_CFG1_12MHZ)
  #define ADC_CFG1_HI_SPEED (ADC_CFG1_12MHZ)
  #define ADC_CFG1_VERY_HIGH_SPEED (ADC_CFG1_24MHZ)

#elif F_BUS == 40000000
  #define ADC_CFG1_2_5MHZ   (ADC_CFG1_ADIV(3) + ADC_CFG1_ADICLK(1))
  #define ADC_CFG1_5MHZ     (ADC_CFG1_ADIV(2) + ADC_CFG1_ADICLK(1))
  #define ADC_CFG1_10MHZ    (ADC_CFG1_ADIV(1) + ADC_CFG1_ADICLK(1))
  #define ADC_CFG1_20MHZ    (ADC_CFG1_ADIV(0) + ADC_CFG1_ADICLK(1)) // this is too fast, so accurancy is not guaranteed

  #define ADC_CFG1_VERY_LOW_SPEED ADC_CFG1_LOW_SPEED
  #define ADC_CFG1_LOW_SPEED (ADC_CFG1_2_5MHZ)
  #define ADC_CFG1_MED_SPEED (ADC_CFG1_5MHZ)
  #define ADC_CFG1_HI_SPEED_16_BITS (ADC_CFG1_10MHZ)
  #define ADC_CFG1_HI_SPEED (ADC_CFG1_10MHZ)
  #define ADC_CFG1_VERY_HIGH_SPEED (ADC_CFG1_20MHZ)

#elif F_BUS == 36000000
  #define ADC_CFG1_2_25MHZ      (ADC_CFG1_ADIV(3) + ADC_CFG1_ADICLK(1))
  #define ADC_CFG1_4_5MHZ       (ADC_CFG1_ADIV(2) + ADC_CFG1_ADICLK(1))
  #define ADC_CFG1_9MHZ         (ADC_CFG1_ADIV(1) + ADC_CFG1_ADICLK(1))
  #define ADC_CFG1_18MHZ        (ADC_CFG1_ADIV(0) + ADC_CFG1_ADICLK(1))

  #define ADC_CFG1_VERY_LOW_SPEED ADC_CFG1_LOW_SPEED
  #define ADC_CFG1_LOW_SPEED (ADC_CFG1_2_25MHZ)
  #define ADC_CFG1_MED_SPEED (ADC_CFG1_9MHZ)
  #define ADC_CFG1_HI_SPEED_16_BITS (ADC_CFG1_9MHZ)
  #define ADC_CFG1_HI_SPEED (ADC_CFG1_18MHZ)
  #define ADC_CFG1_VERY_HIGH_SPEED ADC_CFG1_HI_SPEED

#elif F_BUS == 24000000
  #define ADC_CFG1_1_5MHZ   (ADC_CFG1_ADIV(3) + ADC_CFG1_ADICLK(1))
  #define ADC_CFG1_3MHZ     (ADC_CFG1_ADIV(3) + ADC_CFG1_ADICLK(0)) // Clock divide select: 3=div8 + Input clock: 0=bus
  #define ADC_CFG1_6MHZ     (ADC_CFG1_ADIV(2) + ADC_CFG1_ADICLK(0)) // Clock divide select: 2=div4 + Input clock: 0=bus
  #define ADC_CFG1_12MHZ    (ADC_CFG1_ADIV(1) + ADC_CFG1_ADICLK(0)) // Clock divide select: 1=div2 + Input clock: 0=bus
  #define ADC_CFG1_24MHZ    (ADC_CFG1_ADIV(0) + ADC_CFG1_ADICLK(0)) // this is way too fast, so accurancy is not guaranteed

  #define ADC_CFG1_VERY_LOW_SPEED (ADC_CFG1_1_5MHZ)
  #define ADC_CFG1_LOW_SPEED (ADC_CFG1_3MHZ)
  #define ADC_CFG1_MED_SPEED (ADC_CFG1_6MHZ)
  #define ADC_CFG1_HI_SPEED_16_BITS (ADC_CFG1_12MHZ)
  #define ADC_CFG1_HI_SPEED (ADC_CFG1_12MHZ)
  #define ADC_CFG1_VERY_HIGH_SPEED (ADC_CFG1_24MHZ)

#elif F_BUS == 4000000
  #define ADC_CFG1_1MHZ   (ADC_CFG1_ADIV(2) + ADC_CFG1_ADICLK(0))
  #define ADC_CFG1_2MHZ   (ADC_CFG1_ADIV(1) + ADC_CFG1_ADICLK(0))
  #define ADC_CFG1_4MHZ   (ADC_CFG1_ADIV(0) + ADC_CFG1_ADICLK(0))

  #define ADC_CFG1_VERY_LOW_SPEED (ADC_CFG1_1MHZ)
  #define ADC_CFG1_LOW_SPEED (ADC_CFG1_2MHZ)
  #define ADC_CFG1_MED_SPEED (ADC_CFG1_4MHZ)
  #define ADC_CFG1_HI_SPEED_16_BITS (ADC_CFG1_4MHZ)
  #define ADC_CFG1_HI_SPEED (ADC_CFG1_4MHZ)
  #define ADC_CFG1_VERY_HIGH_SPEED ADC_CFG1_HI_SPEED

#elif F_BUS == 2000000
  #define ADC_CFG1_1MHZ   (ADC_CFG1_ADIV(1) + ADC_CFG1_ADICLK(0))
  #define ADC_CFG1_2MHZ   (ADC_CFG1_ADIV(0) + ADC_CFG1_ADICLK(0))

  #define ADC_CFG1_VERY_LOW_SPEED (ADC_CFG1_1MHZ)
  #define ADC_CFG1_LOW_SPEED (ADC_CFG1_2MHZ)
  #define ADC_CFG1_MED_SPEED (ADC_CFG1_2MHZ)
  #define ADC_CFG1_HI_SPEED_16_BITS (ADC_CFG1_2MHZ)
  #define ADC_CFG1_HI_SPEED (ADC_CFG1_2MHZ)
  #define ADC_CFG1_VERY_HIGH_SPEED ADC_CFG1_HI_SPEED

#else
    #error "F_BUS must be 60, 56, 48, 40, 36, 24, 4 or 2 MHz"
#endif

// mask the important bit in each register
#define ADC_CFG1_ADICLK_MASK_1 (1<<1)
#define ADC_CFG1_ADICLK_MASK_0 (1<<0)

#define ADC_CFG1_ADIV_MASK_1 (1<<6)
#define ADC_CFG1_ADIV_MASK_0 (1<<5)

// Mask for the channel selection in ADCx_SC1A,
// useful if you want to get the channel number from ADCx_SC1A
#define ADC_SC1A_CHANNELS 0x1F

// Settings for the power/speed of conversions/sampling
/* For conversion speeds:
    ADC_VERY_LOW_SPEED is guaranteed to be the lowest possible speed within specs for resolutions less than 16 bits (higher than 1 MHz),
    it's different from ADC_LOW_SPEED only for 24, 4 or 2 MHz.
    ADC_LOW_SPEED is guaranteed to be the lowest possible speed within specs for all resolutions (higher than 2 MHz).
    ADC_MED_SPEED is always >= ADC_LOW_SPEED and <= ADC_HIGH_SPEED.
    ADC_HIGH_SPEED_16BITS is guaranteed to be the highest possible speed within specs for all resolutions (lower or eq than 12 MHz).
    ADC_HIGH_SPEED is guaranteed to be the highest possible speed within specs for resolutions less than 16 bits (lower or eq than 18 MHz).
    ADC_VERY_HIGH_SPEED may be out of specs, it's different from ADC_HIGH_SPEED only for 48, 40 or 24 MHz.
*/
/* For sampling speeds:
    ADC_VERY_LOW_SPEED is the lowest possible sampling speed (+24 ADCK).
    ADC_LOW_SPEED adds +16 ADCK.
    ADC_MED_SPEED adds +10 ADCK.
    ADC_HIGH_SPEED (or ADC_HIGH_SPEED_16BITS) adds +6 ADCK.
    ADC_VERY_HIGH_SPEED is the highest possible sampling speed (0 ADCK added).
*/
#define ADC_VERY_LOW_SPEED      0
#define ADC_LOW_SPEED           1
#define ADC_MED_SPEED           2
#define ADC_HIGH_SPEED_16BITS   3
#define ADC_HIGH_SPEED          4
#define ADC_VERY_HIGH_SPEED     5


// ADCx_SC2[REFSEL] bit selects the voltage reference sources for ADC.
//   VREFH/VREFL - connected as the primary reference option
//   1.2 V VREF_OUT - connected as the VALT reference option
#define ADC_REF_DEFAULT         0
#define ADC_REF_EXTERNAL        0
#define ADC_REF_INTERNAL        2
#define ADC_REF_INTERNAL1V2     2
#define ADC_REF_INTERNAL1V1     2


// Error codes for analogRead and analogReadDifferential
#define ADC_ERROR_DIFF_VALUE -70000
#define ADC_ERROR_VALUE ADC_ERROR_DIFF_VALUE

// Error flag masks.
// Possible errors are: other, calibration, wrong pin, analogRead, analogDifferentialRead, continuous, continuousDifferential
#define ADC_ERROR_ALL               0x3FF
#define ADC_ERROR_CLEAR             0x0
#define ADC_ERROR_OTHER             (1<<0)
#define ADC_ERROR_CALIB             (1<<1)
#define ADC_ERROR_WRONG_PIN         (1<<2)
#define ADC_ERROR_ANALOG_READ       (1<<3)
#define ADC_ERROR_COMPARISON        (1<<4)
#define ADC_ERROR_ANALOG_DIFF_READ  (1<<5)
#define ADC_ERROR_CONT              (1<<6)
#define ADC_ERROR_CONT_DIFF         (1<<7)
#define ADC_ERROR_WRONG_ADC         (1<<8)
#define ADC_ERROR_SYNCH             (1<<9)


// Other things to measure with the ADC that don't use external pins
// In my Teensy I read 1.22 V for the ADC_VREF_OUT, random values for ADC_BANDGAP,
// 3.3 V for ADC_VREFH and 0.0 V for ADC_VREFL.
#define ADC_TEMP_SENSOR     38
#define ADC_VREF_OUT        39
#define ADC_BANDGAP         41
#define ADC_VREFH           42
#define ADC_VREFL           43


// debug mode: blink the led light
#define ADC_debug 0


/** Class ADC_Module: Implements all functions of the Teensy 3.x analog to digital converter
*
*/
class ADC_Module
{
    friend class ADC;

    public:

        /** Constructor
        *
        */
        ADC_Module(uint8_t ADC_number = 0);


        //! Starts the calibration sequence, waits until it's done and writes the results
        /** Usually it's not necessary to call this function directly, but do it if the "enviroment" changed
        *   significantly since the program was started.
        */
        void recalibrate();


        /////////////// METHODS TO SET/GET SETTINGS OF THE ADC ////////////////////

        //! Set the voltage reference you prefer, default is vcc
        /*!
        * \param type can be DEFAULT, EXTERNAL or INTERNAL.
        *
        *  It recalibrates at the end.
        */
        void setReference(uint8_t type);


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
        void setResolution(uint8_t bits);

        //! Returns the resolution of the ADC_Module.
        uint8_t getResolution();

        //! Returns the maximum value for a measurement.
        uint32_t getMaxValue();


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
        void setConversionSpeed(uint8_t speed);


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
        void setSamplingSpeed(uint8_t speed);


        //! Set the number of averages
        /*!
        * \param num can be 0, 4, 8, 16 or 32.
        *
        *  It doesn't recalibrate at the end.
        */
        void setAveraging(uint8_t num);


        //! Enable interrupts
        /** An IRQ_ADC0 Interrupt will be raised when the conversion is completed
        *  (including hardware averages and if the comparison (if any) is true).
        */
        void enableInterrupts();

        //! Disable interrupts
        void disableInterrupts();


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
        void enableCompare(int16_t compValue, bool greaterThan);

        //! Enable the compare function to a range
        /** A conversion will be completed only when the ADC value is inside (insideRange=1) or outside (=0)
        *  the range given by (lowerLimit, upperLimit),including (inclusive=1) the limits or not (inclusive=0).
        *  See Table 31-78, p. 617 of the freescale manual.
        *  Call it after changing the resolution
        *  Use with interrupts or poll conversion completion with isComplete()
        */
        void enableCompareRange(int16_t lowerLimit, int16_t upperLimit, bool insideRange, bool inclusive);

        //! Disable the compare function
        void disableCompare();


        //! Enable and set PGA
        /** Enables the PGA and sets the gain
        *   Use only for signals lower than 1.2 V
        *   \param gain can be 1 (disabled), 2, 4, 8, 16, 32 or 64
        *
        */
        void enablePGA(uint8_t gain);
        void setPGA(uint8_t gain) {enablePGA(gain);}

        //! Returns the PGA level
        /** PGA from 1 to 64
        */
        uint8_t getPGA();

        //! Disable PGA
        void disablePGA();


        ////////////// INFORMATION ABOUT THE STATE OF THE ADC /////////////////

        //! Is the ADC converting at the moment?
        inline bool isConverting() {
            return (*ADC_SC2_adact);
            //return ((*ADC_SC2) & ADC_SC2_ADACT) >> 7;
        }

        //! Is an ADC conversion ready?
        /**
        *  \return 1 if yes, 0 if not.
        *  When a value is read this function returns 0 until a new value exists
        *  So it only makes sense to call it before analogReadContinuous() or readSingle()
        */
        inline bool isComplete() {
            return (*ADC_SC1A_coco);
            //return ((*ADC_SC1A) & ADC_SC1_COCO) >> 7;
        }

        //! Is the ADC in differential mode?
        inline bool isDifferential() {
            return ((*ADC_SC1A) & ADC_SC1_DIFF) >> 5;
        }

        //! Is the ADC in continuous mode?
        inline bool isContinuous() {
            return (*ADC_SC3_adco);
            //return ((*ADC_SC3) & ADC_SC3_ADCO) >> 3;
        }


        //////////////// BLOCKING CONVERSION METHODS //////////////////

        //! Returns the analog value of the pin.
        /** It waits until the value is read and then returns the result.
        * If a comparison has been set up and fails, it will return ADC_ERROR_VALUE.
        * This function is interrupt safe, so it will restore the adc to the state it was before being called
        */
        int analogRead(uint8_t pin);

        //! Reads the differential analog value of two pins (pinP - pinN).
        /** It waits until the value is read and then returns the result.
        * If a comparison has been set up and fails, it will return ADC_ERROR_DIFF_VALUE.
        * \param pinP must be A10 or A12.
        * \param pinN must be A11 (if pinP=A10) or A13 (if pinP=A12)..
        * Other pins will return ADC_ERROR_DIFF_VALUE.
        * This function is interrupt safe, so it will restore the adc to the state it was before being called
        */
        int analogReadDifferential(uint8_t pinP, uint8_t pinN);


        /////////////// NON-BLOCKING CONVERSION METHODS //////////////

        //! Starts an analog measurement on the pin and enables interrupts.
        /** It returns inmediately, get value with readSingle().
        *   If the pin is incorrect it returns ADC_ERROR_VALUE
        *   If this function interrupts a measurement, it stores the settings in adc_config
        */
        int startSingleRead(uint8_t pin);
        int startSingleReadFast(uint8_t pin);

        //! Start a differential conversion between two pins (pinP - pinN) and enables interrupts.
        /** It returns inmediately, get value with readSingle().
        *   \param pinP must be A10 or A12.
        *   \param pinN must be A11 (if pinP=A10) or A13 (if pinP=A12).
        *   Other pins will return ADC_ERROR_DIFF_VALUE.
        *   If this function interrupts a measurement, it stores the settings in adc_config
        */
        int startSingleDifferential(uint8_t pinP, uint8_t pinN);
        int startSingleDifferentialFast(uint8_t pinP, uint8_t pinN);

        //! Reads the analog value of a single conversion.
        /** Set the conversion with with startSingleRead(pin) or startSingleDifferential(pinP, pinN).
        *   \return the converted value.
        */
        inline int readSingle(){ return analogReadContinuous(); }


        ///////////// CONTINUOUS CONVERSION METHODS ////////////

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
        *   If single-ended and 16 bits it's necessary to typecast it to an unsigned type (like uint16_t),
        *   otherwise values larger than 3.3/2 V are interpreted as negative!
        */
        inline int analogReadContinuous() {
            return (int16_t)(int32_t)*ADC_RA;
        }

        //! Stops continuous conversion
        void stopContinuous();


        //////// OTHER STUFF ///////////

        // struct to store the config of the adc
        typedef struct ADC_CONFIG{
            uint32_t savedSC1A, savedSC2, savedSC3, savedCFG1, savedCFG2;
        } ADC_Config;

        ADC_Config adc_config; // store ADC config
        uint8_t adcWasInUse; // was the adc in use before an analog timer call?


        // struct stores the relevant bits that affect power and speed of conversion
        typedef struct ADC_POWER_SPEED_CONFIG{
            //   low power, low sampling speed, enable async. clock, high speed config
            bool adlpc, adlsmp, adacken, adhsc;
            //      divide input clk, select input clk, long sampling time
            uint8_t adiv, adiclk, adlsts;
        } ADC_Power_Speed_Config;


        // number of measurements that the ADC is performing
        uint8_t num_measurements = 0;


        // translate pin number to SC1A nomenclature and viceversa
        uint8_t channel2sc1a[44];
        uint8_t sc1a2channel[31];


        // This flag indicates that some kind of error took place
        // Use the defines at the beggining of this file to find out what caused the fail.
        uint16_t fail_flag;


    private:

        // is set to 1 when the calibration procedure is taking place
        uint8_t calibrating;

        // the first calibration will use 32 averages and lowest speed,
        // when this calibration is over the averages and speed will be set to default.
        uint8_t init_calib;

        // resolution
        uint8_t analog_res_bits;

        // maximum value possible 2^res-1
        uint32_t analog_max_val;

        // num of averages
        uint8_t analog_num_average;

        // reference can be internal or external
        uint8_t analog_reference_internal;

        // are interrupts enabled?
        uint8_t var_enableInterrupts;

        // value of the pga
        uint8_t pga_value;

        // conversion speed
        uint8_t conversion_speed;

        // sampling speed
        uint8_t sampling_speed;



        // which adc is this?
        uint8_t ADC_num;


        //! Initialize ADC
        void analog_init();


        //! Starts the calibration sequence
        void calibrate();

        /** Waits until calibration is finished and writes the corresponding registers
        *
        */
        void wait_for_cal(void);


        // returns the bitband address of the adc_register's bit.
        // use it for atomic access.
        inline volatile uint32_t* adc_bitband(uint32_t adc_register, uint8_t bit) {
            //return (uint32_t*)(((uint32_t)adc_register&0xBFFFFFFF)<<5) + 0x42000000 + 4*bit;
            return (uint32_t*)(((uint32_t)(adc_register) - 0x40000000)*32 + 0x42000000 + 4*bit);
        }

        // registers point to the correct ADC module
        typedef volatile uint32_t* reg;

        // registers that control the adc module
        reg ADC_SC1A; reg ADC_SC1A_aien; reg ADC_SC1A_coco;
        reg ADC_SC1B;

        reg ADC_CFG1; reg ADC_CFG1_adlpc; reg ADC_CFG1_adiv0, ADC_CFG1_adiv1; reg ADC_CFG1_adlsmp;
                        reg ADC_CFG1_mode0, ADC_CFG1_mode1; reg ADC_CFG1_adiclk0, ADC_CFG1_adiclk1;
        reg ADC_CFG2; reg ADC_CFG2_muxsel; reg ADC_CFG2_adacken; reg ADC_CFG2_adhsc; reg ADC_CFG2_adlsts1, ADC_CFG2_adlsts0;

        reg ADC_RA;
        reg ADC_RB;

        reg ADC_CV1;
        reg ADC_CV2;

        reg ADC_SC2; reg ADC_SC2_adact; reg ADC_SC2_ref; reg ADC_SC2_dma; reg ADC_SC2_cfe; reg ADC_SC2_cfgt; reg ADC_SC2_cren;
        reg ADC_SC3; reg ADC_SC3_cal, ADC_SC3_calf; reg ADC_SC3_avge; reg ADC_SC3_avgs0, ADC_SC3_avgs1; reg ADC_SC3_adco;

        reg ADC_PGA; reg ADC_PGA_pgaen;

        reg ADC_OFS;
        reg ADC_PG;
        reg ADC_MG;
        reg ADC_CLPD;
        reg ADC_CLPS;
        reg ADC_CLP4;
        reg ADC_CLP3;
        reg ADC_CLP2;
        reg ADC_CLP1;
        reg ADC_CLP0;
        reg ADC_CLMD;
        reg ADC_CLMS;
        reg ADC_CLM4;
        reg ADC_CLM3;
        reg ADC_CLM2;
        reg ADC_CLM1;
        reg ADC_CLM0;


    protected:


};


#endif // ADC_MODULE_H
