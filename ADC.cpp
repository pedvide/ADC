 /* ADC Library created by Pedro Villanueva
 * It implements all functions of the Teensy 3.0 internal ADC
 * Some of the code was extracted from the file analog.c included in the distribution of Teensduino by PJRC.COM
 *
 */


 /* TODO
  * Stuff related to the conversion speed and power. Useful if you want more speed or to save power.
  *
  * bugs:
  * - analog timer at 16 bits resolution goes from 0 to +1.65 and then jumps to -1.65 to 0
  * - comaprison values don't work in 16 bit differential mode (they are twice what you write)
 */


#include "ADC.h"


// ADC interrupt function calls a function to add values in the ring buffer
// if there is at least one analog timer, if not, it does nothing.
// implemented as weak so that an user can redefine it
void __attribute__((weak)) adc0_isr(void) {
    ADC::analogTimer_ADC_Callback();
}


// static vars need to be restated here
uint8_t ADC::calibrating;
uint8_t ADC::var_enableInterrupts;
uint8_t ADC::analog_config_bits;
uint32_t ADC::analog_max_val;
uint8_t ADC::analog_num_average;
uint8_t ADC::analog_reference_internal;

const uint8_t ADC::ledPin = 13;

ADC::ADC_Config ADC::adc_config; // store the adc config
uint8_t ADC::adcWasInUse; // was the adc in use before an analog timer call?

const uint8_t ADC::channel2sc1a[]= {
    5, 14, 8, 9, 13, 12, 6, 7, 15, 4,
    0, 19, 3, 21, 26, 22};
const uint8_t ADC::sc1a2channel[]= { // new version, gives directly the pin number
    34, 0, 0, 36, 23, 14, 20, 21, 16, 17, 0, 0, 19, 18, // 0-13
    15, 22, 0, 0, 0, 35, 0, 37, 39, 0, 0, 0, 38}; // 14-26

// struct with the analog timers
ADC::AnalogTimer *ADC::analogTimer[];
//int8_t ADC::ANALOG_TIMER::pinNumber;

// pointer to isr adc
void (*ADC::analogTimer_ADC_Callback)(void);

// the functions for the analog timers
ADC::ISR ADC::analogTimerCallback[] = {
    ADC::analogTimerCallback0,
    ADC::analogTimerCallback1,
    ADC::analogTimerCallback0
};


/* Constructor
*
*/
ADC::ADC() {
    // default settings: 10 bits resolution, 4 averages, vcc reference, no interrupts and single-ended
    analog_config_bits = 10;
    analog_max_val = 1024;
    analog_num_average = 4;
    analog_reference_internal = 0;
    var_enableInterrupts = 0;

    // itinialize analog timers
    int i = 0;
    for(i=0; i<MAX_ANALOG_TIMERS; i++) {
        analogTimer[i] = new AnalogTimer;
        //analogTimer[i]->pinNumber = -1;
    }
    //*analogTimer = new AnalogTimer[MAX_ANALOG_TIMERS]; // doesn't work for some reason

    // call our init
    ADC::analog_init();

    // point ADC callback to function that does nothing
    // when the analogTimers are in use, this will point to the correct ADC_callback
    analogTimer_ADC_Callback = &voidFunction;
}

/* Destructor
*
*/
ADC::~ADC() {
    //dtor
    int i = 0;
    for(i=0; i<MAX_ANALOG_TIMERS; i++) {
        delete analogTimer[i];
    }

}

