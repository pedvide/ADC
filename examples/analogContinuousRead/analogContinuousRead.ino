/* Example for analogContinuousRead
*  It measures continuously the voltage on pin A9,
*  Write c and press enter on the serial console to check that the conversion is taking place,
*  Write t to check if the voltage agrees with the comparison in the setup()
*  Write s to stop the conversion, you can restart it writing r.
*/

#include <ADC.h>

// Teensy 3.0 has the LED on pin 13
const int ledPin = 13;

const int readPin = A3; // ADC0
const int readPin2 = A3; // ADC1

ADC *adc = new ADC(); // adc object

void setup() {

  pinMode(ledPin, OUTPUT);
  pinMode(readPin, INPUT); //pin 23 single ended
  pinMode(readPin2, INPUT); //pin 23 single ended

  Serial.begin(9600);

  //adc->setReference(ADC_REF_INTERNAL, ADC_0); change all 3.3 to 1.2 if you change the reference

  //adc->setAveraging(8); // set number of averages
  //adc->setResolution(12); // set bits of resolution


  //adc->setAveraging(32, ADC_1); // set number of averages
  //adc->setResolution(12, ADC_1); // set bits of resolution

  // always call the compare functions after changing the resolution!
  //adc->enableCompare(1.0/3.3*adc->getMaxValue(ADC_0), 0, ADC_0); // measurement will be ready if value < 1.0V
  //adc->enableCompareRange(1.0*adc->getMaxValue(ADC_1)/3.3, 2.0*adc->getMaxValue(ADC_1)/3.3, 0, 1, ADC_1); // ready if value lies out of [1.0,2.0] V

  delay(500);
  Serial.println("end setup");


  //adc->enableInterrupts(ADC_0); // enable interrupts BEFORE calling a reading function!

  adc->startContinuous(readPin, ADC_0);

  delay(500);
}

int value = 0;
char c=0;

void loop() {

  Serial.print("Comparison with value/range is: ");
  if(adc->isComplete(ADC_0)) {
      Serial.print("True");

      // the test results below were obtained commenting out all Serial communications and the delay() lines
      //GPIOC_PTOR = 1<<5;
  } else {
      Serial.print("False");
  }

  value = adc->analogReadContinuous(ADC_0);

  Serial.print(" (");
  Serial.print(value*3.3/adc->getMaxValue(ADC_0), DEC);
  Serial.println(")");


  if (Serial.available()) {
	c = Serial.read();
	if(c=='c') { // conversion active?
	    Serial.print("Converting? ");
        Serial.println(adc->isConverting(ADC_0));
    } else if(c=='s') { // stop conversion
        adc->stopContinuous(ADC_0);
        Serial.println("Stopped");
    } else if(c=='t') { // conversion complete?
        Serial.print("True? ");
        Serial.println(adc->isComplete(ADC_0));
    } else if(c=='r') { // restart conversion
        Serial.print("Restarting conversions ");
        adc->startContinuous(readPin, ADC_0);
        delay(100);
    }
  }

  delay(100);

}

void adc0_isr(void) {
    // Low-level code
    ADC0_RA;
    GPIOC_PTOR = 1<<5;

    // High-level code
    //adc->analogReadContinuous(ADC_0); // read to clear the COCO flag
    //digitalWrite(ledPin, !digitalRead(ledPin)); // toggle led

}
#if defined(__MK20DX256__)
void adc1_isr(void) {
    // Low-level code
    ADC1_RA;
    GPIOC_PTOR = 1<<5;

    // High-level code
    //adc->analogReadContinuous(ADC_1); // read to clear the COCO flag
    //digitalWrite(ledPin, !digitalRead(ledPin)); // toggle led

}
#endif

// RESULTS OF THE TEST
// Measure continuously the voltage on a voltage divider.
// Measurement pin A9 (23). Clock speed 96 Mhz. Default options for analog_init. Only toggling led and analogReadContinuous code on loop(){}, no Serial communications nor delay.

// ADC resolution     Measurement frequency                     Num. averages
//     16  bits           193.086 kHz                               1
//     12  bits           306.260 kHz                               1
//     10  bits           418.020 kHz                               1
//      8  bits           937.490 kHz                               1
//
//  Same but using interrupts, no code at all in loop(){}
// ADC resolution     Measurement frequency                     Num. averages
//     16  bits           387.088 kHz                               1
//     12  bits           461.528 kHz                               1
//     10  bits           545.442 kHz                               1
//      8  bits          1410.264 kHz                               1
