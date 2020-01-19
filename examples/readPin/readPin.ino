/* Example for readPin
*  It measures continuously the voltage on pin A9,
*  Write a pin number to measure that instead.
*  If the pin is wrong, an error message will appear, change the pin and then write -1 to clear error.
*/

#include <ADC.h>
#include <ADC_util.h>

int readPin = A2; // ADC0

ADC *adc = new ADC();; // adc object

void setup() {

    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(readPin, INPUT);


    Serial.begin(9600);

    ///// ADC0 ////
    adc->adc0->setAveraging(16); // set number of averages
    adc->adc0->setResolution(16); // set bits of resolution
    adc->adc0->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_LOW_SPEED); // change the conversion speed
    adc->adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED); // change the sampling speed

    ////// ADC1 /////
    #ifdef ADC_DUAL_ADCS
    adc->adc1->setAveraging(16); // set number of averages
    adc->adc1->setResolution(16); // set bits of resolution
    adc->adc1->setConversionSpeed(ADC_CONVERSION_SPEED::MED_SPEED); // change the conversion speed
    adc->adc1->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED); // change the sampling speed
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
    Serial.print(value1*3.3/adc->adc0->getMaxValue(), DEC);
    #ifdef ADC_DUAL_ADCS
    Serial.print(", ADC1 value: ");
    value2 = adc->adc1->analogRead(readPin);
    Serial.println(value2*3.3/adc->adc0->getMaxValue(), DEC);
    #else
    Serial.println();
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
    adc->resetError();


    //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));

    delay(200);

}