/* Sets up all initial configurations and starts calibration
*
*/
void ADC::analog_init(uint32_t config)
{
	uint32_t num;

	VREF_TRM = 0x60;
	VREF_SC = 0xE1;		// enable 1.2 volt ref

	if (analog_reference_internal) {
		ADC0_SC2 |= ADC_SC2_REFSEL(1); // 1.2V ref
	} else {
		ADC0_SC2 |= ADC_SC2_REFSEL(0); // vcc/ext ref
	}

    // conversion resolution and frequency
    if ( (analog_config_bits == 8) || (analog_config_bits == 9) )  {
        ADC0_CFG1 = ADC0_CFG1_24MHZ + ADC_CFG1_MODE(0); // 0 clock divide + Input clock: 24 MHz (run at 24 MHz) + Conversion mode: 8 bit + Sample time: Short
        ADC0_CFG2 = ADC_CFG2_MUXSEL + ADC_CFG2_ADLSTS(3); // b channels + Sample time 2 cycles (I think that if ADC_CFG1_ADLSMP isn't set, then ADC_CFG2_ADLSTS doesn't matter)
        analog_max_val = 256; // diff mode 9 bits has 1 bit for sign, so max value is the same as single 8 bits
    } else if ( (analog_config_bits == 10 )|| (analog_config_bits == 11) ) { // total clock cycles to complete conversion: 3 ADCK + 5 BUS + 4 averages*( 20 + 2 ADCK ) = 7.8 us
        ADC0_CFG1 = ADC0_CFG1_12MHZ + ADC_CFG1_MODE(2) + ADC_CFG1_ADLSMP; // Clock divide: 1/2 + Input clock: 24 MHz (run at 12 MHz) + Conversion mode: 10 bit + Sample time: Long
        ADC0_CFG2 = ADC_CFG2_MUXSEL + ADC_CFG2_ADLSTS(3); // b channels + Sample time 2 extra clock cycles
        analog_max_val = 1024;
    } else if ( (analog_config_bits == 12 )|| (analog_config_bits == 13) ) {
        ADC0_CFG1 = ADC0_CFG1_12MHZ + ADC_CFG1_MODE(1) + ADC_CFG1_ADLSMP; // Clock divide: 1/2 + Input clock: 24 MHz (run at 12 MHz) + Conversion mode: 12 bit + Sample time: Long
        ADC0_CFG2 = ADC_CFG2_MUXSEL + ADC_CFG2_ADLSTS(2); // b channels + Sample time: 6 extra clock cycles
        analog_max_val = 4096;
    } else {
        ADC0_CFG1 = ADC0_CFG1_12MHZ + ADC_CFG1_MODE(3) + ADC_CFG1_ADLSMP;  //Clock divide: 1/2 + Input clock: 24 MHz (run at 12 MHz) + Conversion mode: 16 bit + Sample time: Long
        ADC0_CFG2 = ADC_CFG2_MUXSEL + ADC_CFG2_ADLSTS(2); // b channels + Sample time: 6 extra clock cycles
        analog_max_val = 65536;
    }

    // number of averages and calibration
	num = analog_num_average;
	if (num <= 1) {
		ADC0_SC3 = ADC_SC3_CAL;  // begin calibration + no averages
	} else if (num <= 4) {
		ADC0_SC3 = ADC_SC3_CAL + ADC_SC3_AVGE + ADC_SC3_AVGS(0); // start calib. + single-shot mode + 4 averages
	} else if (num <= 8) {
		ADC0_SC3 = ADC_SC3_CAL + ADC_SC3_AVGE + ADC_SC3_AVGS(1); // start calib. + single-shot mode + 8 averages
	} else if (num <= 16) {
		ADC0_SC3 = ADC_SC3_CAL + ADC_SC3_AVGE + ADC_SC3_AVGS(2); // start calib. + single-shot mode + 16 averages
	} else {
		ADC0_SC3 = ADC_SC3_CAL + ADC_SC3_AVGE + ADC_SC3_AVGS(3); // start calib. + single-shot mode + 32 averages
	}

	// calibration works best when averages are 32 and speed is less than 4 MHz
	calibrating = 1;
}


/* Waits until calibration is finished and writes the corresponding registers
*
*/
void ADC::wait_for_cal(void)
{
	uint16_t sum;

	//serial_print("wait_for_cal\n");
	while (ADC0_SC3 & ADC_SC3_CAL) { // Bit ADC_SC3_CAL in register ADC0_SC3 cleared when calib. finishes.
		// wait
		//serial_print(".");
	}

	__disable_irq();
	if (calibrating) {
		//serial_print("\n");
		sum = ADC0_CLPS + ADC0_CLP4 + ADC0_CLP3 + ADC0_CLP2 + ADC0_CLP1 + ADC0_CLP0;
		sum = (sum / 2) | 0x8000;
		ADC0_PG = sum;
		//serial_print("ADC0_PG = ");
		//serial_phex16(sum);
		//serial_print("\n");
		sum = ADC0_CLMS + ADC0_CLM4 + ADC0_CLM3 + ADC0_CLM2 + ADC0_CLM1 + ADC0_CLM0;
		sum = (sum / 2) | 0x8000;
		ADC0_MG = sum;
		//serial_print("ADC0_MG = ");
		//serial_phex16(sum);
		//serial_print("\n");
		calibrating = 0;
	}
	__enable_irq();
}



/* Set the voltage reference you prefer, default is vcc
*   It needs to recalibrate
*/
void ADC::setReference(uint8_t type)
{
	if (type) {
		// internal reference requested
		if (!analog_reference_internal) {
			analog_reference_internal = 1;
			if (calibrating) ADC0_SC3 = 0; // cancel cal
			ADC::analog_init();
		}
	} else {
		// vcc or external reference requested
		if (analog_reference_internal) {
			analog_reference_internal = 0;
			if (calibrating) ADC0_SC3 = 0; // cancel cal
			ADC::analog_init();
		}
	}
}


/* Change the resolution of the measurement
*  For single-ended measurements: 8, 10, 12 or 16 bits.
*  For differential measurements: 9, 11, 13 or 16 bits.
*  If you want something in between (11 bits single-ended for example) select the inmediate higher
*  and shift the result one to the right.
*/
void ADC::setResolution(unsigned int bits)
{
    unsigned int config;

    if (bits <8) {
		config = 8;
	} else if (bits > 16) {
		config = 16;
	} else {
		config = bits;
	}

    // if the new res is the "same" as the old, update analog_config_bits, but do nothing else
	if( (config==8 && analog_config_bits==9) || (config==9 && analog_config_bits==8)
        || (config==10 && analog_config_bits==11) || (config==11 && analog_config_bits==10)
        || (config==12 && analog_config_bits==13) || (config==13 && analog_config_bits==12) ) {
        analog_config_bits = config;
    } else if (config != analog_config_bits) { // change res
        analog_config_bits = config;

		// no recalibration is needed when changing the resolution, p. 619
		// but it's needed if we change the frequency...
		if (calibrating) ADC0_SC3 = 0; // cancel cal
		ADC::analog_init(); // re-cal
	}
}

/* Returns the resolution of the ADC
*
*/
uint8_t ADC::getResolution() {
    return analog_config_bits;
}

