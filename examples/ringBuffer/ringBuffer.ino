#include "ADC.h"

// Teensy 3.0 has the LED on pin 13
const int ledPin = 13;
const int readPin = A9;

ADC *adc = new ADC(); // adc object

RingBuffer *buffer = new RingBuffer;


void setup() {

    pinMode(ledPin, OUTPUT);
    pinMode(readPin, INPUT); //pin 23 single ended

    Serial.begin(9600);

    //adc->setReference(ADC_REF_INTERNAL, ADC_0); change all 3.3 to 1.2 if you change the reference

    adc->setAveraging(8); // set number of averages
    adc->setResolution(12); // set bits of resolution


    adc->setAveraging(32, ADC_1); // set number of averages
    adc->setResolution(12, ADC_1); // set bits of resolution

    // always call the compare functions after changing the resolution!
    //adc->enableCompare(1.0/3.3*adc->getMaxValue(ADC_0), 0, ADC_0); // measurement will be ready if value < 1.0V
    //adc->enableCompareRange(1.0*adc->getMaxValue(ADC_1)/3.3, 2.0*adc->getMaxValue(ADC_1)/3.3, 0, 1, ADC_1); // ready if value lies out of [1.0,2.0] V


}

double value = 0;

void loop() {

    value = adc->analogRead(readPin);

    buffer->write(value);

    Serial.print("Buffer read:");
    Serial.println(buffer->read()*3.3/adc->getMaxValue());

    delay(100);
}

