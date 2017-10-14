/* Teensy 3.x, LC ADC library
 * https://github.com/pedvide/ADC
 * Copyright (c) 2017 Pedro Villanueva
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
#elif defined(__MK64FX512__) // Teensy 3.5
#define ADC_TEENSY_3_5
#elif defined(__MK66FX1M0__) // Teensy 3.6
#define ADC_TEENSY_3_6
#else
#error "Board not supported!"
#endif



// Teensy 3.1 has 2 ADCs, Teensy 3.0 and LC only 1.
#if defined(ADC_TEENSY_3_1) // Teensy 3.1
        #define ADC_NUM_ADCS (2)
#elif defined(ADC_TEENSY_3_0) // Teensy 3.0
        #define ADC_NUM_ADCS (1)
#elif defined(ADC_TEENSY_LC) // Teensy LC
        #define ADC_NUM_ADCS (1)
#elif defined(ADC_TEENSY_3_5) // Teensy 3.5
        #define ADC_NUM_ADCS (2)
#elif defined(ADC_TEENSY_3_6) // Teensy 3.6
        #define ADC_NUM_ADCS (2)
#endif

// Use DMA?
#if defined(ADC_TEENSY_3_1) // Teensy 3.1
        #define ADC_USE_DMA (1)
#elif defined(ADC_TEENSY_3_0) // Teensy 3.0
        #define ADC_USE_DMA (1)
#elif defined(ADC_TEENSY_LC) // Teensy LC
        #define ADC_USE_DMA (1)
#elif defined(ADC_TEENSY_3_5) // Teensy 3.5
        #define ADC_USE_DMA (1)
#elif defined(ADC_TEENSY_3_6) // Teensy 3.6
        #define ADC_USE_DMA (1)
#endif

// Use PGA?
#if defined(ADC_TEENSY_3_1) // Teensy 3.1
        #define ADC_USE_PGA (1)
#elif defined(ADC_TEENSY_3_0) // Teensy 3.0
        #define ADC_USE_PGA (0)
#elif defined(ADC_TEENSY_LC) // Teensy LC
        #define ADC_USE_PGA (0)
#elif defined(ADC_TEENSY_3_5) // Teensy 3.5
        #define ADC_USE_PGA (0)
#elif defined(ADC_TEENSY_3_6) // Teensy 3.6
        #define ADC_USE_PGA (0)
#endif

// Use PDB?
#if defined(ADC_TEENSY_3_1) // Teensy 3.1
        #define ADC_USE_PDB (1)
#elif defined(ADC_TEENSY_3_0) // Teensy 3.0
        #define ADC_USE_PDB (1)
#elif defined(ADC_TEENSY_LC) // Teensy LC
        #define ADC_USE_PDB (0)
#elif defined(ADC_TEENSY_3_5) // Teensy 3.5
        #define ADC_USE_PDB (1)
#elif defined(ADC_TEENSY_3_6) // Teensy 3.6
        #define ADC_USE_PDB (1)
#endif

// Has internal reference?
#if defined(ADC_TEENSY_3_1) // Teensy 3.1
        #define ADC_USE_INTERNAL (1)
#elif defined(ADC_TEENSY_3_0) // Teensy 3.0
        #define ADC_USE_INTERNAL (1)
#elif defined(ADC_TEENSY_LC) // Teensy LC
        #define ADC_USE_INTERNAL (0)
#elif defined(ADC_TEENSY_3_5) // Teensy 3.5
        #define ADC_USE_INTERNAL (1)
#elif defined(ADC_TEENSY_3_6) // Teensy 3.6
        #define ADC_USE_INTERNAL (1)
#endif

// Select the voltage reference sources for ADC. This is an internal setting, do not use
enum class ADC_REF_SOURCE : uint8_t {REF_DEFAULT = 0, REF_ALT = 1, REF_NONE = 2}; // internal, do not use
#if defined(ADC_TEENSY_3_0) || defined(ADC_TEENSY_3_1) || defined(ADC_TEENSY_3_5) || defined(ADC_TEENSY_3_6)
// default is the external, that is connected to the 3.3V supply.
// To use the external simply connect AREF to a different voltage
// alt is connected to the 1.2 V ref.
/*! \file */
/*! Reference for the ADC */
enum class ADC_REFERENCE : uint8_t {
    REF_3V3 = ADC_REF_SOURCE::REF_DEFAULT, /*!< 3.3 volts */
    REF_1V2 = ADC_REF_SOURCE::REF_ALT, /*!< 1.2 volts */
    REF_EXT = ADC_REF_SOURCE::REF_DEFAULT, /*!< External VREF */
    NONE = ADC_REF_SOURCE::REF_NONE // internal, do not use
};
#elif defined(ADC_TEENSY_LC)
// alt is the internal ref, 3.3 V
// the default is AREF
/*! \file */
/*! Reference for the ADC */
enum class ADC_REFERENCE : uint8_t {
    REF_3V3 = ADC_REF_SOURCE::REF_ALT, /*!< 3.3 volts */
    REF_EXT = ADC_REF_SOURCE::REF_DEFAULT, /*!< External VREF */
    NONE = ADC_REF_SOURCE::REF_NONE // internal, do not use
};
#endif

// max number of pins, size of channel2sc1aADCx
#if defined(ADC_TEENSY_3_1) // Teensy 3.1
        #define ADC_MAX_PIN (43)
#elif defined(ADC_TEENSY_3_0) // Teensy 3.0
        #define ADC_MAX_PIN (43)
#elif defined(ADC_TEENSY_LC) // Teensy LC
        #define ADC_MAX_PIN (43)
#elif defined(ADC_TEENSY_3_5) // Teensy 3.5
        #define ADC_MAX_PIN (69)
