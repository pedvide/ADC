/* Teensy 4, 3.x, LC ADC library
 * https://github.com/pedvide/ADC
 * Copyright (c) 2019 Pedro Villanueva
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

#include <Arduino.h>

// Easier names for the boards
#if defined(__MK20DX256__) // Teensy 3.1/3.2
#define ADC_TEENSY_3_1
#elif defined(__MK20DX128__) // Teensy 3.0
#define ADC_TEENSY_3_0
#elif defined(__MKL26Z64__) // Teensy LC
#define ADC_TEENSY_LC
#elif defined(__MK64FX512__) // Teensy 3.5
#define ADC_TEENSY_3_5
#elif defined(__MK66FX1M0__) // Teensy 3.6
#define ADC_TEENSY_3_6
#elif defined(__IMXRT1062__) // Teensy 4.0
#define ADC_TEENSY_4
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
#elif defined(ADC_TEENSY_4) // Teensy 3.6
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
#elif defined(ADC_TEENSY_4) // Teensy 4.0
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
#elif defined(ADC_TEENSY_4) // Teensy 4
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
#elif defined(ADC_TEENSY_4) // Teensy 4
        #define ADC_USE_PDB (0)
#endif

// Has internal reference?
#if defined(ADC_TEENSY_3_1) // Teensy 3.1
        #define ADC_USE_INTERNAL_VREF (1)
#elif defined(ADC_TEENSY_3_0) // Teensy 3.0
        #define ADC_USE_INTERNAL_VREF (1)
#elif defined(ADC_TEENSY_LC) // Teensy LC
        #define ADC_USE_INTERNAL_VREF (0)
#elif defined(ADC_TEENSY_3_5) // Teensy 3.5
        #define ADC_USE_INTERNAL_VREF (1)
#elif defined(ADC_TEENSY_3_6) // Teensy 3.6
        #define ADC_USE_INTERNAL_VREF (1)
#elif defined(ADC_TEENSY_4) // Teensy 4
        #define ADC_USE_INTERNAL_VREF (1)
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
    REF_3V3 = (uint8_t)ADC_REF_SOURCE::REF_DEFAULT, /*!< 3.3 volts */
    REF_1V2 = (uint8_t)ADC_REF_SOURCE::REF_ALT, /*!< 1.2 volts */
    REF_EXT = (uint8_t)ADC_REF_SOURCE::REF_DEFAULT, /*!< External VREF */
    NONE = (uint8_t)ADC_REF_SOURCE::REF_NONE // internal, do not use
};
#elif defined(ADC_TEENSY_LC)
// alt is the internal ref, 3.3 V
// the default is AREF
/*! \file */
/*! Reference for the ADC */
enum class ADC_REFERENCE : uint8_t {
    REF_3V3 = (uint8_t)ADC_REF_SOURCE::REF_ALT, /*!< 3.3 volts */
    REF_EXT = (uint8_t)ADC_REF_SOURCE::REF_DEFAULT, /*!< External VREF */
    NONE = (uint8_t)ADC_REF_SOURCE::REF_NONE // internal, do not use
};
#elif defined(ADC_TEENSY_4)
// default is the external, that is connected to the 3.3V supply.
/*! \file */
/*! Reference for the ADC */
enum class ADC_REFERENCE : uint8_t {
    REF_3V3 = (uint8_t)ADC_REF_SOURCE::REF_DEFAULT, /*!< 3.3 volts */
    NONE = (uint8_t)ADC_REF_SOURCE::REF_NONE // internal, do not use
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
        #define ADC_MAX_PIN (67)
#elif defined(ADC_TEENSY_4) // Teensy 4
        #define ADC_MAX_PIN (27)
#endif


// number of differential pairs PER ADC!
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
#elif defined(ADC_TEENSY_4) // Teensy 4
        #define ADC_DIFF_PAIRS (0)
#endif


// Other things to measure with the ADC that don't use external pins
// In my Teensy I read 1.22 V for the ADC_VREF_OUT (see VREF.h), 1.0V for ADC_BANDGAP (after PMC_REGSC |= PMC_REGSC_BGBE),
// 3.3 V for ADC_VREFH and 0.0 V for ADC_VREFL.
#if defined(ADC_TEENSY_LC)
    /*! Other ADC sources to measure, such as the temperature sensor.
    */
    enum class ADC_INTERNAL_SOURCE : uint8_t{
        TEMP_SENSOR = 38, /*!< Temperature sensor. */ // 0.719 V at 25ºC and slope of 1.715 mV/ºC for Teensy 3.x and 0.716 V, 1.62 mV/ºC for Teensy LC
        BANDGAP = 41, /*!< BANDGAP */ // Enable the Bandgap with PMC_REGSC |= PMC_REGSC_BGBE; (see VREF.h)
        VREFH = 42, /*!< High VREF */
        VREFL = 43, /*!< Low VREF. */
    };
#elif defined(ADC_TEENSY_3_1) || defined(ADC_TEENSY_3_0)
    /*! Other ADC sources to measure, such as the temperature sensor.
    */
    enum class ADC_INTERNAL_SOURCE : uint8_t{
        TEMP_SENSOR = 38, /*!< Temperature sensor. */ // 0.719 V at 25ºC and slope of 1.715 mV/ºC for Teensy 3.x and 0.716 V, 1.62 mV/ºC for Teensy LC
        VREF_OUT = 39, /*!< 1.2 V reference */
        BANDGAP = 41, /*!< BANDGAP */ // Enable the Bandgap with PMC_REGSC |= PMC_REGSC_BGBE; (see VREF.h)
        VREFH = 42, /*!< High VREF */
        VREFL = 43, /*!< Low VREF. */
    };
#elif defined(ADC_TEENSY_3_5) || defined(ADC_TEENSY_3_6)
    /*! Other ADC sources to measure, such as the temperature sensor.
    */
    enum class ADC_INTERNAL_SOURCE : uint8_t{
        TEMP_SENSOR = 24, /*!< Temperature sensor. */ // 0.719 V at 25ºC and slope of 1.715 mV/ºC for Teensy 3.x and 0.716 V, 1.62 mV/ºC for Teensy LC
        VREF_OUT = 28, /*!< 1.2 V reference */ // only on ADC1
        BANDGAP = 25, /*!< BANDGAP */ // Enable the Bandgap with PMC_REGSC |= PMC_REGSC_BGBE; (see VREF::start in VREF.h)
        VREFH = 26, /*!< High VREF */
        VREFL = 27, /*!< Low VREF. */
    };
#elif defined(ADC_TEENSY_4)
    /*! Other ADC sources to measure, such as the temperature sensor.
    */
    enum class ADC_INTERNAL_SOURCE : uint8_t{
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

#define ADC_MHz                (1000000) // not so many zeros
// Min freq for 8-12 bit mode is 1 MHz
#define ADC_MIN_FREQ         (1*ADC_MHz)
// Max freq for 8-12 bit mode is 18 MHz and 24 MHz for Teensy 3.6
#if defined(ADC_TEENSY_3_6)
    #define ADC_MAX_FREQ    (24*ADC_MHz)
#else
    #define ADC_MAX_FREQ    (18*ADC_MHz)
#endif
// Min freq for 16 bit mode is 2 MHz
#define ADC_MIN_FREQ_16BITS  (2*ADC_MHz)
// Max freq for 16 bit mode is 12 MHz
#define ADC_MAX_FREQ_16BITS (12*ADC_MHz)

// We can divide F_BUS by 1, 2, 4, 8, or 16:
/*
Divide by   ADC_CFG1_ADIV   ADC_CFG1_ADICLK TOTAL   VALUE
1           0               0               0       0x00
2           1               0               1       0x20
4           2               0               2       0x40
8           3               0               3       0x60
16          3               1               4       0x61
(Other combinations are possible)
*/

// Redefine from kinetis.h to remove (uint32_t) casts that the preprocessor doesn't understand
// so we can do arithmetic with them when defining ADC_CFG1_MED_SPEED
#define ADC_LIB_CFG1_ADIV(n)		(((n) & 3) << 5)
#define ADC_LIB_CFG1_ADICLK(n)		(((n) & 3) << 0)

// ADC_CFG1_VERY_LOW_SPEED is the lowest freq >= 1 MHz
#if F_BUS/16 >= ADC_MIN_FREQ
    #define ADC_CFG1_VERY_LOW_SPEED (ADC_LIB_CFG1_ADIV(3) + ADC_LIB_CFG1_ADICLK(1))
#elif F_BUS/8 >= ADC_MIN_FREQ
    #define ADC_CFG1_VERY_LOW_SPEED (ADC_LIB_CFG1_ADIV(3) + ADC_LIB_CFG1_ADICLK(0))
#elif F_BUS/4 >= ADC_MIN_FREQ
    #define ADC_CFG1_VERY_LOW_SPEED (ADC_LIB_CFG1_ADIV(2) + ADC_LIB_CFG1_ADICLK(0))
#elif F_BUS/2 >= ADC_MIN_FREQ
    #define ADC_CFG1_VERY_LOW_SPEED (ADC_LIB_CFG1_ADIV(1) + ADC_LIB_CFG1_ADICLK(0))
#else
    #define ADC_CFG1_VERY_LOW_SPEED (ADC_LIB_CFG1_ADIV(0) + ADC_LIB_CFG1_ADICLK(0))
#endif

// ADC_CFG1_LOW_SPEED is the lowest freq >= 2 MHz
#if F_BUS/16 >= ADC_MIN_FREQ_16BITS
    #define ADC_CFG1_LOW_SPEED (ADC_LIB_CFG1_ADIV(3) + ADC_LIB_CFG1_ADICLK(1))
#elif F_BUS/8 >= ADC_MIN_FREQ_16BITS
    #define ADC_CFG1_LOW_SPEED (ADC_LIB_CFG1_ADIV(3) + ADC_LIB_CFG1_ADICLK(0))
#elif F_BUS/4 >= ADC_MIN_FREQ_16BITS
    #define ADC_CFG1_LOW_SPEED (ADC_LIB_CFG1_ADIV(2) + ADC_LIB_CFG1_ADICLK(0))
#elif F_BUS/2 >= ADC_MIN_FREQ_16BITS
    #define ADC_CFG1_LOW_SPEED (ADC_LIB_CFG1_ADIV(1) + ADC_LIB_CFG1_ADICLK(0))
#else
    #define ADC_CFG1_LOW_SPEED (ADC_LIB_CFG1_ADIV(0) + ADC_LIB_CFG1_ADICLK(0))
#endif

// ADC_CFG1_HI_SPEED_16_BITS is the highest freq <= 12 MHz
#if F_BUS <= ADC_MAX_FREQ_16BITS
    #define ADC_CFG1_HI_SPEED_16_BITS (ADC_LIB_CFG1_ADIV(0) + ADC_LIB_CFG1_ADICLK(0))
#elif F_BUS/2 <= ADC_MAX_FREQ_16BITS
    #define ADC_CFG1_HI_SPEED_16_BITS (ADC_LIB_CFG1_ADIV(1) + ADC_LIB_CFG1_ADICLK(0))
#elif F_BUS/4 <= ADC_MAX_FREQ_16BITS
    #define ADC_CFG1_HI_SPEED_16_BITS (ADC_LIB_CFG1_ADIV(2) + ADC_LIB_CFG1_ADICLK(0))
#elif F_BUS/8 <= ADC_MAX_FREQ_16BITS
    #define ADC_CFG1_HI_SPEED_16_BITS (ADC_LIB_CFG1_ADIV(3) + ADC_LIB_CFG1_ADICLK(0))
#else
    #define ADC_CFG1_HI_SPEED_16_BITS (ADC_LIB_CFG1_ADIV(3) + ADC_LIB_CFG1_ADICLK(1))
#endif

// For ADC_CFG1_MED_SPEED the idea is to check if there's an unused setting between
// ADC_CFG1_LOW_SPEED and ADC_CFG1_HI_SPEED_16_BITS
#if (ADC_CFG1_LOW_SPEED - ADC_CFG1_HI_SPEED_16_BITS > 0x20) // higher values are slower speeds
    #define ADC_CFG1_MED_SPEED  ((ADC_CFG1_HI_SPEED_16_BITS) + 0x20) // at least one divisor in between
#else
    #define ADC_CFG1_MED_SPEED  (ADC_CFG1_HI_SPEED_16_BITS)
#endif

// ADC_CFG1_HI_SPEED is the highest freq <= 18 MHz (24 for Teensy 3.6)
#if F_BUS <= ADC_MAX_FREQ
    #define ADC_CFG1_HI_SPEED (ADC_LIB_CFG1_ADIV(0) + ADC_LIB_CFG1_ADICLK(0))
#elif F_BUS/2 <= ADC_MAX_FREQ
    #define ADC_CFG1_HI_SPEED (ADC_LIB_CFG1_ADIV(1) + ADC_LIB_CFG1_ADICLK(0))
#elif F_BUS/4 <= ADC_MAX_FREQ
    #define ADC_CFG1_HI_SPEED (ADC_LIB_CFG1_ADIV(2) + ADC_LIB_CFG1_ADICLK(0))
#elif F_BUS/8 <= ADC_MAX_FREQ
    #define ADC_CFG1_HI_SPEED (ADC_LIB_CFG1_ADIV(3) + ADC_LIB_CFG1_ADICLK(0))
#else
    #define ADC_CFG1_HI_SPEED (ADC_LIB_CFG1_ADIV(3) + ADC_LIB_CFG1_ADICLK(1))
#endif

// ADC_CFG1_VERY_HIGH_SPEED >= ADC_CFG1_HI_SPEED and may be out of specs, but not more than ADC_VERY_HIGH_SPEED_FACTOR*ADC_MAX_FREQ
#define ADC_VERY_HIGH_SPEED_FACTOR  (2)
#if F_BUS <= ADC_VERY_HIGH_SPEED_FACTOR*ADC_MAX_FREQ
    #define ADC_CFG1_VERY_HIGH_SPEED (ADC_LIB_CFG1_ADIV(0) + ADC_LIB_CFG1_ADICLK(0))
#elif F_BUS/2 <= ADC_VERY_HIGH_SPEED_FACTOR*ADC_MAX_FREQ
    #define ADC_CFG1_VERY_HIGH_SPEED (ADC_LIB_CFG1_ADIV(1) + ADC_LIB_CFG1_ADICLK(0))
#elif F_BUS/4 <= ADC_VERY_HIGH_SPEED_FACTOR*ADC_MAX_FREQ
    #define ADC_CFG1_VERY_HIGH_SPEED (ADC_LIB_CFG1_ADIV(2) + ADC_LIB_CFG1_ADICLK(0))
#elif F_BUS/8 <= ADC_VERY_HIGH_SPEED_FACTOR*ADC_MAX_FREQ
    #define ADC_CFG1_VERY_HIGH_SPEED (ADC_LIB_CFG1_ADIV(3) + ADC_LIB_CFG1_ADICLK(0))
#else
    #define ADC_CFG1_VERY_HIGH_SPEED (ADC_LIB_CFG1_ADIV(3) + ADC_LIB_CFG1_ADICLK(1))
#endif



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

//! Handle ADC errors
namespace ADC_Error {

