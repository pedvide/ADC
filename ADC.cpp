 /* ADC Library created by Pedro Villanueva
 * It implements all functions of the Teensy 3.0 internal ADC
 * Some of the code was extracted from the file analog.c included in the distribution of Teensduino by PJRC.COM
 *
 */

#include "ADC.h"


// ADC interrupt function calls a function to add values in the ring buffer
// if there is at least one analog timer, if not, it does nothing.
// implemented as weak so that an user can redefine it
void __attribute__((weak)) adc0_isr(void) {
    ADC::analogTimer_ADC0_Callback();
}
#if defined(__MK20DX256__)
void __attribute__((weak)) adc1_isr(void) {
    ADC::analogTimer_ADC1_Callback();
}
#endif

// struct with the analog timers
ADC::AnalogTimer *ADC::analogTimer[];

// pointer to isr adc
void (*ADC::analogTimer_ADC0_Callback)(void);
#if defined(__MK20DX256__)
void (*ADC::analogTimer_ADC1_Callback)(void);
#endif

// the functions for the analog timers
ADC::ISR ADC::analogTimerCallback[] = {
    ADC::analogTimerCallback0,
    ADC::analogTimerCallback1,
    ADC::analogTimerCallback2
};


// static adc module objects
ADC_Module *ADC::adc0 = new ADC_Module(0);
#if defined(__MK20DX256__)
ADC_Module *ADC::adc1 = new ADC_Module(1);
#endif


ADC::ADC()
{
    //ctor

    // itinialize analog timers
    int i = 0;
    for(i=0; i<MAX_ANALOG_TIMERS; i++) {
        analogTimer[i] = new AnalogTimer;
        //analogTimer[i]->pinNumber = -1;
    }
    //*analogTimer = new AnalogTimer[MAX_ANALOG_TIMERS]; // doesn't work for some reason

    // point ADC callback to function that does nothing
    // when the analogTimers are in use, this will point to the correct ADC_callback
    analogTimer_ADC0_Callback = &voidFunction;
    #if defined(__MK20DX256__)
    analogTimer_ADC1_Callback = &voidFunction;
    #endif

}

ADC::~ADC()
{
    //dtor
    int i = 0;
    for(i=0; i<MAX_ANALOG_TIMERS; i++) {
        delete analogTimer[i];
    }
}



/* Set the voltage reference you prefer, default is vcc
*
*/
void ADC::setReference(uint8_t type, int8_t adc_num) {
    if(adc_num==1){ // user wants ADC 1, do nothing if it's a Teensy 3.0
        #if defined(__MK20DX256__)
        adc1->setReference(type);
        #endif
        return;
    }
    adc0->setReference(type); // adc_num isn't changed or has selected ADC0
    return;
}


//! Change the resolution of the measurement.
/*!
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
        #endif
        return 0;
    }
    return adc0->getMaxValue();
}


//! Set the number of averages
/*!
* \param num can be 0, 4, 8, 16 or 32.
*/
void ADC::setAveraging(uint8_t num, int8_t adc_num) {
    if(adc_num==1){ // user wants ADC 1, do nothing if it's a Teensy 3.0
        #if defined(__MK20DX256__)
        adc1->setAveraging(num);
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
        #endif
        return;
    }
    adc0->disableDMA();
    return;
}


//! Enable the compare function to a single value
/** A conversion will be completed only when the ADC value
*  is >= compValue (greaterThan=1) or < compValue (greaterThan=0)
*  Call it after changing the resolution
*  Use with interrupts or poll conversion completion with isComplete()
*/
void ADC::enableCompare(int16_t compValue, int greaterThan, int8_t adc_num) {
    if(adc_num==1){ // user wants ADC 1, do nothing if it's a Teensy 3.0
        #if defined(__MK20DX256__)
        adc1->enableCompare(compValue, greaterThan);
        #endif
        return;
    }
    adc0->enableCompare(compValue, greaterThan);
    return;
}