#elif defined(ADC_TEENSY_3_6) // Teensy 3.6
        #define ADC_MAX_PIN (69)
#endif


// number of differential pairs PER ADC!!
#if defined(ADC_TEENSY_3_1) // Teensy 3.1
        #define ADC_DIFF_PAIRS (2) // normal and with PGA
#elif defined(ADC_TEENSY_3_0) // Teensy 3.0
        #define ADC_DIFF_PAIRS (2)
#elif defined(ADC_TEENSY_LC) // Teensy LC
        #define ADC_DIFF_PAIRS (1)
#elif defined(ADC_TEENSY_3_5) // Teensy 3.5
        #define ADC_DIFF_PAIRS (1)
#elif defined(ADC_TEENSY_3_6) // Teensy 3.6
        #define ADC_DIFF_PAIRS (1)
#endif


// Other things to measure with the ADC that don't use external pins
// In my Teensy I read 1.22 V for the ADC_VREF_OUT (doesn't exist in Teensy LC), random values for ADC_BANDGAP,
// 3.3 V for ADC_VREFH and 0.0 V for ADC_VREFL.
#if defined(ADC_TEENSY_3_1) || defined(ADC_TEENSY_3_0) || defined(ADC_TEENSY_LC)
    /*! Other ADC sources to measure, such as the temperature sensor.
    */
    enum class ADC_INTERNAL_SOURCE : uint8_t{
        TEMP_SENSOR = 38, /*!< Temperature sensor. */ // 0.719 V at 25ºC and slope of 1.715 mV/ºC for Teensy 3.x and 0.716 V, 1.62 mV/ºC for Teensy LC
        VREF_OUT = 39, /*!< 1.2 V reference */
        BANDGAP = 41, /*!< BANDGAP */
        VREFH = 42, /*!< High VREF */
        VREFL = 43, /*!< Low VREF. */
    };
#elif defined(ADC_TEENSY_3_5) || defined(ADC_TEENSY_3_6)
    /*! Other ADC sources to measure, such as the temperature sensor.
    */
    enum class ADC_INTERNAL_SOURCE : uint8_t{
        TEMP_SENSOR = 24, /*!< Temperature sensor. */ // 0.719 V at 25ºC and slope of 1.715 mV/ºC for Teensy 3.x and 0.716 V, 1.62 mV/ºC for Teensy LC
        VREF_OUT = 28, /*!< 1.2 V reference */ // only on ADC1
        BANDGAP = 25, /*!< BANDGAP */
        VREFH = 26, /*!< High VREF */
        VREFL = 27, /*!< Low VREF. */
    };
#endif

/* MK20DX256 Datasheet:
The 16-bit accuracy specifications listed in Table 24 and Table 25 are achievable on the
differential pins ADCx_DP0, ADCx_DM0
All other ADC channels meet the 13-bit differential/12-bit single-ended accuracy
specifications.

The results in this data sheet were derived from a system which has < 8 Ohm analog source resistance. The RAS/CAS
time constant should be kept to < 1ns.

ADC clock should be 2 to 12 MHz for 16 bit mode
ADC clock should be 1 to 18 MHz for 8-12 bit mode, and 1-24 MHz for Teensy 3.6 (NOT 3.5)
To use the maximum ADC conversion clock frequency, the ADHSC bit must be set and the ADLPC bit must be clear

The ADHSC bit is used to configure a higher clock input frequency. This will allow
faster overall conversion times. To meet internal ADC timing requirements, the ADHSC
bit adds additional ADCK cycles. Conversions with ADHSC = 1 take two more ADCK
cycles. ADHSC should be used when the ADCLK exceeds the limit for ADHSC = 0.

*/
// the alternate clock is connected to OSCERCLK (16 MHz).
// datasheet says ADC clock should be 2 to 12 MHz for 16 bit mode
// datasheet says ADC clock should be 1 to 18 MHz for 8-12 bit mode, and 1-24 MHz for Teensy 3.6 (NOT 3.5)
// calibration works best when averages are 32 and speed is less than 4 MHz
// ADC_CFG1_ADICLK: 0=bus, 1=bus/2, 2=(alternative clk) altclk, 3=(async. clk) adack
// See below for an explanation of VERY_LOW_SPEED, LOW_SPEED, etc.
#if F_BUS == 120000000
    #define ADC_CFG1_7_5MHZ        (ADC_CFG1_ADIV(3) + ADC_CFG1_ADICLK(1))
    #define ADC_CFG1_15MHZ         (ADC_CFG1_ADIV(2) + ADC_CFG1_ADICLK(1))
    #define ADC_CFG1_30MHZ         (ADC_CFG1_ADIV(1) + ADC_CFG1_ADICLK(1))
    #define ADC_CFG1_60MHZ         (ADC_CFG1_ADIV(0) + ADC_CFG1_ADICLK(1)) // too fast

    #define ADC_CFG1_VERY_LOW_SPEED ADC_CFG1_LOW_SPEED
    #define ADC_CFG1_LOW_SPEED (ADC_CFG1_7_5MHZ)
    #define ADC_CFG1_MED_SPEED (ADC_CFG1_7_5MHZ)
    #define ADC_CFG1_HI_SPEED_16_BITS (ADC_CFG1_7_5MHZ)
    #define ADC_CFG1_HI_SPEED (ADC_CFG1_15MHZ)
    #define ADC_CFG1_VERY_HIGH_SPEED (ADC_CFG1_15MHZ)
