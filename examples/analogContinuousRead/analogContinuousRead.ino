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
const int readPin3 = A2; // ADC0 or ADC1

ADC *adc = new ADC(); // adc object

void setup() {

    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(readPin, INPUT);
    pinMode(readPin2, INPUT);
    pinMode(readPin3, INPUT);

    pinMode(A10, INPUT); //Diff Channel 0 Positive
    pinMode(A11, INPUT); //Diff Channel 0 Negative

    #if ADC_NUM_ADCS>1
    pinMode(A12, INPUT); //Diff Channel 3 Positive
    pinMode(A13, INPUT); //Diff Channel 3 Negative
    #endif

    Serial.begin(9600);

    ///// ADC0 ////
    // reference can be ADC_REFERENCE::REF_3V3, ADC_REFERENCE::REF_1V2 (not for Teensy LC) or ADC_REFERENCE::REF_EXT.
    //adc->setReference(ADC_REFERENCE::REF_1V2, ADC_0); // change all 3.3 to 1.2 if you change the reference to 1V2

    adc->setAveraging(16); // set number of averages
    adc->setResolution(16); // set bits of resolution

    // it can be any of the ADC_CONVERSION_SPEED enum: VERY_LOW_SPEED, LOW_SPEED, MED_SPEED, HIGH_SPEED_16BITS, HIGH_SPEED or VERY_HIGH_SPEED
    // see the documentation for more information
    // additionally the conversion speed can also be ADACK_2_4, ADACK_4_0, ADACK_5_2 and ADACK_6_2,
    // where the numbers are the frequency of the ADC clock in MHz and are independent on the bus speed.
    adc->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_LOW_SPEED); // change the conversion speed
    // it can be any of the ADC_MED_SPEED enum: VERY_LOW_SPEED, LOW_SPEED, MED_SPEED, HIGH_SPEED or VERY_HIGH_SPEED
    adc->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED); // change the sampling speed

    // always call the compare functions after changing the resolution!
    //adc->enableCompare(1.0/3.3*adc->getMaxValue(ADC_0), 0, ADC_0); // measurement will be ready if value < 1.0V
    //adc->enableCompareRange(1.0*adc->getMaxValue(ADC_0)/3.3, 2.0*adc->getMaxValue(ADC_0)/3.3, 0, 1, ADC_0); // ready if value lies out of [1.0,2.0] V

    // If you enable interrupts, notice that the isr will read the result, so that isComplete() will return false (most of the time)
    //adc->enableInterrupts(ADC_0);

    adc->startContinuous(readPin, ADC_0);
    //adc->startContinuousDifferential(A10, A11, ADC_0);


    ////// ADC1 /////
    #if ADC_NUM_ADCS>1
    adc->setAveraging(16, ADC_1); // set number of averages
    adc->setResolution(16, ADC_1); // set bits of resolution
    adc->setConversionSpeed(ADC_CONVERSION_SPEED::MED_SPEED, ADC_1); // change the conversion speed
    adc->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED, ADC_1); // change the sampling speed

    //adc->setReference(ADC_REFERENCE::REF_1V2, ADC_1);

    // always call the compare functions after changing the resolution!
    //adc->enableCompare(1.0/3.3*adc->getMaxValue(ADC_1), 0, ADC_1); // measurement will be ready if value < 1.0V
    //adc->enableCompareRange(1.0*adc->getMaxValue(ADC_1)/3.3, 2.0*adc->getMaxValue(ADC_1)/3.3, 0, 1, ADC_1); // ready if value lies out of [1.0,2.0] V


    // If you enable interrupts, note that the isr will read the result, so that isComplete() will return false (most of the time)
    //adc->enableInterrupts(ADC_1);

    adc->startContinuous(readPin2, ADC_1);
    //adc->startContinuousDifferential(A12, A13, ADC_1);

    #endif

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
            #if ADC_NUM_ADCS>1
            Serial.print("Converting? ADC1: ");
            Serial.println(adc->isConverting(ADC_1));
            #endif
        } else if(c=='s') { // stop conversion
            adc->stopContinuous(ADC_0);
            Serial.println("Stopped");
        } else if(c=='t') { // conversion successful?
            Serial.print("Conversion successful? ADC0: ");
            Serial.println(adc->isComplete(ADC_0));
            #if ADC_NUM_ADCS>1
            Serial.print("Conversion successful? ADC1: ");
            Serial.println(adc->isComplete(ADC_1));
            #endif
        } else if(c=='r') { // restart conversion
            Serial.println("Restarting conversions ");
            adc->startContinuous(readPin, ADC_0);
            //adc->startContinuousDifferential(A10, A11, ADC_0);
        } else if(c=='v') { // value
            Serial.print("Value ADC0: ");
            value = (uint16_t)adc->analogReadContinuous(ADC_0); // the unsigned is necessary for 16 bits, otherwise values larger than 3.3/2 V are negative!
            Serial.println(value*3.3/adc->getMaxValue(ADC_0), DEC);
            #if ADC_NUM_ADCS>1
            Serial.print("Value ADC1: ");
            value2 = (uint16_t)adc->analogReadContinuous(ADC_1); // the unsigned is necessary for 16 bits, otherwise values larger than 3.3/2 V are negative!
            Serial.println(value2*3.3/adc->getMaxValue(ADC_1), DEC);
            #endif
        } else if(c=='n') { // new single conversion on readPin3
            // this shows how even when both ADCs are busy with continuous measurements
            // you can still call analogRead, it will pause the conversion, get the value and resume the continuous conversion automatically.
            Serial.print("Single read on readPin3: ");
            Serial.println(adc->analogRead(readPin3)*3.3/adc->getMaxValue(ADC_0), DEC);
        }
    }

    // Print errors, if any.
    adc->printError();

    //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));

    delay(100);

}

