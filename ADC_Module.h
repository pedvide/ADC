/* Teensy 3.x, LC ADC library
 * https://github.com/pedvide/ADC
 * Copyright (c) 2015 Pedro Villanueva
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

/* ADC_Module.h: Declarations of the fuctions of a Teensy 3.x, LC ADC module
 *
 */


#ifndef ADC_MODULE_H
#define ADC_MODULE_H

#include <Arduino.h>

// Easier names for the boards
#if defined(__MK20DX256__) // Teensy 3.1
        #define ADC_TEENSY_3_1
#elif defined(__MK20DX128__) // Teensy 3.0
        #define ADC_TEENSY_3_0
#elif defined(__MKL26Z64__) // Teensy LC
        #define ADC_TEENSY_LC
#endif

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
// TODO: add support for ADACK (asynch clock, that goes from about 2 to 6 MHz)
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
  #define ADC_CFG1_1_5MHZ   (ADC_CFG1_ADIV(3) + ADC_CFG1_ADICLK(1)) // Clock divide select: 3=div8 + Input clock: 1=bus/2
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

// little hack to avoid changing a lot of code in the differential functions in ADC_Module.cpp
// A13 isn't defined for Teensy LC, define it here as a 0
#if defined(ADC_TEENSY_LC)
const static uint8_t A13 = 0;
#endif

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
#define ADC_REF_DEFAULT    0
#define ADC_REF_ALT        1
#if defined(ADC_TEENSY_3_0) || defined(ADC_TEENSY_3_1)
// default is the external, that is connected to the 3.3V supply.
// To use the external simply connect AREF to a different voltage
// Internal is connected to the 1.2 V ref.
        #define ADC_REF_3V3    ADC_REF_DEFAULT
        #define ADC_REF_1V2    ADC_REF_ALT
        #define ADC_REF_EXT    ADC_REF_DEFAULT

#elif defined(ADC_TEENSY_LC)
// default is the internal ref, 3.3 V
// the external is AREF
        #define ADC_REF_3V3    ADC_REF_ALT
        #define ADC_REF_EXT    ADC_REF_DEFAULT
#endif


// Error codes for analogRead and analogReadDifferential
#define ADC_ERROR_DIFF_VALUE -70000
#define ADC_ERROR_VALUE ADC_ERROR_DIFF_VALUE

// Error flag masks.
// Possible errors are: other, calibration, wrong pin, analogRead, analogDifferentialRead, continuous, continuousDifferential
// To globaly disable an error simply change (1<<x) to (0<<x), revert to enable the error again.
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
#define ADC_ERROR_WRONG_ADC         (0<<8)
#define ADC_ERROR_SYNCH             (1<<9)


// Other things to measure with the ADC that don't use external pins
// In my Teensy I read 1.22 V for the ADC_VREF_OUT (doesn't exist in Teensy LC), random values for ADC_BANDGAP,
// 3.3 V for ADC_VREFH and 0.0 V for ADC_VREFL.
#define ADC_TEMP_SENSOR     38 // 0.719 V at 25ºC and slope of 1.715 mV/ºC for Teensy 3.x and 0.716 V, 1.62 mV/ºC for Teensy LC
#define ADC_VREF_OUT        39
#define ADC_BANDGAP         41
#define ADC_VREFH           42
#define ADC_VREFL           43


// debug mode: blink the led light
#define ADC_debug 0


// defines for the bit position in the registers, this makes it easy in case they change in different boards
#define ADC_SC1A_COCO_BIT (7)
#define ADC_SC1A_AIEN_BIT (6)
#define ADC_SC1_DIFF_BIT (5)

#define ADC_CFG1_ADLPC_BIT (7)
#define ADC_CFG1_ADIV1_BIT (6)
#define ADC_CFG1_ADIV0_BIT (5)
#define ADC_CFG1_ADLSMP_BIT (4)
#define ADC_CFG1_MODE1_BIT (3)
#define ADC_CFG1_MODE0_BIT (2)
#define ADC_CFG1_ADICLK1_BIT (1)
#define ADC_CFG1_ADICLK0_BIT (0)

#define ADC_CFG2_MUXSEL_BIT (4)
#define ADC_CFG2_ADACKEN_BIT (3)
#define ADC_CFG2_ADHSC_BIT (2)
#define ADC_CFG2_ADLSTS1_BIT (1)
#define ADC_CFG2_ADLSTS0_BIT (0)

#define ADC_SC2_ADACT_BIT (7)
#define ADC_SC2_ACFE_BIT (5)
#define ADC_SC2_ACFGT_BIT (4)
#define ADC_SC2_ACREN_BIT (3)
#define ADC_SC2_DMAEN_BIT (2)
#define ADC_SC2_REFSEL0_BIT (0)