#elif F_BUS == 108000000
    #define ADC_CFG1_6_75MHZ        (ADC_CFG1_ADIV(3) + ADC_CFG1_ADICLK(1))
    #define ADC_CFG1_13_5MHZ        (ADC_CFG1_ADIV(2) + ADC_CFG1_ADICLK(1))
    #define ADC_CFG1_27MHZ          (ADC_CFG1_ADIV(1) + ADC_CFG1_ADICLK(1))
    #define ADC_CFG1_54MHZ          (ADC_CFG1_ADIV(0) + ADC_CFG1_ADICLK(1)) // too fast

    #define ADC_CFG1_VERY_LOW_SPEED ADC_CFG1_LOW_SPEED
    #define ADC_CFG1_LOW_SPEED (ADC_CFG1_6_75MHZ)
    #define ADC_CFG1_MED_SPEED (ADC_CFG1_6_75MHZ)
    #define ADC_CFG1_HI_SPEED_16_BITS (ADC_CFG1_6_75MHZ)
    #define ADC_CFG1_HI_SPEED (ADC_CFG1_13_5MHZ)
    #define ADC_CFG1_VERY_HIGH_SPEED (ADC_CFG1_27MHZ)
#elif F_BUS == 60000000
    #define ADC_CFG1_3_75MHZ      (ADC_CFG1_ADIV(3) + ADC_CFG1_ADICLK(1))
    #define ADC_CFG1_7_5MHZ       (ADC_CFG1_ADIV(2) + ADC_CFG1_ADICLK(1))
    #define ADC_CFG1_15MHZ        (ADC_CFG1_ADIV(1) + ADC_CFG1_ADICLK(1))

    #define ADC_CFG1_VERY_LOW_SPEED ADC_CFG1_LOW_SPEED
    #define ADC_CFG1_LOW_SPEED (ADC_CFG1_3_75MHZ)
    #define ADC_CFG1_MED_SPEED (ADC_CFG1_7_5MHZ)
    #define ADC_CFG1_HI_SPEED_16_BITS (ADC_CFG1_7_5MHZ)
    #define ADC_CFG1_HI_SPEED (ADC_CFG1_15MHZ)
    #define ADC_CFG1_VERY_HIGH_SPEED ADC_CFG1_HI_SPEED

#elif F_BUS == 56000000 || F_BUS == 54000000 // frequency numbers are good for 56 MHz and slightly smaller for 54 MHz
    #define ADC_CFG1_3_5MHZ   (ADC_CFG1_ADIV(3) + ADC_CFG1_ADICLK(1))
    #define ADC_CFG1_7MHZ     (ADC_CFG1_ADIV(2) + ADC_CFG1_ADICLK(1))
    #define ADC_CFG1_14MHZ    (ADC_CFG1_ADIV(1) + ADC_CFG1_ADICLK(1))
    #define ADC_CFG1_28MHZ    (ADC_CFG1_ADIV(0) + ADC_CFG1_ADICLK(1))  // too fast

    #define ADC_CFG1_VERY_LOW_SPEED ADC_CFG1_LOW_SPEED
    #define ADC_CFG1_LOW_SPEED (ADC_CFG1_3_5MHZ)
    #define ADC_CFG1_MED_SPEED (ADC_CFG1_7MHZ)
    #define ADC_CFG1_HI_SPEED_16_BITS (ADC_CFG1_7MHZ)
    #define ADC_CFG1_HI_SPEED (ADC_CFG1_14MHZ)
    #define ADC_CFG1_VERY_HIGH_SPEED (ADC_CFG1_28MHZ)

#elif F_BUS == 48000000
    #define ADC_CFG1_3MHZ   (ADC_CFG1_ADIV(3) + ADC_CFG1_ADICLK(1)) // Clock divide select: 3=div8 + Input clock: 1=bus/2
    #define ADC_CFG1_6MHZ   (ADC_CFG1_ADIV(2) + ADC_CFG1_ADICLK(1)) // Clock divide select: 2=div4 + Input clock: 1=bus/2
    #define ADC_CFG1_12MHZ  (ADC_CFG1_ADIV(1) + ADC_CFG1_ADICLK(1)) // Clock divide select: 1=div2 Input clock: 1=bus/2
    #define ADC_CFG1_24MHZ  (ADC_CFG1_ADIV(0) + ADC_CFG1_ADICLK(1)) // this is way too fast, so accuracy is not guaranteed, except for T3.6

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
    #define ADC_CFG1_20MHZ    (ADC_CFG1_ADIV(0) + ADC_CFG1_ADICLK(1)) // too fast, except for T3.5

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

    #define ADC_CFG1_VERY_LOW_SPEED (ADC_CFG1_2_25MHZ)
    #define ADC_CFG1_LOW_SPEED (ADC_CFG1_4_5MHZ)
    #define ADC_CFG1_MED_SPEED (ADC_CFG1_9MHZ)
    #define ADC_CFG1_HI_SPEED_16_BITS (ADC_CFG1_9MHZ)
    #define ADC_CFG1_HI_SPEED (ADC_CFG1_18MHZ)
    #define ADC_CFG1_VERY_HIGH_SPEED ADC_CFG1_HI_SPEED

#elif F_BUS == 24000000
    #define ADC_CFG1_1_5MHZ   (ADC_CFG1_ADIV(3) + ADC_CFG1_ADICLK(1)) // Clock divide select: 3=div8 + Input clock: 1=bus/2
    #define ADC_CFG1_3MHZ     (ADC_CFG1_ADIV(3) + ADC_CFG1_ADICLK(0)) // Clock divide select: 3=div8 + Input clock: 0=bus
    #define ADC_CFG1_6MHZ     (ADC_CFG1_ADIV(2) + ADC_CFG1_ADICLK(0)) // Clock divide select: 2=div4 + Input clock: 0=bus
    #define ADC_CFG1_12MHZ    (ADC_CFG1_ADIV(1) + ADC_CFG1_ADICLK(0)) // Clock divide select: 1=div2 + Input clock: 0=bus
    #define ADC_CFG1_24MHZ    (ADC_CFG1_ADIV(0) + ADC_CFG1_ADICLK(0)) // too fast, except for T3.5

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
#error "F_BUS must be 108, 60, 56, 54, 48, 40, 36, 24, 4 or 2 MHz"
#endif

