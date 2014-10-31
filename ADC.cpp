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

/* ADC.cpp: Implements the control of one or more ADC modules of Teensy 3.x
 *
 */

#include "ADC.h"

/*
// static adc module objects
ADC_Module *ADC::adc0 = new ADC_Module(0);
#if defined(__MK20DX256__)
ADC_Module *ADC::adc1 = new ADC_Module(1);
#endif
*/
uint8_t ADC::dma_Ch0 = -1;
uint8_t ADC::dma_Ch1 = -1;


// translate pin number to SC1A nomenclature and viceversa
// we need to create this static const arrays so that we can assign the "normal arrays" to the correct one
// depending on which ADC module we will be.
/* channel2sc1aADCx converts a pin number to their value for the SC1A register, for the ADC0 and ADC1
*  sc1a2channelADCx does the opposite.
*/
const uint8_t ADC::channel2sc1aADC0[]= { // new version, gives directly the sc1a number. 0x1F=31 deactivates the ADC.
    5, 14, 8, 9, 13, 12, 6, 7, 15, 4, 0, 19, 3, 21, // 0-13, we treat them as A0-A13
    5, 14, 8, 9, 13, 12, 6, 7, 15, 4, // 14-23 (A0-A9)
    31, 31, 31, 31, 31, 31, 31, 31, 31, 31, // 24-33
    0, 19, 3, 21, // 34-37 (A10-A13)
    26, 22, 23, 27, 29, 30 // 38-43: temp. sensor, VREF_OUT, A14, bandgap, VREFH, VREFL. A14 isn't connected to anything in Teensy 3.0.
};
const uint8_t ADC::sc1a2channelADC0[]= { // new version, gives directly the pin number
    34, 0, 0, 36, 23, 14, 20, 21, 16, 17, 0, 0, 19, 18, // 0-13
    15, 22, 0, 0, 0, 35, 0, 37, // 14-21
    39, 40, 0, 0, 38, 41, 42, 43, // VREF_OUT, A14, temp. sensor, bandgap, VREFH, VREFL.
    0 // 31 means disabled, but just in case
};

const uint8_t ADC::channel2sc1aADC1[]= { // new version, gives directly the sc1a number. 0x1F=31 deactivates the ADC.
    31, 31, 8, 9, 31, 31, 31, 31, 31, 31, 3, 31, 0, 19, // 0-13, we treat them as A0-A13
    31, 31, 8, 9, 31, 31, 31, 31, 31, 31, // 14-23 (A0-A9)
    31, 31,  // 24,25 are digital only pins
    5, 5, 4, 6, 7, 4, 31, 31, // 26-33 26=5a, 27=5b, 28=4b, 29=6b, 30=7b, 31=4a, 32,33 are digital only
    3, 31, 0, 19, // 34-37 (A10-A13) A11 isn't connected.
    26, 18, 31, 27, 29, 30 // 38-43: temp. sensor, VREF_OUT, A14 (not connected), bandgap, VREFH, VREFL.
};
const uint8_t ADC::sc1a2channelADC1[]= { // new version, gives directly the pin number
    36, 0, 0, 34, 28, 26, 29, 30, 16, 17, 0, 0, 0, 0, // 0-13. 5a=26, 5b=27, 4b=28, 4a=31
    0, 0, 0, 0, 39, 37, 0, 0, // 14-21
    0, 0, 0, 0, 38, 41, 0, 42, // 22-29. VREF_OUT, A14, temp. sensor, bandgap, VREFH, VREFL.
    43
};


ADC::ADC() {
    //ctor

    // make sure the clocks to the ADC are on
    SIM_SCGC6 |= SIM_SCGC6_ADC0;
    #if defined(__MK20DX256__)
    SIM_SCGC3 |= SIM_SCGC3_ADC1;
    #endif

    adc0 = new ADC_Module(0);
    #if defined(__MK20DX256__)
    adc1 = new ADC_Module(1);
    #endif

    //dmaControl = new DMAControl;

}



/* Set the voltage reference you prefer, default is vcc
*
*/
void ADC::setReference(uint8_t type, int8_t adc_num) {
    if(adc_num==1){ // user wants ADC 1, do nothing if it's a Teensy 3.0
        #if defined(__MK20DX256__)
        adc1->setReference(type);
        #else
        adc0->fail_flag |= ADC_ERROR_WRONG_ADC;
        #endif
        return;
    }
    adc0->setReference(type); // adc_num isn't changed or has selected ADC0
    return;
}


// Change the resolution of the measurement.
/*
*  \param bits is the number of bits of resolution.
*  For single-ended measurements: 8, 10, 12 or 16 bits.
*  For differential measurements: 9, 11, 13 or 16 bits.
*  If you want something in between (11 bits single-ended for example) select the inmediate higher
*  and shift the result one to the right.
*  If you select, for example, 9 bits and then do a single-ended reading, the resolution will be adjusted to 8 bits
*  In this case the comparison values will still be correct for analogRead and analogReadDifferential, but not
*  for startSingle* or startContinous*, so whenever you change the resolution, change also the comparison values.
*/
void ADC::setResolution(uint8_t bits, int8_t adc_num) {
    if(adc_num==1){ // user wants ADC 1, do nothing if it's a Teensy 3.0
        #if defined(__MK20DX256__)
        adc1->setResolution(bits);
        #else
        adc0->fail_flag |= ADC_ERROR_WRONG_ADC;
        #endif
        return;
    }
    adc0->setResolution(bits); // adc_num isn't changed or has selected ADC0
    return;
}

