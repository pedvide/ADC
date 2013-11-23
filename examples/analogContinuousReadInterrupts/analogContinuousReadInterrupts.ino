/* Example for analogContinuousReadInterrupts
*  It measures continuously the voltage on pin A9,
*  Write a and press enter on the serial console to check that the conversion is taking place,
*  Write v to get the last converted value
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

  Serial.begin(38400);

  pinMode(A10, INPUT); //Diffy Channel 0 Positive
  pinMode(A11, INPUT); //Diffy Channel 0 Negative
  pinMode(A9, INPUT); //pin 23 single ended



  adc.setAveraging(1);
  adc.setResolution(10); // call this before the compare or continuous functions!!
  adc.enableCompare(1.0/3.3*adc.getMaxValue(), 0); // true if value < 1.0V
  //adc.enableCompareRange(-1.0*adc.getMaxValue()3.3, 1.0*adc.getMaxValue()/3.3, 0, 1);

  adc.enableInterrupts();

  //adc.startContinuousDifferential(A10, A11);
  adc.startContinuous(A9);

  delay(500);
}

int value = 0;
char c=0;

void loop() {

  if (Serial.available()) {
	c = Serial.read();
	if(c=='a') { // conversion active?
	    Serial.print("converting? ");
        Serial.println(adc.isConverting());
    } else if(c=='s') { // stop conversion
        adc.stopContinuous();
        Serial.println("Stopped");
    } else if(c=='c') { // conversion complete?
        Serial.print("complete? ");
        Serial.println(adc.isComplete());
    } else if(c=='v') {
        Serial.print("Value: ");
        Serial.println(adc.analogReadContinuous()*3.3/adc.getMaxValue(), DEC);
    } else if(c=='r') { // conversion complete?
        Serial.println("Restarting conversions ");
        adc.startContinuous(readPin);
        delay(100);
    }

  }

}

void adc0_isr(void) {
    // Low-level code
    ADC0_RA;
    GPIOC_PTOR = 1<<5;

    // High-level code
    //adc.analogReadContinuous(); // read to clear the COCO flag
    //digitalWrite(ledPin, !digitalRead(ledPin)); // toggle led

}

// RESULTS OF THE TEST
// Measure continuously the voltage on a voltage divider.
// Measurement pin A9 (23). Clock speed 96 Mhz. Default options for analog_init. No code on loop(){}. Low level code on isr.

// ADC resolution     Measurement frequency   Measr. time    Num. averages
//     16  bits           387.092 kHz         2.58 us               1
//     12  bits           461.534 kHz         2.17 us               1
//     10  bits           545.300 kHz**       1.83 us               1
//      8  bits           1409.752 kHz*,|     0.71 us               1
// ** 511.45 kHz (1.95 us) if using high-level code in the isr.
// * 1209.09 kHz (0.83 us) if uncommented code in loop(), same frequency for the other resolutions
// | 515.932 kHz (1.94 us) if using high-level code in the isr.

//     16  bits           12.098 kHz          82.7 us               32
//     12  bits           14.422 kHz          69.3 us               32
//     10  bits           17.046 kHz          58.7 us               32
//      8  bits           44.116 kHz          22.7 us               32