// mask the important bit in each register
#define ADC_CFG1_ADICLK_MASK_1 (1<<1)
#define ADC_CFG1_ADICLK_MASK_0 (1<<0)

#define ADC_CFG1_ADIV_MASK_1 (1<<6)
#define ADC_CFG1_ADIV_MASK_0 (1<<5)

// Settings for the power/speed of conversions/sampling
/*! ADC conversion speed.
*   Common set of options to select the ADC clock speed F_ADCK, which depends on F_BUS, except for the ADACK_X_Y options that are independent.
*   This selection affects the sampling speed too.
*   Note: the F_ADCK speed is not equal to the conversion speed; any measurement takes several F_ADCK cycles to complete including the sampling and conversion steps.
*/
enum class ADC_CONVERSION_SPEED : uint8_t {
    VERY_LOW_SPEED, /*!< is guaranteed to be the lowest possible speed within specs for resolutions less than 16 bits (higher than 1 MHz). */
    LOW_SPEED, /*!< is guaranteed to be the lowest possible speed within specs for all resolutions (higher than 2 MHz). */
    MED_SPEED, /*!< is always >= LOW_SPEED and <= HIGH_SPEED. */
    HIGH_SPEED_16BITS, /*!< is guaranteed to be the highest possible speed within specs for all resolutions (lower than or equal to 12 MHz). */
    HIGH_SPEED, /*!< is guaranteed to be the highest possible speed within specs for resolutions less than 16 bits (lower than or equal to 18 MHz),
                            except for Teensy 3.6 (NOT 3.5), for which the maximum is 24 MHz. */
    VERY_HIGH_SPEED, /*!< may be out of specs */

    ADACK_2_4, /*!< 2.4 MHz asynchronous ADC clock (independent of the global clocks F_CPU or F_BUS) */
    ADACK_4_0, /*!< 4.0 MHz asynchronous ADC clock (independent of the global clocks F_CPU or F_BUS) */
    ADACK_5_2, /*!< 5.2 MHz asynchronous ADC clock (independent of the global clocks F_CPU or F_BUS) */
    ADACK_6_2 /*!< 6.2 MHz asynchronous ADC clock (independent of the global clocks F_CPU or F_BUS) */
};
/*! ADC sampling speed.
*   It selects how many ADCK clock cycles to add.
*/
enum class ADC_SAMPLING_SPEED : uint8_t {
    VERY_LOW_SPEED, /*!< is the lowest possible sampling speed (+24 ADCK). */
    LOW_SPEED, /*!< adds +16 ADCK. */
    MED_SPEED, /*!< adds +10 ADCK. */
    HIGH_SPEED, /*!< adds +6 ADCK. */
    VERY_HIGH_SPEED, /*!< is the highest possible sampling speed (0 ADCK added). */
};



// Mask for the channel selection in ADCx_SC1A,
// useful if you want to get the channel number from ADCx_SC1A
#define ADC_SC1A_CHANNELS (0x1F)
// 0x1F=31 in the channel2sc1aADCx means the pin doesn't belong to the ADC module
#define ADC_SC1A_PIN_INVALID (0x1F)
// Muxsel mask, pins in channel2sc1aADCx with bit 7 set use mux A.
#define ADC_SC1A_PIN_MUX (0x80)
// Differential pin mask, pins in channel2sc1aADCx with bit 6 set are differential pins.
#define ADC_SC1A_PIN_DIFF (0x40)
// PGA mask. The pins can use PGA on that ADC
#define ADC_SC1A_PIN_PGA (0x80)


// Error codes for analogRead and analogReadDifferential
#define ADC_ERROR_DIFF_VALUE (-70000)
#define ADC_ERROR_VALUE ADC_ERROR_DIFF_VALUE

// Error flag masks.
// Possible errors are: other, calibration, wrong pin, analogRead, analogDifferentialRead, continuous, continuousDifferential
// To globalLy disable an error simply change (1<<x) to (0<<x), revert to enable the error again.
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
#define ADC_SC2_ADTRG_BIT (6)
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


/** Class ADC_Module: Implements all functions of the Teensy 3.x, LC analog to digital converter
*
*/
class ADC_Module {

public:

    //! Dictionary with the differential pins as keys and the SC1A number as values
    /** Internal, do not use.
    */
    struct ADC_NLIST {
        //! Pin and corresponding SC1A value.
        uint8_t pin, sc1a;
    };

    //! Constructor
    /** Pass the ADC number and the Channel number to SC1A number arrays.
    *   \param ADC_number Number of the ADC module, from 0.
    *   \param a_channel2sc1a contains an index that pairs each pin to its SC1A number (used to start a conversion on that pin)
    *   \param a_diff_table is similar to a_channel2sc1a, but for differential pins.
    */
    ADC_Module(uint8_t ADC_number, const uint8_t* const a_channel2sc1a, const ADC_NLIST* const a_diff_table);


    //! Starts the calibration sequence, waits until it's done and writes the results
    /** Usually it's not necessary to call this function directly, but do it if the "environment" changed
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
    * \param ref_type can be ADC_REFERENCE::REF_3V3, ADC_REFERENCE::REF_1V2 (not for Teensy LC) or ADC_REFERENCE::REF_EXT
    *
    *  It recalibrates at the end.
    */
    void setReference(ADC_REFERENCE ref_type);