//! Enable the compare function to a range
/** A conversion will be completed only when the ADC value is inside (insideRange=1) or outside (=0)
*  the range given by (lowerLimit, upperLimit),including (inclusive=1) the limits or not (inclusive=0).
*  See Table 31-78, p. 617 of the freescale manual.
*  Call it after changing the resolution
*  Use with interrupts or poll conversion completion with isComplete()
*/
void ADC::enableCompareRange(int16_t lowerLimit, int16_t upperLimit, int insideRange, int inclusive, int8_t adc_num) {
    if(adc_num==1){ // user wants ADC 1, do nothing if it's a Teensy 3.0
        #if defined(__MK20DX256__)
        adc1->enableCompareRange(lowerLimit, upperLimit, insideRange, inclusive);
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
        #endif
        return;
    }
    adc0->disableCompare();
    return;
}


//! Enable and set PGA
/** Enables the PGA and sets the gain
*   Use only for signals lower than 1.2 V
*   \param gain From 0 to 6, set PGA to 2^gain
*
*/
void ADC::enablePGA(uint8_t gain, int8_t adc_num) {
    #if defined(__MK20DX256__)
    if(adc_num==1){ // user wants ADC 1, do nothing if it's a Teensy 3.0
        adc1->enablePGA(gain);
        return;
    }
    adc0->enablePGA(gain);
    #endif
    return;
}

//! Returns the PGA level
/** PGA level = 2^gain, from 0 to 64
*/
uint8_t ADC::getPGA(int8_t adc_num) {
    #if defined(__MK20DX256__)
    if(adc_num==1){ // user wants ADC 1, do nothing if it's a Teensy 3.0
        return adc1->getPGA();
    }
    return adc0->getPGA();
    #endif
    return 0;
}

//! Disable PGA
void ADC::disablePGA(int8_t adc_num) {
    #if defined(__MK20DX256__)
    if(adc_num==1){ // user wants ADC 1, do nothing if it's a Teensy 3.0
        adc1->disablePGA();
        return;
    }
    adc0->disablePGA();
    #endif
    return;
}

//! Is the ADC converting at the moment?
bool ADC::isConverting(int8_t adc_num) {
    if(adc_num==1){ // user wants ADC 1, do nothing if it's a Teensy 3.0
        #if defined(__MK20DX256__)
        return adc1->isConverting();
        #endif
        return false;
    }
    return adc0->isConverting();
}

//! Is an ADC conversion ready?
/**
*  \return 1 if yes, 0 if not.
*  When a value is read this function returns 0 until a new value exists
*  So it only makes sense to call it before analogReadContinuous() or readSingle()
*/
bool ADC::isComplete(int8_t adc_num) {
    if(adc_num==1){ // user wants ADC 1, do nothing if it's a Teensy 3.0
        #if defined(__MK20DX256__)
        return adc1->isComplete();
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
    // First check that the pin is correct
    if ( (pin < 0) || (pin > 43) ) {
        return ADC_ERROR_VALUE;   // all others are invalid
    }

    /* Teensy 3.0
    */
    #if defined(__MK20DX128__)
    if( adc_num==1 ) { // If asked to use ADC1, return error
        return ADC_ERROR_VALUE;
    }
    if ( (pin < 23) || (pin>=34) ) { // check that the pin is correct (pin<0 or pin>43 have been ruled out already)
        return adc0->analogRead(pin); // use ADC0
    }

    /* Teensy 3.1
    */
    #elif defined(__MK20DX256__)

    // Check to which ADC the pin corresponds
    if( (pin==16) || (pin==17) || (pin>=34 && pin<=37) )  { // Both ADCs: pins 16, 17, 34, 35, 36, and 37.
        if( adc_num==-1 ) { // use no ADC in particular
            if( (adc0->num_measurements) >= (adc1->num_measurements)) { // use the ADC with less workload
                return adc1->analogRead(pin);
            } else {
                return adc0->analogRead(pin);
            }
        }
        else if( adc_num==0 ) { // use ADC0
            return adc0->analogRead(pin);
        }
        else if( adc_num==1 ){ // user wants ADC 1
            return adc1->analogRead(pin);
        }
    } else if(pin>=26) { // Those pins correspond to ADC1 only
        return adc1->analogRead(pin);
    } else if(pin<23){ // Pin corresponds to ADC0
        return adc0->analogRead(pin);
    }
    #endif

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
        return ADC_ERROR_VALUE;
        #endif
    }
    return ADC_ERROR_VALUE;
}