//! Returns the resolution of the ADC_Module.
uint8_t ADC::getResolution(int8_t adc_num) {
    if(adc_num==1){ // user wants ADC 1, do nothing if it's a Teensy 3.0
        #if defined(__MK20DX256__)
        return adc1->getResolution();
        #else
        adc0->fail_flag |= ADC_ERROR_WRONG_ADC;
        #endif
        return 0;
    }
    return adc0->getResolution(); // adc_num isn't changed or has selected ADC0

}

//! Returns the maximum value for a measurement.
uint32_t ADC::getMaxValue(int8_t adc_num) {
    if(adc_num==1){ // user wants ADC 1, do nothing if it's a Teensy 3.0
        #if defined(__MK20DX256__)
        return adc1->getMaxValue();
        #else
        adc0->fail_flag |= ADC_ERROR_WRONG_ADC;
        #endif
        return 1;
    }
    return adc0->getMaxValue();
}


// Sets the conversion speed
/*
* \param speed can be ADC_LOW_SPEED, ADC_MED_SPEED or ADC_HIGH_SPEED
*
*  It recalibrates at the end.
*/
void ADC::setConversionSpeed(uint8_t speed, int8_t adc_num) {
    if(adc_num==1){ // user wants ADC 1, do nothing if it's a Teensy 3.0
        #if defined(__MK20DX256__)
        adc1->setConversionSpeed(speed);
        #else
        adc0->fail_flag |= ADC_ERROR_WRONG_ADC;
        #endif
        return;
    }
    adc0->setConversionSpeed(speed); // adc_num isn't changed or has selected ADC0
    return;

}


// Sets the sampling speed
/*
* \param speed can be ADC_LOW_SPEED, ADC_MED_SPEED or ADC_HIGH_SPEED
*
*  It recalibrates at the end.
*/
void ADC::setSamplingSpeed(uint8_t speed, int8_t adc_num) {
    if(adc_num==1){ // user wants ADC 1, do nothing if it's a Teensy 3.0
        #if defined(__MK20DX256__)
        adc1->setSamplingSpeed(speed);
        #else
        adc0->fail_flag |= ADC_ERROR_WRONG_ADC;
        #endif
        return;
    }
    adc0->setSamplingSpeed(speed); // adc_num isn't changed or has selected ADC0
    return;

}


// Set the number of averages
/*
* \param num can be 0, 4, 8, 16 or 32.
*/
void ADC::setAveraging(uint8_t num, int8_t adc_num) {
    if(adc_num==1){ // user wants ADC 1, do nothing if it's a Teensy 3.0
        #if defined(__MK20DX256__)
        adc1->setAveraging(num);
        #else
        adc0->fail_flag |= ADC_ERROR_WRONG_ADC;
        #endif
        return;
    }
    adc0->setAveraging(num); // adc_num isn't changed or has selected ADC0
    return;
}


//! Enable interrupts
/** An IRQ_ADC0 Interrupt will be raised when the conversion is completed
*  (including hardware averages and if the comparison (if any) is true).
*/
void ADC::enableInterrupts(int8_t adc_num) {
    if(adc_num==1){ // user wants ADC 1, do nothing if it's a Teensy 3.0
        #if defined(__MK20DX256__)
        adc1->enableInterrupts();
        #else
        adc0->fail_flag |= ADC_ERROR_WRONG_ADC;
        #endif
        return;
    }
    adc0->enableInterrupts();
    return;
}

//! Disable interrupts
void ADC::disableInterrupts(int8_t adc_num) {
    if(adc_num==1){ // user wants ADC 1, do nothing if it's a Teensy 3.0
        #if defined(__MK20DX256__)
        adc1->disableInterrupts();
        #else
        adc0->fail_flag |= ADC_ERROR_WRONG_ADC;
        #endif
        return;
    }
    adc0->disableInterrupts();
    return;
}


//! Enable DMA request
/** An ADC DMA request will be raised when the conversion is completed
*  (including hardware averages and if the comparison (if any) is true).
*/
void ADC::enableDMA(int8_t adc_num) {
    if(adc_num==1){ // user wants ADC 1, do nothing if it's a Teensy 3.0
        #if defined(__MK20DX256__)
        adc1->enableDMA();
        #else
        adc0->fail_flag |= ADC_ERROR_WRONG_ADC;
        #endif
        return;
    }
    adc0->enableDMA();
    return;
}

//! Disable ADC DMA request
void ADC::disableDMA(int8_t adc_num) {
    if(adc_num==1){ // user wants ADC 1, do nothing if it's a Teensy 3.0
        #if defined(__MK20DX256__)
        adc1->disableDMA();
        #else
        adc0->fail_flag |= ADC_ERROR_WRONG_ADC;
        #endif
        return;
    }
    adc0->disableDMA();
    return;
}


// Enable the compare function to a single value
/* A conversion will be completed only when the ADC value
*  is >= compValue (greaterThan=true) or < compValue (greaterThan=false)
*  Call it after changing the resolution
*  Use with interrupts or poll conversion completion with isComplete()
*/
void ADC::enableCompare(int16_t compValue, bool greaterThan, int8_t adc_num) {
    if(adc_num==1){ // user wants ADC 1, do nothing if it's a Teensy 3.0
        #if defined(__MK20DX256__)
        adc1->enableCompare(compValue, greaterThan);
        #else
        adc0->fail_flag |= ADC_ERROR_WRONG_ADC;
        #endif
        return;
    }
    adc0->enableCompare(compValue, greaterThan);
    return;
}