    //! ADC errors.
    /*! Use adc->getError() to print the errors (if any) in a human-readable form.
    *   Use adc->resetError() to reset them.
    */
    enum class ADC_ERROR : uint16_t {
        OTHER               = 1<<0, /*!< Other error not considered below. */
        CALIB               = 1<<1, /*!< Calibration error. */
        WRONG_PIN           = 1<<2, /*!< A pin was selected that cannot be read by this ADC module. */
        ANALOG_READ         = 1<<3, /*!< Error inside the analogRead method. */
        ANALOG_DIFF_READ    = 1<<4, /*!< Error inside the analogReadDifferential method. */
        CONT                = 1<<5, /*!< Continuous single-ended measurement error. */
        CONT_DIFF           = 1<<6, /*!< Continuous differential measurement error. */
        COMPARISON          = 1<<7, /*!< Error during the comparison. */
        WRONG_ADC           = 1<<8, /*!< A non-existent ADC module was selected. */
        SYNCH               = 1<<9, /*!< Error during a synchronized measurement. */

        CLEAR               = 0,    /*!< No error. */
    };
    //! OR operator for ADC_ERRORs.
    inline constexpr ADC_ERROR operator|(ADC_ERROR lhs, ADC_ERROR rhs) {
        return static_cast<ADC_ERROR> (static_cast<uint16_t>(lhs) | static_cast<uint16_t>(rhs));
    }
    //! AND operator for ADC_ERRORs.
    inline constexpr ADC_ERROR operator&(ADC_ERROR lhs, ADC_ERROR rhs) {
        return static_cast<ADC_ERROR> (static_cast<uint16_t>(lhs) & static_cast<uint16_t>(rhs));
    }
    //! |= operator for ADC_ERRORs, it changes the left hand side ADC_ERROR.
    inline ADC_ERROR operator|=(volatile ADC_ERROR& lhs, ADC_ERROR rhs) {
        return lhs = static_cast<ADC_ERROR> (static_cast<uint16_t>(lhs) | static_cast<uint16_t>(rhs));
    }
    //! &= operator for ADC_ERRORs, it changes the left hand side ADC_ERROR.
    inline ADC_ERROR operator&=(volatile ADC_ERROR& lhs, ADC_ERROR rhs) {
        return lhs = static_cast<ADC_ERROR> (static_cast<uint16_t>(lhs) & static_cast<uint16_t>(rhs));
    }