    //! Change the resolution of the measurement.
    /*!
    *  \param bits is the number of bits of resolution.
    *  For single-ended measurements: 8, 10, 12 or 16 bits.
    *  For differential measurements: 9, 11, 13 or 16 bits.
    *  If you want something in between (11 bits single-ended for example) select the immediate higher
    *  and shift the result one to the right.
    *
    *  Whenever you change the resolution, change also the comparison values (if you use them).
    */
    void setResolution(uint8_t bits);

    //! Returns the resolution of the ADC_Module.
    /**
    *   \return the resolution of the ADC_Module.
    */
    uint8_t getResolution();

    //! Returns the maximum value for a measurement: 2^res-1.
    /**
    *   \return the maximum value for a measurement: 2^res-1.
    */
    uint32_t getMaxValue();


    //! Sets the conversion speed (changes the ADC clock, ADCK)
    /**
    * \param speed can be any from the ADC_CONVERSION_SPEED enum: VERY_LOW_SPEED, LOW_SPEED, MED_SPEED, HIGH_SPEED_16BITS, HIGH_SPEED, VERY_HIGH_SPEED,
    *       ADACK_2_4, ADACK_4_0, ADACK_5_2 or ADACK_6_2.
    *
    * VERY_LOW_SPEED is guaranteed to be the lowest possible speed within specs for resolutions less than 16 bits (higher than 1 MHz),
    * it's different from LOW_SPEED only for 24, 4 or 2 MHz bus frequency.
    * LOW_SPEED is guaranteed to be the lowest possible speed within specs for all resolutions (higher than 2 MHz).
    * MED_SPEED is always >= LOW_SPEED and <= HIGH_SPEED.
    * HIGH_SPEED_16BITS is guaranteed to be the highest possible speed within specs for all resolutions (lower or eq than 12 MHz).
    * HIGH_SPEED is guaranteed to be the highest possible speed within specs for resolutions less than 16 bits (lower or eq than 18 MHz).
    * VERY_HIGH_SPEED may be out of specs, it's different from HIGH_SPEED only for 48, 40 or 24 MHz bus frequency.
    *
    * Additionally the conversion speed can also be ADACK_2_4, ADACK_4_0, ADACK_5_2 and ADACK_6_2,
    * where the numbers are the frequency of the ADC clock (ADCK) in MHz and are independent on the bus speed.
    * This is useful if you are using the Teensy at a very low clock frequency but want faster conversions,
    * but if F_BUS<F_ADCK, you can't use VERY_HIGH_SPEED for sampling speed.
    */
    void setConversionSpeed(ADC_CONVERSION_SPEED speed);


    //! Sets the sampling speed
    /** Increase the sampling speed for low impedance sources, decrease it for higher impedance ones.
    * \param speed can be any of the ADC_SAMPLING_SPEED enum: VERY_LOW_SPEED, LOW_SPEED, MED_SPEED, HIGH_SPEED or VERY_HIGH_SPEED.
    *
    * VERY_LOW_SPEED is the lowest possible sampling speed (+24 ADCK).
    * LOW_SPEED adds +16 ADCK.
    * MED_SPEED adds +10 ADCK.
    * HIGH_SPEED adds +6 ADCK.
    * VERY_HIGH_SPEED is the highest possible sampling speed (0 ADCK added).
    */
    void setSamplingSpeed(ADC_SAMPLING_SPEED speed);


    //! Set the number of averages
    /*!
    * \param num can be 0, 4, 8, 16 or 32.
    *
    *  It doesn't recalibrate at the end.
    */
    void setAveraging(uint8_t num);


    //! Enable interrupts
    /** An IRQ_ADCx Interrupt will be raised when the conversion is completed
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
    *   \param compValue value to compare
    *   \param greaterThan true or false
    */
    void enableCompare(int16_t compValue, bool greaterThan);

    //! Enable the compare function to a range
    /** A conversion will be completed only when the ADC value is inside (insideRange=1) or outside (=0)
    *  the range given by (lowerLimit, upperLimit),including (inclusive=1) the limits or not (inclusive=0).
    *  See Table 31-78, p. 617 of the freescale manual.
    *  Call it after changing the resolution
    *  Use with interrupts or poll conversion completion with isComplete()
    *   \param lowerLimit lower value to compare
    *   \param upperLimit upper value to compare
    *   \param insideRange true or false
    *   \param inclusive true or false
    */
    void enableCompareRange(int16_t lowerLimit, int16_t upperLimit, bool insideRange, bool inclusive);

    //! Disable the compare function
    void disableCompare();


    //! Enable and set PGA
    /** Enables the PGA and sets the gain
    *   Use only for signals lower than 1.2 V and only in differential mode
    *   \param gain can be 1, 2, 4, 8, 16, 32 or 64
    */
    void enablePGA(uint8_t gain);

    //! Returns the PGA level
    /**
    *   \return PGA level from 1 to 64
    */
    uint8_t getPGA();

    //! Disable PGA
    void disablePGA();


    //! Set continuous conversion mode
    void continuousMode() __attribute__((always_inline)) {
        setBit(ADC_SC3, ADC_SC3_ADCO_BIT);
    }
    //! Set single-shot conversion mode
    void singleMode() __attribute__((always_inline)) {
        clearBit(ADC_SC3, ADC_SC3_ADCO_BIT);
    }

    //! Use software to trigger the ADC, this is the most common setting
    void setSoftwareTrigger() __attribute__((always_inline)) {
        clearBit(ADC_SC2, ADC_SC2_ADTRG_BIT);
    }

    //! Use hardware to trigger the ADC
    void setHardwareTrigger() __attribute__((always_inline)) {
        setBit(ADC_SC2, ADC_SC2_ADTRG_BIT);
    }


    ////////////// INFORMATION ABOUT THE STATE OF THE ADC /////////////////