// Enable the compare function to a range
/* A conversion will be completed only when the ADC value is inside (insideRange=1) or outside (=0)
*  the range given by (lowerLimit, upperLimit),including (inclusive=1) the limits or not (inclusive=0).
*  See Table 31-78, p. 617 of the freescale manual.
*  Call it after changing the resolution
*  Use with interrupts or poll conversion completion with isComplete()
*/
void ADC::enableCompareRange(int16_t lowerLimit, int16_t upperLimit, bool insideRange, bool inclusive, int8_t adc_num) {
    if(adc_num==1){ // user wants ADC 1, do nothing if it's a Teensy 3.0
        #if defined(__MK20DX256__)
        adc1->enableCompareRange(lowerLimit, upperLimit, insideRange, inclusive);
        #else
        adc0->fail_flag |= ADC_ERROR_WRONG_ADC;
        #endif
        return;
    }
    adc0->enableCompareRange(lowerLimit, upperLimit, insideRange, inclusive);
    return;
}

//! Disable the compare function
void ADC::disableCompare(int8_t adc_num) {
    if(adc_num==1){ // user wants ADC 1, do nothing if it's a Teensy 3.0
        #if defined(__MK20DX256__)
        adc1->disableCompare();
        #else
        adc0->fail_flag |= ADC_ERROR_WRONG_ADC;
        #endif
        return;
    }
    adc0->disableCompare();
    return;
}


// Enable and set PGA
/* Enables the PGA and sets the gain
*   Use only for signals lower than 1.2 V
*   \param gain From 0 to 6, set PGA to 2^gain
*
*/
void ADC::enablePGA(uint8_t gain, int8_t adc_num) {
    if(adc_num==1){ // user wants ADC 1, do nothing if it's a Teensy 3.0
        #if defined(__MK20DX256__)
        adc1->enablePGA(gain);
        #else
        adc0->fail_flag |= ADC_ERROR_WRONG_ADC;
        #endif
        return;
    }
    adc0->enablePGA(gain);
    return;
}

//! Returns the PGA level
/** PGA level = 2^gain, from 0 to 64
*/
uint8_t ADC::getPGA(int8_t adc_num) {
    if(adc_num==1){ // user wants ADC 1, do nothing if it's a Teensy 3.0
        #if defined(__MK20DX256__)
        return adc1->getPGA();
        #else
        adc0->fail_flag |= ADC_ERROR_WRONG_ADC;
        return 1;
        #endif
    }
    return adc0->getPGA();
}

//! Disable PGA
void ADC::disablePGA(int8_t adc_num) {
    if(adc_num==1){ // user wants ADC 1, do nothing if it's a Teensy 3.0
        #if defined(__MK20DX256__)
        adc1->disablePGA();
        #else
        adc0->fail_flag |= ADC_ERROR_WRONG_ADC;
        #endif
        return;
    }
    adc0->disablePGA();
    return;
}

//! Is the ADC converting at the moment?
bool ADC::isConverting(int8_t adc_num) {
    if(adc_num==1){ // user wants ADC 1, do nothing if it's a Teensy 3.0
        #if defined(__MK20DX256__)
        return adc1->isConverting();
        #else
        adc0->fail_flag |= ADC_ERROR_WRONG_ADC;
        #endif
        return false;
    }
    return adc0->isConverting();
}

// Is an ADC conversion ready?
/*
*  \return 1 if yes, 0 if not.
*  When a value is read this function returns 0 until a new value exists
*  So it only makes sense to call it before analogReadContinuous() or readSingle()
*/
bool ADC::isComplete(int8_t adc_num) {
    if(adc_num==1){ // user wants ADC 1, do nothing if it's a Teensy 3.0
        #if defined(__MK20DX256__)
        return adc1->isComplete();
        #else
        adc0->fail_flag |= ADC_ERROR_WRONG_ADC;
        #endif
        return false;
    }
    return adc0->isComplete();;
}

//! Is the ADC in differential mode?
bool ADC::isDifferential(int8_t adc_num) {
    if(adc_num==1){ // user wants ADC 1, do nothing if it's a Teensy 3.0
        #if defined(__MK20DX256__)
        return adc1->isDifferential();
        #else
        adc0->fail_flag |= ADC_ERROR_WRONG_ADC;
        #endif
        return false;
    }
    return adc0->isDifferential();
}

//! Is the ADC in continuous mode?
bool ADC::isContinuous(int8_t adc_num) {
    if(adc_num==1){ // user wants ADC 1, do nothing if it's a Teensy 3.0
        #if defined(__MK20DX256__)
        return adc1->isContinuous();
        #else
        adc0->fail_flag |= ADC_ERROR_WRONG_ADC;
        #endif
        return false;
    }
    return adc0->isContinuous();
}


/* Returns the analog value of the pin.
* It waits until the value is read and then returns the result.
* If a comparison has been set up and fails, it will return ADC_ERROR_VALUE.
* This function is interrupt safe, so it will restore the adc to the state it was before being called
* If more than one ADC exists, it will select the module with less workload, you can force a selection using
* adc_num. If you select ADC1 in Teensy 3.0 it will return ADC_ERROR_VALUE.
*/
int ADC::analogRead(uint8_t pin, int8_t adc_num) {
    #if defined(__MK20DX128__)
    /* Teensy 3.0
    */
    if( adc_num==1 ) { // If asked to use ADC1, return error
        adc0->fail_flag |= ADC_ERROR_WRONG_ADC;
        return ADC_ERROR_VALUE;
    }
    if ( (pin <= 23) || (pin>=34) ) { // check that the pin is correct (pin<0 or pin>43 have been ruled out already)
        return adc0->analogRead(pin); // use ADC0
    } else {
        adc0->fail_flag |= ADC_ERROR_WRONG_PIN;
    }
    #elif defined(__MK20DX256__)
    /* Teensy 3.1
    */
    // Check to which ADC the pin corresponds
    if( (pin==16) || (pin==17) || (pin>=34 && pin<=37) )  { // Both ADCs: pins 16, 17, 34, 35, 36, and 37.
        if( adc_num==-1 ) { // use no ADC in particular
            if( (adc0->num_measurements) >= (adc1->num_measurements)) { // use the ADC with less workload
                return adc1->analogRead(pin);
            } else {
                return adc0->analogRead(pin);
            }
        }
        else if( adc_num==0 ) { // user wants ADC0
            return adc0->analogRead(pin);
        }
        else if( adc_num==1 ){ // user wants ADC 1
            return adc1->analogRead(pin);
        }
    } else if(pin>=26) { // Those pins correspond to ADC1 only
        return adc1->analogRead(pin);
    } else if(pin<=23){ // Pin corresponds to ADC0
        return adc0->analogRead(pin);
    }
    #endif
    adc0->fail_flag |= ADC_ERROR_OTHER;
    return ADC_ERROR_VALUE;
}

