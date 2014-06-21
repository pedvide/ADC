/* Example for analogContinuousRead
*  It measures continuously the voltage on pin A9,
*  Write v and press enter on the serial console to get the value
*  Write c and press enter on the serial console to check that the conversion is taking place,
*  Write t to check if the voltage agrees with the comparison in the setup()
*  Write s to stop the conversion, you can restart it writing r.
*/

#include <ADC.h>

const int readPin = A9; // ADC0
const int readPin2 = A3; // ADC1

ADC *adc = new ADC(); // adc object

void setup() {

    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(readPin, INPUT); //pin 23 single ended
    pinMode(readPin2, INPUT); //pin 23 single ended

    Serial.begin(9600);

    ///// ADC0 ////
    //adc->setReference(ADC_REF_INTERNAL, ADC_0); change all 3.3 to 1.2 if you change the reference

    adc->setAveraging(1); // set number of averages
    adc->setResolution(16); // set bits of resolution

    // it can be ADC_VERY_LOW_SPEED, ADC_LOW_SPEED, ADC_MED_SPEED, ADC_HIGH_SPEED_16BITS, ADC_HIGH_SPEED or ADC_VERY_HIGH_SPEED
    // see the documentation for more information
    adc->setConversionSpeed(ADC_HIGH_SPEED); // change the conversion speed
    // it can be ADC_VERY_LOW_SPEED, ADC_LOW_SPEED, ADC_MED_SPEED, ADC_HIGH_SPEED or ADC_VERY_HIGH_SPEED
    adc->setSamplingSpeed(ADC_HIGH_SPEED); // change the sampling speed

    // always call the compare functions after changing the resolution!
    adc->enableCompare(1.0/3.3*adc->getMaxValue(ADC_0), 0, ADC_0); // measurement will be ready if value < 1.0V
    //adc->enableCompareRange(1.0*adc->getMaxValue(ADC_0)/3.3, 2.0*adc->getMaxValue(ADC_0)/3.3, 0, 1, ADC_0); // ready if value lies out of [1.0,2.0] V

    ////// ADC1 /////
    adc->setAveraging(32, ADC_1); // set number of averages
    adc->setResolution(12, ADC_1); // set bits of resolution
    adc->setConversionSpeed(ADC_VERY_LOW_SPEED, ADC_1); // change the conversion speed
    adc->setSamplingSpeed(ADC_VERY_LOW_SPEED, ADC_1); // change the sampling speed

    // always call the compare functions after changing the resolution!
    //adc->enableCompare(1.0/3.3*adc->getMaxValue(ADC_1), 0, ADC_1); // measurement will be ready if value < 1.0V
    adc->enableCompareRange(1.0*adc->getMaxValue(ADC_1)/3.3, 2.0*adc->getMaxValue(ADC_1)/3.3, 0, 1, ADC_1); // ready if value lies out of [1.0,2.0] V


    // If you enable interrupts, notice that the isr will read the result, so that isComplete() will return false (most of the time)
    //adc->enableInterrupts(ADC_0); // enable interrupts BEFORE calling a measurement method
    //adc->enableInterrupts(ADC_1);


    adc->startContinuous(readPin, ADC_0);

    adc->startContinuous(readPin2, ADC_1);

    delay(500);
}

int value = 0;
int value2 = 0;
char c=0;

