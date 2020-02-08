/* Example for synchonized measurements using both ADC present in Teensy 3.1
*  You can change the number of averages, bits of resolution and also the comparison value or range.
*/

#ifdef ADC_DUAL_ADCS


#include <ADC.h>
#include <ADC_util.h>

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
    adc->adc0->setAveraging(1); // set number of averages
    adc->adc0->setResolution(8); // set bits of resolution
    adc->adc0->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED); // change the conversion speed
    adc->adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED); // change the sampling speed

    ////// ADC1 /////
    adc->adc1->setAveraging(1); // set number of averages
    adc->adc1->setResolution(8); // set bits of resolution
    adc->adc1->setConversionSpeed(ADC_CONVERSION_SPEED::HIGH_SPEED); // change the conversion speed
    adc->adc1->setSamplingSpeed(ADC_SAMPLING_SPEED::HIGH_SPEED); // change the sampling speed


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
    Serial.print(result.result_adc0*3.3/adc->adc0->getMaxValue(), DEC);
    Serial.print(" ");
    Serial.println(result.result_adc1*3.3/adc->adc1->getMaxValue(), DEC);


    // Print errors, if any.
    if(adc->adc0->fail_flag != ADC_ERROR::CLEAR) {
      Serial.print("ADC0: "); Serial.println(getStringADCError(adc->adc0->fail_flag));
    }
    #ifdef ADC_DUAL_ADCS
    if(adc->adc1->fail_flag != ADC_ERROR::CLEAR) {
      Serial.print("ADC1: "); Serial.println(getStringADCError(adc->adc1->fail_flag));
    }
    #endif

    digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));


  //delay(100);
}

/*
 With a AWG I generated a sine wave of 1 Hz and 2 V amplitude.
 I measure synchronously on pins A0 (ADC0) and A2 (ADC1), sampling at 20 Hz (every 50ms).
 The relative error: mean(value(A0)-value(A2))/mean(value(A0)) is approx 0.02%


*/

#else // make sure the example can run for any boards (automated testing)
void setup() {}
void loop() {}
#endif // ADC_DUAL_ADCS