/* Reads the differential analog value of two pins (pinP - pinN).
* It waits until the value is read and then returns the result.
* If a comparison has been set up and fails, it will return ADC_ERROR_VALUE.
* \param pinP must be A10 or A12.
* \param pinN must be A11 (if pinP=A10) or A13 (if pinP=A12).
* Other pins will return ADC_ERROR_VALUE.
* This function is interrupt safe, so it will restore the adc to the state it was before being called
* If more than one ADC exists, it will select the module with less workload, you can force a selection using
* adc_num. If you select ADC1 in Teensy 3.0 it will return ADC_ERROR_VALUE.
*/
int ADC::analogReadDifferential(uint8_t pinP, uint8_t pinN, int8_t adc_num) {
    if( adc_num==-1 ) { // adc_num isn't changed
        #if defined(__MK20DX128__)
        return adc0->analogReadDifferential(pinP, pinN); // use ADC0
        #elif defined(__MK20DX256__)
        if( (adc0->num_measurements) >= (adc1->num_measurements)) { // use the ADC with less workload
            return adc1->analogReadDifferential(pinP, pinN);
        } else {
            return adc0->analogReadDifferential(pinP, pinN);
        }
        #endif
    }
    else if( adc_num==0 ) { // use ADC0
        return adc0->analogReadDifferential(pinP, pinN);
    }
    else if( adc_num==1 ){ // user wants ADC 1, do nothing if it's a Teensy 3.0
        #if defined(__MK20DX256__)
        return adc1->analogReadDifferential(pinP, pinN);
        #else
        adc0->fail_flag |= ADC_ERROR_WRONG_ADC;
        return ADC_ERROR_VALUE;
        #endif
    }
    adc0->fail_flag |= ADC_ERROR_WRONG_ADC;
    return ADC_ERROR_VALUE;
}


// Starts an analog measurement on the pin and enables interrupts.
/* It returns inmediately, get value with readSingle().
*   If the pin is incorrect it returns ADC_ERROR_VALUE
*   This function is interrupt safe. The ADC interrupt will restore the adc to its previous settings and
*   restart the adc if it stopped a measurement. If you modify the adc_isr then this won't happen.
*/
int ADC::startSingleRead(uint8_t pin, int8_t adc_num) {
    /* Teensy 3.0
    */
    #if defined(__MK20DX128__)
    if( adc_num==1 ) { // If asked to use ADC1, return error
        adc0->fail_flag |= ADC_ERROR_WRONG_ADC;
        return ADC_ERROR_VALUE;
    }
    if ( (pin <= 23) || (pin>=34) ) { // check that the pin is correct (pin<0 or pin>43 have been ruled out already)
        return adc0->startSingleRead(pin); // use ADC0
    }

    /* Teensy 3.1
    */
    #elif defined(__MK20DX256__)

    // Check to which ADC the pin corresponds
    if( (pin==16) || (pin==17) || (pin>=34 && pin<=37) )  { // Both ADCs: pins 16, 17, 34, 35, 36, and 37.
        if( adc_num==-1 ) { // use no ADC in particular
            if( (adc0->num_measurements) >= (adc1->num_measurements)) { // use the ADC with less workload
                return adc1->startSingleRead(pin);
            } else {
                return adc0->startSingleRead(pin);
            }
        }
        else if( adc_num==0 ) { // use ADC0
            return adc0->startSingleRead(pin);
        }
        else if( adc_num==1 ){ // user wants ADC 1
            return adc1->startSingleRead(pin);
        }
    } else if(pin>=26) { // Those pins correspond to ADC1 only
        return adc1->startSingleRead(pin);
    } else if(pin<=23){ // Pin corresponds to ADC0
        return adc0->startSingleRead(pin);
    }
    #endif
    adc0->fail_flag |= ADC_ERROR_OTHER;
    return ADC_ERROR_VALUE;
}

// Start a differential conversion between two pins (pinP - pinN) and enables interrupts.
/* It returns inmediately, get value with readSingle().
*   \param pinP must be A10 or A12.
*   \param pinN must be A11 (if pinP=A10) or A13 (if pinP=A12).
*   Other pins will return ADC_ERROR_DIFF_VALUE.
*   This function is interrupt safe. The ADC interrupt will restore the adc to its previous settings and
*   restart the adc if it stopped a measurement. If you modify the adc_isr then this won't happen.
*/
int ADC::startSingleDifferential(uint8_t pinP, uint8_t pinN, int8_t adc_num) {
    if( adc_num==-1 ) { // adc_num isn't changed
        #if defined(__MK20DX128__)
        return adc0->startSingleDifferential(pinP, pinN); // use ADC0
        #elif defined(__MK20DX256__)
        if( (adc0->num_measurements) >= (adc1->num_measurements)) { // use the ADC with less workload
            return adc1->startSingleDifferential(pinP, pinN);
        } else {
            return adc0->startSingleDifferential(pinP, pinN);
        }
        #endif
    }
    else if( adc_num==0 ) { // use ADC0
        return adc0->startSingleDifferential(pinP, pinN);
    }
    else if( adc_num==1 ){ // user wants ADC 1, do nothing if it's a Teensy 3.0
        #if defined(__MK20DX256__)
        return adc1->startSingleDifferential(pinP, pinN);
        #else
        adc0->fail_flag |= ADC_ERROR_WRONG_ADC;
        return ADC_ERROR_VALUE;
        #endif
    }
    adc0->fail_flag |= ADC_ERROR_OTHER;
    return ADC_ERROR_VALUE;
}