    //! Is the ADC converting at the moment?
    /**
    *   \return true or false
    */
    volatile bool isConverting() __attribute__((always_inline)) {
        //return (*ADC_SC2_adact);
        return getBit(ADC_SC2, ADC_SC2_ADACT_BIT);
        //return ((*ADC_SC2) & ADC_SC2_ADACT) >> 7;
    }

    //! Is an ADC conversion ready?
    /**
    *  \return true if yes, false if not.
    *  When a value is read this function returns false until a new value exists,
    *  so it only makes sense to call it before analogReadContinuous() or readSingle()
    */
    volatile bool isComplete() __attribute__((always_inline)) {
        //return (*ADC_SC1A_coco);
        return getBit(ADC_SC1A, ADC_SC1A_COCO_BIT);
        //return ((*ADC_SC1A) & ADC_SC1_COCO) >> 7;
    }

    //! Is the ADC in differential mode?
    /**
    *   \return true or false
    */
    volatile bool isDifferential() __attribute__((always_inline)) {
        //return ((*ADC_SC1A) & ADC_SC1_DIFF) >> 5;
        return getBit(ADC_SC1A, ADC_SC1_DIFF_BIT);
    }

    //! Is the ADC in continuous mode?
    /**
    *   \return true or false
    */
    volatile bool isContinuous() __attribute__((always_inline)) {
        //return (*ADC_SC3_adco);
        return getBit(ADC_SC3, ADC_SC3_ADCO_BIT);
        //return ((*ADC_SC3) & ADC_SC3_ADCO) >> 3;
    }

    //! Is the PGA function enabled?
    /**
    *   \return true or false
    */
    volatile bool isPGAEnabled() __attribute__((always_inline)) {
        return getBit(ADC_PGA, ADC_PGA_PGAEN_BIT);
    }


    //////////////// INFORMATION ABOUT VALID PINS //////////////////

    //! Check whether the pin is a valid analog pin
    /**
    *   \param pin to check.
    *   \return true if the pin is valid, false otherwise.
    */
    bool checkPin(uint8_t pin);

    //! Check whether the pins are a valid analog differential pair of pins
    /** If PGA is enabled it also checks that this ADCx can use PGA on this pins
    *   \param pinP positive pin to check.
    *   \param pinN negative pin to check.
    *   \return true if the pin is valid, false otherwise.
    */
    bool checkDifferentialPins(uint8_t pinP, uint8_t pinN);


    //////////////// HELPER METHODS FOR CONVERSION /////////////////

    //! Starts a single-ended conversion on the pin
    /** It sets the mux correctly, doesn't do any of the checks on the pin and
    *   doesn't change the continuous conversion bit.
    *   \param pin to read.
    */
    void startReadFast(uint8_t pin); // helper method

    //! Starts a differential conversion on the pair of pins
    /** It sets the mux correctly, doesn't do any of the checks on the pin and
    *   doesn't change the continuous conversion bit.
    *   \param pinP positive pin to read.
    *   \param pinN negative pin to read.
    */
    void startDifferentialFast(uint8_t pinP, uint8_t pinN);


    //////////////// BLOCKING CONVERSION METHODS //////////////////

    //! Returns the analog value of the pin.
    /** It waits until the value is read and then returns the result.
    * If a comparison has been set up and fails, it will return ADC_ERROR_VALUE.
    * This function is interrupt safe, so it will restore the adc to the state it was before being called
    *   \param pin pin to read.
    *   \return the value of the pin.
    */
    int analogRead(uint8_t pin);

    //! Returns the analog value of the special internal source, such as the temperature sensor.
    /** It calls analogRead(uint8_t pin) internally, with the correct value for the pin for all boards.
    *   Possible values:
    *   TEMP_SENSOR,  Temperature sensor.
    *   VREF_OUT,  1.2 V reference.
    *   BANDGAP, BANDGAP.
    *   VREFH, High VREF.
    *   VREFL, Low VREF.
    *   \param pin ADC_INTERNAL_SOURCE to read.
    *   \return the value of the pin.
    */
    int analogRead(ADC_INTERNAL_SOURCE pin) __attribute__((always_inline)) {
        return analogRead(static_cast<uint8_t>(pin));
    }


    //! Reads the differential analog value of two pins (pinP - pinN).
    /** It waits until the value is read and then returns the result.
    *   If a comparison has been set up and fails, it will return ADC_ERROR_DIFF_VALUE.
    *   \param pinP must be A10 or A12.
    *   \param pinN must be A11 (if pinP=A10) or A13 (if pinP=A12).
    *   \return the difference between the pins if they are valid, othewise returns ADC_ERROR_DIFF_VALUE.
    *   This function is interrupt safe, so it will restore the adc to the state it was before being called
    */
    int analogReadDifferential(uint8_t pinP, uint8_t pinN);


    /////////////// NON-BLOCKING CONVERSION METHODS //////////////

    //! Starts an analog measurement on the pin and enables interrupts.
    /** It returns immediately, get value with readSingle().
    *   If this function interrupts a measurement, it stores the settings in adc_config
    *   \param pin pin to read.
    *   \return true if the pin is valid, false otherwise.
    */
    bool startSingleRead(uint8_t pin);

    //! Start a differential conversion between two pins (pinP - pinN) and enables interrupts.
    /** It returns immediately, get value with readSingle().
    *   If this function interrupts a measurement, it stores the settings in adc_config
    *   \param pinP must be A10 or A12.
    *   \param pinN must be A11 (if pinP=A10) or A13 (if pinP=A12).
    *   \return true if the pins are valid, false otherwise.
    */
    bool startSingleDifferential(uint8_t pinP, uint8_t pinN);

