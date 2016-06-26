/* Example for analogContinuousRead
*  It measures continuously the voltage on pin A9,
*  Write v and press enter on the serial console to get the value
*  Write c and press enter on the serial console to check that the conversion is taking place,
*  Write t to check if the voltage agrees with the comparison in the setup()
*  Write s to stop the conversion, you can restart it writing r.
*/

#include <ADC.h>

ADC *adc = new ADC();; // adc object

#if defined(__MKL26Z64__) // teensy LC
#define PINS 13
#define PINS_DIFF 2
uint8_t adc_pins[] = {A0,A1,A2,A3,A4,A5,A6,A7,A8, A9,A10,A11, A12};
uint8_t adc_pins_diff[] = {A10, A11};

#elif defined(__MK20DX128__)  // teensy 3.0
#define PINS 14
#define PINS_DIFF 4
uint8_t adc_pins[] = {A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13};
uint8_t adc_pins_diff[] = {A10, A11, A12, A13};

#elif defined(__MK20DX256__)  // teensy 3.1/3.2
#define PINS 21
#define PINS_DIFF 4
uint8_t adc_pins[] = {A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,
                      A14, A15,A16,A17,A18,A19,A20 };
uint8_t adc_pins_diff[] = {A10, A11, A12, A13};

#elif defined(__MK66FX1M0__)  // teensy 3.4/3.5
#define PINS 23
#define PINS_DIFF 2
uint8_t adc_pins[] = {A0,A1,A2,A3,A4,A5,A6,A7,A8, A9, A10,
                      A11,A12,A13,A14,A15,A16,A17,A18,A19,A20,A21,A22 };
uint8_t adc_pins_diff[] = {A10, A11};
#endif // defined

void setup() {

    pinMode(LED_BUILTIN, OUTPUT);

    for (int i=0;i<PINS;i++) {
        pinMode(adc_pins[i], INPUT);
    }

    Serial.begin(9600);


    ///// ADC0 ////
    // reference can be ADC_REF_3V3, ADC_REF_1V2 (not for Teensy LC) or ADC_REF_EXT.
    //adc->setReference(ADC_REF_1V2, ADC_0); // change all 3.3 to 1.2 if you change the reference to 1V2

    adc->setAveraging(1); // set number of averages
    adc->setResolution(8); // set bits of resolution

    // it can be ADC_VERY_LOW_SPEED, ADC_LOW_SPEED, ADC_MED_SPEED, ADC_HIGH_SPEED_16BITS, ADC_HIGH_SPEED or ADC_VERY_HIGH_SPEED
    // see the documentation for more information
    // additionally the conversion speed can also be ADC_ADACK_2_4, ADC_ADACK_4_0, ADC_ADACK_5_2 and ADC_ADACK_6_2,
    // where the numbers are the frequency of the ADC clock in MHz and are independent on the bus speed.
    adc->setConversionSpeed(ADC_HIGH_SPEED); // change the conversion speed
    // it can be ADC_VERY_LOW_SPEED, ADC_LOW_SPEED, ADC_MED_SPEED, ADC_HIGH_SPEED or ADC_VERY_HIGH_SPEED
    adc->setSamplingSpeed(ADC_HIGH_SPEED); // change the sampling speed

    // always call the compare functions after changing the resolution!
    //adc->enableCompare(1.0/3.3*adc->getMaxValue(ADC_0), 0, ADC_0); // measurement will be ready if value < 1.0V
    //adc->enableCompareRange(1.0*adc->getMaxValue(ADC_0)/3.3, 2.0*adc->getMaxValue(ADC_0)/3.3, 0, 1, ADC_0); // ready if value lies out of [1.0,2.0] V

    // If you enable interrupts, notice that the isr will read the result, so that isComplete() will return false (most of the time)
    //adc->enableInterrupts(ADC_0);


    ////// ADC1 /////
    #if ADC_NUM_ADCS>1
    adc->setAveraging(1, ADC_1); // set number of averages
    adc->setResolution(8, ADC_1); // set bits of resolution
    adc->setConversionSpeed(ADC_HIGH_SPEED, ADC_1); // change the conversion speed
    adc->setSamplingSpeed(ADC_HIGH_SPEED, ADC_1); // change the sampling speed

    // always call the compare functions after changing the resolution!
    //adc->enableCompare(1.0/3.3*adc->getMaxValue(ADC_1), 0, ADC_1); // measurement will be ready if value < 1.0V
    //adc->enableCompareRange(1.0*adc->getMaxValue(ADC_1)/3.3, 2.0*adc->getMaxValue(ADC_1)/3.3, 0, 1, ADC_1); // ready if value lies out of [1.0,2.0] V


    // If you enable interrupts, notice that the isr will read the result, so that isComplete() will return false (most of the time)
    //adc->enableInterrupts(ADC_1);

    #endif

    delay(500);
}

int value = 0;
int pin=0;

void loop() {



    for (int i=0;i<PINS;i++) {
        value = adc->analogRead(adc_pins[i]); // read a new value, will return ADC_ERROR_VALUE if the comparison is false.
        Serial.print("A");
        Serial.print(i);
        Serial.print(": ");
        Serial.print(value*3.3/adc->getMaxValue(ADC_0), 2);
        Serial.print(". ");
    }
    Serial.println();
    Serial.print("Differential pairs: ");
    for (int i=0;i<PINS_DIFF;i+=2) {
        value = adc->analogReadDifferential(adc_pins_diff[i], adc_pins_diff[i+1]); // read a new value, will return ADC_ERROR_VALUE if the comparison is false.
        Serial.print(i);
        Serial.print(": ");
        Serial.print(value*3.3/adc->getMaxValue(ADC_0), 2);
        Serial.print(". ");
    }
    Serial.println();Serial.println();



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
    #if ADC_NUM_ADCS>1
    if(adc->adc1->fail_flag) {
        Serial.print("ADC1 error flags: 0x");
        Serial.println(adc->adc1->fail_flag, HEX);
    }
    #endif



    //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));

    delay(50);

}
