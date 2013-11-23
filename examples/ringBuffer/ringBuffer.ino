#include "ADC.h"

// Teensy 3.0 has the LED on pin 13
const int ledPin = 13;
const int readPin = A9;

ADC adc; // adc object

RingBuffer *buffer = new RingBuffer;


void setup() {

    pinMode(ledPin, OUTPUT);
    pinMode(readPin, INPUT); //pin 23 single ended

    Serial.begin(9600);

    delay(1000);

    adc.setAveraging(1); // set number of averages
    adc.setResolution(10); // set bits of resolution

    // always call the compare functions after changing the resolution!
    //adc.enableCompare(1.0/3.3*adc.getMaxValue(), 0); // measurement will be ready if value < 1.0V
    //adc.enableCompareRange(1.0*adc.getMaxValue()/3.3, 2.0*adc.getMaxValue()/3.3, 0, 1); // ready if value lies out of [1.0,2.0] V

}

int value = 0;

void loop() {

    value = adc.analogRead(readPin);

    buffer->write(value);

    Serial.print("Buffer read:");
    Serial.println(buffer->read());

    delay(100);
}

