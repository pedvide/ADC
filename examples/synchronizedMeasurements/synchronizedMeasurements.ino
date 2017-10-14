/* Example for synchonized measurements using both ADC present in Teensy 3.1
*  You can change the number of averages, bits of resolution and also the comparison value or range.
*/


#include <ADC.h>

const int readPin = A9;
const int readPin2 = A3;

ADC *adc = new ADC(); // adc object

elapsedMicros time;

void setup() {

    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(readPin, INPUT);
    pinMode(readPin2, INPUT);

    Serial.begin(9600);

    ///// ADC0 ////
    // reference can be ADC_REFERENCE::REF_3V3, ADC_REFERENCE::REF_1V2 (not for Teensy LC) or ADC_REFERENCE::REF_EXT.
    //adc->setReference(ADC_REFERENCE::REF_1V2, ADC_0); // change all 3.3 to 1.2 if you change the reference to 1V2

    adc->setAveraging(1); // set number of averages
    adc->setResolution(8); // set bits of resolution

    // it can be any of the ADC_CONVERSION_SPEED enum: VERY_LOW_SPEED, LOW_SPEED, MED_SPEED, HIGH_SPEED_16BITS, HIGH_SPEED or VERY_HIGH_SPEED
    // see the documentation for more information
    // additionally the conversion speed can also be ADACK_2_4, ADACK_4_0, ADACK_5_2 and ADACK_6_2,
    // where the numbers are the frequency of the ADC clock in MHz and are independent on the bus speed.
    adc->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED); // change the conversion speed
    // it can be any of the ADC_MED_SPEED enum: VERY_LOW_SPEED, LOW_SPEED, MED_SPEED, HIGH_SPEED or VERY_HIGH_SPEED
    adc->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED); // change the sampling speed

    // always call the compare functions after changing the resolution!
    //adc->enableCompare(1.0/3.3*adc->getMaxValue(ADC_0), 0, ADC_0); // measurement will be ready if value < 1.0V
    //adc->enableCompareRange(1.0*adc->getMaxValue(ADC_0)/3.3, 2.0*adc->getMaxValue(ADC_0)/3.3, 0, 1, ADC_0); // ready if value lies out of [1.0,2.0] V

    // If you enable interrupts, notice that the isr will read the result, so that isComplete() will return false (most of the time)
    //adc->enableInterrupts(ADC_0);


    ////// ADC1 /////
    adc->setAveraging(1, ADC_1); // set number of averages
    adc->setResolution(8, ADC_1); // set bits of resolution
    adc->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED, ADC_1); // change the conversion speed
    adc->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED, ADC_1); // change the sampling speed

    //adc->setReference(ADC_REFERENCE::REF_1V2, ADC_1);

    // always call the compare functions after changing the resolution!
    //adc->enableCompare(1.0/3.3*adc->getMaxValue(ADC_1), 0, ADC_1); // measurement will be ready if value < 1.0V
    //adc->enableCompareRange(1.0*adc->getMaxValue(ADC_1)/3.3, 2.0*adc->getMaxValue(ADC_1)/3.3, 0, 1, ADC_1); // ready if value lies out of [1.0,2.0] V



    adc->startSynchronizedContinuous(readPin, readPin2);
    // You can also try:
    //adc->startSynchronizedContinuousDifferential(A10, A11, A12, A13);
    // Read the values in the loop() with readSynchronizedContinuous()

    delay(100);
    Serial.println("end setup");
}

int value = 0;
int value2 = 0;

ADC::Sync_result result;

void loop() {

    // You can also try:
    //result = adc->analogSynchronizedRead(readPin, readPin2);
    //result = adc->analogSynchronizedReadDifferential(A10, A11, A12, A13);

    result = adc->readSynchronizedContinuous();
    // if using 16 bits and single-ended is necessary to typecast to unsigned,
    // otherwise values larger than 3.3/2 will be interpreted as negative
    result.result_adc0 = (uint16_t)result.result_adc0;
    result.result_adc1 = (uint16_t)result.result_adc1;

    //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));

    //Serial.print("Value ADC0: ");
    Serial.print(time, DEC);
    Serial.print(" ");
    Serial.print(result.result_adc0*3.3/adc->getMaxValue(ADC_0), DEC);
    Serial.print(" ");
    Serial.println(result.result_adc1*3.3/adc->getMaxValue(ADC_1), DEC);


    // Print errors, if any.
    adc->printError();

    digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));


  //delay(100);
}

/*
 With a AWG I generated a sine wave of 1 Hz and 2 V amplitude.
 I measure synchronously on pins A0 (ADC0) and A2 (ADC1), sampling at 20 Hz (every 50ms).
 The relative error: mean(value(A0)-value(A2))/mean(value(A0)) is approx 0.02%


*/