/* Returns the maximum value for a measurement, that is 2^resolution
*
*/
uint32_t ADC::getMaxValue() {
    return analog_max_val;
}


/* Set the number of averages: 0, 4, 8, 16 or 32.
*
*/
void ADC::setAveraging(uint8_t num)
{

	if (calibrating) wait_for_cal();
	if (num <= 1) {
		num = 0;
		ADC0_SC3 = 0;
	} else if (num <= 4) {
		num = 4;
		ADC0_SC3 = ADC_SC3_AVGE + ADC_SC3_AVGS(0);
	} else if (num <= 8) {
		num = 8;
		ADC0_SC3 = ADC_SC3_AVGE + ADC_SC3_AVGS(1);
	} else if (num <= 16) {
		num = 16;
		ADC0_SC3 = ADC_SC3_AVGE + ADC_SC3_AVGS(2);
	} else {
		num = 32;
		ADC0_SC3 = ADC_SC3_AVGE + ADC_SC3_AVGS(3);
	}
	analog_num_average = num;
}


/* Enable interrupts: An ADC Interrupt will be raised when the conversion is completed
*  (including hardware averages and if the comparison (if any) is true).
*/
void ADC::enableInterrupts() {
    var_enableInterrupts = 1;
    NVIC_ENABLE_IRQ(IRQ_ADC0);
}

/* Disable interrupts
*
*/
void ADC::disableInterrupts() {
    var_enableInterrupts = 0;
    NVIC_DISABLE_IRQ(IRQ_ADC0);
}


/* Enable DMA request: An ADC DMA request will be raised when the conversion is completed
*  (including hardware averages and if the comparison (if any) is true).
*/
void ADC::enableDMA() {
    ADC0_SC2 |= ADC_SC2_DMAEN;
}

/* Disable ADC DMA request
*
*/
void ADC::disableDMA() {
    ADC0_SC2 &= !ADC_SC2_DMAEN;
}


/* Enable the compare function: A conversion will be completed only when the ADC value
*  is >= compValue (greaterThan=1) or < compValue (greaterThan=0)
*  Call it after changing the resolution
*  Use with interrupts or poll conversion completion with isADC_Complete()
*/
void ADC::enableCompare(int16_t compValue, int greaterThan) {
    ADC0_SC2 |= ADC_SC2_ACFE | greaterThan*ADC_SC2_ACFGT;
    ADC0_CV1 = compValue;
}

/* Enable the compare function: A conversion will be completed only when the ADC value
*  is inside (insideRange=1) or outside (=0) the range given by (lowerLimit, upperLimit),
*  including (inclusive=1) the limits or not (inclusive=0).
*  See Table 31-78, p. 617 of the freescale manual.
*  Call it after changing the resolution
*  Use with interrupts or poll conversion completion with isComplete()
*/
void ADC::enableCompareRange(int16_t lowerLimit, int16_t upperLimit, int insideRange, int inclusive) {

    if(insideRange && inclusive) { // True if value is inside the range, including the limits. CV1 <= CV2 and ACFGT=1
        ADC0_CV1 = lowerLimit;
        ADC0_CV2 = upperLimit;
        ADC0_SC2 |= ADC_SC2_ACFE | ADC_SC2_ACFGT | ADC_SC2_ACREN;
    } else if(insideRange && !inclusive) {// True if value is inside the range, excluding the limits. CV1 > CV2 and ACFGT=0
        ADC0_CV2 = lowerLimit;
        ADC0_CV1 = upperLimit;
        ADC0_SC2 |= ADC_SC2_ACFE | ADC_SC2_ACREN;
    } else if(!insideRange && inclusive) { // True if value is outside of range or is equal to either limit. CV1 > CV2 and ACFGT=1
        ADC0_CV2 = lowerLimit;
        ADC0_CV1 = upperLimit;
        ADC0_SC2 |= ADC_SC2_ACFE | ADC_SC2_ACFGT | ADC_SC2_ACREN;
    } else if(!insideRange && !inclusive) { // True if value is outside of range and not equal to either limit. CV1 > CV2 and ACFGT=0
        ADC0_CV1 = lowerLimit;
        ADC0_CV2 = upperLimit;
        ADC0_SC2 |= ADC_SC2_ACFE | ADC_SC2_ACREN;
    }
}

/* Disable the compare function
*
*/
void ADC::disableCompare() {
    ADC0_SC2 &= !ADC_SC2_ACFE;
}


