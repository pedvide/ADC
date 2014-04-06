/* Example for analogReadDifferential
*  You can change the number of averages, bits of resolution and also the comparison value or range.
*/

#include <ADC.h>

// Teensy 3.0 has the LED on pin 13
const int ledPin = 13;
const int readPin = A9;
const int readPin2 = A8;

ADC *adc = new ADC(); // adc object

void setup() {


  pinMode(ledPin, OUTPUT);
  pinMode(A10, INPUT); //Diff Channel 0 Positive
  pinMode(A11, INPUT); //Diff Channel 0 Negative
  pinMode(A12, INPUT); //Diff Channel 3 Positive
  pinMode(A13, INPUT); //Diff Channel 3 Negative

  Serial.begin(9600);

  //adc->setReference(ADC_REF_INTERNAL, ADC_0); // change all 3.3 to 1.2 if you change the reference

  //adc->enablePGA(4, ADC_0);
  adc->setAveraging(32); // set number of averages
  adc->setResolution(13); // set bits of resolution

  //adc->enablePGA(5, ADC_1);
  adc->setAveraging(32, ADC_1); // set number of averages
  adc->setResolution(13, ADC_1); // set bits of resolution

  // always call the compare functions after changing the resolution!
  //adc->enableCompare(1.0/3.3*adc->getMaxValue(ADC_0), 0, ADC_0); // measurement will be ready if value < 1.0V
  //adc->enableCompareRange(1.0*adc->getMaxValue(ADC_1)/3.3, 2.0*adc->getMaxValue(ADC_1)/3.3, 0, 1, ADC_1); // ready if value lies out of [1.0,2.0] V

  delay(500);
  Serial.println("end setup");
}

int value = ADC_ERROR_VALUE;
int value2 = ADC_ERROR_VALUE;

int c=0;

void loop() {

  //value = 0;
  value = adc->analogReadDifferential(A12,A13, ADC_0); // read a new value, will return ADC_ERROR_VALUE if the comparison is false.
  //value2 = adc->analogReadDifferential(A12,A13, ADC_1);

  if(value!=ADC_ERROR_VALUE ) {

      Serial.print("Raw value ADC0: ");
      Serial.print(value, DEC);
      Serial.print(", value ADC0: ");
      // Divide by the maximum possible value and the PGA level
      Serial.println(value*3.3/adc->getPGA()/adc->getMaxValue(), DEC);
  } else {
      Serial.println("ADC0 Comparison failed");
  }
  #if defined(__MK20DX256__)
  if(value2!=ADC_ERROR_VALUE) {

      Serial.print("Raw value ADC1: ");
      Serial.print(value2, DEC);
      Serial.print(", value ADC1: ");
      Serial.println(value2*3.3/adc->getPGA(ADC_1)/adc->getMaxValue(ADC_1), DEC);
  } else {
      Serial.println("ADC1 Comparison failed");
  }
  #endif

  GPIOC_PTOR = 1<<5;


  delay(1000);
}
