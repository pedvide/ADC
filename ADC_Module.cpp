 /* ADC Library created by Pedro Villanueva
 * It implements all functions of the Teensy 3.0 internal ADC
 * Some of the code was extracted from the file analog.c included in the distribution of Teensduino by PJRC.COM
 *
 */


 /* TODO
  * Stuff related to the conversion speed and power. Useful if you want more speed or to save power.
  * Function to measure more that 1 pin consecutively
  *
  * bugs:
  * - analog timer at 16 bits resolution goes from 0 to +1.65 and then jumps to -1.65 to 0
  * - comparison values don't work in 16 bit differential mode (they are twice what you write)
 */


#include "ADC_Module.h"


// static vars need to be restated here
/* OLD
const uint8_t ADC_Module::channel2sc1a[]= { // new version, gives directly the sc1a number
    5, 14, 8, 9, 13, 12, 6, 7, 15, 4,
    0, 19, 3, 21, 26, 22};
*/

const uint8_t ADC_Module::channel2sc1aADC0[]= { // new version, gives directly the sc1a number, digital-only pins deactivate ADC (0x1F=31)
    5, 14, 8, 9, 13, 12, 6, 7, 15, 4, 0, 19, 3, 21, // 0-13, we treat them as A0-A13
    5, 14, 8, 9, 13, 12, 6, 7, 15, 4, // 14-23 (A0-A9)
    31, 31, 31, 31, 31, 31, 31, 31, 31, 31, // 24-33
    0, 19, 3, 21, // 34-37 (A10-A13)
    26, 22, 23, 27, 29, 30 // 38-43: temp. sensor, VREF_OUT, A14, bandgap, VREFH, VREFL. A14 isn't connected to anything in Teensy 3.0.
    };
const uint8_t ADC_Module::sc1a2channelADC0[]= { // new version, gives directly the pin number
    34, 0, 0, 36, 23, 14, 20, 21, 16, 17, 0, 0, 19, 18, // 0-13
    15, 22, 0, 0, 0, 35, 0, 37, // 14-21
    39, 40, 0, 0, 38, 41, 42, 43 // VREF_OUT, A14, temp. sensor, bandgap, VREFH, VREFL.
    };

const uint8_t ADC_Module::channel2sc1aADC1[]= { // new version, gives directly the sc1a number, digital-only pins deactivate ADC (0x1F=31)
    31, 31, 8, 9, 31, 31, 31, 31, 31, 31, 3, 31, 0, 19, // 0-13, we treat them as A0-A13
    31, 31, 8, 9, 31, 31, 31, 31, 31, 31, // 14-23 (A0-A9)
    31, 31,  // 24,25 are digital only pins
    5, 5, 4, 6, 7, 4, 31, 31, // 26-33 26=5a, 27=5b, 28=4b, 29=6b, 30=7b, 31=4a, 32,33 are digital only
    3, 31, 0, 19, // 34-37 (A10-A13) A11 isn't connected.
    26, 18, 31, 27, 29, 30 // 38-41: temp. sensor, VREF_OUT, A14 (not connected), bandgap, VREFH, VREFL.
    };
const uint8_t ADC_Module::sc1a2channelADC1[]= { // new version, gives directly the pin number INCOMPLETE!!!!
    34, 0, 0, 36, 23, 14, 20, 21, 16, 17, 0, 0, 19, 18, // 0-13
    15, 22, 0, 0, 0, 35, 0, 37, // 14-21
    39, 40, 0, 0, 38, 41, 42, 43 // VREF_OUT, A14, temp. sensor, bandgap, VREFH, VREFL.
    };


