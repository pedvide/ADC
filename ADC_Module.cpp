/* Teensy 3.x, LC ADC library
 * https://github.com/pedvide/ADC
 * Copyright (c) 2016 Pedro Villanueva
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

/* ADC_Module.cpp: Implements the fuctions of a Teensy 3.x, LC ADC module
 *
 */



#include "ADC_Module.h"
//#include "ADC.h"

// include the internal reference
#include <VREF.h>


/* Constructor
*   Point the registers to the correct ADC module
*   Copy the correct channel2sc1a
*   Call init
*   The very long initializer list could be shorter using some kind of struct?
*/
ADC_Module::ADC_Module(uint8_t ADC_number, const uint8_t* const a_channel2sc1a, const ADC_NLIST* const a_diff_table) :
        ADC_num(ADC_number)
        , channel2sc1a(a_channel2sc1a)
        , diff_table(a_diff_table)
        , ADC_SC1A(ADC_num? ADC1_SC1A : ADC0_SC1A)
        , ADC_SC1B(ADC_num? ADC1_SC1B : ADC0_SC1B)
        , ADC_CFG1(ADC_num? ADC1_CFG1 : ADC0_CFG1)
        , ADC_CFG2(ADC_num? ADC1_CFG2 : ADC0_CFG2)
        , ADC_RA(ADC_num? ADC1_RA : ADC0_RA)
        , ADC_RB(ADC_num? ADC1_RB : ADC0_RB)
        , ADC_CV1(ADC_num? ADC1_CV1 : ADC0_CV1)
        , ADC_CV2(ADC_num? ADC1_CV2 : ADC0_CV2)
        , ADC_SC2(ADC_num? ADC1_SC2 : ADC0_SC2)
        , ADC_SC3(ADC_num? ADC1_SC3 : ADC0_SC3)
        , ADC_PGA(ADC_num? ADC1_PGA : ADC0_PGA)

        , ADC_OFS(ADC_num? ADC1_OFS : ADC0_OFS)
        , ADC_PG(ADC_num? ADC1_PG : ADC0_PG)
        , ADC_MG(ADC_num? ADC1_MG : ADC0_MG)
        , ADC_CLPD(ADC_num? ADC1_CLPD : ADC0_CLPD)
        , ADC_CLPS(ADC_num? ADC1_CLPS : ADC0_CLPS)
        , ADC_CLP4(ADC_num? ADC1_CLP4 : ADC0_CLP4)
        , ADC_CLP3(ADC_num? ADC1_CLP3 : ADC0_CLP3)
        , ADC_CLP2(ADC_num? ADC1_CLP2 : ADC0_CLP2)
        , ADC_CLP1(ADC_num? ADC1_CLP1 : ADC0_CLP1)
        , ADC_CLP0(ADC_num? ADC1_CLP0 : ADC0_CLP0)
        , ADC_CLMD(ADC_num? ADC1_CLMD : ADC0_CLMD)
        , ADC_CLMS(ADC_num? ADC1_CLMS : ADC0_CLMS)
        , ADC_CLM4(ADC_num? ADC1_CLM4 : ADC0_CLM4)
        , ADC_CLM3(ADC_num? ADC1_CLM3 : ADC0_CLM3)
        , ADC_CLM2(ADC_num? ADC1_CLM2 : ADC0_CLM2)
        , ADC_CLM1(ADC_num? ADC1_CLM1 : ADC0_CLM1)
        , ADC_CLM0(ADC_num? ADC1_CLM0 : ADC0_CLM0)
        , PDB0_CHnC1(ADC_num? PDB0_CH1C1 : PDB0_CH0C1)
        #if ADC_NUM_ADCS==2
        // IRQ_ADC0 and IRQ_ADC1 aren't consecutive in Teensy 3.6
        , IRQ_ADC(ADC_num? IRQ_ADC1 : IRQ_ADC0) // fix by SB, https://github.com/pedvide/ADC/issues/19
        #else
        , IRQ_ADC(IRQ_ADC0)
        #endif
        {


    // call our init
    analog_init();

}

/* Initialize stuff:
*  - Clear all fail flags
*  - Internal reference (default: external vcc)
*  - Mux between a and b channels (b channels)
*  - Calibrate with 32 averages and low speed
*  - When first calibration is done it sets:
*     - Resolution (default: 10 bits)
*     - Conversion speed and sampling time (both set to medium speed)
*     - Averaging (set to 4)
*/
void ADC_Module::analog_init() {

    // default settings:
    /*
        - 10 bits resolution
        - 4 averages
        - vcc reference
        - no interrupts
        - pga gain=1
        - conversion speed = medium
        - sampling speed = medium
    initiate to 0 (or 1) so the corresponding functions change it to the correct value
    */
    analog_res_bits = 0;
    analog_max_val = 0;
    analog_num_average = 0;
    analog_reference_internal = ADC_REF_SOURCE::REF_NONE;
    pga_value = 1;

    conversion_speed = ADC_CONVERSION_SPEED::VERY_HIGH_SPEED; // set to something different from line 139 so it gets changed there
    sampling_speed =  ADC_SAMPLING_SPEED::VERY_HIGH_SPEED;

    calibrating = 0;

    fail_flag = ADC_ERROR::CLEAR; // clear all errors

    num_measurements = 0;

    // select b channels
    // ADC_CFG2_muxsel = 1;
    atomic::setBitFlag(ADC_CFG2, ADC_CFG2_MUXSEL);

    // set reference to vcc
    setReference(ADC_REFERENCE::REF_3V3);

    // set resolution to 10
    setResolution(10);

    // the first calibration will use 32 averages and lowest speed,
    // when this calibration is over the averages and speed will be set to default by wait_for_cal and init_calib will be cleared.
    init_calib = 1;
    setAveraging(32);
    setConversionSpeed(ADC_CONVERSION_SPEED::LOW_SPEED);
    setSamplingSpeed(ADC_SAMPLING_SPEED::LOW_SPEED);

    // begin init calibration
    calibrate();
}

