/* Example for analogRead
*  You can change the number of averages, bits of resolution and also the comparison value or range.
*/


#include <ADC.h>
#include <ADC_util.h>

const int readPin = A9; // ADC0
const int readPin2 = A2; // ADC1

ADC *adc = new ADC(); // adc object;

void setup() {

    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(readPin, INPUT);
    pinMode(readPin2, INPUT);

    pinMode(A10, INPUT); //Diff Channel 0 Positive
    pinMode(A11, INPUT); //Diff Channel 0 Negative
    #ifdef ADC_DUAL_ADCS
    pinMode(A12, INPUT); //Diff Channel 3 Positive
    pinMode(A13, INPUT); //Diff Channel 3 Negative
    #endif

    Serial.begin(9600);

    Serial.println("Begin setup");

    ///// ADC0 ////
    // reference can be ADC_REFERENCE::REF_3V3, ADC_REFERENCE::REF_1V2 (not for Teensy LC) or ADC_REFERENCE::REF_EXT.
    //adc->adc0->setReference(ADC_REFERENCE::REF_1V2); // change all 3.3 to 1.2 if you change the reference to 1V2

    adc->adc0->setAveraging(16); // set number of averages
    adc->adc0->setResolution(16); // set bits of resolution

    // it can be any of the ADC_CONVERSION_SPEED enum: VERY_LOW_SPEED, LOW_SPEED, MED_SPEED, HIGH_SPEED_16BITS, HIGH_SPEED or VERY_HIGH_SPEED
    // see the documentation for more information
    // additionally the conversion speed can also be ADACK_2_4, ADACK_4_0, ADACK_5_2 and ADACK_6_2,
    // where the numbers are the frequency of the ADC clock in MHz and are independent on the bus speed.
    adc->adc0->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_LOW_SPEED); // change the conversion speed
    // it can be any of the ADC_MED_SPEED enum: VERY_LOW_SPEED, LOW_SPEED, MED_SPEED, HIGH_SPEED or VERY_HIGH_SPEED
    adc->adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED); // change the sampling speed

    // always call the compare functions after changing the resolution!
    //adc->adc0->enableCompare(1.0/3.3*adc->adc0->getMaxValue(), 0); // measurement will be ready if value < 1.0V
    //adc->adc0->enableCompareRange(1.0*adc->adc0->getMaxValue()/3.3, 2.0*adc->adc0->getMaxValue()/3.3, 0, 1); // ready if value lies out of [1.0,2.0] V

    // If you enable interrupts, notice that the isr will read the result, so that isComplete() will return false (most of the time)
    //adc->adc0->enableInterrupts(adc0_isr);


    ////// ADC1 /////
    #ifdef ADC_DUAL_ADCS
    adc->adc1->setAveraging(16); // set number of averages
    adc->adc1->setResolution(16); // set bits of resolution
    adc->adc1->setConversionSpeed(ADC_CONVERSION_SPEED::MED_SPEED); // change the conversion speed
    adc->adc1->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED); // change the sampling speed

    //adc->adc1->setReference(ADC_REFERENCE::REF_1V2);

    // always call the compare functions after changing the resolution!
    //adc->adc1->enableCompare(1.0/3.3*adc->adc1->getMaxValue(), 0); // measurement will be ready if value < 1.0V
    //adc->adc1->enableCompareRange(1.0*adc->adc1->getMaxValue()/3.3, 2.0*adc->adc1->getMaxValue()/3.3, 0, 1); // ready if value lies out of [1.0,2.0] V


    // If you enable interrupts, note that the isr will read the result, so that isComplete() will return false (most of the time)
    //adc->adc1->enableInterrupts(adc1_isr);

    #endif

    Serial.println("End setup");

}

int value;
int value2;

void loop() {

    // Single reads

    value = adc->adc0->analogRead(readPin); // read a new value, will return ADC_ERROR_VALUE if the comparison is false.

    Serial.print("Pin: ");
    Serial.print(readPin);
    Serial.print(", value ADC0: ");
    Serial.println(value*3.3/adc->adc0->getMaxValue(), DEC);

    #ifdef ADC_DUAL_ADCS
    value2 = adc->adc1->analogRead(readPin2);

    Serial.print("Pin: ");
    Serial.print(readPin2);
    Serial.print(", value ADC1: ");
    Serial.println(value2*3.3/adc->adc1->getMaxValue(), DEC);
    #endif

    // Differential reads
    #if ADC_DIFF_PAIRS > 0
    #ifdef ADC_USE_PGA
    double V_per_bit = 3.3/adc->adc0->getPGA()/adc->adc0->getMaxValue();
    #else
    double V_per_bit = 3.3/adc->adc0->getMaxValue();
    #endif

    value = adc->adc0->analogReadDifferential(A10, A11); // read a new value, will return ADC_ERROR_VALUE if the comparison is false.

    Serial.print(" Value A10-A11: ");
    // Divide by the maximum possible value and the PGA level
    
    Serial.println(value*V_per_bit, DEC);

    #ifdef ADC_DUAL_ADCS
    value2 = adc->adc1->analogReadDifferential(A12, A13);

    Serial.print(" Value A12-A13: ");
    Serial.println(value2*V_per_bit, DEC);
    #endif
    #endif

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

    delay(50);
}

// If you enable interrupts make sure to call readSingle() to clear the interrupt.
void adc0_isr() {
        adc->adc0->readSingle();
}
