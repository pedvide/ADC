/* Example for analogReadDifferential
*  You can change the number of averages, bits of resolution and also the comparison value or range.
*/

#include <ADC.h>

ADC *adc = new ADC(); // adc object

void setup() {

    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(A10, INPUT); //Diff Channel 0 Positive
    pinMode(A11, INPUT); //Diff Channel 0 Negative
    pinMode(A12, INPUT); //Diff Channel 3 Positive
    pinMode(A13, INPUT); //Diff Channel 3 Negative

    Serial.begin(9600);

    ///// ADC0 ////
    //adc->setReference(ADC_REF_INTERNAL, ADC_0); change all 3.3 to 1.2 if you change the reference

    adc->setAveraging(1); // set number of averages
    adc->setResolution(13); // set bits of resolution

    // it can be ADC_VERY_LOW_SPEED, ADC_LOW_SPEED, ADC_MED_SPEED, ADC_HIGH_SPEED_16BITS, ADC_HIGH_SPEED or ADC_VERY_HIGH_SPEED
    // see the documentation for more information
    adc->setConversionSpeed(ADC_HIGH_SPEED); // change the conversion speed
    // it can be ADC_VERY_LOW_SPEED, ADC_LOW_SPEED, ADC_MED_SPEED, ADC_HIGH_SPEED or ADC_VERY_HIGH_SPEED
    adc->setSamplingSpeed(ADC_HIGH_SPEED); // change the sampling speed

    // always call the compare functions after changing the resolution!
    // Compare values at 16 bits differential resolution are twice what you write!
    adc->enableCompare(1.0/3.3*adc->getMaxValue(ADC_0), 0, ADC_0); // measurement will be ready if value < 1.0V
    //adc->enableCompareRange(-1.0*adc->getMaxValue(ADC_0)/3.3, 2.0*adc->getMaxValue(ADC_0)/3.3, 0, 1, ADC_0); // ready if value lies out of [-1.0,2.0] V

    ////// ADC1 /////
    adc->setAveraging(32, ADC_1); // set number of averages
    adc->setResolution(16, ADC_1); // set bits of resolution
    adc->setConversionSpeed(ADC_VERY_LOW_SPEED, ADC_1); // change the conversion speed
    adc->setSamplingSpeed(ADC_VERY_LOW_SPEED, ADC_1); // change the sampling speed

    // always call the compare functions after changing the resolution!
    //adc->enableCompare(1.0/3.3*adc->getMaxValue(ADC_1), 0, ADC_1); // measurement will be ready if value < 1.0V
    //adc->enableCompareRange(-1.0*adc->getMaxValue(ADC_1)/3.3, 2.0*adc->getMaxValue(ADC_1)/3.3, 0, 1, ADC_1); // ready if value lies out of [-1.0,2.0] V

    Serial.println("End setup");
    delay(500);
}

int value = ADC_ERROR_VALUE;
int value2 = ADC_ERROR_VALUE;


void loop() {

    value = adc->analogReadDifferential(A10, A11, ADC_0); // read a new value, will return ADC_ERROR_VALUE if the comparison is false.
    value2 = adc->analogReadDifferential(A12,A13, ADC_1);

    if(value!=ADC_ERROR_VALUE ) {

      //Serial.print("Raw value ADC0: ");
      //Serial.print(ADC0_RA, DEC);
      //Serial.print("raw Value: : ");
      //Serial.print(value, DEC);
      Serial.print(" Value ADC0: ");
      Serial.print("(PGA: ");
      Serial.print(adc->getPGA());
      Serial.print(" , MaxVal: ");
      Serial.print(adc->getMaxValue());
      Serial.print(") ");
      // Divide by the maximum possible value and the PGA level
      Serial.println(value*3.3/adc->getPGA()/adc->getMaxValue(), DEC);
    } else {
      Serial.println("ADC0 Comparison failed");
    }
    #if defined(__MK20DX256__)
    if(value2!=ADC_ERROR_VALUE) {

      //Serial.print("Raw value ADC1: ");
      //Serial.print(value2, DEC);
      Serial.print(" Value ADC1: ");
      Serial.println(value2*3.3/adc->getPGA(ADC_1)/adc->getMaxValue(ADC_1), DEC);
    } else {
      Serial.println("ADC1 Comparison failed");
    }
    #endif

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
        if(adc->adc0->fail_flag == ADC_ERROR_COMPARISON) {
            adc->adc0->fail_flag &= ~ADC_ERROR_COMPARISON; // clear that error
            Serial.println("Comparison error in ADC0");
        }
    }
    #if defined(__MK20DX256__)
    if(adc->adc1->fail_flag) {
        Serial.print("ADC1 error flags: 0x");
        Serial.println(adc->adc1->fail_flag, HEX);
        if(adc->adc1->fail_flag == ADC_ERROR_COMPARISON) {
            adc->adc1->fail_flag &= ~ADC_ERROR_COMPARISON; // clear that error
            Serial.println("Comparison error in ADC1");
        }
    }
    #endif

    digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));


    delay(1000);
}
