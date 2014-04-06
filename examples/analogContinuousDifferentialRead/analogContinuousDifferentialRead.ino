/* Example for analogContinuousDifferentialRead
*  It measures continuously the voltage difference on pins A10-A11,
*  Write c and press enter on the serial console to check that the conversion is taking place,
*  Write t to check if the voltage agrees with the comparison in the setup()
*  Write s to stop the conversion, you can restart it writing r.
*/

#include <ADC.h>

// Teensy 3.0 has the LED on pin 13
const int ledPin = 13;

const int readPin = A10; // ADC0

ADC *adc = new ADC(); // adc object

void setup() {

  pinMode(ledPin, OUTPUT);
  pinMode(A10, INPUT); //Diff Channel 0 Positive
  pinMode(A11, INPUT); //Diff Channel 0 Negative
  pinMode(A12, INPUT); //Diff Channel 3 Positive
  pinMode(A13, INPUT); //Diff Channel 3 Negative

  Serial.begin(9600);

  //adc->setReference(ADC_REF_INTERNAL, ADC_0); change all 3.3 to 1.2 if you change the reference

  //adc->enablePGA(1, ADC_0); // PGA=2^gain
  //adc->setAveraging(8); // set number of averages
  //adc->setResolution(12); // set bits of resolution

  adc->enablePGA(2, ADC_1);
  //adc->setAveraging(32, ADC_1); // set number of averages
  //adc->setResolution(12, ADC_1); // set bits of resolution

  // always call the compare functions after changing the resolution!
  //adc->enableCompare(1.0/3.3*adc->getMaxValue(ADC_0), 0, ADC_0); // measurement will be ready if value < 1.0V
  //adc->enableCompareRange(1.0*adc->getMaxValue(ADC_1)/3.3, 2.0*adc->getMaxValue(ADC_1)/3.3, 0, 1, ADC_1); // ready if value lies out of [1.0,2.0] V

  delay(500);
  Serial.println("end setup");


  //adc->enableInterrupts(ADC_0); // enable interrupts BEFORE calling a reading function!

  adc->startContinuousDifferential(A12, A13, ADC_1);

  delay(500);
}

int value = 0;
char c=0;

void loop() {

  Serial.print("Comparison with value/range is: ");
  if(adc->isComplete(ADC_1)) {
      Serial.print("True");

      // the test results below were obtained commenting out all Serial communications and the delay() lines
      //GPIOC_PTOR = 1<<5;
  } else {
      Serial.print("False");
  }

  value = adc->analogReadContinuous(ADC_1);

  Serial.print(" (");
  Serial.print(value*3.3/adc->getPGA(ADC_1)/adc->getMaxValue(ADC_1), DEC);
  Serial.println(")");


  if (Serial.available()) {
	c = Serial.read();
	if(c=='c') { // conversion active?
	    Serial.print("Converting? ");
        Serial.println(adc->isConverting(ADC_1));
    } else if(c=='s') { // stop conversion
        adc->stopContinuous(ADC_1);
        Serial.println("Stopped");
    } else if(c=='t') { // conversion complete?
        Serial.print("True? ");
        Serial.println(adc->isComplete(ADC_1));
    } else if(c=='r') { // restart conversion
        Serial.print("Restarting conversions ");
        adc->startContinuousDifferential(A12, A13, ADC_1);
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
// Measure continuously the voltage difference on a voltage divider.
// Measurement pins A10-A11. Clock speed 96 Mhz. Default options for analog_init. No code Serial or delays on loop(){}.

// ADC resolution     Measurement frequency (1 Hz uncertainty)    Num. averages
//     16  bits           214.164 kHz                               1
//     13  bits           250.100 kHz                               1
//     11  bits           294.620 kHz                               1
//      9  bits           663.828 kHz                               1