// starts calibration
void ADC_Module::calibrate() {

    __disable_irq();

    calibrating = 1;
    // ADC_SC3_cal = 0; // stop possible previous calibration
    atomic::clearBitFlag(ADC_SC3, ADC_SC3_CAL);
    // ADC_SC3_calf = 1; // clear possible previous error
    atomic::setBitFlag(ADC_SC3, ADC_SC3_CALF);
    // ADC_SC3_cal = 1; // start calibration
    atomic::setBitFlag(ADC_SC3, ADC_SC3_CAL);

    __enable_irq();
}


/* Waits until calibration is finished and writes the corresponding registers
*
*/
void ADC_Module::wait_for_cal(void) {
    uint16_t sum;

    while(atomic::getBitFlag(ADC_SC3, ADC_SC3_CAL)) { // Bit ADC_SC3_CAL in register ADC0_SC3 cleared when calib. finishes.
        yield();
    }

    if(atomic::getBitFlag(ADC_SC3, ADC_SC3_CALF)) { // calibration failed
        fail_flag |= ADC_ERROR::CALIB; // the user should know and recalibrate manually
    }

    __disable_irq();
    if (calibrating) {
        sum = ADC_CLPS + ADC_CLP4 + ADC_CLP3 + ADC_CLP2 + ADC_CLP1 + ADC_CLP0;
        sum = (sum / 2) | 0x8000;
        ADC_PG = sum;

        sum = ADC_CLMS + ADC_CLM4 + ADC_CLM3 + ADC_CLM2 + ADC_CLM1 + ADC_CLM0;
        sum = (sum / 2) | 0x8000;
        ADC_MG = sum;

        calibrating = 0;
    }
    __enable_irq();

    // the first calibration uses 32 averages and lowest speed,
    // when this calibration is over, set the averages and speed to default.
    if(init_calib) {

        // set conversion speed to medium
        setConversionSpeed(ADC_CONVERSION_SPEED::MED_SPEED);

        // set sampling speed to medium
        setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED);

        // number of averages to 4
        setAveraging(4);

        init_calib = 0; // clear
    }

}

//! Starts the calibration sequence, waits until it's done and writes the results
/** Usually it's not necessary to call this function directly, but do it if the "environment" changed
*   significantly since the program was started.
*/
void ADC_Module::recalibrate() {

    calibrate();

    wait_for_cal();
}



/////////////// METHODS TO SET/GET SETTINGS OF THE ADC ////////////////////


/* Set the voltage reference you prefer, default is 3.3V
*   It needs to recalibrate
*  Use ADC_REF_3V3, ADC_REF_1V2 (not for Teensy LC) or ADC_REF_EXT
*/
void ADC_Module::setReference(ADC_REFERENCE type) {
    ADC_REF_SOURCE ref_type = static_cast<ADC_REF_SOURCE>(type); // cast to source type, that is, either internal or default

    if (analog_reference_internal==ref_type) { // don't need to change anything
        return;
    }

    if (ref_type == ADC_REF_SOURCE::REF_ALT) { // 1.2V ref for Teensy 3.x, 3.3 VDD for Teensy LC
        // internal reference requested
        #if ADC_USE_INTERNAL_VREF
        VREF::start(); // enable VREF if Teensy 3.x
        #endif

        analog_reference_internal = ADC_REF_SOURCE::REF_ALT;

        // *ADC_SC2_ref = 1; // uses bitband: atomic
        atomic::setBitFlag(ADC_SC2, ADC_SC2_REFSEL(1));

    } else if(ref_type == ADC_REF_SOURCE::REF_DEFAULT) { // ext ref for all Teensys, vcc also for Teensy 3.x
        // vcc or external reference requested

        #if ADC_USE_INTERNAL_VREF
        VREF::stop(); // disable 1.2V reference source when using the external ref (p. 102, 3.7.1.7)
        #endif

        analog_reference_internal = ADC_REF_SOURCE::REF_DEFAULT;

        // *ADC_SC2_ref = 0; // uses bitband: atomic
        atomic::clearBitFlag(ADC_SC2, ADC_SC2_REFSEL(1));
    }

    calibrate();
}


/* Change the resolution of the measurement
*  For single-ended measurements: 8, 10, 12 or 16 bits.
*  For differential measurements: 9, 11, 13 or 16 bits.
*  If you want something in between (11 bits single-ended for example) select the inmediate higher
*  and shift the result one to the right.
*
*  It doesn't recalibrate
*/
void ADC_Module::setResolution(uint8_t bits) {

    if(analog_res_bits==bits) {
        return;
    }

    uint8_t config;

    if (calibrating) wait_for_cal();

    if (bits <= 9) {
        config = 8;
    } else if (bits <= 11) {
        config = 10;
    } else if (bits <= 13) {
        config = 12;
    } else if (bits > 13) {
        config = 16;
    } else {
        config = 8; // default to 8 bits
    }

    // conversion resolution
    // single-ended 8 bits is the same as differential 9 bits, etc.
    if ( (config == 8) || (config == 9) )  {
        // *ADC_CFG1_mode1 = 0;
        // *ADC_CFG1_mode0 = 0;
        atomic::clearBitFlag(ADC_CFG1, ADC_CFG1_MODE(3));
        analog_max_val = 255; // diff mode 9 bits has 1 bit for sign, so max value is the same as single 8 bits
    } else if ( (config == 10 )|| (config == 11) ) {
        // *ADC_CFG1_mode1 = 1;
        // *ADC_CFG1_mode0 = 0;
        atomic::changeBitFlag(ADC_CFG1, ADC_CFG1_MODE(3), ADC_CFG1_MODE(2));
        analog_max_val = 1023;
    } else if ( (config == 12 )|| (config == 13) ) {
        // *ADC_CFG1_mode1 = 0;
        // *ADC_CFG1_mode0 = 1;
        atomic::changeBitFlag(ADC_CFG1, ADC_CFG1_MODE(3), ADC_CFG1_MODE(1));
        analog_max_val = 4095;
    } else {
        // *ADC_CFG1_mode1 = 1;
        // *ADC_CFG1_mode0 = 1;
        atomic::setBitFlag(ADC_CFG1, ADC_CFG1_MODE(3));
        analog_max_val = 65535;
    }

    analog_res_bits = config;

    // no recalibration is needed when changing the resolution, p. 619

}