void loop() {


    if (Serial.available()) {
        c = Serial.read();
        if(c=='c') { // conversion active?
            Serial.print("Converting? ADC0: ");
            Serial.println(adc->isConverting(ADC_0));
            Serial.print("Converting? ADC1: ");
            Serial.println(adc->isConverting(ADC_1));
        } else if(c=='s') { // stop conversion
            adc->stopContinuous(ADC_0);
            Serial.println("Stopped");
        } else if(c=='t') { // conversion successful?
            Serial.print("Conversion successful? ADC0: ");
            Serial.println(adc->isComplete(ADC_0));
            Serial.print("Conversion successful? ADC1: ");
            Serial.println(adc->isComplete(ADC_1));
        } else if(c=='r') { // restart conversion
            Serial.println("Restarting conversions ");
            adc->startContinuous(readPin, ADC_0);
        } else if(c=='v') { // value
            Serial.print("Value ADC0: ");
            value = (uint16_t)adc->analogReadContinuous(ADC_0); // the unsigned is necessary for 16 bits, otherwise values larger than 3.3/2 V are negative!
            Serial.println(value*3.3/adc->getMaxValue(ADC_0), DEC);
            Serial.print("Value ADC1: ");
            value2 = (uint16_t)adc->analogReadContinuous(ADC_1); // the unsigned is necessary for 16 bits, otherwise values larger than 3.3/2 V are negative!
            Serial.println(value2*3.3/adc->getMaxValue(ADC_1), DEC);
        }
    }

    /* fail_flag contains all possible errors,
        They are defined in  ADC_Module.h as

        ADC_ERROR_OTHER
        ADC_ERROR_CALIB
        ADC_ERROR_WRONG_PIN
        ADC_ERROR_ANALOG_READ
        ADC_ERROR_COMPARISON
        ADC_ERROR_ANALOG_DIFF_READ
        ADC_ERROR_CONT
        ADC_ERROR_CONT_DIFF
        ADC_ERROR_WRONG_ADC
        ADC_ERROR_SYNCH

        You can compare the value of the flag with those masks to know what's the error.
    */
    if(adc->adc0->fail_flag) {
        Serial.print("ADC0 error flags: 0x");
        Serial.println(adc->adc0->fail_flag, HEX);
    }
    #if defined(__MK20DX256__)
    if(adc->adc1->fail_flag) {
        Serial.print("ADC1 error flags: 0x");
        Serial.println(adc->adc1->fail_flag, HEX);
    }
    #endif

    //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));

    delay(100);

}

void adc0_isr(void) {
    adc->analogReadContinuous(ADC_0);
    digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN)); // Toggle the led
}
#if defined(__MK20DX256__)
void adc1_isr(void) {
    // Low-level code
    adc->analogReadContinuous(ADC_1);
    digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));

}
#endif

// RESULTS OF THE TEST
// Measure continuously a voltage divider.
// Measurement pin A9 (23). Clock speed 96 Mhz, bus speed 48 MHz.

//
//  Using ADC_LOW_SPEED (same as ADC_VERY_LOW_SPEED) for sampling and conversion speeds
// ADC resolution     Measurement frequency                 Num. averages
//     16  bits            81 kHz                               1
//     12  bits            94 kHz                               1
//     10  bits            94 kHz                               1
//      8  bits           103 kHz                               1

//     16  bits             2.5 kHz                              32
//     12  bits             2.9 kHz                              32
//     10  bits             2.9 kHz                              32
//      8  bits             3.2 kHz                              32

//
//  Using ADC_MED_SPEED for sampling and conversion speeds
// ADC resolution     Measurement frequency                 Num. averages
//     16  bits           193 kHz                               1
//     12  bits           231 kHz                               1
//     10  bits           231 kHz                               1
//      8  bits           261 kHz                               1

//     10  bits            58 kHz                               4 (default settings) corresponds to about 17.24 us

//
//  Using ADC_HIGH_SPEED (same as ADC_HIGH_SPEED_16BITS) for sampling and conversion speeds
// ADC resolution     Measurement frequency                 Num. averages
//     16  bits           414 kHz                               1
//     12  bits           500 kHz                               1
//     10  bits           500 kHz                               1
//      8  bits           571 kHz                               1
//
//      8  bits           308 kHz                               1 ADC_VERY_LOW_SPEED sampling
//      8  bits           387 kHz                               1 ADC_LOW_SPEED sampling
//      8  bits           480 kHz                               1 ADC_MED_SPEED sampling
//      8  bits           632 kHz                               1 ADC_VERY_HIGH_SPEED sampling

//
//  Using ADC_VERY_HIGH_SPEED for sampling and conversion speeds
//  This conversion speed is over the limit of the specs! (speed=24MHz, limit = 18 MHz for res<16 and 12 for res=16)
// ADC resolution     Measurement frequency                 Num. averages
//     16  bits           888 kHz                               1
//     12  bits          1090 kHz                               1
//     10  bits          1090 kHz                               1
//      8  bits          1262 kHz                               1


// At 96 Mhz (bus at 48 MHz), 632 KHz is the fastest we can do within the specs, and only if the sample's impedance is low enough.
