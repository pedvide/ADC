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

/* ADC_Module.cpp: Implements the fuctions of a Teensy 3.x ADC module
 *
 */



#include "ADC_Module.h"
#include "ADC.h"


/* Constructor
*   Point the registers to the correct ADC module
*   Copy the correct channel2sc1a and sc1a2channel
*   Call init
*/
ADC_Module::ADC_Module(uint8_t ADC_number) {

    // ADC0 or ADC1?
    ADC_num = ADC_number;

    // point the control registers to the correct addresses
    // use bitband where necessary
    uint32_t adc_offset = (uint32_t)0x20000;

    ADC_SC1A = &ADC0_SC1A + adc_offset*ADC_num;
        ADC_SC1A_coco = adc_bitband((uint32_t)ADC_SC1A, 7); // conversion complete
        ADC_SC1A_aien = adc_bitband((uint32_t)ADC_SC1A, 6); // interrupts enabled
    ADC_SC1B = &ADC0_SC1B + adc_offset*ADC_num;

    ADC_CFG1 = &ADC0_CFG1 + adc_offset*ADC_num;
        ADC_CFG1_adlpc = adc_bitband((uint32_t)ADC_CFG1, 7); // low power conf.
        ADC_CFG1_adiv1 = adc_bitband((uint32_t)ADC_CFG1, 6); // divide input clock
        ADC_CFG1_adiv0 = adc_bitband((uint32_t)ADC_CFG1, 5); //
        ADC_CFG1_adlsmp = adc_bitband((uint32_t)ADC_CFG1, 4); // low sampling speed
        ADC_CFG1_mode1 = adc_bitband((uint32_t)ADC_CFG1, 3); // resolution mode
        ADC_CFG1_mode0 = adc_bitband((uint32_t)ADC_CFG1, 2); //
        ADC_CFG1_adiclk1 = adc_bitband((uint32_t)ADC_CFG1, 1); // input clock
        ADC_CFG1_adiclk0 = adc_bitband((uint32_t)ADC_CFG1, 0); //
    ADC_CFG2 = &ADC0_CFG2 + adc_offset*ADC_num;
        ADC_CFG2_muxsel = adc_bitband((uint32_t)ADC_CFG2, 4); // mux to select a or b channels
        ADC_CFG2_adacken = adc_bitband((uint32_t)ADC_CFG2, 3); // enable the async. clock
        ADC_CFG2_adhsc = adc_bitband((uint32_t)ADC_CFG2, 2); // high-speed config: add 2 ADCK
        ADC_CFG2_adlsts1 = adc_bitband((uint32_t)ADC_CFG2, 1); // loger sampling time
        ADC_CFG2_adlsts0 = adc_bitband((uint32_t)ADC_CFG2, 0);

    ADC_RA = &ADC0_RA + adc_offset*ADC_num;
    ADC_RB = &ADC0_RB + adc_offset*ADC_num;

    ADC_CV1 = &ADC0_CV1 + adc_offset*ADC_num;
    ADC_CV2 = &ADC0_CV2 + adc_offset*ADC_num;

    ADC_SC2 = &ADC0_SC2 + adc_offset*ADC_num;
        ADC_SC2_adact = adc_bitband((uint32_t)ADC_SC2, 7); // conversion active
        ADC_SC2_cfe = adc_bitband((uint32_t)ADC_SC2, 5); // compare function enable, greater than and range enable
        ADC_SC2_cfgt = adc_bitband((uint32_t)ADC_SC2, 4);
        ADC_SC2_cren = adc_bitband((uint32_t)ADC_SC2, 3);
        ADC_SC2_dma = adc_bitband((uint32_t)ADC_SC2, 2); // dma enable
        ADC_SC2_ref = adc_bitband((uint32_t)ADC_SC2, 0); // refsel only uses bit 0, not really bit 1.


    ADC_SC3 = &ADC0_SC3 + adc_offset*ADC_num;
        ADC_SC3_cal = adc_bitband((uint32_t)ADC_SC3, 7); // start/stop calibration
        ADC_SC3_calf = adc_bitband((uint32_t)ADC_SC3, 6); // calibration failed flag
        ADC_SC3_adco = adc_bitband((uint32_t)ADC_SC3, 3); // continuous conversion
        ADC_SC3_avge = adc_bitband((uint32_t)ADC_SC3, 2); // enable averages bit
        ADC_SC3_avgs0 = adc_bitband((uint32_t)ADC_SC3, 0); // num of averages bits
        ADC_SC3_avgs1 = adc_bitband((uint32_t)ADC_SC3, 1);

    ADC_PGA = &ADC0_PGA + adc_offset*ADC_num; ADC_PGA_pgaen = adc_bitband((uint32_t)ADC_PGA, 23); // enable pga

    ADC_OFS = &ADC0_OFS + adc_offset*ADC_num;
    ADC_PG = &ADC0_PG + adc_offset*ADC_num;
    ADC_MG = &ADC0_MG + adc_offset*ADC_num;
    ADC_CLPD = &ADC0_CLPD + adc_offset*ADC_num;
    ADC_CLPS = &ADC0_CLPS + adc_offset*ADC_num;
    ADC_CLP4 = &ADC0_CLP4 + adc_offset*ADC_num;
    ADC_CLP3 = &ADC0_CLP3 + adc_offset*ADC_num;
    ADC_CLP2 = &ADC0_CLP2 + adc_offset*ADC_num;
    ADC_CLP1 = &ADC0_CLP1 + adc_offset*ADC_num;
    ADC_CLP0 = &ADC0_CLP0 + adc_offset*ADC_num;
    ADC_CLMD = &ADC0_CLMD + adc_offset*ADC_num;
    ADC_CLMS = &ADC0_CLMS + adc_offset*ADC_num;
    ADC_CLM4 = &ADC0_CLM4 + adc_offset*ADC_num;
    ADC_CLM3 = &ADC0_CLM3 + adc_offset*ADC_num;
    ADC_CLM2 = &ADC0_CLM2 + adc_offset*ADC_num;
    ADC_CLM1 = &ADC0_CLM1 + adc_offset*ADC_num;
    ADC_CLM0 = &ADC0_CLM0 + adc_offset*ADC_num;

    // copy the correct version of channel2sc1a and sc1a2channel depending on the ADC module this instance is:
    #if defined(__MK20DX128__)
    // Tennsy 3.0, only ADC0
    memcpy(&channel2sc1a, &ADC::channel2sc1aADC0, sizeof(channel2sc1a));
    memcpy(&sc1a2channel, &ADC::sc1a2channelADC0, sizeof(sc1a2channel));
    #elif defined(__MK20DX256__)
    // Teensy 3.1: can be ADC0 or ADC1
    if(ADC_num==0) { // ADC0
        memcpy(&channel2sc1a, &ADC::channel2sc1aADC0, sizeof(channel2sc1a));
        memcpy(&sc1a2channel, &ADC::sc1a2channelADC0, sizeof(sc1a2channel));
    } else { // ADC1
        memcpy(&channel2sc1a, &ADC::channel2sc1aADC1, sizeof(channel2sc1a));
        memcpy(&sc1a2channel, &ADC::sc1a2channelADC1, sizeof(sc1a2channel));
    }
    #endif

    // call our init
    analog_init();

}