/* Returns the resolution of the ADC
*
*/
uint8_t ADC_Module::getResolution() {
    return analog_res_bits;
}

/* Returns the maximum value for a measurement, that is: 2^resolution-1
*
*/
uint32_t ADC_Module::getMaxValue() {
    return analog_max_val;
}


// Sets the conversion speed
/* Increase the sampling speed for low impedance sources, decrease it for higher impedance ones.
* \param speed can be any of the ADC_SAMPLING_SPEED enum: VERY_LOW_SPEED, LOW_SPEED, MED_SPEED, HIGH_SPEED or VERY_HIGH_SPEED.
*
* VERY_LOW_SPEED is the lowest possible sampling speed (+24 ADCK).
* LOW_SPEED adds +16 ADCK.
* MED_SPEED adds +10 ADCK.
* HIGH_SPEED adds +6 ADCK.
* VERY_HIGH_SPEED is the highest possible sampling speed (0 ADCK added).
*/
void ADC_Module::setConversionSpeed(ADC_CONVERSION_SPEED speed) {

    if(speed==conversion_speed) { // no change
        return;
    }

    if (calibrating) wait_for_cal();

    // internal asynchronous clock settings: fADK = 2.4, 4.0, 5.2 or 6.2 MHz
    if( (speed == ADC_CONVERSION_SPEED::ADACK_2_4) ||
        (speed == ADC_CONVERSION_SPEED::ADACK_4_0) ||
        (speed == ADC_CONVERSION_SPEED::ADACK_5_2) ||
        (speed == ADC_CONVERSION_SPEED::ADACK_6_2)) {
        atomic::setBitFlag(ADC_CFG2, ADC_CFG2_ADACKEN); // enable ADACK (takes max 5us to be ready)
        atomic::setBitFlag(ADC_CFG1, ADC_CFG1_ADICLK(3)); // select ADACK as clock source

        atomic::clearBitFlag(ADC_CFG1, ADC_CFG1_ADIV(3)); // select no dividers

        if(speed == ADC_CONVERSION_SPEED::ADACK_2_4) {
            atomic::clearBitFlag(ADC_CFG2, ADC_CFG2_ADHSC);
            atomic::setBitFlag(ADC_CFG1, ADC_CFG1_ADLPC);
        } else if(speed == ADC_CONVERSION_SPEED::ADACK_4_0) {
            atomic::setBitFlag(ADC_CFG2, ADC_CFG2_ADHSC);
            atomic::setBitFlag(ADC_CFG1, ADC_CFG1_ADLPC);
        } else if(speed == ADC_CONVERSION_SPEED::ADACK_5_2) {
            atomic::clearBitFlag(ADC_CFG2, ADC_CFG2_ADHSC);
            atomic::clearBitFlag(ADC_CFG1, ADC_CFG1_ADLPC);
        } else if(speed == ADC_CONVERSION_SPEED::ADACK_6_2) {
            atomic::setBitFlag(ADC_CFG2, ADC_CFG2_ADHSC);
            atomic::clearBitFlag(ADC_CFG1, ADC_CFG1_ADLPC);
        }
        conversion_speed = speed;
        return;
    }


    // normal bus clock used

    // *ADC_CFG2_adacken = 0; // disable the internal asynchronous clock
    atomic::clearBitFlag(ADC_CFG2, ADC_CFG2_ADACKEN);

    uint32_t ADC_CFG1_speed; // store the clock and divisor

    if(speed == ADC_CONVERSION_SPEED::VERY_LOW_SPEED) {
        // *ADC_CFG2_adhsc = 0; // no high-speed config
        // *ADC_CFG1_adlpc  = 1; // use low power conf.
        atomic::clearBitFlag(ADC_CFG2, ADC_CFG2_ADHSC);
        atomic::setBitFlag(ADC_CFG1, ADC_CFG1_ADLPC);

        ADC_CFG1_speed = ADC_CFG1_VERY_LOW_SPEED;

    } else if(speed == ADC_CONVERSION_SPEED::LOW_SPEED) {
        // *ADC_CFG2_adhsc = 0; // no high-speed config
        // *ADC_CFG1_adlpc  = 1; // use low power conf.
        atomic::clearBitFlag(ADC_CFG2, ADC_CFG2_ADHSC);
        atomic::setBitFlag(ADC_CFG1, ADC_CFG1_ADLPC);

        ADC_CFG1_speed = ADC_CFG1_LOW_SPEED;

    } else if(speed == ADC_CONVERSION_SPEED::MED_SPEED) {
        // *ADC_CFG2_adhsc = 0; // no high-speed config
        // *ADC_CFG1_adlpc  = 0; // no low power conf.
        atomic::clearBitFlag(ADC_CFG2, ADC_CFG2_ADHSC);
        atomic::clearBitFlag(ADC_CFG1, ADC_CFG1_ADLPC);

        ADC_CFG1_speed = ADC_CFG1_MED_SPEED;

    } else if(speed == ADC_CONVERSION_SPEED::HIGH_SPEED_16BITS) {
        // *ADC_CFG2_adhsc = 1; // high-speed config: add 2 ADCK
        // *ADC_CFG1_adlpc  = 0; // no low power conf.
        atomic::setBitFlag(ADC_CFG2, ADC_CFG2_ADHSC);
        atomic::clearBitFlag(ADC_CFG1, ADC_CFG1_ADLPC);

        ADC_CFG1_speed = ADC_CFG1_HI_SPEED_16_BITS;

    } else if(speed == ADC_CONVERSION_SPEED::HIGH_SPEED) {
        // *ADC_CFG2_adhsc = 1; // high-speed config: add 2 ADCK
        // *ADC_CFG1_adlpc  = 0; // no low power conf.
        atomic::setBitFlag(ADC_CFG2, ADC_CFG2_ADHSC);
        atomic::clearBitFlag(ADC_CFG1, ADC_CFG1_ADLPC);

        ADC_CFG1_speed = ADC_CFG1_HI_SPEED;

    } else if(speed == ADC_CONVERSION_SPEED::VERY_HIGH_SPEED) { // this speed is most likely out of specs, so accuracy can be bad
        // *ADC_CFG2_adhsc = 1; // high-speed config: add 2 ADCK
        // *ADC_CFG1_adlpc  = 0; // no low power conf.
        atomic::setBitFlag(ADC_CFG2, ADC_CFG2_ADHSC);
        atomic::clearBitFlag(ADC_CFG1, ADC_CFG1_ADLPC);

        ADC_CFG1_speed = ADC_CFG1_VERY_HIGH_SPEED;

    } else {
        fail_flag |= ADC_ERROR::OTHER;
        return;
    }

    // clock source is bus or bus/2
    atomic::changeBitFlag(ADC_CFG1, ADC_CFG1_ADICLK(3), ADC_CFG1_speed & ADC_CFG1_ADICLK(3));

    // divisor for the clock source: 1, 2, 4 or 8.
    // so total speed can be: bus, bus/2, bus/4, bus/8 or bus/16.
    atomic::changeBitFlag(ADC_CFG1, ADC_CFG1_ADIV(3), ADC_CFG1_speed & ADC_CFG1_ADIV(3));

    conversion_speed = speed;

}