/* Constructor
*
*/
ADC_Module::ADC_Module(uint8_t ADC_number) {

    // default settings: 10 bits resolution (in analog_init), 4 averages, vcc reference, no interrupts, pga gain=0 (pga_value=2^gain)
    analog_config_bits = 0; // initiate to 0 so setResolution() changes it to the correct value
    analog_max_val = 1024;
    analog_num_average = 0; // initiate to 0 so setAveraging() changes it to the correct value
    analog_reference_internal = 0;
    var_enableInterrupts = 0;
    pga_value = 1;

    // ADC0 or ADC1?
    ADC_num = ADC_number;

    // point the control registers to the correct addresses
    // the offset is supposed to be 0x80000, but it actually is 0x20000, why? no idea....
    uint32_t adc_offset = (uint32_t)0x20000;
    ADC_SC1A = &ADC0_SC1A + adc_offset*ADC_num;
    ADC_SC1B = &ADC0_SC1B + adc_offset*ADC_num;

    ADC_CFG1 = &ADC0_CFG1 + adc_offset*ADC_num;
    ADC_CFG2 = &ADC0_CFG2 + adc_offset*ADC_num;

    ADC_RA = &ADC0_RA + adc_offset*ADC_num;
    ADC_RB = &ADC0_RB + adc_offset*ADC_num;

    ADC_CV1 = &ADC0_CV1 + adc_offset*ADC_num;
    ADC_CV2 = &ADC0_CV2 + adc_offset*ADC_num;

    ADC_SC2 = &ADC0_SC2 + adc_offset*ADC_num;
    ADC_SC3 = &ADC0_SC3 + adc_offset*ADC_num;

    ADC_PGA = &ADC0_PGA + adc_offset*ADC_num;

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

    // copy the correct version of channel2sc1a and sc1a2channel depending on the ADC module we are.
    #if defined(__MK20DX128__)
    memcpy(&channel2sc1a, &channel2sc1aADC0, sizeof(channel2sc1a));
    memcpy(&sc1a2channel, &sc1a2channelADC0, sizeof(sc1a2channel));
    #elif defined(__MK20DX256__)
    if(ADC_num==0) { // ADC0
        memcpy(&channel2sc1a, &channel2sc1aADC0, sizeof(channel2sc1a));
        memcpy(&sc1a2channel, &sc1a2channelADC0, sizeof(sc1a2channel));
    } else { // ADC1
        memcpy(&channel2sc1a, &channel2sc1aADC1, sizeof(channel2sc1a));
        memcpy(&sc1a2channel, &sc1a2channelADC1, sizeof(sc1a2channel));
    } // end ADC0 or ADC1?
    #endif

    // call our init
    analog_init();

}

/* Destructor
*
*/
ADC_Module::~ADC_Module() {
    //dtor

}


void ADC_Module::analog_init(uint32_t config)
{

    // Internal reference initialization
    VREF_TRM = VREF_TRM_CHOPEN | 0x20; // enable module and set the trimmer to medium (max=0x3F=63)
	VREF_SC = VREF_SC_VREFEN | VREF_SC_REGEN | VREF_SC_ICOMPEN | VREF_SC_MODE_LV(1); // (=0xE1) enable 1.2 volt ref with all compensations
    //VREF_TRM = 0x60;
	//VREF_SC = 0xE1;

	if (analog_reference_internal) {
		ADC0_SC2 |= ADC_SC2_REFSEL(1); // 1.2V ref
	} else {
		ADC0_SC2 |= ADC_SC2_REFSEL(0); // vcc/ext ref
	}

    // set resolution
    setResolution(10);

    // number of averages
	setAveraging(4);

    // begin calibration
	calibrate();
}


void ADC_Module::calibrate() {
    *ADC_SC3 |= ADC_SC3_CAL;

	calibrating = 1;
}


