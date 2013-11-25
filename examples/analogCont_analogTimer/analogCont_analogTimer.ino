#include "ADC.h"

// Teensy 3.0 has the LED on pin 13
const int ledPin = 13;

const int readPinCont = A9;
const int readPinTimer = A10;
const int readPinTimer2 = A7;

ADC adc; // adc object

int startTimerValue = 0, startTimerValue2 = 0;

void setup() {

    pinMode(ledPin, OUTPUT); pinMode(14, OUTPUT);
    pinMode(readPinTimer, INPUT); pinMode(readPinCont, INPUT); //pin 23 single ended

    Serial.begin(9600);

    delay(1000);

    adc.setAveraging(1); // set number of averages
    adc.setResolution(13); // set bits of resolution

    // always call the compare functions after changing the resolution!
    //adc.enableCompare(1.0/3.3*adc.getMaxValue(), 0); // measurement will be ready if value < 1.0V
    //adc.enableCompareRange(1.0*adc.getMaxValue()/3.3, 2.0*adc.getMaxValue()/3.3, 0, 1); // ready if value lies out of [1.0,2.0] V

    Serial.println("Starting Cont");
    adc.startContinuous(readPinCont);
    Serial.println("Cont started");

    //adc.enableInterrupts();

    Serial.println("Starting Timer 1");
    // start a analogRead every 10 ms, the values will be placed in a circular buffer
    // for 1 average and 10 bits of resolution the timer interrupt, ADC measurement and inclusion into the buffer takes around 9.0 us
    startTimerValue = adc.startAnalogTimer(readPinTimer, 1000000); // us
    //startTimerValue = adc.startAnalogTimerDifferential(A10, A11, 1000000);
    Serial.println("Timer 1 started");

    //delay(500);
}

int valueCont = 0, valueTimer = 0;
char c=0;

void loop() {

    if(startTimerValue==ANALOG_TIMER_ERROR) {
            Serial.println("Setup 1 failed");
    }
    if(startTimerValue2==ANALOG_TIMER_ERROR) {
            Serial.println("Setup 2 failed");
    }

    if(!adc.isTimerLastValue(readPinTimer)) { // read the values in the buffer, the values are placed every 10 ms, but we read them every 50 ms
        Serial.print("Read pin 1: ");
        Serial.println(adc.getTimerValue(readPinTimer)*3.3/adc.getMaxValue(), DEC);
        //Serial.println("New value!");
    }
    if(!adc.isTimerLastValue(readPinTimer2)) { // read the values in the buffer, the values are placed every 10 ms, but we read them every 50 ms
        Serial.print("Read pin 2: ");
        Serial.println(adc.getTimerValue(readPinTimer2)*3.3/adc.getMaxValue(), DEC);
        //Serial.println("New value!");
    }

  valueCont = adc.analogReadContinuous();

  Serial.print("Continuous value:");
  Serial.println(valueCont*3.3/adc.getMaxValue(), DEC);

  if (Serial.available()) {
	c = Serial.read();
	if(c=='a') { // conversion active?
	    Serial.print("Converting? ");
        Serial.println(adc.isConverting());
    } else if(c=='t') { // conversion complete?
        Serial.print("True? ");
        Serial.println(adc.isComplete());
    } else if(c=='s') { // stop timer
        Serial.println("Stop timer 2");
        adc.stopAnalogTimer(readPinTimer2);
    } else if(c=='r') { // restart timer
        Serial.println("Start timer 2");
        startTimerValue2 = adc.startAnalogTimer(readPinTimer2, 1500);
        //startTimerValue2 = adc.startAnalogTimerDifferential(A10, A11, 1500);
    }
  }

  delay(200);
}