// Sets the sampling speed
/* Increase the sampling speed for low impedance sources, decrease it for higher impedance ones.
* \param speed can be any of the ADC_SAMPLING_SPEED enum: VERY_LOW_SPEED, LOW_SPEED, MED_SPEED, HIGH_SPEED or VERY_HIGH_SPEED.
*
* VERY_LOW_SPEED is the lowest possible sampling speed (+24 ADCK).
* LOW_SPEED adds +16 ADCK.
* MED_SPEED adds +10 ADCK.
* HIGH_SPEED adds +6 ADCK.
* VERY_HIGH_SPEED is the highest possible sampling speed (0 ADCK added).
*/
void ADC_Module::setSamplingSpeed(ADC_SAMPLING_SPEED speed) {
    if (calibrating) wait_for_cal();

    switch(speed) {
    case ADC_SAMPLING_SPEED::VERY_LOW_SPEED:
        atomic::setBitFlag(ADC_CFG1, ADC_CFG1_ADLSMP); // long sampling time enable
        atomic::clearBitFlag(ADC_CFG2, ADC_CFG2_ADLSTS(3)); // maximum sampling time (+24 ADCK)
        break;
    case ADC_SAMPLING_SPEED::LOW_SPEED:
        atomic::setBitFlag(ADC_CFG1, ADC_CFG1_ADLSMP); // long sampling time enable
        atomic::changeBitFlag(ADC_CFG2, ADC_CFG2_ADLSTS(3), ADC_CFG2_ADLSTS(1)); // high sampling time (+16 ADCK)
        break;
    case ADC_SAMPLING_SPEED::MED_SPEED:
        atomic::setBitFlag(ADC_CFG1, ADC_CFG1_ADLSMP); // long sampling time enable
        atomic::changeBitFlag(ADC_CFG2, ADC_CFG2_ADLSTS(3), ADC_CFG2_ADLSTS(2)); // medium sampling time (+10 ADCK)
        break;
    case ADC_SAMPLING_SPEED::HIGH_SPEED:
        atomic::setBitFlag(ADC_CFG1, ADC_CFG1_ADLSMP); // long sampling time enable
        atomic::setBitFlag(ADC_CFG2, ADC_CFG2_ADLSTS(3)); // low sampling time (+6 ADCK)
        break;
    case ADC_SAMPLING_SPEED::VERY_HIGH_SPEED:
        atomic::clearBitFlag(ADC_CFG1, ADC_CFG1_ADLSMP); // shortest sampling time
        break;
    }
    sampling_speed =  speed;
}


/* Set the number of averages: 0, 4, 8, 16 or 32.
*
*/
void ADC_Module::setAveraging(uint8_t num) {

    if (calibrating) wait_for_cal();

    if (num <= 1) {
        num = 0;
        // ADC_SC3_avge = 0;
        atomic::clearBitFlag(ADC_SC3, ADC_SC3_AVGE);
    } else {
        // ADC_SC3_avge = 1;
        atomic::setBitFlag(ADC_SC3, ADC_SC3_AVGE);
        if (num <= 4) {
            num = 4;
            // ADC_SC3_avgs0 = 0;
            // ADC_SC3_avgs1 = 0;
            atomic::clearBitFlag(ADC_SC3, ADC_SC3_AVGS(3));
        } else if (num <= 8) {
            num = 8;
            // ADC_SC3_avgs0 = 1;
            // ADC_SC3_avgs1 = 0;
            atomic::changeBitFlag(ADC_SC3, ADC_SC3_AVGS(3), ADC_SC3_AVGS(1));
        } else if (num <= 16) {
            num = 16;
            // ADC_SC3_avgs0 = 0;
            // ADC_SC3_avgs1 = 1;
            atomic::changeBitFlag(ADC_SC3, ADC_SC3_AVGS(3), ADC_SC3_AVGS(2));
        } else {
            num = 32;
            // ADC_SC3_avgs0 = 1;
            // ADC_SC3_avgs1 = 1;
            atomic::setBitFlag(ADC_SC3, ADC_SC3_AVGS(3));
        }
    }
    analog_num_average = num;
}