/* Waits until calibration is finished and writes the corresponding registers
*
*/
void ADC_Module::wait_for_cal(void)
{
	uint16_t sum;

    while(!(VREF_SC & VREF_SC_VREFST)) { // Wait until the reference is stable before calibrating
		// wait
		//serial_print(".");
	}

	while((*ADC_SC3) & ADC_SC3_CAL) { // Bit ADC_SC3_CAL in register ADC0_SC3 cleared when calib. finishes.
		// wait
		//serial_print(".");
	}

	__disable_irq();
	if (calibrating) {
		//serial_print("\n");
		sum = *ADC_CLPS + *ADC_CLP4 + *ADC_CLP3 + *ADC_CLP2 + *ADC_CLP1 + *ADC_CLP0;
		sum = (sum / 2) | 0x8000;
		*ADC_PG = sum;
		//serial_print("ADC0_PG = ");
		//serial_phex16(sum);
		//serial_print("\n");
		sum = *ADC_CLMS + *ADC_CLM4 + *ADC_CLM3 + *ADC_CLM2 + *ADC_CLM1 + *ADC_CLM0;
		sum = (sum / 2) | 0x8000;
		*ADC_MG = sum;
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
void ADC_Module::setReference(uint8_t type)
{
    if (analog_reference_internal==type) { // don't need to change anything
            return;
    }
	if (type == ADC_REF_INTERNAL) {
		// internal reference requested
        analog_reference_internal = ADC_REF_INTERNAL;
        *ADC_SC2 |= ADC_SC2_REFSEL(1); // 1.2V ref
        if (calibrating) *ADC_SC3 |= ADC_SC3_CAL; // cancel cal
        calibrate();
	} else if(type == ADC_REF_EXTERNAL) {
		// vcc or external reference requested
        analog_reference_internal = ADC_REF_EXTERNAL;
        *ADC_SC2 |= ADC_SC2_REFSEL(0); // vcc/ext ref
        if (calibrating) *ADC_SC3 |= ADC_SC3_CAL; // cancel cal
        calibrate();
	}
}


/* Change the resolution of the measurement
*  For single-ended measurements: 8, 10, 12 or 16 bits.
*  For differential measurements: 9, 11, 13 or 16 bits.
*  If you want something in between (11 bits single-ended for example) select the inmediate higher
*  and shift the result one to the right.
*/
void ADC_Module::setResolution(uint8_t bits)
{
    uint8_t config;

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

        // conversion resolution and frequency
        if ( (analog_config_bits == 8) || (analog_config_bits == 9) )  {
            *ADC_CFG1 = ADC_CFG1_24MHZ + ADC_CFG1_MODE(0); // 0 clock divide + Input clock: 24 MHz (run at 24 MHz) + Conversion mode: 8 bit + Sample time: Short
            *ADC_CFG2 = ADC_CFG2_MUXSEL + ADC_CFG2_ADLSTS(3); // b channels + Sample time 2 cycles (I think that if ADC_CFG1_ADLSMP isn't set, then ADC_CFG2_ADLSTS doesn't matter)
            analog_max_val = 256; // diff mode 9 bits has 1 bit for sign, so max value is the same as single 8 bits
        } else if ( (analog_config_bits == 10 )|| (analog_config_bits == 11) ) { // total clock cycles to complete conversion: 3 ADCK + 5 BUS + 4 averages*( 20 + 2 ADCK ) = 7.8 us
            *ADC_CFG1 = ADC_CFG1_12MHZ + ADC_CFG1_MODE(2) + ADC_CFG1_ADLSMP; // Clock divide: 1/2 + Input clock: 24 MHz (run at 12 MHz) + Conversion mode: 10 bit + Sample time: Long
            *ADC_CFG2 = ADC_CFG2_MUXSEL + ADC_CFG2_ADLSTS(3); // b channels + Sample time 2 extra clock cycles
            analog_max_val = 1024;
        } else if ( (analog_config_bits == 12 )|| (analog_config_bits == 13) ) {
            *ADC_CFG1 = ADC_CFG1_12MHZ + ADC_CFG1_MODE(1) + ADC_CFG1_ADLSMP; // Clock divide: 1/2 + Input clock: 24 MHz (run at 12 MHz) + Conversion mode: 12 bit + Sample time: Long
            *ADC_CFG2 = ADC_CFG2_MUXSEL + ADC_CFG2_ADLSTS(2); // b channels + Sample time: 6 extra clock cycles
            analog_max_val = 4096;
        } else {
            *ADC_CFG1 = ADC_CFG1_12MHZ + ADC_CFG1_MODE(3) + ADC_CFG1_ADLSMP;  //Clock divide: 1/2 + Input clock: 24 MHz (run at 12 MHz) + Conversion mode: 16 bit + Sample time: Long
            *ADC_CFG2 = ADC_CFG2_MUXSEL + ADC_CFG2_ADLSTS(2); // b channels + Sample time: 6 extra clock cycles
            analog_max_val = 65536;
        }

		// no recalibration is needed when changing the resolution, p. 619
		// but it's needed if we change the frequency...
		if (calibrating) *ADC_SC3 |= ADC_SC3_CAL; // cancel cal
		calibrate(); // re-cal
	} // end if change res
}

/* Returns the resolution of the ADC
*
*/
uint8_t ADC_Module::getResolution() {
    return analog_config_bits;
}

/* Returns the maximum value for a measurement, that is 2^resolution
*
*/
uint32_t ADC_Module::getMaxValue() {
    return analog_max_val-1;
}


/* Set the number of averages: 0, 4, 8, 16 or 32.
*
*/
void ADC_Module::setAveraging(uint8_t num)
{

	if (calibrating) wait_for_cal();
	if (num <= 1) {
		num = 0;
		*ADC_SC3 &= ~ADC_SC3_AVGE;
	} else if (num <= 4) {
		num = 4;
		*ADC_SC3 |= ADC_SC3_AVGE + ADC_SC3_AVGS(0);
	} else if (num <= 8) {
		num = 8;
		*ADC_SC3 |= ADC_SC3_AVGE + ADC_SC3_AVGS(1);
	} else if (num <= 16) {
		num = 16;
		*ADC_SC3 |= ADC_SC3_AVGE + ADC_SC3_AVGS(2);
	} else {
		num = 32;
		*ADC_SC3 |= ADC_SC3_AVGE + ADC_SC3_AVGS(3);
	}
	analog_num_average = num;
}


/* Enable interrupts: An ADC Interrupt will be raised when the conversion is completed
*  (including hardware averages and if the comparison (if any) is true).
*/
void ADC_Module::enableInterrupts() {
    var_enableInterrupts = 1;
    if(ADC_num==1) { // enable correct interrupt
        NVIC_ENABLE_IRQ(IRQ_ADC1);
    } else {
        NVIC_ENABLE_IRQ(IRQ_ADC0);
    }
}

/* Disable interrupts
*
*/
void ADC_Module::disableInterrupts() {
    var_enableInterrupts = 0;
    if(ADC_num==1) { // disable correct interrupt
        NVIC_DISABLE_IRQ(IRQ_ADC1);
    } else {
        NVIC_DISABLE_IRQ(IRQ_ADC0);
    }
}


/* Enable DMA request: An ADC DMA request will be raised when the conversion is completed
*  (including hardware averages and if the comparison (if any) is true).
*/
void ADC_Module::enableDMA() {
    *ADC_SC2 |= ADC_SC2_DMAEN;
}

/* Disable ADC DMA request
*
*/
void ADC_Module::disableDMA() {
    *ADC_SC2 &= ~ADC_SC2_DMAEN;
}


/* Enable the compare function: A conversion will be completed only when the ADC value
*  is >= compValue (greaterThan=1) or < compValue (greaterThan=0)
*  Call it after changing the resolution
*  Use with interrupts or poll conversion completion with isADC_Complete()
*/
void ADC_Module::enableCompare(int16_t compValue, int greaterThan) {
    *ADC_SC2 |= ADC_SC2_ACFE | greaterThan*ADC_SC2_ACFGT;
    *ADC_CV1 = compValue;
}

/* Enable the compare function: A conversion will be completed only when the ADC value
*  is inside (insideRange=1) or outside (=0) the range given by (lowerLimit, upperLimit),
*  including (inclusive=1) the limits or not (inclusive=0).
*  See Table 31-78, p. 617 of the freescale manual.
*  Call it after changing the resolution
*  Use with interrupts or poll conversion completion with isComplete()
*/
void ADC_Module::enableCompareRange(int16_t lowerLimit, int16_t upperLimit, int insideRange, int inclusive) {

    if(insideRange && inclusive) { // True if value is inside the range, including the limits. CV1 <= CV2 and ACFGT=1
        *ADC_CV1 = lowerLimit;
        *ADC_CV2 = upperLimit;
        *ADC_SC2 |= ADC_SC2_ACFE | ADC_SC2_ACFGT | ADC_SC2_ACREN;
    } else if(insideRange && !inclusive) {// True if value is inside the range, excluding the limits. CV1 > CV2 and ACFGT=0
        *ADC_CV2 = lowerLimit;
        *ADC_CV1 = upperLimit;
        *ADC_SC2 |= ADC_SC2_ACFE | ADC_SC2_ACREN;
    } else if(!insideRange && inclusive) { // True if value is outside of range or is equal to either limit. CV1 > CV2 and ACFGT=1
        *ADC_CV2 = lowerLimit;
        *ADC_CV1 = upperLimit;
        *ADC_SC2 |= ADC_SC2_ACFE | ADC_SC2_ACFGT | ADC_SC2_ACREN;
    } else if(!insideRange && !inclusive) { // True if value is outside of range and not equal to either limit. CV1 > CV2 and ACFGT=0
        *ADC_CV1 = lowerLimit;
        *ADC_CV2 = upperLimit;
        *ADC_SC2 |= ADC_SC2_ACFE | ADC_SC2_ACREN;
    }
}

/* Disable the compare function
*
*/
void ADC_Module::disableCompare() {
    *ADC_SC2 &= ~ADC_SC2_ACFE;
}

/* Enables the PGA and sets the gain
*   Use only for signals lower than 1.2 V
*   \param gain From 0 to 6, set PGA to 2^gain
*
*/
void ADC_Module::enablePGA(uint8_t gain) {
    if(gain==0) {
        disablePGA();
    } else {
        *ADC_PGA = ADC0_PGA_PGAEN | ADC0_PGA_PGAG(gain);
        pga_value=1<<gain;
    }
}

/* Returns the PGA level
*  PGA level = 2^gain, from 0 to 64
*/
uint8_t ADC_Module::getPGA() {
    return pga_value;
}

//! Disable PGA
void ADC_Module::disablePGA() {
    *ADC_PGA &= ~ADC0_PGA_PGAEN;
    pga_value = 1;
}


/* Reads the analog value of the pin.
* It waits until the value is read and then returns the result.
* If a comparison has been set up and fails, it will return ADC_ERROR_VALUE.
* Set the resolution, number of averages and voltage reference using the appropriate functions.
*/
int ADC_Module::analogRead(uint8_t pin)
{
	uint16_t result;

	// increase the counter of measurements
	num_measurements++;

	if ( (pin < 0) || (pin > 43) ) {
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
        savedSC1A = *ADC_SC1A;
        savedCFG1 = *ADC_CFG1;
        savedCFG2 = *ADC_CFG2;
        savedSC2 = *ADC_SC2;
        savedSC3 = *ADC_SC3;

        // change the comparison values if interrupting a 16 bits and diff mode
        if(res==16 && isDifferential()) {
            *ADC_CV1 /= 2;
            *ADC_CV2 /= 2;
        }

        // disable continuous mode in case analogRead is interrupting a continuous mode
        *ADC_SC3 &= ~ADC_SC3_ADCO;

        __enable_irq(); ////keep irq disabled until we start our conversion

    }


    // if the resolution is incorrect (i.e. 9, 11 or 13) silently correct it
	if( (res==9) || (res==11) || (res==13) ) {
        setResolution(res-1);
        diffRes = 1; // resolution changed
	}

	#if defined(__MK20DX256__)
	// ADC1 has A15=5a and A20=4a so we have to change the mux to read the "a" channels
	if( (ADC_num==1) && ( (pin==26) || (pin==31) ) ) {
        *ADC_CFG2 &= ~ADC_CFG2_MUXSEL;
	} else {
        *ADC_CFG2 |= ADC_CFG2_MUXSEL;
	}
	#endif

    __disable_irq();
    *ADC_SC1A = channel2sc1a[pin] + var_enableInterrupts*ADC_SC1_AIEN; // start conversion on pin and with interrupts enabled if requested
	__enable_irq();


	while (1) {
		__disable_irq();
		if ((*ADC_SC1A & ADC_SC1_COCO)) { // conversion completed
			result = *ADC_RA;

			// if we interrupted a conversion, set it again
            if (wasADCInUse) {
                //GPIOC_PCOR = 1<<5;

                // restore ADC config, and restart conversion
                if(diffRes) setResolution(savedRes); // don't change res if isn't necessary
                if(res==16 && ((savedSC1A & ADC_SC1_DIFF) >> 5) )  { // change back the comparison values if interrupting a 16 bits and diff mode
                    *ADC_CV1 *= 2;
                    *ADC_CV2 *= 2;
                }
                *ADC_CFG1 = savedCFG1;
                *ADC_CFG2 = savedCFG2;
                *ADC_SC2 = savedSC2 & 0x7F; // restore first 8 bits
                *ADC_SC3 = savedSC3 & 0xF; // restore first 4 bits
                *ADC_SC1A = savedSC1A & 0x7F; // restore first 8 bits
            }
			__enable_irq();
			return result;
		} else if( ((*ADC_SC2 & ADC_SC2_ADACT) == 0) && ((*ADC_SC1A & ADC_SC1_COCO) == 0) ) { // comparison was false
		    // we needed to check that ADACT wasn't 0 because COCO just turned 1.

		    // if we interrupted a conversion, set it again
            if (wasADCInUse) {
                //GPIOC_PCOR = 1<<5;

                // restore ADC config, and restart conversion
                if(diffRes) setResolution(savedRes); // don't change res if isn't necessary
                if(res==16 && ((savedSC1A & ADC_SC1_DIFF) >> 5) )  { // change back the comparison values if interrupting a 16 bits and diff mode
                    *ADC_CV1 *= 2;
                    *ADC_CV2 *= 2;
                }
                *ADC_CFG1 = savedCFG1;
                *ADC_CFG2 = savedCFG2;
                *ADC_SC2 = savedSC2 & 0x7F; // restore first 8 bits
                *ADC_SC3 = savedSC3 & 0xF; // restore first 4 bits
                *ADC_SC1A = savedSC1A & 0x7F; // restore first 8 bits
            }

		    // comparison was false, we return an error value to indicate this
		    __enable_irq();
            return ADC_ERROR_VALUE;
		} // end if comparison false

		__enable_irq();
		yield();
	} // end while

	// deacrease the counter of measurements
	num_measurements--;

} // analogRead



/* Reads the differential analog value of two pins (pinP - pinN)
* It waits until the value is read and then returns the result
* If a comparison has been set up and fails, it will return -70000
* Set the resolution, number of averages and voltage reference using the appropriate functions
*/
int ADC_Module::analogReadDifferential(uint8_t pinP, uint8_t pinN)
{
	int16_t result;

	// increase the counter of measurements
	num_measurements++;

	// check for calibration before setting channels,
	// because conversion will start as soon as we write to *ADC_SC1A
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
        savedSC1A = *ADC_SC1A;// & 0x7F; // get first 7 bits
        savedCFG1 = *ADC_CFG1;
        savedCFG2 = *ADC_CFG2;
        savedSC2 = *ADC_SC2;// & 0x7F; // get first 7 bits
        savedSC3 = *ADC_SC3;// & 0xF; // get first 4 bits

        // disable continuous mode in case analogReadDifferential is interrupting a continuous mode
        *ADC_SC3 &= ~ADC_SC3_ADCO;
        __enable_irq();
    }

    // if the resolution is incorrect (i.e. 8, 10 or 12) silently correct it
	if( (res==8) || (res==10) || (res==12) ) {
        setResolution(res+1);
        diffRes = 1; // resolution changed
    } else if(res==16) {
        *ADC_CV1 /= 2; // correct also compare function in case it was enabled
        *ADC_CV2 /= 2;
    }

    // once *ADC_SC1A is set, conversion will start, enable interrupts if requested
	if ( (pinP == A10) && (pinN == A11) ) { // DAD0 selected, pins 34 and 35
        __disable_irq();
        #if defined(__MK20DX256__)
        if ( *ADC_PGA & ADC0_PGA_PGAEN ) { // PGA is enabled
            if(ADC_num == 0) { // PGA0 connects to A10-A11
                *ADC_SC1A = ADC_SC1_DIFF + 0x2 + var_enableInterrupts*ADC_SC1_AIEN;
                __enable_irq();
            } else { // If this is ADC1 we can't connect to PGA0
                __enable_irq();
                return ADC_ERROR_DIFF_VALUE;
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
        if ( *ADC_PGA & ADC0_PGA_PGAEN ) { // PGA is enabled
            if(ADC_num == 1) { // PGA1 connects to A12-A13
                *ADC_SC1A = ADC_SC1_DIFF + 0x2 + var_enableInterrupts*ADC_SC1_AIEN;
                __enable_irq();
            } else { // If this is ADC0 we can't connect to PGA1
                __enable_irq();
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
        return ADC_ERROR_DIFF_VALUE; // all others aren't capable of differential measurements, perhaps return analogRead(pinP)-analogRead(pinN)?
    }

    while (1) {
		__disable_irq();
		if ((*ADC_SC1A & ADC_SC1_COCO)) { // conversion completed
			result = *ADC_RA;

			// if we interrupted a conversion, set it again
            if (adcWasInUse) {
                //GPIOC_PTOR = 1<<5;

                // restore ADC config, and restart conversion
                if(diffRes) setResolution(savedRes); // don't change res if isn't necessary
                if(res==16) {
                    *ADC_CV1 *= 2; // change back the comparison values
                    *ADC_CV2 *= 2;
                }
                *ADC_CFG1 = savedCFG1;
                *ADC_CFG2 = savedCFG2;
                *ADC_SC2 = savedSC2 & 0x7F;
                *ADC_SC3 = savedSC3 & 0xF;
                *ADC_SC1A = savedSC1A & 0x7F;
            }

			__enable_irq();
			if (result & (1<<15)) { // number is negative
                result |= 0xFFFF0000; // result is a 32 bit integer
            }
			return result;
		} else if( ((*ADC_SC2 & ADC_SC2_ADACT) == 0) && ((*ADC_SC1A & ADC_SC1_COCO) == 0) ) {
		    // we needed to check that ADACT wasn't 0 because COCO just turned 1.

		    // if we interrupted a conversion, set it again
            if (adcWasInUse) {
                //GPIOC_PTOR = 1<<5;

                // restore ADC config, and restart conversion
                if(diffRes) setResolution(savedRes); // don't change res if isn't necessary
                if(res==16) {
                    *ADC_CV1 *= 2; // change back the comparison values
                    *ADC_CV2 *= 2;
                }
                *ADC_CFG1 = savedCFG1;
                *ADC_CFG2 = savedCFG2;
                *ADC_SC2 = savedSC2 & 0x7F;
                *ADC_SC3 = savedSC3 & 0xF;
                *ADC_SC1A = savedSC1A & 0x7F;
            }

		    // comparison was false, we return an error value to indicate this
            __enable_irq();
            return ADC_ERROR_DIFF_VALUE;
		} // end if comparison false

		__enable_irq();
		yield();
	} // while

	// deacrease the counter of measurements
	num_measurements--;

} // analogReadDifferential



/* Starts an analog measurement on the pin.
*  It returns inmediately, read value with readSingle().
*  If the pin is incorrect it returns ADC_ERROR_VALUE.
*/
int ADC_Module::startSingleRead(uint8_t pin) {

    if ( (pin < 0) || (pin > 43) ) {
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
        adc_config.savedSC1A = *ADC_SC1A;
        adc_config.savedCFG1 = *ADC_CFG1;
        adc_config.savedCFG2 = *ADC_CFG2;
        adc_config.savedSC2 = *ADC_SC2;
        adc_config.savedSC3 = *ADC_SC3;
        //__enable_irq(); //keep irq disabled until we start our conversion

    }

    #if defined(__MK20DX256__)
	// ADC1 has A15=5a and A20=4a so we have to change the mux to read the "a" channels
	if( (ADC_num==1) && ( (pin==26) || (pin==31) ) ) {
        *ADC_CFG2 &= ~ADC_CFG2_MUXSEL;
	} else {
        *ADC_CFG2 |= ADC_CFG2_MUXSEL;
	}
	#endif

    // select pin for single-ended mode and start conversion, enable interrupts to know when it's done
    __disable_irq();
    *ADC_SC1A = channel2sc1a[pin] + var_enableInterrupts*ADC_SC1_AIEN;
	__enable_irq();


    return true;
}
int ADC_Module::startSingleReadFast(uint8_t pin) {
    #if defined(__MK20DX256__)
	// ADC1 has A15=5a and A20=4a so we have to change the mux to read the "a" channels
	if( (ADC_num==1) && ( (pin==26) || (pin==31) ) ) {
        *ADC_CFG2 &= ~ADC_CFG2_MUXSEL;
	} else {
        *ADC_CFG2 |= ADC_CFG2_MUXSEL;
	}
	#endif

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
int ADC_Module::startSingleDifferential(uint8_t pinP, uint8_t pinN)
{

	// check for calibration before setting channels,
	// because conversion will start as soon as we write to *ADC_SC1A
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
        adc_config.savedSC1A = *ADC_SC1A;
        adc_config.savedCFG1 = *ADC_CFG1;
        adc_config.savedCFG2 = *ADC_CFG2;
        adc_config.savedSC2 = *ADC_SC2;
        adc_config.savedSC3 = *ADC_SC3;
        //__enable_irq(); //keep irq disabled until we start our conversion

    }

    // once *ADC_SC1A is set, conversion will start, enable interrupts if requested
	if ( (pinP == A10) && (pinN == A11) ) { // DAD0 selected, pins 34 and 35
        __disable_irq();
        #if defined(__MK20DX256__)
        if ( *ADC_PGA & ADC0_PGA_PGAEN ) { // PGA is enabled
            if(ADC_num == 0) { // PGA0 connects to A10-A11
                *ADC_SC1A = ADC_SC1_DIFF + 0x2 + var_enableInterrupts*ADC_SC1_AIEN;
                __enable_irq();
            } else { // If this is ADC1 we can't connect to PGA0
                __enable_irq();
                return ADC_ERROR_DIFF_VALUE;
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
        if ( *ADC_PGA & ADC0_PGA_PGAEN ) { // PGA is enabled
            if(ADC_num == 1) { // PGA1 connects to A12-A13
                *ADC_SC1A = ADC_SC1_DIFF + 0x2 + var_enableInterrupts*ADC_SC1_AIEN;
                __enable_irq();
            } else { // If this is ADC0 we can't connect to PGA1
                __enable_irq();
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
        return ADC_ERROR_DIFF_VALUE; // all others aren't capable of differential measurements, perhaps return analogRead(pinP)-analogRead(pinN)?
    }

    return true;
}



/* Starts continuous conversion on the pin
 * It returns as soon as the ADC is set, use analogReadContinuous() to read the values
 * Set the resolution, number of averages and voltage reference using the appropriate functions BEFORE calling this function
*/
void ADC_Module::startContinuous(uint8_t pin)
{
    if ( (pin < 0) || (pin > 43) ) {
        return;   // all others are invalid
    }

    // check for calibration before setting channels,
	if (calibrating) wait_for_cal();

    // increase the counter of measurements
	num_measurements++;

    // if the resolution is incorrect (i.e. 9, 11 or 13) silently correct it
	uint8_t res = getResolution();
	if( (res==9) || (res==11) || (res==13) ) {
        setResolution(res-1);
	} else if(res==16) {
        analog_max_val = 65536; // make sure a differential function didn't change this
    }

    #if defined(__MK20DX256__)
	// ADC1 has A15=5a and A20=4a so we have to change the mux to read the "a" channels
	if( (ADC_num==1) && ( (pin==26) || (pin==31) ) ) {
        *ADC_CFG2 &= ~ADC_CFG2_MUXSEL;
	} else {
        *ADC_CFG2 |= ADC_CFG2_MUXSEL;
	}
	#endif

    __disable_irq();
    // set continuous conversion flag
	*ADC_SC3 |= ADC_SC3_ADCO;
    // select pin for single-ended mode and start conversion, enable interrupts if requested
    *ADC_SC1A = channel2sc1a[pin] + var_enableInterrupts*ADC_SC1_AIEN;
    __enable_irq();


	return;
}


/* Starts continuous and differential conversion between the pins (pinP-pinN)
 * It returns as soon as the ADC is set, use analogReadContinuous() to read the value
 * Set the resolution, number of averages and voltage reference using the appropriate functions BEFORE calling this function
*/
void ADC_Module::startContinuousDifferential(uint8_t pinP, uint8_t pinN)
{

    // increase the counter of measurements
	num_measurements++;

    // if the resolution is incorrect (i.e. 8, 10 or 12) silently correct
	uint8_t res = getResolution();
	if( (res==8) || (res==10) || (res==12) ) {
        setResolution(res+1);
    } else if(res==16) {
        analog_max_val = 32768; // 16 bit diff mode is actually 15 bits + 1 sign bit
        //*ADC_CV1 /= 2; // change back the comparison values
        //*ADC_CV2 /= 2;
    }


    // check for calibration before setting channels,
	// because conversion will start as soon as we write to *ADC_SC1A
	if (calibrating) wait_for_cal();

    // set continuous conversion flag
	__disable_irq();
	*ADC_SC3 |= ADC_SC3_ADCO;
	__enable_irq();

    // once *ADC_SC1A is set, conversion will start, enable interrupts if requested
	if ( (pinP == A10) && (pinN == A11) ) { // DAD0 selected, pins 34 and 35
        __disable_irq();
        #if defined(__MK20DX256__)
        if ( *ADC_PGA & ADC0_PGA_PGAEN ) { // PGA is enabled
            if(ADC_num == 0) { // PGA0 connects to A10-A11
                *ADC_SC1A = ADC_SC1_DIFF + 0x2 + var_enableInterrupts*ADC_SC1_AIEN;
                __enable_irq();
            } else { // If this is ADC1 we can't connect to PGA0
                __enable_irq();
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
        if ( *ADC_PGA & ADC0_PGA_PGAEN ) { // PGA is enabled
            if(ADC_num == 1) { // PGA1 connects to A12-A13
                *ADC_SC1A = ADC_SC1_DIFF + 0x2 + var_enableInterrupts*ADC_SC1_AIEN;
                __enable_irq();
            } else { // If this is ADC0 we can't connect to PGA1
                __enable_irq();
                return;
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
        return; // all others aren't capable of differential measurements, perhaps return analogRead(pinP)-analogRead(pinN)?
    }

    return;
}

/* Reads the analog value of the continuous conversion set with analogStartContinuous(pin)
 * It returns the last converted value.
*/
int ADC_Module::analogReadContinuous()
{
    // The result is a 16 bit extended sign 2's complement number (the sign bit goes from bit 15 to analog_config_bits-1)
	// if the number is negative we fill the rest of the 1's upto 32 bits (we extend the sign)
	int16_t result = *ADC_RA;
    if (result & (1<<15)) { // number is negative
        result |= 0xFFFF0000; // result is a 32 bit integer
    }

	return result;
}

/* Stops continuous conversion
*/
void ADC_Module::stopContinuous()
{
	*ADC_SC1A |= 0x1F; // set channel select to all 1's (31) to stop it.

	// decrease the counter of measurements
	num_measurements--;

	return;
}


/* Is the ADC converting at the moment?
*  Returns 1 if yes, 0 if not
*/
bool ADC_Module::isConverting() {

    return (*ADC_SC2 & ADC_SC2_ADACT) >> 7;
}


/* Is an ADC conversion ready?
*  Returns 1 if yes, 0 if not.
*  When a value is read this function returns 0 until a new value exists
*  So it only makes sense to call it before analogRead(), analogReadContinuous() or analogReadDifferential()
*/
bool ADC_Module::isComplete() {

    return (*ADC_SC1A & ADC_SC1_COCO) >> 7;
}


bool ADC_Module::isDifferential() {
    return (*ADC_SC1A & ADC_SC1_DIFF) >> 5;
}

bool ADC_Module::isContinuous() {
    return (*ADC_SC3 & ADC_SC3_ADCO) >> 3;
}

// period in seconds
void ADC_Module::startPDB(double period) {
    //                  software trigger    enable PDB     PDB interrupt
    #define PDB_CONFIG (PDB_SC_TRGSEL(15) | PDB_SC_PDBEN | PDB_SC_PDBIE \
        | PDB_SC_CONT |  PDB_SC_LDMOD(0))
    //    continuous mode load inmediately
    // period = (MOD * PRESCALER * MULT)/F_BUS

    #define PDB_CH0C1_TOS 0x0100
    #define PDB_CH0C1_EN 0x01

    if (!(SIM_SCGC6 & SIM_SCGC6_PDB)) { // setup PDB
        SIM_SCGC6 |= SIM_SCGC6_PDB; // enable pdb clock
    }

    uint8_t mult = 0;
    if(period>0.05) {
        period = period/10;
        mult = 1;
    }

    double temp = period*F_BUS/65535;
    uint8_t power_2 = ceil(1.443*log(temp)); // max period 0.17 s
    if(power_2>7) {
        mult=power_2-7+1; // use mult
        power_2=7;
    }
    uint16_t mult_value = 1;
    if(mult==2) {
        mult_value = 10;
    } else if(mult==3) {
        mult_value = 20;
    } else if(mult==4) {
        mult_value = 40;
    }

    //first we set the period to be closest to the desired period,
    // then we use MOD to actually get our period

    PDB0_MOD = (uint16_t)(period/pow(2,power_2)/mult_value*F_BUS); // we adjust the counter to fit the period
    PDB0_IDLY = 0; // the pdb interrupt happens when IDLY is equal to CNT
    PDB0_SC = PDB_CONFIG | PDB_SC_PRESCALER(power_2) | PDB_SC_MULT(mult) | PDB_SC_LDOK; // load all new values

    PDB0_SC |= PDB_SC_SWTRIG; // start the counter!

    NVIC_ENABLE_IRQ(IRQ_PDB);

    // real period
    adc_pdb_period = PDB0_MOD*pow(2,power_2)*mult_value/F_BUS;

}