    //! Reads the analog value of a single conversion.
    /** Set the conversion with with startSingleRead(pin) or startSingleDifferential(pinP, pinN).
    *   \return the converted value.
    */
    int readSingle() __attribute__((always_inline)) {
        return analogReadContinuous();
    }


    ///////////// CONTINUOUS CONVERSION METHODS ////////////

    //! Starts continuous conversion on the pin.
    /** It returns as soon as the ADC is set, use analogReadContinuous() to read the value.
    *   \param pin can be any of the analog pins
    *   \return true if the pin is valid, false otherwise.
    */
    bool startContinuous(uint8_t pin);

    //! Starts continuous conversion between the pins (pinP-pinN).
    /** It returns as soon as the ADC is set, use analogReadContinuous() to read the value.
    * \param pinP must be A10 or A12.
    * \param pinN must be A11 (if pinP=A10) or A13 (if pinP=A12).
    * \return true if the pins are valid, false otherwise.
    */
    bool startContinuousDifferential(uint8_t pinP, uint8_t pinN);

    //! Reads the analog value of a continuous conversion.
    /** Set the continuous conversion with with analogStartContinuous(pin) or startContinuousDifferential(pinP, pinN).
    *   \return the last converted value.
    *   If single-ended and 16 bits it's necessary to typecast it to an unsigned type (like uint16_t),
    *   otherwise values larger than 3.3/2 V are interpreted as negative!
    */
    int analogReadContinuous() __attribute__((always_inline)) {
        return (int16_t)(int32_t)*ADC_RA;
    }

    //! Stops continuous conversion
    void stopContinuous();


    //////////// PDB ////////////////
    //// Only works for Teensy 3.0 and 3.1, not LC (it doesn't have PDB)
    #if ADC_USE_PDB