// Reads the analog value of a single conversion.
/* Set the conversion with with startSingleRead(pin) or startSingleDifferential(pinP, pinN).
*   \return the converted value.
*/
int ADC::readSingle(int8_t adc_num) {
    if(adc_num==1){ // user wants ADC 1, do nothing if it's a Teensy 3.0
        #if defined(__MK20DX256__)
        return adc1->readSingle();
        #else
        adc0->fail_flag |= ADC_ERROR_WRONG_ADC;
        #endif
        return false;
    }
    return adc0->readSingle();
}


// Starts continuous conversion on the pin.
/* It returns as soon as the ADC is set, use analogReadContinuous() to read the value.
*/
void ADC::startContinuous(uint8_t pin, int8_t adc_num) {
    /* Teensy 3.0
    */
    #if defined(__MK20DX128__)
    if( adc_num==1 ) { // If asked to use ADC1, return error
        adc0->fail_flag |= ADC_ERROR_WRONG_ADC;
        return;
    }
    if ( (pin <= 23) || (pin>=34) ) { // check that the pin is correct (pin<0 or pin>43 have been ruled out already)
        adc0->startContinuous(pin); // use ADC0
        return;
    }

    /* Teensy 3.1
    */
    #elif defined(__MK20DX256__)

    // Check to which ADC the pin corresponds
    if( (pin==16) || (pin==17) || (pin>=34 && pin<=37) )  { // Both ADCs: pins 16, 17, 34, 35, 36, and 37.
        if( adc_num==-1 ) { // use no ADC in particular
            if( (adc0->num_measurements) >= (adc1->num_measurements)) { // use the ADC with less workload
                adc1->startContinuous(pin);
                return;
            } else {
                adc0->startContinuous(pin);
                return;
            }
        }
        else if( adc_num==0 ) { // use ADC0
            adc0->startContinuous(pin);
            return;
        }
        else if( adc_num==1 ){ // user wants ADC 1
            adc1->startContinuous(pin);
            return;
        }
    } else if(pin>=26) { // Those pins correspond to ADC1 only
        adc1->startContinuous(pin);
        return;
    } else if(pin<=23){ // Pin corresponds to ADC0
        adc0->startContinuous(pin);
        return;
    }
    #endif
    adc0->fail_flag |= ADC_ERROR_OTHER;
    return;
}

// Starts continuous conversion between the pins (pinP-pinN).
/* It returns as soon as the ADC is set, use analogReadContinuous() to read the value.
* \param pinP must be A10 or A12.
* \param pinN must be A11 (if pinP=A10) or A13 (if pinP=A12).
* Other pins will return ADC_ERROR_DIFF_VALUE.
*/
void ADC::startContinuousDifferential(uint8_t pinP, uint8_t pinN, int8_t adc_num) {
    if( adc_num==-1 ) { // adc_num isn't changed
        #if defined(__MK20DX128__)
        adc0->startContinuousDifferential(pinP, pinN); // use ADC0
        return;
        #elif defined(__MK20DX256__)
        if( (adc0->num_measurements) >= (adc1->num_measurements)) { // use the ADC with less workload
            adc1->startContinuousDifferential(pinP, pinN);
            return;
        } else {
            adc0->startContinuousDifferential(pinP, pinN);
            return;
        }
        #endif
    }
    else if( adc_num==0 ) { // use ADC0
        adc0->startContinuousDifferential(pinP, pinN);
        return;
    }
    else if( adc_num==1 ){ // user wants ADC 1, do nothing if it's a Teensy 3.0
        #if defined(__MK20DX256__)
        adc1->startContinuousDifferential(pinP, pinN);
        return;
        #else
        adc0->fail_flag |= ADC_ERROR_WRONG_ADC;
        return;
        #endif
    }
    adc0->fail_flag |= ADC_ERROR_OTHER;
    return;
}

// Reads the analog value of a continuous conversion.
/* Set the continuous conversion with with analogStartContinuous(pin) or startContinuousDifferential(pinP, pinN).
*   \return the last converted value.
*/
// inlined! see .h

//! Stops continuous conversion
void ADC::stopContinuous(int8_t adc_num) {
    if(adc_num==1){ // user wants ADC 1, do nothing if it's a Teensy 3.0
        #if defined(__MK20DX256__)
        adc1->stopContinuous();
        #else
        adc0->fail_flag |= ADC_ERROR_WRONG_ADC;
        #endif
        return;
    }
    adc0->stopContinuous();
    return;
}



//////////////// SYNCHRONIZED BLOCKING METHODS //////////////////

