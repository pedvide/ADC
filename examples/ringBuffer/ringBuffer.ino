#include "ADC.h"
#include "RingBuffer.h"

const int readPin = A9;

ADC *adc = new ADC(); // adc object

RingBuffer *buffer = new RingBuffer;


void setup() {

    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(readPin, INPUT); //pin 23 single ended

    Serial.begin(9600);

    adc->adc0->setAveraging(8); // set number of averages
    adc->adc0->setResolution(12); // set bits of resolution
    adc->adc1->setAveraging(32); // set number of averages
    adc->adc1->setResolution(12); // set bits of resolution

}

double value = 0;

void loop() {

    value = adc->analogRead(readPin);

    buffer->write(value);

    Serial.print("Buffer read:");
    Serial.println(buffer->read()*3.3/adc->adc0->getMaxValue());

    delay(100);
}

