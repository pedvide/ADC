/* Example for analogContinuousRead
*  It measures continuously the voltage on pin A9,
*  Write c and press enter on the serial console to check that the conversion is taking place,
*  Write t to check if the voltage agrees with the comparison in the setup()
*  Write s to stop the conversion, you can restart it writing r.
*/

#include <ADC.h>

// Teensy 3.0 has the LED on pin 13
const int ledPin = 13;
const int readPin = A9;

ADC adc; // adc object

void setup() {
  // initialize the digital pin as an output.
  pinMode(ledPin, OUTPUT);
  pinMode(readPin, INPUT); //pin 23 single ended

  Serial.begin(38400);

  adc.setAveraging(4);
  adc.setResolution(16); // call this before the compare function!!
  //adc.enableCompare(1.0/3.3*adc.getMaxValue(), 0); // true if value < 1.0V
  //adc.enableCompareRange(-1.0*adc.getMaxValue()/3.3, 1.0*adc.getMaxValue()/3.3, 0, 1);

  adc.startContinuous(readPin);

  delay(500);
}

int value = 0;
char c=0;

void loop() {

  Serial.print("Comparison with value/range is: ");
  if(adc.isComplete()) {
      Serial.print("True");

      // the test results below were obtained commenting out all Serial communications and the delay() lines
      //GPIOC_PTOR = 1<<5;
  } else {
      Serial.print("False");
  }

  value = adc.analogReadContinuous();

  Serial.print(" (");
  Serial.print(value*3.3/adc.getMaxValue(), DEC);
  Serial.println(")");


  if (Serial.available()) {
	c = Serial.read();
	if(c=='c') { // conversion active?
	    Serial.print("Converting? ");
        Serial.println(adc.isConverting());
    } else if(c=='s') { // stop conversion
        adc.stopContinuous();
        Serial.println("Stopped");
    } else if(c=='t') { // conversion complete?
        Serial.print("True? ");
        Serial.println(adc.isComplete());
    } else if(c=='r') { // restart conversion
        Serial.print("Restarting conversions ");
        adc.startContinuous(readPin);
        delay(100);
    }
  }

  delay(100);
}

// RESULTS OF THE TEST
// Measure continuously the voltage on a voltage divider.
// Measurement pin A9 (23). Clock speed 96 Mhz. Default options for analog_init. Only toggling led code on loop(){}, no Serial communications nor delay.

// ADC resolution     Measurement frequency                     Num. averages
//     16  bits           325.500 kHz                               1
//     12  bits           345.620 kHz                               1
//     10  bits           364.230 kHz                               1
//      8  bits           772.274 kHz                               1