/* Reads the analog value of the pin.
* It waits until the value is read and then returns the result.
* If a comparison has been set up and fails, it will return -1.
* Set the resolution, number of averages and voltage reference using the appropriate functions.
*/
int ADC::analogRead(uint8_t pin)
{
	uint16_t result;

	if (pin >= 14 && pin <= 39) {
		if (pin <= 23) {
			pin -= 14;  // 14-23 are A0-A9
		} else if (pin >= 34) {
			pin -= 24;  // 34-37 are A10-A13, 38 is temp sensor, 39 is vref
		}
    } else {
        return ADC_ERROR_VALUE;   // all others are invalid
    }

	if (calibrating) wait_for_cal();

	uint8_t res = getResolution();
	uint8_t diffRes = 0; // is the new resolution different from the old one?

    // vars to save the current state of the ADC in case it's in use
    uint32_t savedSC1A, savedSC2, savedSC3, savedCFG1, savedCFG2, savedRes;
    uint8_t wasADCInUse = isConverting(); // is the ADC running now?

    if(wasADCInUse) { // this means we're interrupting a conversion
        // save the current conversion config, we don't want any other interrupts messing up the configs
        __disable_irq();
        //GPIOC_PSOR = 1<<5;
        savedRes = res;
        savedSC1A = ADC0_SC1A;
        savedCFG1 = ADC0_CFG1;
        savedSC2 = ADC0_SC2;
        savedSC3 = ADC0_SC3;

        // change the comparison values if interrupting a 16 bits and diff mode
        if(res==16 && isDifferential()) {
            ADC0_CV1 /= 2;
            ADC0_CV2 /= 2;
        }

        // disable continuous mode in case analogRead is interrupting a continuous mode
        ADC0_SC3 &= !ADC_SC3_ADCO;

        __enable_irq(); ////keep irq disabled until we start our conversion

    }


    // if the resolution is incorrect (i.e. 9, 11 or 13) silently correct it
	if( (res==9) || (res==11) || (res==13) ) {
        setResolution(res-1);
        diffRes = 1; // resolution changed
	}

    __disable_irq();
    ADC0_SC1A = channel2sc1a[pin] + var_enableInterrupts*ADC_SC1_AIEN; // start conversion on pin and with interrupts enabled if requested
	__enable_irq();


	while (1) {
		__disable_irq();
		if ((ADC0_SC1A & ADC_SC1_COCO)) { // conversion completed
			result = ADC0_RA;

			// if we interrupted a conversion, set it again
            if (wasADCInUse) {
                //GPIOC_PCOR = 1<<5;

                // restore ADC config, and restart conversion
                if(diffRes) setResolution(savedRes); // don't change res if isn't necessary
                if(res==16 && ((savedSC1A & ADC_SC1_DIFF) >> 5) )  { // change back the comparison values if interrupting a 16 bits and diff mode
                    ADC0_CV1 *= 2;
                    ADC0_CV2 *= 2;
                }
                ADC0_CFG1 = savedCFG1;
                ADC0_SC2 = savedSC2 & 0x7F; // restore first 8 bits
                ADC0_SC3 = savedSC3 & 0xF; // restore first 4 bits
                ADC0_SC1A = savedSC1A & 0x7F; // restore first 8 bits
            }
			__enable_irq();
			return result;
		} else if( ((ADC0_SC2 & ADC_SC2_ADACT) == 0) && ((ADC0_SC1A & ADC_SC1_COCO) == 0) ) { // comparison was false
		    // we needed to check that ADACT wasn't 0 because COCO just turned 1.

		    // if we interrupted a conversion, set it again
            if (wasADCInUse) {
                //GPIOC_PCOR = 1<<5;

                // restore ADC config, and restart conversion
                if(diffRes) setResolution(savedRes); // don't change res if isn't necessary
                if(res==16 && ((savedSC1A & ADC_SC1_DIFF) >> 5) )  { // change back the comparison values if interrupting a 16 bits and diff mode
                    ADC0_CV1 *= 2;
                    ADC0_CV2 *= 2;
                }
                ADC0_CFG1 = savedCFG1;
                ADC0_SC2 = savedSC2 & 0x7F; // restore first 8 bits
                ADC0_SC3 = savedSC3 & 0xF; // restore first 4 bits
                ADC0_SC1A = savedSC1A & 0x7F; // restore first 8 bits
            }

		    // comparison was false, we return an error value to indicate this
		    __enable_irq();
            return ADC_ERROR_VALUE;
		} // end if comparison false

		__enable_irq();
		yield();
	} // end while

} // analogRead