    //! Prints the human-readable error, if any.
    inline const char* getError(ADC_ERROR fail_flag) {
        if(fail_flag != ADC_ERROR::CLEAR) {
            switch(fail_flag) {
                case ADC_ERROR::CALIB:
                    return (const char*)"Calibration";
                    break;
                case ADC_ERROR::WRONG_PIN:
                    return (const char*)"Wrong pin";
                    break;
                case ADC_ERROR::ANALOG_READ:
                    return (const char*)"Analog read";
                    break;
                case ADC_ERROR::COMPARISON:
                    return (const char*)"Comparison";
                    break;
                case ADC_ERROR::ANALOG_DIFF_READ:
                    return (const char*)"Analog differential read";
                    break;
                case ADC_ERROR::CONT:
                    return (const char*)"Continuous read";
                    break;
                case ADC_ERROR::CONT_DIFF:
                    return (const char*)"Continuous differential read";
                    break;
                case ADC_ERROR::WRONG_ADC:
                    return (const char*)"Wrong ADC";
                    break;
                case ADC_ERROR::SYNCH:
                    return (const char*)"Synchronous";
                    break;
                case ADC_ERROR::OTHER:
                case ADC_ERROR::CLEAR: // silence warnings
                default:
                    return (const char*)"Unknown";
                    break;
            }
        }
        return (const char*)"";
    }
    

    //! Resets all errors from the ADC, if any.
    inline void resetError(volatile ADC_ERROR& fail_flag) {
        fail_flag = ADC_ERROR::CLEAR;
    }

}