#define ADC_SC3_CAL_BIT (7)
#define ADC_SC3_CALF_BIT (6)
#define ADC_SC3_ADCO_BIT (3)
#define ADC_SC3_AVGE_BIT (2)
#define ADC_SC3_AVGS1_BIT (1)
#define ADC_SC3_AVGS0_BIT (0)

#define ADC_PGA_PGAEN_BIT (23)

/** Class ADC_Module: Implements all functions of the Teensy 3.x analog to digital converter
*
*/
class ADC_Module
{

    public:

        /** Constructor
        *
        *   Pass the ADC number and the Channel number to SC1A number array.
        */
        ADC_Module(uint8_t ADC_number, const uint8_t *a_channel2sc1a);


        //! Starts the calibration sequence, waits until it's done and writes the results
        /** Usually it's not necessary to call this function directly, but do it if the "enviroment" changed
        *   significantly since the program was started.
        */
        void recalibrate();

        //! Starts the calibration sequence
        void calibrate();

        //! Waits until calibration is finished and writes the corresponding registers
        void wait_for_cal();


        /////////////// METHODS TO SET/GET SETTINGS OF THE ADC ////////////////////

        //! Set the voltage reference you prefer, default is vcc
        /*!
        * \param type can be ADC_REF_3V3, ADC_REF_1V2 (not for Teensy LC) or ADC_REF_EXT
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

        //! Returns the maximum value for a measurement: 2^res-1.
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
        *   \param gain can be 1, 2, 4, 8, 16, 32 or 64
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
        bool isConverting() {
            //return (*ADC_SC2_adact);
            return getBit(ADC_SC2, ADC_SC2_ADACT_BIT);
            //return ((*ADC_SC2) & ADC_SC2_ADACT) >> 7;
        }

        //! Is an ADC conversion ready?
        /**
        *  \return 1 if yes, 0 if not.
        *  When a value is read this function returns 0 until a new value exists
        *  So it only makes sense to call it before analogReadContinuous() or readSingle()
        */
        bool isComplete() {
            //return (*ADC_SC1A_coco);
            return getBit(ADC_SC1A, ADC_SC1A_COCO_BIT);
            //return ((*ADC_SC1A) & ADC_SC1_COCO) >> 7;
        }

        //! Is the ADC in differential mode?
        bool isDifferential() {
            //return ((*ADC_SC1A) & ADC_SC1_DIFF) >> 5;
            return getBit(ADC_SC1A, ADC_SC1_DIFF_BIT);
        }