/* Reads the differential analog value of two pins (pinP - pinN)
* It waits until the value is read and then returns the result
* If a comparison has been set up and fails, it will return -70000
* Set the resolution, number of averages and voltage reference using the appropriate functions
*/
int ADC::analogReadDifferential(uint8_t pinP, uint8_t pinN)
{
	int16_t result;

	// check for calibration before setting channels,
	// because conversion will start as soon as we write to ADC0_SC1A
	if (calibrating) wait_for_cal();

	uint8_t res = getResolution();
	uint8_t diffRes = 0; // is the new resolution different from the old one?

    // vars to saved the current state of the ADC in case it's in use
    uint32_t savedSC1A, savedSC2, savedSC3, savedCFG1, savedCFG2, savedRes;
    uint8_t adcWasInUse = isConverting(); // is the ADC running now?

    if(adcWasInUse) { // this means we're interrupting a conversion
        // save the current conversion config, we don't want any other interrupts messing up the configs
        __disable_irq();
        //GPIOC_PSOR = 1<<5;
        savedRes = res;
        savedSC1A = ADC0_SC1A;// & 0x7F; // get first 7 bits
        savedCFG1 = ADC0_CFG1;
        savedCFG2 = ADC0_CFG2;
        savedSC2 = ADC0_SC2;// & 0x7F; // get first 7 bits
        savedSC3 = ADC0_SC3;// & 0xF; // get first 4 bits

        // disable continuous mode in case analogReadDifferential is interrupting a continuous mode
        ADC0_SC3 &= !ADC_SC3_ADCO;
        __enable_irq(); ////keep irq disabled until we start our conversion

    }

    // if the resolution is incorrect (i.e. 8, 10 or 12) silently correct it
	if( (res==8) || (res==10) || (res==12) ) {
        setResolution(res+1);
    diffRes = 1; // resolution changed
    } else if(res==16) {
        ADC0_CV1 /= 2; // correct also compare function in case it was enabled
        ADC0_CV2 /= 2;
    }


    // once ADC0_SC1A is set, conversion will start, enable interrupts if requested
	if ( (pinP == A10) && (pinN == A11) ) { // DAD0 selected, pins 34 and 35
        __disable_irq();
	    ADC0_SC1A = ADC_SC1_DIFF + 0x0 + var_enableInterrupts*ADC_SC1_AIEN;
        __enable_irq();
	} else if ( (pinP == A12) && (pinN == A13) ) { // DAD3 selected, pins 36 and 37
	    __disable_irq();
	    ADC0_SC1A = ADC_SC1_DIFF + 0x3 + var_enableInterrupts*ADC_SC1_AIEN;
	    __enable_irq();
	} else {
	    __enable_irq(); // just in case we disabled them in the if above.
        return ADC_ERROR_DIFF_VALUE; // all others aren't capable of differential measurements, perhaps return analogRead(pinP)-analogRead(pinN)?
    }

    while (1) {
		__disable_irq();
		if ((ADC0_SC1A & ADC_SC1_COCO)) { // conversion completed
			result = ADC0_RA;

			// if we interrupted a conversion, set it again
            if (adcWasInUse) {
                //GPIOC_PTOR = 1<<5;

                // restore ADC config, and restart conversion
                if(diffRes) setResolution(savedRes); // don't change res if isn't necessary
                if(res==16) {
                    ADC0_CV1 *= 2; // change back the comparison values
                    ADC0_CV2 *= 2;
                }
                ADC0_CFG1 = savedCFG1;
                ADC0_CFG2 = savedCFG2;
                ADC0_SC2 = savedSC2 & 0x7F;
                ADC0_SC3 = savedSC3 & 0xF;
                ADC0_SC1A = savedSC1A & 0x7F;
            }

			__enable_irq();
			if (result & (1<<15)) { // number is negative
                result |= 0xFFFF0000; // result is a 32 bit integer
            }
			return result;
		} else if( ((ADC0_SC2 & ADC_SC2_ADACT) == 0) && ((ADC0_SC1A & ADC_SC1_COCO) == 0) ) {
		    // we needed to check that ADACT wasn't 0 because COCO just turned 1.

		    // if we interrupted a conversion, set it again
            if (adcWasInUse) {
                //GPIOC_PTOR = 1<<5;

                // restore ADC config, and restart conversion
                if(diffRes) setResolution(savedRes); // don't change res if isn't necessary
                if(res==16) {
                    ADC0_CV1 *= 2; // change back the comparison values
                    ADC0_CV2 *= 2;
                }
                ADC0_CFG1 = savedCFG1;
                ADC0_CFG2 = savedCFG2;
                ADC0_SC2 = savedSC2 & 0x7F;
                ADC0_SC3 = savedSC3 & 0xF;
                ADC0_SC1A = savedSC1A & 0x7F;
            }

		    // comparison was false, we return an error value to indicate this
            __enable_irq();
            return ADC_ERROR_DIFF_VALUE;
		} // end if comparison false

		__enable_irq();
		yield();
	} // while

} // analogReadDifferential



/* Starts an analog measurement on the pin.
*  It returns inmediately, read value with readSingle().
*  If the pin is incorrect it returns ADC_ERROR_VALUE.
*/
int ADC::startSingleRead(uint8_t pin) {

	if (pin >= 14 && pin <= 39) {
		if (pin <= 23) {
			pin -= 14;  // 14-23 are A0-A9
		} else if (pin >= 34) {
			pin -= 24;  // 34-37 are A10-A13, 38 is temp sensor, 39 is vref
		}
    } else {
        return ADC_ERROR_VALUE;   // all others are invalid
    }

	if (calibrating) wait_for_cal();

	uint8_t res = getResolution();

    // if the resolution is incorrect (i.e. 9, 11 or 13) silently correct it
    adc_config.diffRes = 0;
	if( (res==9) || (res==11) || (res==13) ) {
        setResolution(res-1);
        adc_config.diffRes = 1; // resolution changed
	} else if(res==16){ // make sure the max value corresponds to a single-ended 16 bits mode
	     analog_max_val = 65536;
    }

    // vars to saved the current state of the ADC in case it's in use
    adcWasInUse = isConverting(); // is the ADC running now?

    if(adcWasInUse) { // this means we're interrupting a conversion
        // save the current conversion config, the adc isr will restore the adc
        __disable_irq();
        //GPIOC_PSOR = 1<<5;
        adc_config.savedRes = res;
        adc_config.savedSC1A = ADC0_SC1A;
        adc_config.savedCFG1 = ADC0_CFG1;
        adc_config.savedCFG2 = ADC0_CFG2;
        adc_config.savedSC2 = ADC0_SC2;
        adc_config.savedSC3 = ADC0_SC3;
        //__enable_irq(); //keep irq disabled until we start our conversion

    }

    // select pin for single-ended mode and start conversion, enable interrupts to know when it's done
    __disable_irq();
    ADC0_SC1A = channel2sc1a[pin] + var_enableInterrupts*ADC_SC1_AIEN;
	__enable_irq();

}