//! Starts an analog measurement on the pin and enables interrupts.
/** It returns inmediately, get value with readSingle().
*   If the pin is incorrect it returns ADC_ERROR_VALUE
*   This function is interrupt safe. The ADC interrupt will restore the adc to its previous settings and
*   restart the adc if it stopped a measurement. If you modify the adc_isr then this won't happen.
*/
int ADC::startSingleRead(uint8_t pin, int8_t adc_num) {
    // First check that the pin is correct
    if ( (pin < 0) || (pin > 43) ) {
        return ADC_ERROR_VALUE;   // all others are invalid
    }

    /* Teensy 3.0
    */
    #if defined(__MK20DX128__)
    if( adc_num==1 ) { // If asked to use ADC1, return error
        return ADC_ERROR_VALUE;
    }
    if ( (pin < 23) || (pin>=34) ) { // check that the pin is correct (pin<0 or pin>43 have been ruled out already)
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
    } else if(pin<23){ // Pin corresponds to ADC0
        return adc0->startSingleRead(pin);
    }
    #endif

    return ADC_ERROR_VALUE;
}

//! Start a differential conversion between two pins (pinP - pinN) and enables interrupts.
/** It returns inmediately, get value with readSingle().
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
        return ADC_ERROR_VALUE;
        #endif
    }
    return ADC_ERROR_VALUE;
}

//! Reads the analog value of a single conversion.
/** Set the conversion with with startSingleRead(pin) or startSingleDifferential(pinP, pinN).
*   \return the converted value.
*/
int ADC::readSingle(int8_t adc_num) {
    if(adc_num==1){ // user wants ADC 1, do nothing if it's a Teensy 3.0
        #if defined(__MK20DX256__)
        return adc1->readSingle();
        #endif
        return false;
    }
    return adc0->readSingle();
}


//! Starts continuous conversion on the pin.
/** It returns as soon as the ADC is set, use analogReadContinuous() to read the value.
*/
void ADC::startContinuous(uint8_t pin, int8_t adc_num) {
    // First check that the pin is correct
    if ( (pin < 0) || (pin > 43) ) {
        return;   // all others are invalid
    }

    /* Teensy 3.0
    */
    #if defined(__MK20DX128__)
    if( adc_num==1 ) { // If asked to use ADC1, return error
        return;
    }
    if ( (pin < 23) || (pin>=34) ) { // check that the pin is correct (pin<0 or pin>43 have been ruled out already)
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
    } else if(pin<23){ // Pin corresponds to ADC0
        adc0->startContinuous(pin);
        return;
    }
    #endif

    return;
}

//! Starts continuous conversion between the pins (pinP-pinN).
/** It returns as soon as the ADC is set, use analogReadContinuous() to read the value.
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
        return;
        #endif
    }
    return;
}

//! Reads the analog value of a continuous conversion.
/** Set the continuous conversion with with analogStartContinuous(pin) or startContinuousDifferential(pinP, pinN).
*   \return the last converted value.
*/
int ADC::analogReadContinuous(int8_t adc_num) {
    if(adc_num==1){ // user wants ADC 1, do nothing if it's a Teensy 3.0
        #if defined(__MK20DX256__)
        return adc1->analogReadContinuous();
        #endif
        return false;
    }
    return adc0->analogReadContinuous();
}

//! Stops continuous conversion
void ADC::stopContinuous(int8_t adc_num) {
    if(adc_num==1){ // user wants ADC 1, do nothing if it's a Teensy 3.0
        #if defined(__MK20DX256__)
        adc1->stopContinuous();
        return;
        #endif
        return;
    }
    adc0->stopContinuous();
    return;
}




/* void function that does nothing
*/
void ADC::voidFunction(){return;}

