/* Example for analogContinuousRead
*  It measures continuously the voltage on pin A9,
*  Write v to read the value on pin A8, it will stop the continuous conversion, read the value and restart the conversion automatically
*  Write c and press enter on the serial console to check that the conversion is taking place,
*  Write t to check if the voltage agrees with the comparison in the setup()
*  Write s to stop the conversion, you can restart it writing r.
*
*  You can also select a differential continuous measurement and read single-ended or differential pins, just comment/uncomment the lines
*/

#include <ADC.h>

// Teensy 3.0 has the LED on pin 13
const int ledPin = 13;
const int readPinCont = A9;
const int readPinSingle = A8;

ADC adc; // adc object

void setup() {
  // initialize the digital pin as an output.
  pinMode(ledPin, OUTPUT);
  pinMode(readPinCont, INPUT); //pin 23 single ended
  pinMode(readPinSingle, INPUT);
  pinMode(A10, INPUT);
  pinMode(A11, INPUT);

  Serial.begin(38400);

  adc.setAveraging(4);
  adc.setResolution(16); // call this before the compare function!!
  adc.enableCompare(1.0/3.3*adc.getMaxValue(), 0); // true if value < 1.0V
  //adc.enableCompareRange(-1.0*adc.getMaxValue()/3.3, 1.0*adc.getMaxValue()/3.3, 0, 1);

  adc.startContinuous(readPinCont);
  //adc.startContinuousDifferential(A10, A11);

  delay(500);
}

int valueCont = 0, valueSingle = 0;
char c=0;

void loop() {


  Serial.print("Comparison with value/range is: ");
  if(adc.isComplete()) {
      Serial.print("True");
  } else {
      Serial.print("False");
  }

  valueCont = adc.analogReadContinuous();

  Serial.print(" (");
  Serial.print(valueCont*3.3/adc.getMaxValue(), DEC);
  Serial.println(")");

  if (Serial.available()) {
	c = Serial.read();
	if(c=='a') { // conversion active?
	    Serial.print("Converting? ");
        Serial.println(adc.isConverting());
    } else if(c=='s') { // stop conversion
        adc.stopContinuous();
        Serial.println("Stopped");
    } else if(c=='t') { // conversion complete?
        Serial.print("True? ");
        Serial.println(adc.isComplete());
    } else if(c=='v') {
        Serial.print("Single value: ");
        Serial.println(adc.analogRead(readPinSingle)*3.3/adc.getMaxValue(), DEC);
        //Serial.println(adc.analogReadDifferential(A10,A11)*2.0*3.3/adc.getMaxValue(), DEC);
    } else if(c=='r') { // restart conversion
        Serial.println("Restarting conversions ");
        adc.startContinuous(readPinCont);
        delay(100);
    }
  }

  delay(100);
}