/* Start a differential conversion between two pins (pinP - pinN).
* It returns inmediately, get value with readSingle().
* Incorrect pins will return ADC_ERROR_DIFF_VALUE.
* Set the resolution, number of averages and voltage reference using the appropriate functions
*/
int ADC::startSingleDifferential(uint8_t pinP, uint8_t pinN)
{

	// check for calibration before setting channels,
	// because conversion will start as soon as we write to ADC0_SC1A
	if (calibrating) wait_for_cal();

	uint8_t res = getResolution();

	// if the resolution is incorrect (i.e. 8, 10 or 12) silently correct it
    adc_config.diffRes = 0;
    if( (res==8) || (res==10) || (res==12) ) {
        setResolution(res+1);
        adc_config.diffRes = 1; // resolution changed
    } else if(res==16) {
        analog_max_val = 32768; // 16 bit diff mode is actually 15 bits + 1 sign bit
    }

    // vars to saved the current state of the ADC in case it's in use
    adcWasInUse = isConverting(); // is the ADC running now?

    if(adcWasInUse) { // this means we're interrupting a conversion
        // save the current conversion config, the adc isr will restore the adc
        __disable_irq();
        //GPIOC_PSOR = 1<<5;
        adc_config.savedRes = res;
        adc_config.savedSC1A = ADC0_SC1A;
        adc_config.savedCFG1 = ADC0_CFG1;
        adc_config.savedCFG2 = ADC0_CFG2;
        adc_config.savedSC2 = ADC0_SC2;
        adc_config.savedSC3 = ADC0_SC3;
        //__enable_irq(); //keep irq disabled until we start our conversion

    }

    // once ADC0_SC1A is set, conversion will start, enable interrupts if requested
	if ( (pinP == A10) && (pinN == A11) ) { // DAD0 selected, pins 34 and 35
        __disable_irq();
	    ADC0_SC1A = ADC_SC1_DIFF + 0x0 + var_enableInterrupts*ADC_SC1_AIEN;
        __enable_irq();
	} else if ( (pinP == A12) && (pinN == A13) ) { // DAD3 selected, pins 36 and 37
	    __disable_irq();
	    ADC0_SC1A = ADC_SC1_DIFF + 0x3 + var_enableInterrupts*ADC_SC1_AIEN;
	    __enable_irq();
	} else {
	    __enable_irq();
        return ADC_ERROR_DIFF_VALUE; // all others aren't capable of differential measurements, perhaps return analogRead(pinP)-analogRead(pinN)?
    }

}

/* Reads the analog value of the single conversion set with startAnalogRead(pin)
*/
int ADC::readSingle() { return analogReadContinuous(); }




/* Starts continuous conversion on the pin
 * It returns as soon as the ADC is set, use analogReadContinuous() to read the values
 * Set the resolution, number of averages and voltage reference using the appropriate functions BEFORE calling this function
*/
void ADC::startContinuous(uint8_t pin)
{

    // if the resolution is incorrect (i.e. 9, 11 or 13) silently correct it
	uint8_t res = getResolution();
	if( (res==9) || (res==11) || (res==13) ) {
        setResolution(res-1);
	} else if(res==16) {
        analog_max_val = 65536; // make sure a differential function didn't change this
    }

    // check for calibration before setting channels,
	if (calibrating) wait_for_cal();

	if (pin >= 14) {
		if (pin <= 23) {
			pin -= 14;  // 14-23 are A0-A9
		} else if (pin >= 34 && pin <= 39) {
			pin -= 24;  // 34-37 are A10-A13, 38 is temp sensor, 39 is vref
		} else {
			return;   // all others are invalid
		}
	}

    __disable_irq();
    // set continuous conversion flag
	ADC0_SC3 |= ADC_SC3_ADCO;
    // select pin for single-ended mode and start conversion, enable interrupts if requested
    ADC0_SC1A = channel2sc1a[pin] + var_enableInterrupts*ADC_SC1_AIEN;
    __enable_irq();
}


/* Starts continuous and differential conversion between the pins (pinP-pinN)
 * It returns as soon as the ADC is set, use analogReadContinuous() to read the value
 * Set the resolution, number of averages and voltage reference using the appropriate functions BEFORE calling this function
*/
void ADC::startContinuousDifferential(uint8_t pinP, uint8_t pinN)
{

    // if the resolution is incorrect (i.e. 8, 10 or 12) silently correct
	uint8_t res = getResolution();
	if( (res==8) || (res==10) || (res==12) ) {
        setResolution(res+1);
    } else if(res==16) {
        analog_max_val = 32768; // 16 bit diff mode is actually 15 bits + 1 sign bit
        //ADC0_CV1 /= 2; // change back the comparison values
        //ADC0_CV2 /= 2;
    }


    // check for calibration before setting channels,
	// because conversion will start as soon as we write to ADC0_SC1A
	if (calibrating) wait_for_cal();

    // set continuous conversion flag
	__disable_irq();
	ADC0_SC3 |= ADC_SC3_ADCO;
	__enable_irq();

    // select pins for differential mode and start conversion, enable interrupts if requested
	if ( (pinP == A10) && (pinN == A11) ) { // DAD0 selected, pins 34 and 35
        __disable_irq();
	    ADC0_SC1A = ADC_SC1_DIFF + 0x0 + var_enableInterrupts*ADC_SC1_AIEN;
        __enable_irq();
	} else if ( (pinP == A12) && (pinN == A13) ) { // DAD3 selected, pins 36 and 37
        __disable_irq();
	    ADC0_SC1A = ADC_SC1_DIFF + 0x3 + var_enableInterrupts*ADC_SC1_AIEN;
	    __enable_irq();
	} else {
        return;
    }

    return;
}