void adc0_isr(void) {
    adc->analogReadContinuous(ADC_0);
    digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN)); // Toggle the led
}
#if ADC_NUM_ADCS>1
void adc1_isr(void) {
    adc->analogReadContinuous(ADC_1);
    digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));

}
#endif

// RESULTS OF THE TEST Teesny 3.x
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


// RESULTS OF THE TEST Teensy LC
// Measure continuously a voltage divider.
// Measurement pin A9 (23). Clock speed 48 Mhz, bus speed 24 MHz.

//
//  Using ADC_VERY_LOW_SPEED for sampling and conversion speeds
// ADC resolution     Measurement frequency                 Num. averages
//     16  bits            33.3 kHz                               1
//     12  bits            37.5 kHz                               1
//     10  bits            37.5 kHz                               1
//      8  bits            40.5 kHz                               1

//     16  bits             1.04 kHz                             32
//     12  bits             1.2 kHz                              32
//     10  bits             1.2 kHz                              32
//      8  bits             1.3 kHz                              32

//
//  ADC_LOW_SPEED, ADC_MED_SPEED, ADC_HIGH_SPEED_16BITS, ADC_HIGH_SPEED and ADC_VERY_HIGH_SPEED are the same for Teensy 3.x and LC,
//  except for a very small amount that depends on the bus speed and not on the ADC clock (which is the same for those speeds).
//  This difference corresponds to 5 bus clock cycles, which is about 0.1 us.
//
//  For 8 bits resolution, 1 average, ADC_MED_SPEED sampling speed the measurement frequencies for the different ADACK are:
//  ADC_ADACK_2_4       106.8 kHz
//  ADC_ADACK_4_0       162.6 kHz
//  ADC_ADACK_5_2       235.1 kHz
//  ADC_ADACK_6_2       263.3 kHz
//  For Teensy 3.x the results are similar but not identical for two reasons: the bus clock plays a small role in the total time and
//  the frequency of this ADACK clock is actually quite variable, the values are the typical ones, but in the electrical datasheet
//  it says that they can range from +-50% their values approximately, so every Teensy can have different frequencies.
