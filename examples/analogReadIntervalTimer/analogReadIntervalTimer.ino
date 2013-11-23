#include "ADC.h"

// Teensy 3.0 has the LED on pin 13
const int ledPin = 13;

const int readPin = A9;
const int period1 = 50; // us

const int readPin2 = A8;
const int period2 = 50; // us

ADC adc; // adc object

int startTimerValue = 0, startTimerValue2 = 0;

void setup() {

    pinMode(ledPin, OUTPUT); pinMode(14, OUTPUT);
    pinMode(readPin, INPUT); pinMode(readPin2, INPUT); //pin 23 single ended

    Serial.begin(9600);

    delay(1000);

    adc.setAveraging(1); // set number of averages
    adc.setResolution(10); // set bits of resolution

    // always call the compare functions after changing the resolution!
    //adc.enableCompare(1.0/3.3*adc.getMaxValue(), 0); // measurement will be ready if value < 1.0V
    //adc.enableCompareRange(1.0*adc.getMaxValue()/3.3, 2.0*adc.getMaxValue()/3.3, 0, 1); // ready if value lies out of [1.0,2.0] V

    Serial.println("Starting Timer");

    // start a analogRead every 10 ms, the values will be placed in a circular buffer
    // for 1 average and 10 bits of resolution the timer interrupt, ADC measurement and addition into the buffer takes around 6.0 us
    startTimerValue = adc.startAnalogTimer(readPin, period1);
    delay(25);
    //startTimerValue2 = adc.startAnalogTimer(readPin2, period2);

    Serial.println("Timer started");

    delay(500);
}

int value = 0;
char c=0;

void loop() {

    if(startTimerValue==ANALOG_TIMER_ERROR) {
            Serial.println("Setup failed");
    }
    if(startTimerValue2==ANALOG_TIMER_ERROR) {
            Serial.println("Setup failed 2");
    }

    if(!adc.isTimerLastValue(readPin)) { // read the values in the buffer, the values are placed every 10 ms, but we read them every 50 ms
        Serial.print("Read pin 1: ");
        Serial.println(adc.getTimerValue(readPin)*3.3/adc.getMaxValue());
        //Serial.println("New value!");
    }/*
    if(!adc.isTimerLastValue(readPin2)) { // read the values in the buffer, the values are placed every 10 ms, but we read them every 50 ms
        Serial.print("Read pin 2: ");
        Serial.println(adc.getTimerValue(readPin2)*3.3/adc.getMaxValue());
        //Serial.println("New value!");
    }*/

    if (Serial.available()) {
        c = Serial.read();
        if(c=='s') { // stop timer
            Serial.print("Stop timer");
            adc.stopAnalogTimer(readPin);
        } else if(c=='r') { // restart timer
            Serial.print("Restart timer");
            startTimerValue = adc.startAnalogTimer(readPin, period1);
        }
    }

  delay(50);
}