/*Returns the analog values of both pins, measured at the same time by the two ADC modules.
* It waits until the value is read and then returns the result as a struct Sync_result,
* use Sync_result.result_adc0 and Sync_result.result_adc1.
* If a comparison has been set up and fails, it will return ADC_ERROR_VALUE in both fields of the struct.
*/
ADC::Sync_result ADC::analogSynchronizedRead(uint8_t pin0, uint8_t pin1) {

    Sync_result res;

    #if defined(__MK20DX128__)
    res.result_adc0 = ADC_ERROR_VALUE;
    res.result_adc1 = ADC_ERROR_VALUE;
    adc0->fail_flag |= ADC_ERROR_WRONG_ADC;
    return res;
    #else

    if ( (pin0 < 0) || (pin0 > 43) || (channel2sc1aADC0[pin0]==31) ) {
        adc0->fail_flag |= ADC_ERROR_WRONG_PIN;
        res.result_adc0 = ADC_ERROR_VALUE;
        return res;
    }
    if ( (pin1 < 0) || (pin1 > 43) || (channel2sc1aADC1[pin1]==31) ) {
        adc1->fail_flag |= ADC_ERROR_WRONG_PIN;
        res.result_adc1 = ADC_ERROR_VALUE;
        return res;
    }


    // check if we are interrupting a measurement, store setting if so.
    // vars to save the current state of the ADC in case it's in use
    ADC_Module::ADC_Config old_adc0_config = {0};
    uint8_t wasADC0InUse = adc0->isConverting(); // is the ADC running now?
    if(wasADC0InUse) { // this means we're interrupting a conversion
        // save the current conversion config, the adc isr will restore the adc
        __disable_irq();
        //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN) );
        old_adc0_config.savedSC1A = ADC0_SC1A;
        old_adc0_config.savedCFG1 = ADC0_CFG1;
        old_adc0_config.savedCFG2 = ADC0_CFG2;
        old_adc0_config.savedSC2 = ADC0_SC2;
        old_adc0_config.savedSC3 = ADC0_SC3;
        __enable_irq();
    }
    ADC_Module::ADC_Config old_adc1_config = {0};
    uint8_t wasADC1InUse = adc1->isConverting(); // is the ADC running now?
    if(wasADC1InUse) { // this means we're interrupting a conversion
        // save the current conversion config, the adc isr will restore the adc
        __disable_irq();
        //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN) );
        old_adc1_config.savedSC1A = ADC1_SC1A;
        old_adc1_config.savedCFG1 = ADC1_CFG1;
        old_adc1_config.savedCFG2 = ADC1_CFG2;
        old_adc1_config.savedSC2 = ADC1_SC2;
        old_adc1_config.savedSC3 = ADC1_SC3;
        __enable_irq();
    }


    // start both measurements
    adc0->startSingleReadFast(pin0);
    adc1->startSingleReadFast(pin1);

    // wait for both ADCs to finish
    while( (adc0->isConverting()) || (adc1->isConverting()) ) { // wait for both to finish
        yield();
        //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN) );
    }
    __disable_irq(); // make sure nothing interrupts this part
    if ( adc0->isComplete() ) { // conversion succeded
        res.result_adc0 = adc0->readSingle();
    } else { // comparison was false
        adc0->fail_flag |= ADC_ERROR_COMPARISON;
        res.result_adc0 = ADC_ERROR_VALUE;
    }
    if ( adc1->isComplete() ) { // conversion succeded
        res.result_adc1 = adc1->readSingle();
    } else { // comparison was false
        adc1->fail_flag |= ADC_ERROR_COMPARISON;
        res.result_adc1 = ADC_ERROR_VALUE;
    }
    __enable_irq();



    // if we interrupted a conversion, set it again
    if (wasADC0InUse) {
        //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN) );
        ADC0_CFG1 = old_adc0_config.savedCFG1;
        ADC0_CFG2 = old_adc0_config.savedCFG2;
        ADC0_SC2 = old_adc0_config.savedSC2;
        ADC0_SC3 = old_adc0_config.savedSC3;
        ADC0_SC1A = old_adc0_config.savedSC1A;
    }
    if (wasADC1InUse) {
        //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN) );
        ADC1_CFG1 = old_adc1_config.savedCFG1;
        ADC1_CFG2 = old_adc1_config.savedCFG2;
        ADC1_SC2 = old_adc1_config.savedSC2;
        ADC1_SC3 = old_adc1_config.savedSC3;
        ADC1_SC1A = old_adc1_config.savedSC1A;
    }

    //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN) );

    return res;

    #endif

}