/* Initialize stuff:
*  - Start Vref module
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
    analog_reference_internal = 1;
    var_enableInterrupts = 0;
    pga_value = 1;

    conversion_speed = 0;
    sampling_speed =  0;

    // the first calibration will use 32 averages and lowest speed,
    // when this calibration is over the averages and speed will be set to default by wait_for_cal and init_calib will be cleared.
    init_calib = 1;

    fail_flag = ADC_ERROR_CLEAR; // clear all errors

    // Internal reference initialization
    VREF_TRM = VREF_TRM_CHOPEN | 0x20; // enable module and set the trimmer to medium (max=0x3F=63)
	VREF_SC = VREF_SC_VREFEN | VREF_SC_REGEN | VREF_SC_ICOMPEN | VREF_SC_MODE_LV(1); // (=0xE1) enable 1.2 volt ref with all compensations

	// select b channels
	*ADC_CFG2_muxsel = 1;

    // set reference to vcc/external
	setReference(ADC_REF_EXTERNAL);

    // set resolution to 10
    setResolution(10);

    // calibrate at low speed and max repetitions
    setAveraging(32);
    setConversionSpeed(ADC_LOW_SPEED);
    setSamplingSpeed(ADC_LOW_SPEED);
    // begin init calibration
	calibrate();
}

// starts calibration
void ADC_Module::calibrate() {

    __disable_irq();

    calibrating = 1;
    *ADC_SC3_cal = 0; // stop possible previous calibration
    *ADC_SC3_calf = 1; // clear possible previous error
    //*ADC_SC3 |= ADC_SC3_CAL;
    *ADC_SC3_cal = 1; // start calibration

    __enable_irq();
}


/* Waits until calibration is finished and writes the corresponding registers
*
*/
void ADC_Module::wait_for_cal(void)
{
	uint16_t sum;

	while(*ADC_SC3_cal) { // Bit ADC_SC3_CAL in register ADC0_SC3 cleared when calib. finishes.

	}
	if(*ADC_SC3_calf) { // calibration failed
        fail_flag |= ADC_ERROR_CALIB; // the user should know and recalibrate manually
	}

	__disable_irq();
	if (calibrating) {
		sum = *ADC_CLPS + *ADC_CLP4 + *ADC_CLP3 + *ADC_CLP2 + *ADC_CLP1 + *ADC_CLP0;
		sum = (sum / 2) | 0x8000;
		*ADC_PG = sum;

		sum = *ADC_CLMS + *ADC_CLM4 + *ADC_CLM3 + *ADC_CLM2 + *ADC_CLM1 + *ADC_CLM0;
		sum = (sum / 2) | 0x8000;
		*ADC_MG = sum;

		calibrating = 0;
	}
	__enable_irq();

	// the first calibration uses 32 averages and lowest speed,
    // when this calibration is over, set the averages and speed to default.
	if(init_calib) {

	    // set conversion speed to medium
        setConversionSpeed(ADC_MED_SPEED);

        // set sampling speed to medium
        setSamplingSpeed(ADC_MED_SPEED);

        // number of averages to 4
        setAveraging(4);

        init_calib = 0; // clear
	}

}

//! Starts the calibration sequence, waits until it's done and writes the results
/** Usually it's not necessary to call this function directly, but do it if the "enviroment" changed
*   significantly since the program was started.
*/
void ADC_Module::recalibrate() {

    calibrate();

    wait_for_cal();
}



/////////////// METHODS TO SET/GET SETTINGS OF THE ADC ////////////////////