/* Enable interrupts: An ADC Interrupt will be raised when the conversion is completed
*  (including hardware averages and if the comparison (if any) is true).
*/
void ADC_Module::enableInterrupts() {
    if (calibrating) wait_for_cal();

    // ADC_SC1A_aien = 1;
    atomic::setBitFlag(ADC_SC1A, ADC_SC1_AIEN);

    NVIC_ENABLE_IRQ(IRQ_ADC);
}

/* Disable interrupts
*
*/
void ADC_Module::disableInterrupts() {
    // ADC_SC1A_aien = 0;
    atomic::clearBitFlag(ADC_SC1A, ADC_SC1_AIEN);

    NVIC_DISABLE_IRQ(IRQ_ADC);
}


/* Enable DMA request: An ADC DMA request will be raised when the conversion is completed
*  (including hardware averages and if the comparison (if any) is true).
*/
void ADC_Module::enableDMA() {

    if (calibrating) wait_for_cal();

    // ADC_SC2_dma = 1;
    atomic::setBitFlag(ADC_SC2, ADC_SC2_DMAEN);
}

/* Disable ADC DMA request
*
*/
void ADC_Module::disableDMA() {

    // ADC_SC2_dma = 0;
    atomic::clearBitFlag(ADC_SC2, ADC_SC2_DMAEN);
}


/* Enable the compare function: A conversion will be completed only when the ADC value
*  is >= compValue (greaterThan=1) or < compValue (greaterThan=0)
*  Call it after changing the resolution
*  Use with interrupts or poll conversion completion with isADC_Complete()
*/
void ADC_Module::enableCompare(int16_t compValue, bool greaterThan) {

    if (calibrating) wait_for_cal(); // if we modify the adc's registers when calibrating, it will fail

    // ADC_SC2_cfe = 1; // enable compare
    // ADC_SC2_cfgt = (int32_t)greaterThan; // greater or less than?
    atomic::setBitFlag(ADC_SC2, ADC_SC2_ACFE);
    atomic::changeBitFlag(ADC_SC2, ADC_SC2_ACFGT, ADC_SC2_ACFGT*greaterThan);

    ADC_CV1 = (int16_t)compValue; // comp value
}

/* Enable the compare function: A conversion will be completed only when the ADC value
*  is inside (insideRange=1) or outside (=0) the range given by (lowerLimit, upperLimit),
*  including (inclusive=1) the limits or not (inclusive=0).
*  See Table 31-78, p. 617 of the freescale manual.
*  Call it after changing the resolution
*/
void ADC_Module::enableCompareRange(int16_t lowerLimit, int16_t upperLimit, bool insideRange, bool inclusive) {

    if (calibrating) wait_for_cal(); // if we modify the adc's registers when calibrating, it will fail

    // ADC_SC2_cfe = 1; // enable compare
    // ADC_SC2_cren = 1; // enable compare range
    atomic::setBitFlag(ADC_SC2, ADC_SC2_ACFE);
    atomic::setBitFlag(ADC_SC2, ADC_SC2_ACREN);

    if(insideRange && inclusive) { // True if value is inside the range, including the limits. CV1 <= CV2 and ACFGT=1
        // ADC_SC2_cfgt = 1;
        atomic::setBitFlag(ADC_SC2, ADC_SC2_ACFGT);

        ADC_CV1 = (int16_t)lowerLimit;
        ADC_CV2 = (int16_t)upperLimit;
    } else if(insideRange && !inclusive) {// True if value is inside the range, excluding the limits. CV1 > CV2 and ACFGT=0
        // ADC_SC2_cfgt = 0;
        atomic::clearBitFlag(ADC_SC2, ADC_SC2_ACFGT);

        ADC_CV2 = (int16_t)lowerLimit;
        ADC_CV1 = (int16_t)upperLimit;
    } else if(!insideRange && inclusive) { // True if value is outside of range or is equal to either limit. CV1 > CV2 and ACFGT=1
        // ADC_SC2_cfgt = 1;
        atomic::setBitFlag(ADC_SC2, ADC_SC2_ACFGT);

        ADC_CV2 = (int16_t)lowerLimit;
        ADC_CV1 = (int16_t)upperLimit;
    } else if(!insideRange && !inclusive) { // True if value is outside of range and not equal to either limit. CV1 > CV2 and ACFGT=0
        // ADC_SC2_cfgt = 0;
        atomic::clearBitFlag(ADC_SC2, ADC_SC2_ACFGT);

        ADC_CV1 = (int16_t)lowerLimit;
        ADC_CV2 = (int16_t)upperLimit;
    }
}

/* Disable the compare function
*
*/
void ADC_Module::disableCompare() {

    // ADC_SC2_cfe = 0;
    atomic::clearBitFlag(ADC_SC2, ADC_SC2_ACFE);
}

/* Enables the PGA and sets the gain
*   Use only for signals lower than 1.2 V
*   \param gain can be 1, 2, 4, 8, 16 32 or 64
*
*/
void ADC_Module::enablePGA(uint8_t gain) {
#if ADC_USE_PGA

    if (calibrating) wait_for_cal();

    uint8_t setting;
    if(gain <= 1) {
        setting = 0;
    } else if(gain<=2) {
        setting = 1;
    } else if(gain<=4) {
        setting = 2;
    } else if(gain<=8) {
        setting = 3;
    } else if(gain<=16) {
        setting = 4;
    } else if(gain<=32) {
        setting = 5;
    } else { // 64
        setting = 6;
    }

    ADC_PGA = ADC_PGA_PGAEN | ADC_PGA_PGAG(setting);
    pga_value=1<<setting;
#endif
}

/* Returns the PGA level
*  PGA level = from 0 to 64
*/
uint8_t ADC_Module::getPGA() {
    return pga_value;
}

//! Disable PGA
void ADC_Module::disablePGA() {
#if ADC_USE_PGA
    // ADC_PGA_pgaen = 0;
    atomic::clearBitFlag(ADC_PGA, ADC_PGA_PGAEN);
#endif
    pga_value = 1;
}