        //! Is the ADC in continuous mode?
        bool isContinuous() {
            //return (*ADC_SC3_adco);
            return getBit(ADC_SC3, ADC_SC3_ADCO_BIT);
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
        int readSingle(){ return analogReadContinuous(); }


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
        int analogReadContinuous() {
            return (int16_t)(int32_t)*ADC_RA;
        }

        //! Stops continuous conversion
        void stopContinuous();


        //////// OTHER STUFF ///////////

        // struct to store the config of the adc
        struct ADC_Config{
            uint32_t savedSC1A, savedSC2, savedSC3, savedCFG1, savedCFG2;
        };

        ADC_Config adc_config; // store ADC config
        uint8_t adcWasInUse; // was the adc in use before an analog timer call?

        // save config of the ADC
        void saveConfig(ADC_Config* config) {
                config->savedSC1A = *ADC_SC1A;
                config->savedCFG1 = *ADC_CFG1;
                config->savedCFG2 = *ADC_CFG2;
                config->savedSC2 = *ADC_SC2;
                config->savedSC3 = *ADC_SC3;
        }

        // load config to the ADC
        void loadConfig(ADC_Config* config) {
                *ADC_CFG1 = config->savedCFG1;
                *ADC_CFG2 = config->savedCFG2;
                *ADC_SC2 = config->savedSC2;
                *ADC_SC3 = config->savedSC3;
                *ADC_SC1A = config->savedSC1A; // restore last
        }


        // number of measurements that the ADC is performing
        uint8_t num_measurements = 0;


        // translate pin number to SC1A nomenclature
        const uint8_t* channel2sc1a;


        // This flag indicates that some kind of error took place
        // Use the defines at the beggining of this file to find out what caused the fail.
        uint16_t fail_flag;

    private:

        // is set to 1 when the calibration procedure is taking place
        volatile uint8_t calibrating;

        // the first calibration will use 32 averages and lowest speed,
        // when this calibration is over the averages and speed will be set to default.
        volatile uint8_t init_calib;

        // resolution
        volatile uint8_t analog_res_bits;

        // maximum value possible 2^res-1
        volatile uint32_t analog_max_val;

        // num of averages
        volatile uint8_t analog_num_average;

        // reference can be internal or external
        volatile uint8_t analog_reference_internal;

        // are interrupts enabled?
        volatile uint8_t var_enableInterrupts;

        // value of the pga
        volatile uint8_t pga_value;

        // conversion speed
        volatile uint8_t conversion_speed;

        // sampling speed
        volatile uint8_t sampling_speed;


        // which adc is this?
        uint8_t ADC_num;


        //! Initialize ADC
        void analog_init();


         /////// Atomic bit set/clear
        /* Clear bit in address (make it zero), set bit (make it one) or return the value of that bit
        *   We can change this functions depending on the board.
        *   Teensy 3.x use bitband while Teensy LC has a more advanced bit manipulation engine.
        */
        #if defined(ADC_TEENSY_3_1) || defined(ADC_TEENSY_3_0)
        // bitband
        #define ADC_BITBAND_ADDR(reg, bit) (((uint32_t)(reg) - 0x40000000) * 32 + (bit) * 4 + 0x42000000)

        void setBit(volatile uint32_t* reg, uint8_t bit) {
                (*(uint32_t *)ADC_BITBAND_ADDR((reg), (bit))) = 1;
        }
        void clearBit(volatile uint32_t* reg, uint8_t bit) {
                (*(uint32_t *)ADC_BITBAND_ADDR((reg), (bit))) = 0;
        }

        void changeBit(volatile uint32_t* reg, uint8_t bit, bool state) {
                (*(uint32_t *)ADC_BITBAND_ADDR((reg), (bit))) = state;
        }

        volatile bool getBit(volatile uint32_t* reg, uint8_t bit) {
                return (volatile bool)*(uint32_t*)(ADC_BITBAND_ADDR(reg, bit));
        }

        #elif defined(ADC_TEENSY_LC)
        // bit manipulation engine
        //#define ADC_SETBIT_ATOMIC(reg, bit) (*(uint32_t *)(((uint32_t)&(reg) - 0xF8000000) | 0x48000000) = 1 << (bit)) // OR
        //#define ADC_CLRBIT_ATOMIC(reg, bit) (*(uint32_t *)(((uint32_t)&(reg) - 0xF8000000) | 0x44000000) = ~(1 << (bit))) // XOR

        void setBit(volatile uint32_t* reg, uint8_t bit) {
                //temp = *(uint32_t *)((uint32_t)(reg) | (1<<26) | (bit<<21)); // LAS
                *(volatile uint32_t*)((uint32_t)(reg) | (1<<27)) = 1<<bit; // OR
        }
        void clearBit(volatile uint32_t* reg, uint8_t bit) {
                //temp = *(uint32_t *)((uint32_t)(reg) | (3<<27) | (bit<<21)); // LAC
                *(volatile uint32_t*)((uint32_t)(reg) | (1<<26)) = ~(1<<bit); // AND
        }

        void changeBit(volatile uint32_t* reg, uint8_t bit, bool state) {
                //temp = *(uint32_t *)((uint32_t)(reg) | ((3-2*!!state)<<27) | (bit<<21)); // LAS/LAC
                if(state) { // set
                        *(volatile uint32_t*)((uint32_t)(reg) | (1<<27)) = 1<<bit; // OR
                } else { // clear
                        *(volatile uint32_t*)((uint32_t)(reg) | (1<<26)) = ~(1<<bit); // AND
                }

        }

        volatile bool getBit(volatile uint32_t* reg, uint8_t bit) {
                return (volatile bool)*(uint32_t *)((uint32_t)(reg) | (1<<28) | (bit<<23) ); // UBFX
        }

        #endif

        // registers point to the correct ADC module
        typedef volatile uint32_t* reg;

        // registers that control the adc module
        reg ADC_SC1A; //reg ADC_SC1A_aien; reg ADC_SC1A_coco;
        reg ADC_SC1B;

        reg ADC_CFG1; //reg ADC_CFG1_adlpc; reg ADC_CFG1_adiv0, ADC_CFG1_adiv1; reg ADC_CFG1_adlsmp;
                      //  reg ADC_CFG1_mode0, ADC_CFG1_mode1; reg ADC_CFG1_adiclk0, ADC_CFG1_adiclk1;
        reg ADC_CFG2; //reg ADC_CFG2_muxsel; reg ADC_CFG2_adacken; reg ADC_CFG2_adhsc; reg ADC_CFG2_adlsts1, ADC_CFG2_adlsts0;

        reg ADC_RA;
        reg ADC_RB;

        reg ADC_CV1;
        reg ADC_CV2;

        reg ADC_SC2; //reg ADC_SC2_adact; reg ADC_SC2_ref; reg ADC_SC2_dma; reg ADC_SC2_cfe; reg ADC_SC2_cfgt; reg ADC_SC2_cren;
        reg ADC_SC3; //reg ADC_SC3_cal, ADC_SC3_calf; reg ADC_SC3_avge; reg ADC_SC3_avgs0, ADC_SC3_avgs1; reg ADC_SC3_adco;

        reg ADC_PGA; //reg ADC_PGA_pgaen;

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
