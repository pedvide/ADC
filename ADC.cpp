 /* ADC Library created by Pedro Villanueva
 * It implements all functions of the Teensy 3.0 internal ADC
 * Some of the code was extracted from the file analog.c included in the distribution of Teensduino by PJRC.COM
 *
 */


 /* TODO
  * Stuff related to the conversion speed and power. Useful if you want more speed or to save power.
  * Make some accesses to registers interrupt-proof.
 */


#include "ADC.h"

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

}

/* Destructor
*
*/
ADC::~ADC() {
    //dtor
}

/* Sets up all initial configurations and starts calibration
*
*/
void ADC::analog_init(void)
{
	uint32_t num;

	VREF_TRM = 0x60;
	VREF_SC = 0xE1;		// enable 1.2 volt ref

	if ( (analog_config_bits == 8) || (analog_config_bits == 9) )  {
		ADC0_CFG1 = ADC0_CFG1_24MHZ + ADC_CFG1_MODE(0); // 0 clock divide + Input clock: 24 MHz (run at 24 MHz) + Conversion mode: 8 bit + Sample time: Short
		ADC0_CFG2 = ADC_CFG2_MUXSEL + ADC_CFG2_ADLSTS(3); // b channels + Sample time 2 cycles (I think that if ADC_CFG1_ADLSMP isn't set, then ADC_CFG2_ADLSTS doesn't matter)
	} else if ( (analog_config_bits == 10 )|| (analog_config_bits == 11) ) { // total clock cycles to complete conversion: 3 ADCK + 5 BUS + 4 averages*( 20 + 2 ADCK ) = 7.8 us
		ADC0_CFG1 = ADC0_CFG1_12MHZ + ADC_CFG1_MODE(2) + ADC_CFG1_ADLSMP; // Clock divide: 1/2 + Input clock: 24 MHz (run at 12 MHz) + Conversion mode: 10 bit + Sample time: Long
		ADC0_CFG2 = ADC_CFG2_MUXSEL + ADC_CFG2_ADLSTS(3); // b channels + Sample time 2 extra clock cycles
	} else if ( (analog_config_bits == 12 )|| (analog_config_bits == 13) ) {
		ADC0_CFG1 = ADC0_CFG1_12MHZ + ADC_CFG1_MODE(1) + ADC_CFG1_ADLSMP; // Clock divide: 1/2 + Input clock: 24 MHz (run at 12 MHz) + Conversion mode: 12 bit + Sample time: Long
		ADC0_CFG2 = ADC_CFG2_MUXSEL + ADC_CFG2_ADLSTS(2); // b channels + Sample time: 6 extra clock cycles
	} else {
		ADC0_CFG1 = ADC0_CFG1_12MHZ + ADC_CFG1_MODE(3) + ADC_CFG1_ADLSMP;  //Clock divide: 1/2 + Input clock: 24 MHz (run at 12 MHz) + Conversion mode: 16 bit + Sample time: Long
		ADC0_CFG2 = ADC_CFG2_MUXSEL + ADC_CFG2_ADLSTS(2); // b channels + Sample time: 6 extra clock cycles
	}

	if (analog_reference_internal) {
		ADC0_SC2 |= ADC_SC2_REFSEL(1); // 1.2V ref
	} else {
		ADC0_SC2 |= ADC_SC2_REFSEL(0); // vcc/ext ref
	}

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
*
*/
void ADC::setReference(uint8_t type)
{
	if (type) {
		// internal reference requested
		if (!analog_reference_internal) {
			analog_reference_internal = 1;
			if (calibrating) ADC0_SC3 = 0; // cancel cal
			analog_init();
		}
	} else {
		// vcc or external reference requested
		if (analog_reference_internal) {
			analog_reference_internal = 0;
			if (calibrating) ADC0_SC3 = 0; // cancel cal
			analog_init();
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
	if (config != analog_config_bits) {
		analog_config_bits = config;
		analog_max_val = pow(2, analog_config_bits);
		if (calibrating) ADC0_SC3 = 0; // cancel cal
		analog_init(); // re-cal
	}
}

/* Returns the resolution of the ADC
*
*/
int ADC::getResolution() {
    return analog_config_bits;
}

/* Returns the maximum value for a measurement, that is 2^resolution
*
*/
double ADC::getMaxValue() {
    return analog_max_val;
}


/* Set the number of averages: 0, 4, 8, 16 or 32.
*
*/
void ADC::setAveraging(unsigned int num)
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
}

/* Disable interrupts
*
*/
void ADC::disableInterrupts() {
    var_enableInterrupts = 0;
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
	int result;

    // if the resolution is incorrect (i.e. 9, 11 or 13) silently correct it
	int res = getResolution();
	if( (res==9) || (res==11) || (res==13) ) {
        setResolution(res-1);
        ADC0_CV1 /= 2; // correct also compare function jut in case it was enabled
        ADC0_CV2 /= 2;
	}

	if (pin >= 14) {
		if (pin <= 23) {
			pin -= 14;  // 14-23 are A0-A9
		} else if (pin >= 34 && pin <= 39) {
			pin -= 24;  // 34-37 are A10-A13, 38 is temp sensor, 39 is vref
		} else {
			return ADC_ERROR_VALUE;   // all others are invalid
		}
	}
	//serial_print("analogRead");
	//return 0;
	if (calibrating) wait_for_cal();

    // select pin for single-ended mode and start conversion, enable interrupts if requested
	ADC0_SC1A = channel2sc1a[pin] + var_enableInterrupts*ADC_SC1_AIEN;
	while ((ADC0_SC1A & ADC_SC1_COCO) == 0) { // wait until conversion is completed
		yield();
		// if the comparison is false it'd be stuck in the loop forever without this:
		if( ((ADC0_SC2 & ADC_SC2_ADACT) == 0) && ((ADC0_SC1A & ADC_SC1_COCO) == 0) ) {
		    // comparison was false, we return an obviously erroneous value to indicate this
            return ADC_ERROR_VALUE; // a single-ended read can never be negative
		}
		// wait
		//serial_print(".");
	}
	//serial_print("\n");
	result = ADC0_RA;
	//serial_phex16(result >> 3);
	//serial_print("\n");
	return result;
}


/* Reads the differential analog value of two pins (pinP - pinN)
* It waits until the value is read and then returns the result
* If a comparison has been set up and fails, it will return -70000
* Set the resolution, number of averages and voltage reference using the appropriate functions
*/
int ADC::analogReadDifferential(uint8_t pinP, uint8_t pinN)
{
	int result;

	// if the resolution is incorrect (i.e. 8, 10 or 12) silently correct it
	int res = getResolution();
	if( (res==8) || (res==10) || (res==12) ) {
        setResolution(res+1);
        ADC0_CV1 *= 2; // correct also compare function jut in case it was enabled
        ADC0_CV2 *= 2;
	}

	// check for calibration before setting channels,
	// because conversion will start as soon as we write to ADC0_SC1A
	if (calibrating) wait_for_cal();

    // once ADC0_SC1A is set, conversion will start, enable interrupts if requested
	if ( (pinP == A10) && (pinN == A11) ) { // DAD0 selected, pins 34 and 35
	    ADC0_SC1A = ADC_SC1_DIFF + 0x0 + var_enableInterrupts*ADC_SC1_AIEN;
	} else if ( (pinP == A12) && (pinN == A13) ) { // DAD3 selected, pins 36 and 37
	    ADC0_SC1A = ADC_SC1_DIFF + 0x3 + var_enableInterrupts*ADC_SC1_AIEN;
	} else {
        return ADC_ERROR_DIFF_VALUE; // all others aren't capable of differential measurements, perhaps return analogRead(pinP)-analogRead(pinN)?
    }

	while ((ADC0_SC1A & ADC_SC1_COCO) == 0) { // check bit ADC_SC1_COCO in ADC0_SC1A and wait until conversion is completed
		yield();
        // if the comparison is false it'd be stuck in the loop forever without this:
		if( ((ADC0_SC2 & ADC_SC2_ADACT) == 0) && ((ADC0_SC1A & ADC_SC1_COCO) == 0) ) {
		    // comparison was false, we return an obviously erroneous value to indicate this
            return ADC_ERROR_DIFF_VALUE; // a differential reading can be negative, but never less than -65536
		}
	}

	// The result is a 16 bit extended sign 2's complement number (the sign bit goes from bit 15 to analog_config_bits-1)
	// if the number is negative we fill the rest of the 1's up to 32 bits (we extend the sign)
	result = ADC0_RA;
    if (result & (1<<15)) { // number is negative
        result |= 0xFFFF0000; // result is a 32 bit integer
    }

	return result;
}


/* Starts continuous conversion on the pin
 * It returns as soon as the ADC is set, use analogReadContinuous() to read the values
 * Set the resolution, number of averages and voltage reference using the appropriate functions BEFORE calling this function
*/
void ADC::startContinuous(uint8_t pin)
{

    // if the resolution is incorrect (i.e. 9, 11 or 13) silently correct it
	int res = getResolution();
	if( (res==9) || (res==11) || (res==13) ) {
        setResolution(res-1);
        ADC0_CV1 /= 2; // correct also compare function jut in case it was enabled
        ADC0_CV2 /= 2;
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

    // set continuous conversion flag
    // we could call analog_init() adding this flag to the SC3 register, but then we would need to calibrate again
    // I don't think it's necessary to waste time in that, although that means that this statement is not atomic
    // and if an interrupt occurs after reading ADC0_SC3, and modifies it, the modification will be reversed when the interrupt returns
	ADC0_SC3 |= ADC_SC3_ADCO;

    // select pin for single-ended mode and start conversion, enable interrupts if requested
    ADC0_SC1A = channel2sc1a[pin] + var_enableInterrupts*ADC_SC1_AIEN;

}


/* Starts continuous and differential conversion between the pins (pinP-pinN)
 * It returns as soon as the ADC is set, use analogReadContinuous() to read the value
 * Set the resolution, number of averages and voltage reference using the appropriate functions BEFORE calling this function
*/
void ADC::startContinuousDifferential(uint8_t pinP, uint8_t pinN)
{

    // if the resolution is incorrect (i.e. 8, 10 or 12) silently correct
	int res = getResolution();
	if( (res==8) || (res==10) || (res==12) ) {
        setResolution(res+1);
        ADC0_CV1 *= 2; // correct also compare function jut in case it was enabled
        ADC0_CV2 *= 2;
	}

    // set continuous conversion flag
    // we could call analog_init() adding this flag to the SC3 register, but then we would need to calibrate again
    // I don't think it's necessary to waste time in that, although that means that this statement is not atomic
    // and if an interrupt occurs after reading ADC0_SC3, and modifies it, the modification will be reversed when the interrupt returns
	ADC0_SC3 |= ADC_SC3_ADCO;

	// check for calibration before setting channels,
	// because conversion will start as soon as we write to ADC0_SC1A
	if (calibrating) wait_for_cal();

    // select pins for differential mode and start conversion, enable interrupts if requested
	if ( (pinP == A10) && (pinN == A11) ) { // DAD0 selected, pins 34 and 35
	    ADC0_SC1A = ADC_SC1_DIFF + 0x0 + var_enableInterrupts*ADC_SC1_AIEN;
	} else if ( (pinP == A12) && (pinN == A13) ) { // DAD3 selected, pins 36 and 37
	    ADC0_SC1A = ADC_SC1_DIFF + 0x3 + var_enableInterrupts*ADC_SC1_AIEN;
	} else {
        return;
    }

}

/* Reads the analog value of the continuous conversion set with analogStartContinuous(pin)
 * It returns the last converted value.
*/
int ADC::analogReadContinuous()
{
    // The result is a 16 bit extended sign 2's complement number (the sign bit goes from bit 15 to analog_config_bits-1)
	// if the number is negative we fill the rest of the 1's upto 32 bits (we extend the sign)
	int result = ADC0_RA;
    if (result & (1<<15)) { // number is negative
        result |= 0xFFFF0000; // result is a 32 bit integer
    }

	return result;
}

/* Stops continuous conversion
*/
void ADC::stopContinuous()
{
	ADC0_SC1A = 31; // set channel select to all 1's to stop it.

	return;
}



/* Is the ADC converting at the moment?
*  Returns 1 if yes, 0 if not
*/
int ADC::isConverting() {

    return (ADC0_SC2 & ADC_SC2_ADACT) >> 7;
}


/* Is an ADC conversion ready?
*  Returns 1 if yes, 0 if not.
*  When a value is read this function returns 0 until a new value exists
*  So it only makes sense to call it before analogRead(), analogReadContinuous() or analogReadDifferential()
*/
int ADC::isComplete() {

    return (ADC0_SC1A & ADC_SC1_COCO) >> 7;
}