//////////////// INFORMATION ABOUT VALID PINS //////////////////

// check whether the pin is a valid analog pin
bool ADC_Module::checkPin(uint8_t pin) {

    if(pin>ADC_MAX_PIN) {
        return false;   // all others are invalid
    }

    // translate pin number to SC1A number, that also contains MUX a or b info.
    const uint8_t sc1a_pin = channel2sc1a[pin];

    // check for valid pin
    if( (sc1a_pin&ADC_SC1A_CHANNELS) == ADC_SC1A_PIN_INVALID ) {
        return false;   // all others are invalid
    }

    return true;
}

// check whether the pins are a valid analog differential pins (including PGA if enabled)
bool ADC_Module::checkDifferentialPins(uint8_t pinP, uint8_t pinN) {
    if(pinP>ADC_MAX_PIN) {
        return false;   // all others are invalid
    }

    // translate pinP number to SC1A number, to make sure it's differential
    uint8_t sc1a_pin = channel2sc1a[pinP];

    if( !(sc1a_pin&ADC_SC1A_PIN_DIFF) ) {
        return false;   // all others are invalid
    }

    // get SC1A number, also whether it can do PGA
    sc1a_pin = getDifferentialPair(pinP);

    // the pair can't be measured with this ADC
    if( (sc1a_pin&ADC_SC1A_CHANNELS) == ADC_SC1A_PIN_INVALID ) {
        return false;   // all others are invalid
    }

    #if ADC_USE_PGA
    // check if PGA is enabled, and whether the pin has access to it in this ADC module
    if( isPGAEnabled() && !(sc1a_pin&ADC_SC1A_PIN_PGA) ) {
        return false;
    }
    #endif // ADC_USE_PGA

    return true;
}


//////////////// HELPER METHODS FOR CONVERSION /////////////////

// Starts a single-ended conversion on the pin (sets the mux correctly)
// Doesn't do any of the checks on the pin
// It doesn't change the continuous conversion bit
void ADC_Module::startReadFast(uint8_t pin) {

    // translate pin number to SC1A number, that also contains MUX a or b info.
    const uint8_t sc1a_pin = channel2sc1a[pin];

    if(sc1a_pin&ADC_SC1A_PIN_MUX) { // mux a
        atomic::clearBitFlag(ADC_CFG2, ADC_CFG2_MUXSEL);
    } else { // mux b
        atomic::setBitFlag(ADC_CFG2, ADC_CFG2_MUXSEL);
    }

    // select pin for single-ended mode and start conversion, enable interrupts if requested
    __disable_irq();
    ADC_SC1A = (sc1a_pin&ADC_SC1A_CHANNELS) + atomic::getBitFlag(ADC_SC1A, ADC_SC1_AIEN)*ADC_SC1_AIEN;
    __enable_irq();

}

// Starts a differential conversion on the pair of pins
// Doesn't do any of the checks on the pins
// It doesn't change the continuous conversion bit
void ADC_Module::startDifferentialFast(uint8_t pinP, uint8_t pinN) {

    // get SC1A number
     uint8_t sc1a_pin = getDifferentialPair(pinP);

    #if ADC_USE_PGA
    // check if PGA is enabled
    if(isPGAEnabled()) {
        sc1a_pin = 0x2; // PGA always uses DAD2
    }
    #endif // ADC_USE_PGA

    __disable_irq();
    ADC_SC1A = ADC_SC1_DIFF + (sc1a_pin&ADC_SC1A_CHANNELS) + atomic::getBitFlag(ADC_SC1A, ADC_SC1_AIEN)*ADC_SC1_AIEN;
    __enable_irq();

}



//////////////// BLOCKING CONVERSION METHODS //////////////////
/*
    This methods are implemented like this:

    1. Check that the pin is correct
    2. if calibrating, wait for it to finish before modifiying any ADC register
    3. Check if we're interrupting a measurement, if so store the settings.
    4. Disable continuous conversion mode and start the current measurement
    5. Wait until it's done, and check whether the comparison (if any) was succesful.
    6. Get the result.
    7. If step 3. is true, restore the previous ADC settings

*/


/* Reads the analog value of the pin.
* It waits until the value is read and then returns the result.
* If a comparison has been set up and fails, it will return ADC_ERROR_VALUE.
* Set the resolution, number of averages and voltage reference using the appropriate functions.
*/
int ADC_Module::analogRead(uint8_t pin) {

    //digitalWriteFast(LED_BUILTIN, HIGH);

    // check whether the pin is correct
    if(!checkPin(pin)) {
        fail_flag |= ADC_ERROR::WRONG_PIN;
        return ADC_ERROR_VALUE;
    }

    // increase the counter of measurements
    num_measurements++;

    //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));

    if (calibrating) wait_for_cal();

    //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));

    // check if we are interrupting a measurement, store setting if so.
    // vars to save the current state of the ADC in case it's in use
    ADC_Config old_config = {0};
    const uint8_t wasADCInUse = isConverting(); // is the ADC running now?

    if(wasADCInUse) { // this means we're interrupting a conversion
        // save the current conversion config, we don't want any other interrupts messing up the configs
        __disable_irq();
        //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN) );
        saveConfig(&old_config);
        __enable_irq();
    }


    // no continuous mode
    singleMode();

    startReadFast(pin); // start single read

    // wait for the ADC to finish
    while(isConverting()) {
        yield();
    }

    // it's done, check if the comparison (if any) was true
    int32_t result;
    __disable_irq(); // make sure nothing interrupts this part
    if (isComplete()) { // conversion succeded
        result = (uint16_t)ADC_RA;
    } else { // comparison was false
        fail_flag |= ADC_ERROR::COMPARISON;
        result = ADC_ERROR_VALUE;
    }
    __enable_irq();

    // if we interrupted a conversion, set it again
    if (wasADCInUse) {
        //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN) );
        __disable_irq();
        loadConfig(&old_config);
        __enable_irq();
    }

    num_measurements--;
    return result;

} // analogRead