/* Reads the analog value of the continuous conversion set with analogStartContinuous(pin)
 * It returns the last converted value.
*/
int ADC::analogReadContinuous()
{
    // The result is a 16 bit extended sign 2's complement number (the sign bit goes from bit 15 to analog_config_bits-1)
	// if the number is negative we fill the rest of the 1's upto 32 bits (we extend the sign)
	int16_t result = ADC0_RA;
    if (result & (1<<15)) { // number is negative
        result |= 0xFFFF0000; // result is a 32 bit integer
    }

	return result;
}

/* Stops continuous conversion
*/
void ADC::stopContinuous()
{
	ADC0_SC1A |= 0x1F; // set channel select to all 1's (31) to stop it.

	return;
}


/* void function that does nothing
*/
void ADC::voidFunction(){return;}

/* Callback from the ADC interrupt, it adds the new value to the ring buffer
* it takes around 3 us
*/
void ADC::ADC_callback() {
    //digitalWriteFast(ledPin, HIGH);

    // get the pin number
    int pin = sc1a2channel[ADC0_SC1A & 0x1F];

    // find the index of the pin
    int i = 0;
    while( (i<MAX_ANALOG_TIMERS) && (analogTimer[i]->pinNumber!=pin) ) {i++;}
    if( i==MAX_ANALOG_TIMERS) {
        //digitalWriteFast(ledPin, LOW);
        return; // the last measurement doesn't belong to an analog timer buffer.
    }

    // place value in its buffer
    analogTimer[i]->buffer->write(readSingle());

    // restore ADC config if it was in use before being interrupted by the analog timer
    if (adcWasInUse) {
        // restore ADC config, and restart conversion
        //if(adc_config.diffRes)
        setResolution(adc_config.savedRes); // don't change res if isn't necessary
        ADC0_CFG1 = adc_config.savedCFG1;
        ADC0_CFG2 = adc_config.savedCFG2;
        ADC0_SC2 = adc_config.savedSC2 & 0x7F;
        ADC0_SC3 = adc_config.savedSC3 & 0xF;
        ADC0_SC1A = adc_config.savedSC1A & 0x7F;
    }

    //digitalWriteFast(ledPin, LOW);
}


/* callback function for the analog timers
*  it takes around 2.5 us
*/
void ADC::analogTimerCallback0() {
    //digitalWriteFast(ledPin, HIGH);

    uint8_t pin = analogTimer[0]->pinNumber;
    if(analogTimer[0]->isDiff) {
        if(pin == A10) {
            startSingleDifferential(A10, A11);
        } else if(pin == A12) {
            startSingleDifferential(A12, A13);
        }
    } else {
        startSingleRead(pin);
    }

    //digitalWriteFast(ledPin, LOW);
}
/* callback function for the analog timers
*  it takes around 2.5 us
*/
void ADC::analogTimerCallback1() {
    //digitalWriteFast(ledPin, HIGH);

    uint8_t pin = analogTimer[1]->pinNumber;
    if(analogTimer[1]->isDiff) {
        if(pin == A10) {
            startSingleDifferential(A10, A11);
        } else if(pin == A12) {
            startSingleDifferential(A12, A13);
        }
    } else {
        startSingleRead(pin);
    }

    //digitalWriteFast(ledPin, LOW);
}
/* callback function for the analog timers
*  it takes around 2.5 us
*/
void ADC::analogTimerCallback2() {
    //digitalWriteFast(ledPin, HIGH);

    uint8_t pin = analogTimer[2]->pinNumber;
    if(analogTimer[2]->isDiff) {
        if(pin == A10) {
            startSingleDifferential(A10, A11);
        } else if(pin == A12) {
            startSingleDifferential(A12, A13);
        }
    } else {
        startSingleRead(pin);
    }

    //digitalWriteFast(ledPin, LOW);
}

/* Starts a periodic measurement.
* The values will be added to a ring buffer of a fixed size.
*   Read the oldest value with getTimerValue(pin), check if it's the last value with isLastValue(pin).
*   When the buffer is full, new data will overwrite the oldest values.
*/
int ADC::startAnalogTimer(uint8_t pin, uint32_t period) {

    // check pin
    if (pin < 14 || pin > 39) {
        return ANALOG_TIMER_ERROR;   // invalid pin
    }

    // if the resolution is incorrect (i.e. 9, 11 or 13) silently correct it here
	uint8_t res = getResolution();
	if( (res==9) || (res==11) || (res==13) ) {
        setResolution(res-1);
	}

    // find next timerPin not in use
    int i = 0;
    for(i=0; i<MAX_ANALOG_TIMERS; i++) {
        if(analogTimer[i]->pinNumber==-1) {
            break;
        } else if(analogTimer[i]->pinNumber==pin) { // the timer already exists, do nothing
            return true;
        }
    }
    if( i == (MAX_ANALOG_TIMERS) ) {  // All timers are being used, stop at least one of them!
        return ANALOG_TIMER_ERROR;
    }

    analogTimer[i]->pinNumber = pin; // reserve a timer for this pin

    // create both objects
    analogTimer[i]->timer = new IntervalTimer;
    analogTimer[i]->buffer = new RingBuffer;

    // store period
    analogTimer[i]->period = period;

	// point the adc_isr to the function that takes care of the timers
	analogTimer_ADC_Callback = &ADC_callback;
    // enable interrupts
	enableInterrupts();

    // start timerPin # i
    int result = analogTimer[i]->timer->begin(analogTimerCallback[i], period);
    if(!result) { // begin returns true/false
            return ANALOG_TIMER_ERROR;
    }

    return result;
}

