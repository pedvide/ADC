/* Example for analogContinuousDifferentialRead
*  It measures continuously the voltage difference on pins A10-A11,
*  Write c and press enter on the serial console to check that the conversion is taking place,
*  Write t to check if the voltage agrees with the comparison in the setup()
*  Write s to stop the conversion, you can restart it writing r.
*/

#include <ADC.h>

// Teensy 3.0 has the LED on pin 13
const int ledPin = 13;

ADC adc; // adc object

void setup() {
  // initialize the digital pin as an output.
  pinMode(ledPin, OUTPUT);

  Serial.begin(38400);

  pinMode(A10, INPUT); //Diffy Channel 0 Positive
  pinMode(A11, INPUT); //Diffy Channel 0 Negative

  adc.setAveraging(1);
  adc.setResolution(9); // call this before the compare or continuous functions!!
  //adc.enableCompare(1.0/6.6*adc.getMaxValue(), 0); // true if value < 1.0V
  adc.enableCompareRange(-1.0*adc.getMaxValue()/6.6, 2.0*adc.getMaxValue()/6.6, 0, 1);

  adc.startContinuousDifferential(A10, A11);

  delay(500);
}

int value = 0;
char c=0;

void loop() {


  Serial.print("Conversion ");
  if(adc.isComplete()) {
      Serial.print("True");

      // the test results below were obtained commenting out all Serial communications and the delay() lines
      GPIOC_PTOR = 1<<5;
  } else {
      Serial.print("False");
  }

  value = adc.analogReadContinuous();


  Serial.print(" (");
  Serial.print(value*6.6/adc.getMaxValue(), DEC);
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
    } else if(c=='r') { // conversion complete?
        Serial.print("Restarting conversions ");
        adc.startContinuousDifferential(A10, A11);
        delay(100);
    }
  }

  delay(100);
}

// RESULTS OF THE TEST
// Measure continuously the voltage difference on a voltage divider. Result true if less than 1.0 V
// Measurement pins A10-A11. Clock speed 96 Mhz. Default options for analog_init. No code Serial or delays on loop(){}.

// ADC resolution     Measurement frequency (1 Hz uncertainty)    Num. averages
//     16  bits           100.221 kHz                               1
//     13  bits           127.498 kHz                               1
//     11  bits           129.400 kHz                               1
//      9  bits           322.243 kHz                               1