    //                  software trigger    enable PDB     PDB interrupt
    #define ADC_PDB_CONFIG (PDB_SC_TRGSEL(15) | PDB_SC_PDBEN | PDB_SC_PDBIE \
        | PDB_SC_CONT |  PDB_SC_LDMOD(0))
    //    continuous mode load immediately

    #define PDB_CHnC1_TOS_1 0x0100
    #define PDB_CHnC1_EN_1 0x01

    //! Start PDB triggering the ADC at the frequency
    /**   Call analogRead on the pin that you want to measure before calling this function.
    *   See the example.
    *   \param freq is the frequency of the ADC conversion, it can't be lower that 1 Hz
    */
    void startPDB(uint32_t freq);

    //! Stop the PDB
    /**
    */
    void stopPDB();

    #endif


    //////// OTHER STUFF ///////////

    //! Start the 1.2V internal reference (if present)
    /** This is called automatically by setReference(ADC_REFERENCE::REF_1V2)
    *   Use it to switch on the internal reference on the VREF_OUT pin.
    *   Mode can be: 0 for stand-by, 1 for high-power buffer and 2 for low-power buffer.
    *    VREF_SC_MODE_LV_BANDGAPONLY for stand-by
    *    VREF_SC_MODE_LV_HIGHPOWERBUF for high power buffer and
    *    VREF_SC_MODE_LV_LOWPOWERBUF for low power buffer
    *    (these are defined in the teensyduino core)
    */
    static inline void startInternalReference(uint8_t mode = VREF_SC_MODE_LV_LOWPOWERBUF);
        
    //! Check if the internal reference has stabilized.
    /** This should be polled after enabling the reference after reset, or after changing
    *   its buffer mode from VREF_SC_MODE_LV_BANDGAPONLY to any of the buffered modes.
    *   Typical start-up time is 35 ms (as per datasheet).
    */
    static inline bool isInternalReferenceStable() {return VREF_SC & VREF_SC_VREFST;}

    //! Wait for the internal reference to stabilize.
    /** This function can be called to wait for the internal reference to stabilize.
    *   It will block until the reference has stabilized, or return immediately if the
    *   reference is not enabled in the first place.
    */
    static inline void waitForInternalReference() {while((VREF_SC & VREF_SC_VREFEN) && (!isInternalReferenceStable));}
        
    //! Stops the internal reference
    /** This is called automatically by setReference(ADC_REFERENCE::REF_1V2)
    */
    static inline void stopInternalReference();

    //! Store the config of the adc
    struct ADC_Config {
        //! ADC registers
        uint32_t savedSC1A, savedSC2, savedSC3, savedCFG1, savedCFG2;
    } adc_config;

    //! Was the adc in use before a call?
    uint8_t adcWasInUse;

    //! Save config of the ADC to the ADC_Config struct
    void saveConfig(ADC_Config* config) {
        config->savedSC1A = *ADC_SC1A;
        config->savedCFG1 = *ADC_CFG1;
        config->savedCFG2 = *ADC_CFG2;
        config->savedSC2 = *ADC_SC2;
        config->savedSC3 = *ADC_SC3;
    }

    //! Load config to the ADC
    void loadConfig(const ADC_Config* config) {
        *ADC_CFG1 = config->savedCFG1;
        *ADC_CFG2 = config->savedCFG2;
        *ADC_SC2 = config->savedSC2;
        *ADC_SC3 = config->savedSC3;
        *ADC_SC1A = config->savedSC1A; // restore last
    }


    //! Number of measurements that the ADC is performing
    uint8_t num_measurements;


    //! This flag indicates that some kind of error took place
    /** Use the defines at the beggining of this file to find out what caused the fail.
    */
    volatile uint16_t fail_flag;

    //! Prints the human-readable error, if any.
    void printError() {
        if(fail_flag != ADC_ERROR_CLEAR) {
            Serial.print("ADC"); Serial.print(ADC_num);
            Serial.print(" error: ");
            switch(fail_flag) {
                case ADC_ERROR_OTHER:
                    Serial.print("Unknown");
                    break;
                case ADC_ERROR_CALIB:
                    Serial.print("Calibration");
                    break;
                case ADC_ERROR_WRONG_PIN:
                    Serial.print("Wrong pin");
                    break;
                case ADC_ERROR_ANALOG_READ:
                    Serial.print("Analog read");
                    break;
                case ADC_ERROR_COMPARISON:
                    Serial.print("Comparison");
                    break;
                case ADC_ERROR_ANALOG_DIFF_READ:
                    Serial.print("Analog differential read");
                    break;
                case ADC_ERROR_CONT:
                    Serial.print("Continuous read");
                    break;
                case ADC_ERROR_CONT_DIFF:
                    Serial.print("Continuous differential read");
                    break;
                case ADC_ERROR_WRONG_ADC:
                    Serial.print("Wrong ADC");
                    break;
                case ADC_ERROR_SYNCH:
                    Serial.print("Synchronous");
                    break;
            }
            Serial.println(" error.");
        }
    }

    //! Resets all errors from the ADC, if any.
    void resetError() {
        fail_flag = ADC_ERROR_CLEAR;
    }


    //! Which adc is this?
    uint8_t ADC_num;



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
    volatile ADC_REF_SOURCE analog_reference_internal;

    // are interrupts enabled?
    volatile uint8_t var_enableInterrupts;

    // value of the pga
    volatile uint8_t pga_value;

    // conversion speed
    volatile ADC_CONVERSION_SPEED conversion_speed;

    // sampling speed
    volatile ADC_SAMPLING_SPEED sampling_speed;

    // translate pin number to SC1A nomenclature
    const uint8_t* const channel2sc1a;

    // same for differential pins
    const ADC_NLIST* const diff_table;


    //! Get the SC1A value of the differential pair for this pin
    uint8_t getDifferentialPair(uint8_t pin) {
        for(uint8_t i=0; i<ADC_DIFF_PAIRS; i++) {
            if(diff_table[i].pin == pin) {
                return diff_table[i].sc1a;
            }
        }
        return ADC_SC1A_PIN_INVALID;
    }



    //! Initialize ADC
    void analog_init();


    /////// Atomic bit set/clear
    /* Clear bit in address (make it zero), set bit (make it one), or return the value of that bit
    *   We can change this functions depending on the board.
    *   Teensy 3.x use bitband while Teensy LC has a more advanced bit manipulation engine.
    */
    #if defined(ADC_TEENSY_3_1) || defined(ADC_TEENSY_3_0) || defined(ADC_TEENSY_3_5) || defined(ADC_TEENSY_3_6)
    // bitband
    #define ADC_BITBAND_ADDR(reg, bit) (((uint32_t)(reg) - 0x40000000) * 32 + (bit) * 4 + 0x42000000)

    __attribute__((always_inline)) void setBit(volatile uint32_t* reg, uint8_t bit) {
        (*(uint32_t *)ADC_BITBAND_ADDR((reg), (bit))) = 1;
    }
    __attribute__((always_inline)) void clearBit(volatile uint32_t* reg, uint8_t bit) {
        (*(uint32_t *)ADC_BITBAND_ADDR((reg), (bit))) = 0;
    }

    __attribute__((always_inline)) void changeBit(volatile uint32_t* reg, uint8_t bit, bool state) {
        (*(uint32_t *)ADC_BITBAND_ADDR((reg), (bit))) = state;
    }

    __attribute__((always_inline)) volatile bool getBit(volatile uint32_t* reg, uint8_t bit) {
        return (volatile bool)*(uint32_t*)(ADC_BITBAND_ADDR(reg, bit));
    }

    #elif defined(ADC_TEENSY_LC)
    // bit manipulation engine
    //#define ADC_SETBIT_ATOMIC(reg, bit) (*(uint32_t *)(((uint32_t)&(reg) - 0xF8000000) | 0x48000000) = 1 << (bit)) // OR
    //#define ADC_CLRBIT_ATOMIC(reg, bit) (*(uint32_t *)(((uint32_t)&(reg) - 0xF8000000) | 0x44000000) = ~(1 << (bit))) // XOR

    __attribute__((always_inline)) void setBit(volatile uint32_t* reg, uint8_t bit) {
        //temp = *(uint32_t *)((uint32_t)(reg) | (1<<26) | (bit<<21)); // LAS
        *(volatile uint32_t*)((uint32_t)(reg) | (1<<27)) = 1<<bit; // OR
    }
    __attribute__((always_inline)) void clearBit(volatile uint32_t* reg, uint8_t bit) {
        //temp = *(uint32_t *)((uint32_t)(reg) | (3<<27) | (bit<<21)); // LAC
        *(volatile uint32_t*)((uint32_t)(reg) | (1<<26)) = ~(1<<bit); // AND
    }

    __attribute__((always_inline)) void changeBit(volatile uint32_t* reg, uint8_t bit, bool state) {
        //temp = *(uint32_t *)((uint32_t)(reg) | ((3-2*!!state)<<27) | (bit<<21)); // LAS/LAC
        if(state) { // set
            setBit(reg, bit);
        } else { // clear
            clearBit(reg, bit);
        }

    }

    __attribute__((always_inline)) volatile bool getBit(volatile uint32_t* reg, uint8_t bit) {
        return (volatile bool)*(uint32_t *)((uint32_t)(reg) | (1<<28) | (bit<<23) ); // UBFX
    }

    #endif

    uint32_t adc_offset;

    // registers point to the correct ADC module
    typedef volatile uint32_t* const reg;

    // registers that control the adc modulesetSoftwareTrigger
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

    reg PDB0_CHnC1; // PDB channel 0 or 1

    uint8_t IRQ_ADC; // IRQ number will be IRQ_ADC0 or IRQ_ADC1


protected:


};


#endif // ADC_MODULE_H