/*  Starts a periodic measurement using the IntervalTimer library.
*   The values will be added to a ring buffer of a fixed size.
*   Read the oldest value with getTimerValue(pinP), check if it's the last value with isLastValue(pinP).
*   When the buffer is full, new data will overwrite the oldest values.
*   \param pinP must be A10 or A12.
*   \param pinN must be A11 (if pinP=A10) or A13 (if pinP=A12).
*   \returns ANALOG_TIMER_ERROR if the timer could not be started. Stop other analog timer and retry.
*/
int ADC::startAnalogTimerDifferential(uint8_t pinP, uint8_t pinN, uint32_t period) {

    // check pin
    if ( (pinP != A10) && (pinP != A12) ) {
        return ANALOG_TIMER_ERROR;   // invalid pin
    }

    // if the resolution is incorrect (i.e. 8, 10 or 12) silently correct it here
	uint8_t res = getResolution();
	if( (res==8) || (res==10) || (res==12) ) {
        setResolution(res+1);
	}

    // find next timerPin not in use
    int i = 0;
    for(i=0; i<MAX_ANALOG_TIMERS; i++) {
        if(analogTimer[i]->pinNumber==-1) {
            break;
        } else if(analogTimer[i]->pinNumber==pinP) { // the timer already exists, do nothing
            return true;
        }
    }
    if( i == (MAX_ANALOG_TIMERS) ) {  // All timers are being used, stop at least one of them!
        return ANALOG_TIMER_ERROR;
    }

    analogTimer[i]->pinNumber = pinP; // reserve a timer for this pin

    // create both objects
    analogTimer[i]->timer = new IntervalTimer;
    analogTimer[i]->buffer = new RingBuffer;

    analogTimer[i]->period = period;
    analogTimer[i]->isDiff = true;

	// point the adc_isr to the function that takes care of the timers
	analogTimer_ADC_Callback = &ADC_callback;
    // enable interrupts
	enableInterrupts();

    // start timerPin # i
    int result = analogTimer[i]->timer->begin(analogTimerCallback[i], period);
    if(!result) { // begin returns true/false
            return ANALOG_TIMER_ERROR;
    }

    return result;
}


// Stops the periodic measurement
void ADC::stopAnalogTimer(uint8_t pin) {
    // find corresponding timerPin
    int i = 0;
    while( (i<MAX_ANALOG_TIMERS) && (analogTimer[i]->pinNumber!=pin) ) {i++;}
    if( i == MAX_ANALOG_TIMERS ) { // Timer doesn't exist
        return;
    }

    // stop timerPin # i
    analogTimer[i]->timer->end();
    analogTimer[i]->pinNumber = -1; // timerPin i is free now

    delete analogTimer[i]->timer;
    delete analogTimer[i]->buffer;

    // check if there are more analog timers in use
    i = 0;
    while( (i<MAX_ANALOG_TIMERS) && (analogTimer[i]->pinNumber!=-1) ) {i++;}
    if( i == MAX_ANALOG_TIMERS ) { // no more analog timers
        disableInterrupts();
        // point the adc_isr to the function that does nothing
        analogTimer_ADC_Callback = &voidFunction;
    }


    return;
}

// Returns the oldest value of the ring buffer where the periodic measurements go
int ADC::getTimerValue(uint8_t pin) {
    // find corresponding timerPin
    int i = 0;
    while( (i<MAX_ANALOG_TIMERS) && (analogTimer[i]->pinNumber!=pin) ) {i++;}
    if( i == MAX_ANALOG_TIMERS ) { // Timer not found with that pin number!
        return ANALOG_TIMER_ERROR;
    }

    return analogTimer[i]->buffer->read(); // read the oldest value in the buffer
}

// Is the oldest value also the last one?
bool ADC::isTimerLastValue(uint8_t pin) {
    // find corresponding timerPin
    int i = 0;
    while( (i<MAX_ANALOG_TIMERS) && (analogTimer[i]->pinNumber!=pin) ) {i++;}
    if( i == MAX_ANALOG_TIMERS ) { // Timer not found with that pin number!
        return ANALOG_TIMER_ERROR;
    }

    //return timerPin[i].buffer->isEmpty();
    return analogTimer[i]->buffer->isEmpty();
}



/* Is the ADC converting at the moment?
*  Returns 1 if yes, 0 if not
*/
bool ADC::isConverting() {

    return (ADC0_SC2 & ADC_SC2_ADACT) >> 7;
}


/* Is an ADC conversion ready?
*  Returns 1 if yes, 0 if not.
*  When a value is read this function returns 0 until a new value exists
*  So it only makes sense to call it before analogRead(), analogReadContinuous() or analogReadDifferential()
*/
bool ADC::isComplete() {

    return (ADC0_SC1A & ADC_SC1_COCO) >> 7;
}


bool ADC::isDifferential() {
    return (ADC0_SC1A & ADC_SC1_DIFF) >> 5;
}

bool ADC::isContinuous() {
    return (ADC0_SC3 & ADC_SC3_ADCO) >> 3;
}