/* Set the voltage reference you prefer, default is vcc
*   It needs to recalibrate
*/
void ADC_Module::setReference(uint8_t type)
{
    if (analog_reference_internal==type) { // don't need to change anything
        return;
    }

	if (type == ADC_REF_INTERNAL) { // 1.2V ref
		// internal reference requested
        analog_reference_internal = ADC_REF_INTERNAL;
        //*ADC_SC2 |= ADC_SC2_REFSEL(1);
        *ADC_SC2_ref = 1; // uses bitband: atomic
	} else if(type == ADC_REF_EXTERNAL) { // vcc/ext ref
		// vcc or external reference requested
        analog_reference_internal = ADC_REF_EXTERNAL;
        //*ADC_SC2 |= ADC_SC2_REFSEL(0);
        *ADC_SC2_ref = 0; // uses bitband: atomic
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

    if (bits <8) {
		config = 8;
	} else if (bits >= 14) {
		config = 16;
	} else {
		config = bits;
	}

    // conversion resolution
    // single-ended 8 bits is the same as differential 9 bits, etc.
    if ( (config == 8) || (config == 9) )  {
        *ADC_CFG1_mode1 = 0;
        *ADC_CFG1_mode0 = 0;
        analog_max_val = 255; // diff mode 9 bits has 1 bit for sign, so max value is the same as single 8 bits
    } else if ( (config == 10 )|| (config == 11) ) {
        *ADC_CFG1_mode1 = 1;
        *ADC_CFG1_mode0 = 0;
        analog_max_val = 1023;
    } else if ( (config == 12 )|| (config == 13) ) {
        *ADC_CFG1_mode1 = 0;
        *ADC_CFG1_mode0 = 1;
        analog_max_val = 4095;
    } else {
        *ADC_CFG1_mode1 = 1;
        *ADC_CFG1_mode0 = 1;
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
/*
* \param speed can be ADC_VERY_LOW_SPEED, ADC_LOW_SPEED, ADC_MED_SPEED, ADC_HIGH_SPEED_16BITS, ADC_HIGH_SPEED or ADC_VERY_HIGH_SPEED
    ADC_VERY_LOW_SPEED is guaranteed to be the lowest possible speed within specs for resolutions less than 16 bits (higher than 1 MHz),
    it's different from ADC_LOW_SPEED only for 24, 4 or 2 MHz.
    ADC_LOW_SPEED is guaranteed to be the lowest possible speed within specs for all resolutions (higher than 2 MHz).
    ADC_MED_SPEED is always >= ADC_LOW_SPEED and <= ADC_HIGH_SPEED.
    ADC_HIGH_SPEED_16BITS is guaranteed to be the highest possible speed within specs for all resolutions (lower or eq than 12 MHz).
    ADC_HIGH_SPEED is guaranteed to be the highest possible speed within specs for resolutions less than 16 bits (lower or eq than 18 MHz).
    ADC_VERY_HIGH_SPEED may be out of specs, it's different from ADC_HIGH_SPEED only for 48, 40 or 24 MHz.
* It doesn't recalibrate at the end.
*/
void ADC_Module::setConversionSpeed(uint8_t speed) {

    if(speed==conversion_speed) { // no change
        return;
    }

    if (calibrating) wait_for_cal();

    // in the future allow the user to select this clock source
    *ADC_CFG2_adacken = 0; // disable the internal asynchronous clock

    uint32_t ADC_CFG1_speed; // store the clock and divisor

    if(speed == ADC_VERY_LOW_SPEED) {
        *ADC_CFG2_adhsc = 0; // no high-speed config
        *ADC_CFG1_adlpc  = 1; // use low power conf.

        ADC_CFG1_speed = ADC_CFG1_VERY_LOW_SPEED;

    } else if(speed == ADC_LOW_SPEED) {
        *ADC_CFG2_adhsc = 0; // no high-speed config
        *ADC_CFG1_adlpc  = 1; // use low power conf.

        ADC_CFG1_speed = ADC_CFG1_LOW_SPEED;

    } else if(speed == ADC_MED_SPEED) {
        *ADC_CFG2_adhsc = 0; // no high-speed config
        *ADC_CFG1_adlpc  = 0; // no low power conf.

        ADC_CFG1_speed = ADC_CFG1_MED_SPEED;

    } else if(speed == ADC_HIGH_SPEED_16BITS) {
        *ADC_CFG2_adhsc = 1; // high-speed config: add 2 ADCK
        *ADC_CFG1_adlpc  = 0; // no low power conf.

        ADC_CFG1_speed = ADC_CFG1_HI_SPEED_16_BITS;

    } else if(speed == ADC_HIGH_SPEED) {
        *ADC_CFG2_adhsc = 1; // high-speed config: add 2 ADCK
        *ADC_CFG1_adlpc  = 0; // no low power conf.

        ADC_CFG1_speed = ADC_CFG1_HI_SPEED;

    } else if(speed == ADC_VERY_HIGH_SPEED) { // this speed is most likely out of specs, so accurancy can be bad
        *ADC_CFG2_adhsc = 1; // high-speed config: add 2 ADCK
        *ADC_CFG1_adlpc  = 0; // no low power conf.

        ADC_CFG1_speed = ADC_CFG1_VERY_HIGH_SPEED;

    } else {
        fail_flag |= ADC_ERROR_OTHER;
        return;
    }

    // clock source is bus or bus/2
    *ADC_CFG1_adiclk1 = !!(ADC_CFG1_speed & ADC_CFG1_ADICLK_MASK_1); // !!x converts the number x to either 0 or 1.
    *ADC_CFG1_adiclk0 = !!(ADC_CFG1_speed & ADC_CFG1_ADICLK_MASK_0);

    // divisor for the clock source: 1, 2, 4 or 8.
    // so total speed can be: bus, bus/2, bus/4, bus/8 or bus/16.
    *ADC_CFG1_adiv1 = !!(ADC_CFG1_speed & ADC_CFG1_ADIV_MASK_1);
    *ADC_CFG1_adiv0 = !!(ADC_CFG1_speed & ADC_CFG1_ADIV_MASK_0);

    conversion_speed = speed;

}


// Sets the sampling speed
/* Increase the sampling speed for low impedance sources, decrease it for higher impedance ones.
* \param speed can be ADC_VERY_LOW_SPEED, ADC_LOW_SPEED, ADC_MED_SPEED, ADC_HIGH_SPEED or ADC_VERY_HIGH_SPEED
    ADC_VERY_LOW_SPEED is the lowest possible sampling speed (+24 ADCK).
    ADC_LOW_SPEED adds +16 ADCK.
    ADC_MED_SPEED adds +10 ADCK.
    ADC_HIGH_SPEED (or ADC_HIGH_SPEED_16BITS) adds +6 ADCK.
    ADC_VERY_HIGH_SPEED is the highest possible sampling speed (0 ADCK added).
* It doesn't recalibrate at the end.
*/
void ADC_Module::setSamplingSpeed(uint8_t speed) {

    if(speed==sampling_speed) { // no change
        return;
    }

    if (calibrating) wait_for_cal();

    // Select between the settings
    if(speed == ADC_VERY_LOW_SPEED) {
        *ADC_CFG1_adlsmp = 1; // long sampling time enable
        *ADC_CFG2_adlsts1 = 0; // maximum sampling time (+24 ADCK)
        *ADC_CFG2_adlsts0 = 0;

    } else if(speed == ADC_LOW_SPEED) {
        *ADC_CFG1_adlsmp = 1; // long sampling time enable
        *ADC_CFG2_adlsts1 = 0;// high sampling time (+16 ADCK)
        *ADC_CFG2_adlsts0 = 1;

    } else if(speed == ADC_MED_SPEED) {
        *ADC_CFG1_adlsmp = 1; // long sampling time enable
        *ADC_CFG2_adlsts1 = 1;// medium sampling time (+10 ADCK)
        *ADC_CFG2_adlsts0 = 0;

    } else if( (speed == ADC_HIGH_SPEED) || (speed == ADC_HIGH_SPEED_16BITS) ) {
        *ADC_CFG1_adlsmp = 1; // long sampling time enable
        *ADC_CFG2_adlsts1 = 1;// low sampling time (+6 ADCK)
        *ADC_CFG2_adlsts0 = 1;

    } else if(speed == ADC_VERY_HIGH_SPEED) {
        *ADC_CFG1_adlsmp = 0; // shortest sampling time

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
		//*ADC_SC3 &= ~ADC_SC3_AVGE;
		*ADC_SC3_avge = 0;
	} else {
	    *ADC_SC3_avge = 1;
	    if (num <= 4) {
            num = 4;
            //*ADC_SC3 |= ADC_SC3_AVGE + ADC_SC3_AVGS(0);
            *ADC_SC3_avgs0 = 0;
            *ADC_SC3_avgs1 = 0;
        } else if (num <= 8) {
            num = 8;
            //*ADC_SC3 |= ADC_SC3_AVGE + ADC_SC3_AVGS(1);
            *ADC_SC3_avgs0 = 1;
            *ADC_SC3_avgs1 = 0;
        } else if (num <= 16) {
            num = 16;
            //*ADC_SC3 |= ADC_SC3_AVGE + ADC_SC3_AVGS(2);
            *ADC_SC3_avgs0 = 0;
            *ADC_SC3_avgs1 = 1;
        } else {
            num = 32;
            //*ADC_SC3 |= ADC_SC3_AVGE + ADC_SC3_AVGS(3);
            *ADC_SC3_avgs0 = 1;
            *ADC_SC3_avgs1 = 1;
        }
	}
	analog_num_average = num;
}


/* Enable interrupts: An ADC Interrupt will be raised when the conversion is completed
*  (including hardware averages and if the comparison (if any) is true).
*/
void ADC_Module::enableInterrupts() {

    if (calibrating) wait_for_cal();

    var_enableInterrupts = 1;
    *ADC_SC1A_aien = 1;
    #if defined(__MK20DX128__)
    NVIC_ENABLE_IRQ(IRQ_ADC0);
    #elif defined(__MK20DX256__)
    if(ADC_num==1) { // enable correct interrupt
        NVIC_ENABLE_IRQ(IRQ_ADC1);
    } else {
        NVIC_ENABLE_IRQ(IRQ_ADC0);
    }
    #endif // defined
}

/* Disable interrupts
*
*/
void ADC_Module::disableInterrupts() {

    var_enableInterrupts = 0;
    *ADC_SC1A_aien = 0;
    #if defined(__MK20DX128__)
    NVIC_DISABLE_IRQ(IRQ_ADC0);
    #elif defined(__MK20DX256__)
    if(ADC_num==1) { // enable correct interrupt
        NVIC_DISABLE_IRQ(IRQ_ADC1);
    } else {
        NVIC_DISABLE_IRQ(IRQ_ADC0);
    }
    #endif // defined
}


/* Enable DMA request: An ADC DMA request will be raised when the conversion is completed
*  (including hardware averages and if the comparison (if any) is true).
*/
void ADC_Module::enableDMA() {

    if (calibrating) wait_for_cal();

    //*ADC_SC2 |= ADC_SC2_DMAEN;
    *ADC_SC2_dma = 1;
}

/* Disable ADC DMA request
*
*/
void ADC_Module::disableDMA() {

    //*ADC_SC2 &= ~ADC_SC2_DMAEN;
    *ADC_SC2_dma = 0;
}


/* Enable the compare function: A conversion will be completed only when the ADC value
*  is >= compValue (greaterThan=1) or < compValue (greaterThan=0)
*  Call it after changing the resolution
*  Use with interrupts or poll conversion completion with isADC_Complete()
*/
void ADC_Module::enableCompare(int16_t compValue, bool greaterThan) {

    if (calibrating) wait_for_cal(); // if we modify the adc's registers when calibrating, it will fail

    //*ADC_SC2 |= ADC_SC2_ACFE | greaterThan*ADC_SC2_ACFGT;
    *ADC_SC2_cfe = 1; // enable compare
    *ADC_SC2_cfgt = (int32_t)greaterThan; // greater or less than?
    *ADC_CV1 = (int16_t)compValue; // comp value
}

/* Enable the compare function: A conversion will be completed only when the ADC value
*  is inside (insideRange=1) or outside (=0) the range given by (lowerLimit, upperLimit),
*  including (inclusive=1) the limits or not (inclusive=0).
*  See Table 31-78, p. 617 of the freescale manual.
*  Call it after changing the resolution
*/
void ADC_Module::enableCompareRange(int16_t lowerLimit, int16_t upperLimit, bool insideRange, bool inclusive) {

    if (calibrating) wait_for_cal(); // if we modify the adc's registers when calibrating, it will fail

    *ADC_SC2_cfe = 1; // enable compare
    *ADC_SC2_cren = 1; // enable compare range

    if(insideRange && inclusive) { // True if value is inside the range, including the limits. CV1 <= CV2 and ACFGT=1
        //*ADC_SC2 |= ADC_SC2_ACFE | ADC_SC2_ACFGT | ADC_SC2_ACREN;
        *ADC_SC2_cfgt = 1;

        *ADC_CV1 = (int16_t)lowerLimit;
        *ADC_CV2 = (int16_t)upperLimit;
    } else if(insideRange && !inclusive) {// True if value is inside the range, excluding the limits. CV1 > CV2 and ACFGT=0
        //*ADC_SC2 |= ADC_SC2_ACFE | ADC_SC2_ACREN;
        *ADC_SC2_cfgt = 0;

        *ADC_CV2 = (int16_t)lowerLimit;
        *ADC_CV1 = (int16_t)upperLimit;
    } else if(!insideRange && inclusive) { // True if value is outside of range or is equal to either limit. CV1 > CV2 and ACFGT=1
        //*ADC_SC2 |= ADC_SC2_ACFE | ADC_SC2_ACFGT | ADC_SC2_ACREN;
        *ADC_SC2_cfgt = 1;

        *ADC_CV2 = (int16_t)lowerLimit;
        *ADC_CV1 = (int16_t)upperLimit;
    } else if(!insideRange && !inclusive) { // True if value is outside of range and not equal to either limit. CV1 > CV2 and ACFGT=0
        //*ADC_SC2 |= ADC_SC2_ACFE | ADC_SC2_ACREN;
        *ADC_SC2_cfgt = 0;

        *ADC_CV1 = (int16_t)lowerLimit;
        *ADC_CV2 = (int16_t)upperLimit;
    }
}

/* Disable the compare function
*
*/
void ADC_Module::disableCompare() {

    //*ADC_SC2 &= ~ADC_SC2_ACFE;
    *ADC_SC2_cfe = 0;
}

/* Enables the PGA and sets the gain
*   Use only for signals lower than 1.2 V
*   \param gain can be 1 (disabled), 2, 4, 8, 16 32 or 64
*
*/
void ADC_Module::enablePGA(uint8_t gain) {
    #if defined(__MK20DX256__)

    if (calibrating) wait_for_cal();

    uint8_t setting;
    if(gain <= 1) {
        disablePGA();
        return;
    } else if(gain<=2){
        setting = 1;
    } else if(gain<=4){
        setting = 2;
    } else if(gain<=8){
        setting = 3;
    } else if(gain<=16){
        setting = 4;
    } else if(gain<=32){
        setting = 5;
    } else { // 64
        setting = 6;
    }

    *ADC_PGA = ADC_PGA_PGAEN | ADC_PGA_PGAG(setting);
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
    #if defined(__MK20DX256__)
    //*ADC_PGA &= ~ADC_PGA_PGAEN;
    *ADC_PGA_pgaen = 0;
    #endif
    pga_value = 1;
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
int ADC_Module::analogRead(uint8_t pin)
{
	int32_t result;

	if ( (pin < 0) || (pin > 43) ) {
        fail_flag |= ADC_ERROR_WRONG_PIN;
        return ADC_ERROR_VALUE;   // all others are invalid
    }

    // increase the counter of measurements
	num_measurements++;

	if (calibrating) wait_for_cal();

    // check if we are interrupting a measurement, store setting if so.
    // vars to save the current state of the ADC in case it's in use
    ADC_Config old_config = {0};
    uint8_t wasADCInUse = isConverting(); // is the ADC running now?

    if(wasADCInUse) { // this means we're interrupting a conversion
        // save the current conversion config, we don't want any other interrupts messing up the configs
        __disable_irq();
        //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN) );
        old_config.savedSC1A = *ADC_SC1A;
        old_config.savedCFG1 = *ADC_CFG1;
        old_config.savedCFG2 = *ADC_CFG2;
        old_config.savedSC2 = *ADC_SC2;
        old_config.savedSC3 = *ADC_SC3;
        __enable_irq();
    }

	#if defined(__MK20DX256__)
	// ADC1 has A15=5a and A20=4a so we have to change the mux to read the "a" channels
	if( (ADC_num==1) && ( (pin==26) || (pin==31) ) ) { // mux a
        //*ADC_CFG2 &= ~ADC_CFG2_MUXSEL;
        *ADC_CFG2_muxsel = 0;
	} else { // mux b
        //*ADC_CFG2 |= ADC_CFG2_MUXSEL;
        *ADC_CFG2_muxsel = 1;
	}
	#endif

    // no continuous mode
    *ADC_SC3_adco = 0;

    __disable_irq();
    *ADC_SC1A = channel2sc1a[pin] + var_enableInterrupts*ADC_SC1_AIEN; // start conversion on pin and with interrupts enabled if requested
	__enable_irq();

	// wait for the ADC to finish
    while(isConverting()) {
        yield();
        //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN) );
    }

    // it's done, check if the comparison (if any) was true
    __disable_irq(); // make sure nothing interrupts this part
    if (isComplete()) { // conversion succeded
        result = (uint16_t)*ADC_RA;
    } else { // comparison was false
        fail_flag |= ADC_ERROR_COMPARISON;
        result = ADC_ERROR_VALUE;
    }
    __enable_irq();

    // if we interrupted a conversion, set it again
    if (wasADCInUse) {
        //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN) );
        *ADC_CFG1 = old_config.savedCFG1;
        *ADC_CFG2 = old_config.savedCFG2;
        *ADC_SC2 = old_config.savedSC2;
        *ADC_SC3 = old_config.savedSC3;
        *ADC_SC1A = old_config.savedSC1A;
    }

    num_measurements--;
    return result;

} // analogRead



/* Reads the differential analog value of two pins (pinP - pinN)
* It waits until the value is read and then returns the result
* If a comparison has been set up and fails, it will return ADC_ERROR_DIFF_VALUE
* Set the resolution, number of averages and voltage reference using the appropriate functions
*/
int ADC_Module::analogReadDifferential(uint8_t pinP, uint8_t pinN)
{
	int32_t result;

	if( ((pinP != A10) && (pinP != A12)) || ((pinN != A11) && (pinN != A13)) ) {
        fail_flag |= ADC_ERROR_WRONG_PIN;
        return ADC_ERROR_DIFF_VALUE;   // all others are invalid
	}

	// increase the counter of measurements
	num_measurements++;

	// check for calibration before setting channels,
	// because conversion will start as soon as we write to *ADC_SC1A
	if (calibrating) wait_for_cal();

	uint8_t res = getResolution();

    // vars to saved the current state of the ADC in case it's in use
    ADC_Config old_config = {0};
    uint8_t wasADCInUse = isConverting(); // is the ADC running now?

    if(wasADCInUse) { // this means we're interrupting a conversion
        // save the current conversion config, we don't want any other interrupts messing up the configs
        __disable_irq();
        //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN) );
        old_config.savedSC1A = *ADC_SC1A;
        old_config.savedCFG1 = *ADC_CFG1;
        old_config.savedCFG2 = *ADC_CFG2;
        old_config.savedSC2 = *ADC_SC2;
        old_config.savedSC3 = *ADC_SC3;
        __enable_irq();
    }

    // no continuous mode
    *ADC_SC3_adco = 0;

    // once *ADC_SC1A is set, conversion will start, enable interrupts if requested
	if ( (pinP == A10) && (pinN == A11) ) { // pins 34 and 35
        __disable_irq();
        #if defined(__MK20DX256__)
        if ( *ADC_PGA & ADC_PGA_PGAEN ) { // PGA is enabled
            if(ADC_num == 0) { // PGA0 connects to A10-A11
                *ADC_SC1A = ADC_SC1_DIFF + 0x2 + var_enableInterrupts*ADC_SC1_AIEN;
                __enable_irq();
            } else { // If this is ADC1 we can't connect to PGA0
                __enable_irq();
                fail_flag |= ADC_ERROR_WRONG_ADC;
                num_measurements--;
                return ADC_ERROR_DIFF_VALUE;
            }
        } else { // no pga
            if(ADC_num == 0) {
                *ADC_SC1A = ADC_SC1_DIFF + 0x0 + var_enableInterrupts*ADC_SC1_AIEN; // In ADC0, A10-A11 is DAD0
                __enable_irq();
            } else if(ADC_num == 1) {
                *ADC_SC1A = ADC_SC1_DIFF + 0x3 + var_enableInterrupts*ADC_SC1_AIEN; // In ADC1, A10-A11 is DAD3
                __enable_irq()
            }
        }
        #elif defined(__MK20DX128__)
	    *ADC_SC1A = ADC_SC1_DIFF + 0x0 + var_enableInterrupts*ADC_SC1_AIEN;
        __enable_irq();
        #endif
	} else if ( (pinP == A12) && (pinN == A13) ) { // pins 36 and 37
        __disable_irq();
        #if defined(__MK20DX256__)
        if ( *ADC_PGA & ADC_PGA_PGAEN ) { // PGA is enabled
            if(ADC_num == 1) { // PGA1 connects to A12-A13
                *ADC_SC1A = ADC_SC1_DIFF + 0x2 + var_enableInterrupts*ADC_SC1_AIEN;
                __enable_irq();
            } else { // If this is ADC0 we can't connect to PGA1
                __enable_irq();
                fail_flag |= ADC_ERROR_WRONG_ADC;
                num_measurements--;
                return ADC_ERROR_DIFF_VALUE;
            }
        } else {
            if(ADC_num == 0) {
                *ADC_SC1A = ADC_SC1_DIFF + 0x3 + var_enableInterrupts*ADC_SC1_AIEN; // In ADC0, A10-A11 is DAD3
                __enable_irq();
            } else if(ADC_num == 1) {
                *ADC_SC1A = ADC_SC1_DIFF + 0x0 + var_enableInterrupts*ADC_SC1_AIEN; // In ADC1, A10-A11 is DAD0
                __enable_irq()
            }
        }
        #elif defined(__MK20DX128__)
	    *ADC_SC1A = ADC_SC1_DIFF + 0x3 + var_enableInterrupts*ADC_SC1_AIEN;
        __enable_irq();
        #endif
	} else {
	    __enable_irq(); // just in case we disabled them in the if above.
	    fail_flag |= ADC_ERROR_ANALOG_DIFF_READ;
	    num_measurements--;
        return ADC_ERROR_DIFF_VALUE;
    }

    // wait for the ADC to finish
    while( isConverting() ) {
        yield();
        //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN) );
    }

    // it's done, check if the comparison (if any) was true
    __disable_irq(); // make sure nothing interrupts this part
    if (isComplete()) { // conversion succeded
        result = (int16_t)(int32_t)(*ADC_RA); // cast to 32 bits
        if(res==16) { // 16 bit differential is actually 15 bit + 1 bit sign
            result *= 2; // multiply by 2 as if it were really 16 bits, so that getMaxValue gives a correct value.
        }
    } else { // comparison was false
        result = ADC_ERROR_VALUE;
        fail_flag |= ADC_ERROR_COMPARISON;
    }
    __enable_irq();

    // if we interrupted a conversion, set it again
    if (wasADCInUse) {
        //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN) );
        *ADC_CFG1 = old_config.savedCFG1;
        *ADC_CFG2 = old_config.savedCFG2;
        *ADC_SC2 = old_config.savedSC2;
        *ADC_SC3 = old_config.savedSC3;
        *ADC_SC1A = old_config.savedSC1A;
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
*  If the pin is incorrect it returns ADC_ERROR_VALUE.
*/
int ADC_Module::startSingleRead(uint8_t pin) {

    if ( (pin < 0) || (pin > 43) ) {
        fail_flag |= ADC_ERROR_WRONG_PIN;
        return ADC_ERROR_VALUE;   // all others are invalid
    }

	if (calibrating) wait_for_cal();

    // vars to saved the current state of the ADC in case it's in use
    adcWasInUse = isConverting(); // is the ADC running now?

    if(adcWasInUse) { // this means we're interrupting a conversion
        // save the current conversion config, the adc isr will restore the adc
        __disable_irq();
        //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN) );
        adc_config.savedSC1A = *ADC_SC1A;
        adc_config.savedCFG1 = *ADC_CFG1;
        adc_config.savedCFG2 = *ADC_CFG2;
        adc_config.savedSC2 = *ADC_SC2;
        adc_config.savedSC3 = *ADC_SC3;
        __enable_irq();
    }

    startSingleReadFast(pin);

/*
    #if defined(__MK20DX256__)
	// ADC1 has A15=5a and A20=4a so we have to change the mux to read the "a" channels
	if( (ADC_num==1) && ( (pin==26) || (pin==31) ) ) {
        // *ADC_CFG2 &= ~ADC_CFG2_MUXSEL;
        *ADC_CFG2_muxsel = 0;
	} else {
        // *ADC_CFG2 |= ADC_CFG2_MUXSEL;
        *ADC_CFG2_muxsel = 1;
	}
	#endif

	// no continuous mode
    *ADC_SC3_adco = 0;

    // select pin for single-ended mode and start conversion, enable interrupts to know when it's done
    __disable_irq();
    *ADC_SC1A = channel2sc1a[pin] + var_enableInterrupts*ADC_SC1_AIEN;
	__enable_irq();
*/

    return true;
}
// Doesn't do any of the checks of the normal version, but it's faster
int ADC_Module::startSingleReadFast(uint8_t pin) {
    #if defined(__MK20DX256__)
	// ADC1 has A15=5a and A20=4a so we have to change the mux to read the "a" channels
	if( (ADC_num==1) && ( (pin==26) || (pin==31) ) ) {
        //*ADC_CFG2 &= ~ADC_CFG2_MUXSEL;
        *ADC_CFG2_muxsel = 0;
	} else {
        //*ADC_CFG2 |= ADC_CFG2_MUXSEL;
        *ADC_CFG2_muxsel = 1;
	}
	#endif

	// no continuous mode
    *ADC_SC3_adco = 0;

    // select pin for single-ended mode and start conversion, enable interrupts to know when it's done
    __disable_irq();
    *ADC_SC1A = channel2sc1a[pin] + var_enableInterrupts*ADC_SC1_AIEN;
	__enable_irq();

    return true;
}


/* Start a differential conversion between two pins (pinP - pinN).
* It returns inmediately, get value with readSingle().
* Incorrect pins will return ADC_ERROR_DIFF_VALUE.
* Set the resolution, number of averages and voltage reference using the appropriate functions
*/
int ADC_Module::startSingleDifferential(uint8_t pinP, uint8_t pinN) {

    if( ((pinP != A10) && (pinP != A12)) || ((pinN != A11) && (pinN != A13)) ) {
        fail_flag |= ADC_ERROR_WRONG_PIN;
        return ADC_ERROR_VALUE;   // all others are invalid
	}

	// check for calibration before setting channels,
	// because conversion will start as soon as we write to *ADC_SC1A
	if (calibrating) wait_for_cal();

    // vars to saved the current state of the ADC in case it's in use
    adcWasInUse = isConverting(); // is the ADC running now?

    if(adcWasInUse) { // this means we're interrupting a conversion
        // save the current conversion config, the adc isr will restore the adc
        __disable_irq();
        //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN) );
        adc_config.savedSC1A = *ADC_SC1A;
        adc_config.savedCFG1 = *ADC_CFG1;
        adc_config.savedCFG2 = *ADC_CFG2;
        adc_config.savedSC2 = *ADC_SC2;
        adc_config.savedSC3 = *ADC_SC3;
        __enable_irq();
    }

    startSingleDifferentialFast(pinP, pinN);

 /*
    // no continuous mode
    *ADC_SC3_adco = 0;

    // once *ADC_SC1A is set, conversion will start, enable interrupts if requested
	if ( (pinP == A10) && (pinN == A11) ) { // DAD0 selected, pins 34 and 35
        __disable_irq();
        #if defined(__MK20DX256__)
        if ( *ADC_PGA & ADC_PGA_PGAEN ) { // PGA is enabled
            if(ADC_num == 0) { // PGA0 connects to A10-A11
                *ADC_SC1A = ADC_SC1_DIFF + 0x2 + var_enableInterrupts*ADC_SC1_AIEN;
                __enable_irq();
            } else { // If this is ADC1 we can't connect to PGA0
                __enable_irq();
                fail_flag |= ADC_ERROR_WRONG_ADC;
                return ADC_ERROR_DIFF_VALUE;
            }
        } else { // no PGA
            if(ADC_num == 0) {
                *ADC_SC1A = ADC_SC1_DIFF + 0x0 + var_enableInterrupts*ADC_SC1_AIEN; // In ADC0, A10-A11 is DAD0
                __enable_irq();
            } else if(ADC_num == 1) {
                *ADC_SC1A = ADC_SC1_DIFF + 0x3 + var_enableInterrupts*ADC_SC1_AIEN; // In ADC1, A10-A11 is DAD3
                __enable_irq()
            }
        }
        #elif defined(__MK20DX128__)
	    *ADC_SC1A = ADC_SC1_DIFF + 0x0 + var_enableInterrupts*ADC_SC1_AIEN;
        __enable_irq();
        #endif
	} else if ( (pinP == A12) && (pinN == A13) ) { // DAD3 selected, pins 36 and 37
        __disable_irq();
        #if defined(__MK20DX256__)
        if ( *ADC_PGA & ADC_PGA_PGAEN ) { // PGA is enabled
            if(ADC_num == 1) { // PGA1 connects to A12-A13
                *ADC_SC1A = ADC_SC1_DIFF + 0x2 + var_enableInterrupts*ADC_SC1_AIEN;
                __enable_irq();
            } else { // If this is ADC0 we can't connect to PGA1
                __enable_irq();
                fail_flag |= ADC_ERROR_WRONG_ADC;
                return ADC_ERROR_DIFF_VALUE;
            }
        } else {
            if(ADC_num == 0) {
                *ADC_SC1A = ADC_SC1_DIFF + 0x3 + var_enableInterrupts*ADC_SC1_AIEN; // In ADC0, A10-A11 is DAD3
                __enable_irq();
            } else if(ADC_num == 1) {
                *ADC_SC1A = ADC_SC1_DIFF + 0x0 + var_enableInterrupts*ADC_SC1_AIEN; // In ADC1, A10-A11 is DAD0
                __enable_irq()
            }
        }
        #elif defined(__MK20DX128__)
	    *ADC_SC1A = ADC_SC1_DIFF + 0x3 + var_enableInterrupts*ADC_SC1_AIEN;
        __enable_irq();
        #endif
	} else {
	    __enable_irq(); // just in case we disabled them in the if above.
	    fail_flag |= ADC_ERROR_ANALOG_DIFF_READ;
        return ADC_ERROR_DIFF_VALUE; // all others aren't capable of differential measurements
    }
*/


    return true;
}
int ADC_Module::startSingleDifferentialFast(uint8_t pinP, uint8_t pinN) {

    // no continuous mode
    *ADC_SC3_adco = 0;

    // once *ADC_SC1A is set, conversion will start, enable interrupts if requested
	if ( (pinP == A10) && (pinN == A11) ) { // DAD0 selected, pins 34 and 35
        __disable_irq();
        #if defined(__MK20DX256__)
        if ( *ADC_PGA & ADC_PGA_PGAEN ) { // PGA is enabled
            if(ADC_num == 0) { // PGA0 connects to A10-A11
                *ADC_SC1A = ADC_SC1_DIFF + 0x2 + var_enableInterrupts*ADC_SC1_AIEN;
                __enable_irq();
            } else { // If this is ADC1 we can't connect to PGA0
                __enable_irq();
                fail_flag |= ADC_ERROR_WRONG_ADC;
                return ADC_ERROR_DIFF_VALUE;
            }
        } else { // no PGA
            if(ADC_num == 0) {
                *ADC_SC1A = ADC_SC1_DIFF + 0x0 + var_enableInterrupts*ADC_SC1_AIEN; // In ADC0, A10-A11 is DAD0
                __enable_irq();
            } else if(ADC_num == 1) {
                *ADC_SC1A = ADC_SC1_DIFF + 0x3 + var_enableInterrupts*ADC_SC1_AIEN; // In ADC1, A10-A11 is DAD3
                __enable_irq()
            }
        }
        #elif defined(__MK20DX128__)
	    *ADC_SC1A = ADC_SC1_DIFF + 0x0 + var_enableInterrupts*ADC_SC1_AIEN;
        __enable_irq();
        #endif
	} else if ( (pinP == A12) && (pinN == A13) ) { // DAD3 selected, pins 36 and 37
        __disable_irq();
        #if defined(__MK20DX256__)
        if ( *ADC_PGA & ADC_PGA_PGAEN ) { // PGA is enabled
            if(ADC_num == 1) { // PGA1 connects to A12-A13
                *ADC_SC1A = ADC_SC1_DIFF + 0x2 + var_enableInterrupts*ADC_SC1_AIEN;
                __enable_irq();
            } else { // If this is ADC0 we can't connect to PGA1
                __enable_irq();
                fail_flag |= ADC_ERROR_WRONG_ADC;
                return ADC_ERROR_DIFF_VALUE;
            }
        } else {
            if(ADC_num == 0) {
                *ADC_SC1A = ADC_SC1_DIFF + 0x3 + var_enableInterrupts*ADC_SC1_AIEN; // In ADC0, A10-A11 is DAD3
                __enable_irq();
            } else if(ADC_num == 1) {
                *ADC_SC1A = ADC_SC1_DIFF + 0x0 + var_enableInterrupts*ADC_SC1_AIEN; // In ADC1, A10-A11 is DAD0
                __enable_irq()
            }
        }
        #elif defined(__MK20DX128__)
	    *ADC_SC1A = ADC_SC1_DIFF + 0x3 + var_enableInterrupts*ADC_SC1_AIEN;
        __enable_irq();
        #endif
	} else {
	    __enable_irq(); // just in case we disabled them in the if above.
	    fail_flag |= ADC_ERROR_ANALOG_DIFF_READ;
        return ADC_ERROR_DIFF_VALUE; // all others aren't capable of differential measurements
    }

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
void ADC_Module::startContinuous(uint8_t pin)
{
    if ( (pin < 0) || (pin > 43) ) {
        fail_flag |= ADC_ERROR_WRONG_PIN;
        return;   // all others are invalid
    }

    // check for calibration before setting channels,
	if (calibrating) wait_for_cal();

    // increase the counter of measurements
	num_measurements++;

    #if defined(__MK20DX256__)
	// ADC1 has A15=5a and A20=4a so we have to change the mux to read the "a" channels
	if( (ADC_num==1) && ( (pin==26) || (pin==31) ) ) {
        //*ADC_CFG2 &= ~ADC_CFG2_MUXSEL;
        *ADC_CFG2_muxsel = 0;
	} else {
        //*ADC_CFG2 |= ADC_CFG2_MUXSEL;
        *ADC_CFG2_muxsel = 1;
	}
	#endif

    __disable_irq();
    // set continuous conversion flag
	//*ADC_SC3 |= ADC_SC3_ADCO;
	*ADC_SC3_adco = 1;
    // select pin for single-ended mode and start conversion, enable interrupts if requested
    *ADC_SC1A = channel2sc1a[pin] + var_enableInterrupts*ADC_SC1_AIEN;
    __enable_irq();


	return;
}


/* Starts continuous and differential conversion between the pins (pinP-pinN)
 * It returns as soon as the ADC is set, use analogReadContinuous() to read the value
 * Set the resolution, number of averages and voltage reference using the appropriate functions BEFORE calling this function
*/
void ADC_Module::startContinuousDifferential(uint8_t pinP, uint8_t pinN) {

    if( ((pinP != A10) && (pinP != A12)) || ((pinN != A11) && (pinN != A13)) ) {
        fail_flag |= ADC_ERROR_WRONG_PIN;
        return;   // all others are invalid
	}

	// check for calibration before setting channels,
	if (calibrating) wait_for_cal();

    // increase the counter of measurements
	num_measurements++;

    // set continuous conversion flag
	__disable_irq();
	//*ADC_SC3 |= ADC_SC3_ADCO;
	*ADC_SC3_adco = 1;
	__enable_irq();

    // once *ADC_SC1A is set, conversion will start, enable interrupts if requested
	if ( (pinP == A10) && (pinN == A11) ) { // DAD0 selected, pins 34 and 35
        __disable_irq();
        #if defined(__MK20DX256__)
        if ( *ADC_PGA & ADC_PGA_PGAEN ) { // PGA is enabled
            if(ADC_num == 0) { // PGA0 connects to A10-A11
                *ADC_SC1A = ADC_SC1_DIFF + 0x2 + var_enableInterrupts*ADC_SC1_AIEN;
                __enable_irq();
            } else { // If this is ADC1 we can't connect to PGA0
                __enable_irq();
                fail_flag |= ADC_ERROR_WRONG_ADC;
                return;
            }
        } else {
            if(ADC_num == 0) {
                *ADC_SC1A = ADC_SC1_DIFF + 0x0 + var_enableInterrupts*ADC_SC1_AIEN; // In ADC0, A10-A11 is DAD0
                __enable_irq();
            } else if(ADC_num == 1) {
                *ADC_SC1A = ADC_SC1_DIFF + 0x3 + var_enableInterrupts*ADC_SC1_AIEN; // In ADC1, A10-A11 is DAD3
                __enable_irq()
            }
        }
        #elif defined(__MK20DX128__)
	    *ADC_SC1A = ADC_SC1_DIFF + 0x0 + var_enableInterrupts*ADC_SC1_AIEN;
        __enable_irq();
        #endif
	} else if ( (pinP == A12) && (pinN == A13) ) { // DAD3 selected, pins 36 and 37
        __disable_irq();
        #if defined(__MK20DX256__)
        if ( *ADC_PGA & ADC_PGA_PGAEN ) { // PGA is enabled
            if(ADC_num == 1) { // PGA1 connects to A12-A13
                *ADC_SC1A = ADC_SC1_DIFF + 0x2 + var_enableInterrupts*ADC_SC1_AIEN;
                __enable_irq();
            } else { // If this is ADC0 we can't connect to PGA1
                __enable_irq();
                fail_flag |= ADC_ERROR_WRONG_ADC;
                return;
            }
        } else { // no PGA
            if(ADC_num == 0) {
                *ADC_SC1A = ADC_SC1_DIFF + 0x3 + var_enableInterrupts*ADC_SC1_AIEN; // In ADC0, A10-A11 is DAD3
                __enable_irq();
            } else if(ADC_num == 1) {
                *ADC_SC1A = ADC_SC1_DIFF + 0x0 + var_enableInterrupts*ADC_SC1_AIEN; // In ADC1, A10-A11 is DAD0
                __enable_irq()
            }
        }
        #elif defined(__MK20DX128__)
	    *ADC_SC1A = ADC_SC1_DIFF + 0x3 + var_enableInterrupts*ADC_SC1_AIEN;
        __enable_irq();
        #endif
	} else {
	    __enable_irq(); // just in case we disabled them in the if above.
	    fail_flag |= ADC_ERROR_CONT_DIFF;
        return; // all others aren't capable of differential measurements, perhaps return analogRead(pinP)-analogRead(pinN)?
    }

    return;
}


/* Stops continuous conversion
*/
void ADC_Module::stopContinuous() {
	*ADC_SC1A = 0x1F; // set channel select to all 1's (31) to stop it.

	// decrease the counter of measurements
	num_measurements--;

	return;
}