/*Returns the diff analog values of both sets of pins, measured at the same time by the two ADC modules.
* It waits until the value is read and then returns the result as a struct Sync_result,
* use Sync_result.result_adc0 and Sync_result.result_adc1.
* If a comparison has been set up and fails, it will return ADC_ERROR_VALUE in both fields of the struct.
*/
ADC::Sync_result ADC::analogSynchronizedReadDifferential(uint8_t pin0P, uint8_t pin0N, uint8_t pin1P, uint8_t pin1N) {

    ADC::Sync_result res;

    #if defined(__MK20DX128__)
    res.result_adc0 = ADC_ERROR_VALUE;
    res.result_adc1 = ADC_ERROR_VALUE;
    adc0->fail_flag |= ADC_ERROR_WRONG_ADC;
    return res;
    #else

    // check if we are interrupting a measurement, store setting if so.
    // vars to save the current state of the ADC in case it's in use
    ADC_Module::ADC_Config old_adc0_config = {0};
    uint8_t wasADC0InUse = adc0->isConverting(); // is the ADC running now?
    if(wasADC0InUse) { // this means we're interrupting a conversion
        // save the current conversion config, the adc isr will restore the adc
        __disable_irq();
        //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN) );
        old_adc0_config.savedSC1A = ADC0_SC1A;
        old_adc0_config.savedCFG1 = ADC0_CFG1;
        old_adc0_config.savedCFG2 = ADC0_CFG2;
        old_adc0_config.savedSC2 = ADC0_SC2;
        old_adc0_config.savedSC3 = ADC0_SC3;
        __enable_irq();
    }
    ADC_Module::ADC_Config old_adc1_config = {0};
    uint8_t wasADC1InUse = adc1->isConverting(); // is the ADC running now?
    if(wasADC1InUse) { // this means we're interrupting a conversion
        // save the current conversion config, the adc isr will restore the adc
        __disable_irq();
        //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN) );
        old_adc1_config.savedSC1A = ADC1_SC1A;
        old_adc1_config.savedCFG1 = ADC1_CFG1;
        old_adc1_config.savedCFG2 = ADC1_CFG2;
        old_adc1_config.savedSC2 = ADC1_SC2;
        old_adc1_config.savedSC3 = ADC1_SC3;
        __enable_irq();
    }

    // start both measurements
    adc0->startSingleDifferentialFast(pin0P, pin0N);
    adc1->startSingleDifferentialFast(pin1P, pin1N);

    // wait for both ADCs to finish
    while( (adc0->isConverting()) || (adc1->isConverting()) ) {
        yield();
        //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN) );
    }
    __disable_irq(); // make sure nothing interrupts this part
    if (adc0->isComplete()) { // conversion succeded
        res.result_adc0 = adc0->readSingle();
    } else { // comparison was false
        adc0->fail_flag |= ADC_ERROR_COMPARISON;
        res.result_adc0 = ADC_ERROR_VALUE;
    }
    if (adc1->isComplete()) { // conversion succeded
        res.result_adc1 = adc1->readSingle();
    } else { // comparison was false
        adc1->fail_flag |= ADC_ERROR_COMPARISON;
        res.result_adc1 = ADC_ERROR_VALUE;
    }
    __enable_irq();


    // if we interrupted a conversion, set it again
    if (wasADC0InUse) {
        //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN) );
        ADC0_CFG1 = old_adc0_config.savedCFG1;
        ADC0_CFG2 = old_adc0_config.savedCFG2;
        ADC0_SC2 = old_adc0_config.savedSC2;
        ADC0_SC3 = old_adc0_config.savedSC3;
        ADC0_SC1A = old_adc0_config.savedSC1A;
    }
    if (wasADC1InUse) {
        //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN) );
        ADC1_CFG1 = old_adc1_config.savedCFG1;
        ADC1_CFG2 = old_adc1_config.savedCFG2;
        ADC1_SC2 = old_adc1_config.savedSC2;
        ADC1_SC3 = old_adc1_config.savedSC3;
        ADC1_SC1A = old_adc1_config.savedSC1A;
    }

    //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN) );

    return res;

    #endif

}

/////////////// SYNCHRONIZED NON-BLOCKING METHODS //////////////

// Starts an analog measurement at the same time on the two ADC modules
/* It returns inmediately, get value with readSynchronizedSingle().
*   If the pin is incorrect it returns ADC_ERROR_VALUE
*   If this function interrupts a measurement, it stores the settings in adc_config
*/
int ADC::startSynchronizedSingleRead(uint8_t pin0, uint8_t pin1) {

    #if defined(__MK20DX128__)
    adc0->fail_flag |= ADC_ERROR_WRONG_ADC;
    return false;
    #else

    // check if we are interrupting a measurement, store setting if so.
    adc0->adcWasInUse = adc0->isConverting(); // is the ADC running now?
    if(adc0->adcWasInUse) { // this means we're interrupting a conversion
        // save the current conversion config, the adc isr will restore the adc
        __disable_irq();
        //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN) );
        adc0->adc_config.savedSC1A = ADC0_SC1A;
        adc0->adc_config.savedCFG1 = ADC0_CFG1;
        adc0->adc_config.savedCFG2 = ADC0_CFG2;
        adc0->adc_config.savedSC2 = ADC0_SC2;
        adc0->adc_config.savedSC3 = ADC0_SC3;
        __enable_irq();
    }
    adc1->adcWasInUse = adc1->isConverting(); // is the ADC running now?
    if(adc1->adcWasInUse) { // this means we're interrupting a conversion
        // save the current conversion config, the adc isr will restore the adc
        __disable_irq();
        //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN) );
        adc1->adc_config.savedSC1A = ADC0_SC1A;
        adc1->adc_config.savedCFG1 = ADC0_CFG1;
        adc1->adc_config.savedCFG2 = ADC0_CFG2;
        adc1->adc_config.savedSC2 = ADC0_SC2;
        adc1->adc_config.savedSC3 = ADC0_SC3;
        __enable_irq();
    }


    // start both measurements
    adc0->startSingleReadFast(pin0);
    adc1->startSingleReadFast(pin1);


    //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN) );
    return true;

    #endif
}

// Start a differential conversion between two pins (pin0P - pin0N) and (pin1P - pin1N)
/* It returns inmediately, get value with readSynchronizedSingle().
*   \param pinP must be A10 or A12.
*   \param pinN must be A11 (if pinP=A10) or A13 (if pinP=A12).
*   Other pins will return ADC_ERROR_DIFF_VALUE.
*   If this function interrupts a measurement, it stores the settings in adc_config
*/
int ADC::startSynchronizedSingleDifferential(uint8_t pin0P, uint8_t pin0N, uint8_t pin1P, uint8_t pin1N) {

    #if defined(__MK20DX128__)
    adc0->fail_flag |= ADC_ERROR_WRONG_ADC;
    return false;
    #else

    // check if we are interrupting a measurement, store setting if so.
    adc0->adcWasInUse = adc0->isConverting(); // is the ADC running now?
    if(adc0->adcWasInUse) { // this means we're interrupting a conversion
        // save the current conversion config, the adc isr will restore the adc
        __disable_irq();
        //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN) );
        adc0->adc_config.savedSC1A = ADC0_SC1A;
        adc0->adc_config.savedCFG1 = ADC0_CFG1;
        adc0->adc_config.savedCFG2 = ADC0_CFG2;
        adc0->adc_config.savedSC2 = ADC0_SC2;
        adc0->adc_config.savedSC3 = ADC0_SC3;
        __enable_irq();
    }
    adc1->adcWasInUse = adc1->isConverting(); // is the ADC running now?
    if(adc1->adcWasInUse) { // this means we're interrupting a conversion
        // save the current conversion config, the adc isr will restore the adc
        __disable_irq();
        //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN) );
        adc1->adc_config.savedSC1A = ADC0_SC1A;
        adc1->adc_config.savedCFG1 = ADC0_CFG1;
        adc1->adc_config.savedCFG2 = ADC0_CFG2;
        adc1->adc_config.savedSC2 = ADC0_SC2;
        adc1->adc_config.savedSC3 = ADC0_SC3;
        __enable_irq();
    }

    // start both measurements
    adc0->startSingleDifferentialFast(pin0P, pin0N);
    adc1->startSingleDifferentialFast(pin1P, pin1N);

    //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN) );

    return true;

    #endif

}

