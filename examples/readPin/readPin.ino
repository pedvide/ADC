/* Example for readPin
*  It measures continuously the voltage on pin A9,
*  Write a pin number to measure that instead.
*  If the pin is wrong, an error message will appear, change the pin and then write -1 to clear error.
*/

#include <ADC.h>

int readPin = A2; // ADC0

ADC *adc = new ADC();; // adc object

void setup() {

    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(readPin, INPUT);


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

    #endif

    delay(500);
}

int value1 = 0, value2 = 0;
int pin=0;

void loop() {

    if (Serial.available()) {
        pin = Serial.parseInt();
        if(pin>0) {
            Serial.print("Changing to pin ");
            Serial.println(pin);
            readPin = pin;
        }
    }


    Serial.print("Pin: ");
    Serial.print(readPin);
    Serial.print(", ADC0 value: ");
    value1 = adc->adc0->analogRead(readPin);
    Serial.print(value1*3.3/adc->getMaxValue(ADC_0), DEC);
    #if ADC_NUM_ADCS>1
    Serial.print(", ADC1 value: ");
    value2 = adc->adc1->analogRead(readPin);
    value2 = adc->adc1->analogRead(readPin);
    Serial.println(value2*3.3/adc->getMaxValue(ADC_1), DEC);
    #else
    Serial.println();
    #endif


    // Print errors, if any.
    adc->printError();
    adc->resetError();


    //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));

    delay(200);

}