/* Reads the differential analog value of two pins (pinP - pinN)
* It waits until the value is read and then returns the result
* If a comparison has been set up and fails, it will return ADC_ERROR_DIFF_VALUE
* Set the resolution, number of averages and voltage reference using the appropriate functions
*/
int ADC_Module::analogReadDifferential(uint8_t pinP, uint8_t pinN) {

    if(!checkDifferentialPins(pinP, pinN)) {
        fail_flag |= ADC_ERROR::WRONG_PIN;
        return ADC_ERROR_VALUE;   // all others are invalid
    }

    // increase the counter of measurements
    num_measurements++;

    // check for calibration before setting channels,
    // because conversion will start as soon as we write to ADC_SC1A
    if (calibrating) wait_for_cal();

    uint8_t res = getResolution();

    // vars to saved the current state of the ADC in case it's in use
    ADC_Config old_config = {0};
    uint8_t wasADCInUse = isConverting(); // is the ADC running now?

    if(wasADCInUse) { // this means we're interrupting a conversion
        // save the current conversion config, we don't want any other interrupts messing up the configs
        __disable_irq();
        saveConfig(&old_config);
        __enable_irq();
    }

    // no continuous mode
    singleMode();

    startDifferentialFast(pinP, pinN); // start conversion

    // wait for the ADC to finish
    while( isConverting() ) {
        yield();
        //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN) );
    }

    // it's done, check if the comparison (if any) was true
    int32_t result;
    __disable_irq(); // make sure nothing interrupts this part
    if (isComplete()) { // conversion succeded
        result = (int16_t)(int32_t)ADC_RA; // cast to 32 bits
        if(res==16) { // 16 bit differential is actually 15 bit + 1 bit sign
            result *= 2; // multiply by 2 as if it were really 16 bits, so that getMaxValue gives a correct value.
        }
    } else { // comparison was false
        result = ADC_ERROR_VALUE;
        fail_flag |= ADC_ERROR::COMPARISON;
    }
    __enable_irq();

    // if we interrupted a conversion, set it again
    if (wasADCInUse) {
        __disable_irq();
        loadConfig(&old_config);
        __enable_irq();
    }

    num_measurements--;
    return result;

} // analogReadDifferential



/////////////// NON-BLOCKING CONVERSION METHODS //////////////
/*
    This methods are implemented like this:

    1. Check that the pin is correct
    2. if calibrating, wait for it to finish before modifiying any ADC register
    3. Check if we're interrupting a measurement, if so store the settings (in a member of the class, so it can be accessed).
    4. Disable continuous conversion mode and start the current measurement

    The fast methods only do step 4.

*/


/* Starts an analog measurement on the pin.
*  It returns inmediately, read value with readSingle().
*  If the pin is incorrect it returns false.
*/
bool ADC_Module::startSingleRead(uint8_t pin) {

    // check whether the pin is correct
    if(!checkPin(pin)) {
        fail_flag |= ADC_ERROR::WRONG_PIN;
        return false;
    }

    if (calibrating) wait_for_cal();

    // save the current state of the ADC in case it's in use
    adcWasInUse = isConverting(); // is the ADC running now?

    if(adcWasInUse) { // this means we're interrupting a conversion
        // save the current conversion config, the adc isr will restore the adc
        __disable_irq();
        saveConfig(&adc_config);
        __enable_irq();
    }

    // no continuous mode
    singleMode();

    // start measurement
    startReadFast(pin);

    return true;
}


/* Start a differential conversion between two pins (pinP - pinN).
* It returns inmediately, get value with readSingle().
* Incorrect pins will return false.
* Set the resolution, number of averages and voltage reference using the appropriate functions
*/
bool ADC_Module::startSingleDifferential(uint8_t pinP, uint8_t pinN) {

    if(!checkDifferentialPins(pinP, pinN)) {
        fail_flag |= ADC_ERROR::WRONG_PIN;
        return false;   // all others are invalid
    }

    // check for calibration before setting channels,
    // because conversion will start as soon as we write to ADC_SC1A
    if (calibrating) wait_for_cal();

    // vars to saved the current state of the ADC in case it's in use
    adcWasInUse = isConverting(); // is the ADC running now?

    if(adcWasInUse) { // this means we're interrupting a conversion
        // save the current conversion config, we don't want any other interrupts messing up the configs
        __disable_irq();
        saveConfig(&adc_config);
        __enable_irq();
    }

    // no continuous mode
    singleMode();

    // start the conversion
    startDifferentialFast(pinP, pinN);

    return true;
}



///////////// CONTINUOUS CONVERSION METHODS ////////////
/*
    This methods are implemented like this:

    1. Check that the pin is correct
    2. If calibrating, wait for it to finish before modifiying any ADC register
    4. Enable continuous conversion mode and start the current measurement

*/

/* Starts continuous conversion on the pin
 * It returns as soon as the ADC is set, use analogReadContinuous() to read the values
 * Set the resolution, number of averages and voltage reference using the appropriate functions BEFORE calling this function
*/
bool ADC_Module::startContinuous(uint8_t pin) {

    // check whether the pin is correct
    if(!checkPin(pin)) {
        fail_flag |= ADC_ERROR::WRONG_PIN;
        return false;
    }

    // check for calibration before setting channels,
    if (calibrating) wait_for_cal();

    // increase the counter of measurements
    num_measurements++;

    // set continuous conversion flag
    continuousMode();

    startReadFast(pin);

    return true;
}