/* Callback from the ADC0 interrupt, it adds the new value to the ring buffer
* it takes around 3 us
*/
<<<<<<< HEAD
void ADC::ADC0_callback() {
    #if ADC_debug
        digitalWriteFast(ledPin, HIGH);
    #endif

    // get the pin number
    int pin = adc0->sc1a2channel[ADC0_SC1A & 0x1F];

    // find the index of the pin
    int i = 0;
    while( (i<MAX_ANALOG_TIMERS) && (analogTimer[i]->pinNumber!=pin) ) {i++;}
    if( i==MAX_ANALOG_TIMERS) {
        #if ADC_debug
            digitalWriteFast(ledPin, LOW);
        #endif
        return; // the last measurement doesn't belong to an analog timer buffer.
    }

    // place value in its buffer
    analogTimer[i]->buffer->write(adc0->readSingle());

    // restore ADC config if it was in use before being interrupted by the analog timer
    if (adc0->adcWasInUse) {
        // restore ADC config, and restart conversion
        //if(adc_config.diffRes)
        adc0->setResolution(adc0->adc_config.savedRes); // don't change res if isn't necessary
        ADC0_CFG1 = adc0->adc_config.savedCFG1;
        ADC0_CFG2 = adc0->adc_config.savedCFG2;
        ADC0_SC2 = adc0->adc_config.savedSC2 & 0x7F;
        ADC0_SC3 = adc0->adc_config.savedSC3 & 0xF;
        ADC0_SC1A = adc0->adc_config.savedSC1A & 0x7F;
    }

    #if ADC_debug
        digitalWriteFast(ledPin, LOW);
    #endif
}
void ADC::ADC1_callback() {
=======
void ADC::ADC_callback() {
>>>>>>> b7e0d98bdc048bda7fddb63fc06cad8d782f4ac3
    #if ADC_debug
        digitalWriteFast(ledPin, HIGH);
    #endif

    // get the pin number
    int pin = adc1->sc1a2channel[ADC0_SC1A & 0x1F];

    // find the index of the pin
    int i = 0;
    while( (i<MAX_ANALOG_TIMERS) && (analogTimer[i]->pinNumber!=pin) ) {i++;}
    if( i==MAX_ANALOG_TIMERS) {
        #if ADC_debug
            digitalWriteFast(ledPin, LOW);
        #endif
        return; // the last measurement doesn't belong to an analog timer buffer.
    }

    // place value in its buffer
    analogTimer[i]->buffer->write(adc1->readSingle());

    // restore ADC config if it was in use before being interrupted by the analog timer
    if (adc1->adcWasInUse) {
        // restore ADC config, and restart conversion
        //if(adc_config.diffRes)
        adc1->setResolution(adc1->adc_config.savedRes); // don't change res if isn't necessary
        ADC1_CFG1 = adc1->adc_config.savedCFG1;
        ADC1_CFG2 = adc1->adc_config.savedCFG2;
        ADC1_SC2 = adc1->adc_config.savedSC2 & 0x7F;
        ADC1_SC3 = adc1->adc_config.savedSC3 & 0xF;
        ADC1_SC1A = adc1->adc_config.savedSC1A & 0x7F;
    }

    #if ADC_debug
        digitalWriteFast(ledPin, LOW);
    #endif
}


/* callback function for the analog timers
*  it takes around 2.5 us
*/
void ADC::analogTimerCallback0() {
    #if ADC_debug
        digitalWriteFast(ledPin, HIGH);
    #endif

    uint8_t pin = analogTimer[0]->pinNumber;
    if(analogTimer[0]->isDiff) {
        if(pin == A10) {
            ADC::startSingleDifferential(A10, A11);
        } else if(pin == A12) {
            ADC::startSingleDifferential(A12, A13);
        }
    } else {
        ADC::startSingleRead(pin);
    }

    #if ADC_debug
        digitalWriteFast(ledPin, LOW);
    #endif
}
/* callback function for the analog timers
*  it takes around 2.5 us
*/
void ADC::analogTimerCallback1() {
    #if ADC_debug
        digitalWriteFast(ledPin, HIGH);
    #endif

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

    #if ADC_debug
        digitalWriteFast(ledPin, LOW);
    #endif
}
/* callback function for the analog timers
*  it takes around 2.5 us
*/
void ADC::analogTimerCallback2() {
    #if ADC_debug
        digitalWriteFast(ledPin, HIGH);
    #endif

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

    #if ADC_debug
        digitalWriteFast(ledPin, LOW);
    #endif
}


/* Starts a periodic measurement.
* The values will be added to a ring buffer of a fixed size.
*   Read the oldest value with getTimerValue(pin), check if it's the last value with isLastValue(pin).
*   When the buffer is full, new data will overwrite the oldest values.
*/
int ADC::startAnalogTimer(uint8_t pin, uint32_t period) {

    // check pin
    if (pin < 0 || pin > 43) {
        return ANALOG_TIMER_ERROR;   // invalid pin
    }

    // if the resolution is incorrect (i.e. 9, 11 or 13) silently correct it here
	uint8_t res = adc0->getResolution();
	if( (res==9) || (res==11) || (res==13) ) {
        adc0->setResolution(res-1);
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

    // Decide which adc will do the job
    #if defined(__MK20DX128__)
    // point the adc_isr to the function that takes care of the timers
    analogTimer_ADC0_Callback = &ADC0_callback; // use ADC0
    // enable interrupts
    adc0->enableInterrupts();
    #elif defined(__MK20DX256__)
    if( (adc0->num_measurements) >= (adc1->num_measurements)) { // use the ADC with less workload
        analogTimer_ADC1_Callback = &ADC1_callback; // use ADC1
        adc1->enableInterrupts();
    } else {
        analogTimer_ADC0_Callback = &ADC0_callback; // use ADC0
        adc0->enableInterrupts();
    }
    #endif


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
	uint8_t res = adc0->getResolution();
	if( (res==8) || (res==10) || (res==12) ) {
        adc0->setResolution(res+1);
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

    // Decide which adc will do the job
    #if defined(__MK20DX128__)
    // point the adc_isr to the function that takes care of the timers
    analogTimer_ADC0_Callback = &ADC0_callback; // use ADC0
    // enable interrupts
    adc0->enableInterrupts();
    #elif defined(__MK20DX256__)
    if( (adc0->num_measurements) >= (adc1->num_measurements)) { // use the ADC with less workload
        analogTimer_ADC1_Callback = &ADC1_callback; // use ADC1
        adc1->enableInterrupts();
    } else {
        analogTimer_ADC0_Callback = &ADC0_callback; // use ADC0
        adc0->enableInterrupts();
    }
    #endif


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
        adc0->disableInterrupts();
        // point the adc_isr to the function that does nothing
        analogTimer_ADC0_Callback = &voidFunction;
        #if defined(__MK20DX256__)
        adc0->disableInterrupts();
        // point the adc_isr to the function that does nothing
        analogTimer_ADC0_Callback = &voidFunction;
        #endif
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



/*Returns the analog values of both pins, measured at the same time by the two ADC modules.
* It waits until the value is read and then returns the result as a struct Sync_result,
* use Sync_result.result_adc0 and Sync_result.result_adc1.
* If a comparison has been set up and fails, it will return ADC_ERROR_VALUE in both fields of the struct.
*/
ADC::Sync_result ADC::analogSynchronizedRead(uint8_t pin0, uint8_t pin1) {

    ADC::Sync_result res;

    #if defined(__MK20DX128__)
    res.result_adc0 = ADC_ERROR_VALUE;
    res.result_adc1 = ADC_ERROR_VALUE;
    #elif defined(__MK20DX256__)
    adc0->startSingleReadFast(pin0);
    adc1->startSingleReadFast(pin1);

    while( (!adc0->isComplete()) && (!adc1->isComplete()) ) {
        yield();
        //GPIOC_PTOR = 1<<5;
    }

    res.result_adc0 = adc0->readSingle();
    res.result_adc1 = adc1->readSingle();
    #endif

    //GPIOC_PCOR = 1<<5;

    return res;

}