// Reads the analog value of a single conversion.
/*
*   \return the converted value.
*/
ADC::Sync_result ADC::readSynchronizedSingle() {
    ADC::Sync_result res;

    #if defined(__MK20DX128__)
    res.result_adc0 = ADC_ERROR_VALUE;
    res.result_adc1 = ADC_ERROR_VALUE;
    adc0->fail_flag |= ADC_ERROR_WRONG_ADC;
    #elif defined(__MK20DX256__)
    res.result_adc0 = adc0->readSingle();
    res.result_adc1 = adc1->readSingle();
    #endif // defined

    return res;
}


///////////// SYNCHRONIZED CONTINUOUS CONVERSION METHODS ////////////

//! Starts a continuous conversion in both ADCs simultaneously
/** Use readSynchronizedContinuous to get the values
*
*/
void ADC::startSynchronizedContinuous(uint8_t pin0, uint8_t pin1) {

    #if defined(__MK20DX128__)
    adc0->fail_flag |= ADC_ERROR_WRONG_ADC;
    return;
    #elif defined(__MK20DX256__)
    adc0->startContinuous(pin0);
    adc1->startContinuous(pin1);

    // setup the conversions the usual way, but to make sure that they are
    // as synchronized as possible we stop and restart them one after the other.
    uint32_t temp_ADC0_SC1A = ADC0_SC1A; ADC0_SC1A = 0x1F;
    uint32_t temp_ADC1_SC1A = ADC1_SC1A; ADC1_SC1A = 0x1F;

    __disable_irq(); // both measurements should have a maximum delay of an instruction time
    ADC0_SC1A = temp_ADC0_SC1A;
    ADC1_SC1A = temp_ADC1_SC1A;
    __enable_irq();
    #endif
}

//! Starts a continuous differential conversion in both ADCs simultaneously
/** Use readSynchronizedContinuous to get the values
*
*/
void ADC::startSynchronizedContinuousDifferential(uint8_t pin0P, uint8_t pin0N, uint8_t pin1P, uint8_t pin1N) {

    #if defined(__MK20DX128__)
    adc0->fail_flag |= ADC_ERROR_WRONG_ADC;
    return;
    #elif defined(__MK20DX256__)
    adc0->startContinuousDifferential(pin0P, pin0N);
    adc1->startContinuousDifferential(pin1P, pin1N);

    // setup the conversions the usual way, but to make sure that they are
    // as synchronized as possible we stop and restart them one after the other.
    uint32_t temp_ADC0_SC1A = ADC0_SC1A; ADC0_SC1A = 0x1F;
    uint32_t temp_ADC1_SC1A = ADC1_SC1A; ADC1_SC1A = 0x1F;

    __disable_irq();
    ADC0_SC1A = temp_ADC0_SC1A;
    ADC1_SC1A = temp_ADC1_SC1A;
    __enable_irq();
    #endif
}

//! Returns the values of both ADCs.
ADC::Sync_result ADC::readSynchronizedContinuous() {
    ADC::Sync_result res;

    #if defined(__MK20DX128__)
    res.result_adc0 = ADC_ERROR_VALUE;
    res.result_adc1 = ADC_ERROR_VALUE;
    adc0->fail_flag |= ADC_ERROR_WRONG_ADC;
    #elif defined(__MK20DX256__)
    res.result_adc0 = adc0->analogReadContinuous();
    res.result_adc1 = adc1->analogReadContinuous();
    #endif // defined

    return res;
}

//! Stops synchronous continuous conversion
void ADC::stopSynchronizedContinuous() {

    #if defined(__MK20DX128__)
    adc0->fail_flag |= ADC_ERROR_WRONG_ADC;
    return;
    #elif defined(__MK20DX256__)
    adc0->stopContinuous();
    adc1->stopContinuous();
    #endif // defined

}


//Our dma ISR
void dma_isr_0(void)
{
    DMA_CINT = ADC::dma_Ch0;
	Serial.print("isr, dma_chanel: "); Serial.println(ADC::dma_Ch0);
	digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN) );
}
void dma_isr_1(void)
{
    DMA_CINT = ADC::dma_Ch1;
	Serial.print("isr, dma_chanel: "); Serial.println(ADC::dma_Ch1);
	digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN) );
}

void ADC::useDMA(uint8_t ch0, uint8_t ch1) {
/*
    // request channels
    DMAChannel dma0, dma1;

    // Initialize buffers
    //channel

    #if defined(__MK20DX128__)
    dma_Ch0 = dma0.channel;
    if(dma_Ch0!=-1) {
        buffer0 = new RingBufferDMA(dma_Ch0);
    }
    #elif defined(__MK20DX256__)
    dma_Ch0 = dma0.channel;
    dma_Ch1 = dma1.channel;
    if(dma_Ch0!=-1) {
        buffer0 = new RingBufferDMA(dma_Ch0);
        buffer0->start();
    }
    if(dma_Ch1!=-1) {
        buffer1 = new RingBufferDMA(dma_Ch1);
        buffer1->start();
    }
    #endif

*/
}



/*
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
*/
