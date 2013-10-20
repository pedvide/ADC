/* Example for analogContinuousReadInterrupts
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

  NVIC_ENABLE_IRQ(IRQ_ADC0);


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
        Serial.print("Restarting conversions ");
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
// Measure continuously the voltage on a voltage divider. Result true if less than 1.0 V
// Measurement pin A9 (23). Clock speed 96 Mhz. Default options for analog_init. No code on loop(){}. Low level code on isr.

// ADC resolution     Measurement frequency (1 Hz uncertainty)    Num. averages
//     16  bits           193.546 kHz                               1
//     12  bits           230.767 kHz                               1
//     10  bits           272.650 kHz**                             1
//      8  bits           704.876 kHz*,|                            1
// * 604.545 kHz if uncommented code in loop(), same frequency for the other resolutions
// ** 255.725 kHz if using high-level code in the isr.
// | 257.966 kHz if using high-level code in the isr.

//     16  bits           6.049 kHz                                 32
//     12  bits           7.211 kHz                                 32
//     10  bits           8.523 kHz                                 32
//      8  bits           22.058 kHz                                32