/* Starts continuous and differential conversion between the pins (pinP-pinN)
 * It returns as soon as the ADC is set, use analogReadContinuous() to read the value
 * Set the resolution, number of averages and voltage reference using the appropriate functions BEFORE calling this function
*/
bool ADC_Module::startContinuousDifferential(uint8_t pinP, uint8_t pinN) {

    if(!checkDifferentialPins(pinP, pinN)) {
        fail_flag |= ADC_ERROR::WRONG_PIN;
        return false;   // all others are invalid
    }

    // increase the counter of measurements
    num_measurements++;

    // check for calibration before setting channels,
    // because conversion will start as soon as we write to ADC_SC1A
    if (calibrating) wait_for_cal();

    // save the current state of the ADC in case it's in use
    uint8_t wasADCInUse = isConverting(); // is the ADC running now?

    if(wasADCInUse) { // this means we're interrupting a conversion
        // save the current conversion config, we don't want any other interrupts messing up the configs
        __disable_irq();
        saveConfig(&adc_config);
        __enable_irq();
    }

    // set continuous mode
    continuousMode();

    // start conversions
    startDifferentialFast(pinP, pinN);

    return true;
}


/* Stops continuous conversion
*/
void ADC_Module::stopContinuous() {

    // set channel select to all 1's (31) to stop it.
    ADC_SC1A = ADC_SC1A_PIN_INVALID + atomic::getBitFlag(ADC_SC1A, ADC_SC1_AIEN)*ADC_SC1_AIEN;

    // decrease the counter of measurements (unless it's 0)
    if(!num_measurements) {
        num_measurements--;
    }


    return;
}

//////////// PDB ////////////////
//// Only works for Teensy 3.0 and 3.1, not LC (it doesn't have PDB)

#if ADC_USE_PDB

// frequency in Hz
void ADC_Module::startPDB(uint32_t freq) {
    if (!(SIM_SCGC6 & SIM_SCGC6_PDB)) { // setup PDB
        SIM_SCGC6 |= SIM_SCGC6_PDB; // enable pdb clock
    }

    if(freq>F_BUS) return; // too high
    if(freq<1) return; // too low

    // mod will have to be a 16 bit value
    // we detect if it's higher than 0xFFFF and scale it back accordingly.
    uint32_t mod = (F_BUS / freq);

    uint8_t prescaler = 0; // from 0 to 7: factor of 1, 2, 4, 8, 16, 32, 64 or 128
    uint8_t mult = 0; // from 0 to 3, factor of 1, 10, 20 or 40

    // if mod is too high we need to use prescaler and mult to bring it down to a 16 bit number
    const uint32_t min_level = 0xFFFF;
    if(mod>min_level) {
        if( mod < 2*min_level ) {
                prescaler = 1;
        }
        else if( mod < 4*min_level ) {
                prescaler = 2;
        }
        else if( mod < 8*min_level ) {
                prescaler = 3;
        }
        else if( mod < 10*min_level ) {
                mult = 1;
        }
        else if( mod < 16*min_level ) {
                prescaler = 4;
        }
        else if( mod < 20*min_level ) {
                mult = 2;
        }
        else if( mod < 32*min_level ) {
                prescaler = 5;
        }
        else if( mod < 40*min_level ) {
                mult = 3;
        }
        else if( mod < 64*min_level ) {
                prescaler = 6;
        }
        else if( mod < 128*min_level ) {
                prescaler = 7;
        }
        else if( mod < 160*min_level ) { // 16*10
                prescaler = 4;
                mult = 1;
        }
        else if( mod < 320*min_level ) { // 16*20
                prescaler = 4;
                mult = 2;
        }
        else if( mod < 640*min_level ) { // 16*40
                prescaler = 4;
                mult = 3;
        }
        else if( mod < 1280*min_level ) { // 32*40
                prescaler = 5;
                mult = 3;
        }
        else if( mod < 2560*min_level ) { // 64*40
                prescaler = 6;
                mult = 3;
        }
        else if( mod < 5120*min_level ) { // 128*40
                prescaler = 7;
                mult = 3;
        }
        else { // frequency too low
            return;
        }

        mod >>= prescaler;
        if(mult>0) {
                mod /= 10;
                mod >>= (mult-1);
        }
    }

    setHardwareTrigger(); // trigger ADC with hardware

    //                                   software trigger    enable PDB     PDB interrupt  continuous mode load immediately
    constexpr uint32_t ADC_PDB_CONFIG = PDB_SC_TRGSEL(15) | PDB_SC_PDBEN | PDB_SC_PDBIE | PDB_SC_CONT |   PDB_SC_LDMOD(0);

    constexpr uint32_t PDB_CHnC1_TOS_1 = 0x0100;
    constexpr uint32_t PDB_CHnC1_EN_1 = 0x01;

    PDB0_IDLY = 1; // the pdb interrupt happens when IDLY is equal to CNT+1

    PDB0_MOD = (uint16_t)(mod-1);

    PDB0_SC = ADC_PDB_CONFIG | PDB_SC_PRESCALER(prescaler) | PDB_SC_MULT(mult) | PDB_SC_LDOK; // load all new values

    PDB0_SC = ADC_PDB_CONFIG | PDB_SC_PRESCALER(prescaler) | PDB_SC_MULT(mult) | PDB_SC_SWTRIG; // start the counter!

    PDB0_CHnC1 = PDB_CHnC1_TOS_1 | PDB_CHnC1_EN_1; // enable pretrigger 0 (SC1A)

    //NVIC_ENABLE_IRQ(IRQ_PDB);

}

void ADC_Module::stopPDB() {
    if (!(SIM_SCGC6 & SIM_SCGC6_PDB)) { // if PDB clock wasn't on, return
        setSoftwareTrigger();
        return;
    }
    PDB0_SC = 0;
    setSoftwareTrigger();

    //NVIC_DISABLE_IRQ(IRQ_PDB);
}

//! Return the PDB's frequency
uint32_t ADC_Module::getPDBFrequency() {
    const uint32_t mod = (uint32_t)PDB0_MOD;
    const uint8_t prescaler = (PDB0_SC&0x7000)>>12;
    const uint8_t mult = (PDB0_SC&0xC)>>2;

    const uint32_t freq = uint32_t((mod + 1)<<(prescaler)) * uint32_t((mult==0) ? 1 : 10<<(mult-1));
    return F_BUS/freq;
}

#endif
