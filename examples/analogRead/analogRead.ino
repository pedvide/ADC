/* Example for analogRead
*  You can change the number of averages, bits of resolution and also the comparison value or range.
*/


#include <ADC.h>

#define ADC_0 0
#define ADC_1 1


// Teensy 3.0 has the LED on pin 13
const int ledPin = 13;

const int readPin = 23; // ADC0
const int readPin2 = A3; // ADC1

ADC *adc = new ADC(); // adc object

void setup() {



  pinMode(ledPin, OUTPUT);
  pinMode(readPin, INPUT); //pin 23 single ended
  pinMode(readPin2, INPUT); //pin 23 single ended

  Serial.begin(9600);

  //adc->setReference(ADC_REF_INTERNAL, ADC_0); change all 3.3 to 1.2 if you change the reference

  adc->setAveraging(8); // set number of averages
  adc->setResolution(12); // set bits of resolution


  adc->setAveraging(32, ADC_1); // set number of averages
  adc->setResolution(12, ADC_1); // set bits of resolution

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

  value = adc->analogRead(readPin, ADC_0); // read a new value, will return ADC_ERROR_VALUE if the comparison is false.

  value2 = adc->analogRead(readPin2, ADC_1);

  if(value!=ADC_ERROR_VALUE ) {

      Serial.print("Pin: ");
      Serial.print(readPin);
      Serial.print(", value ADC0: ");
      Serial.println(value*3.3/adc->getMaxValue(ADC_0), DEC);
  } else {
      Serial.println("ADC0 Comparison failed");
  }
  #if defined(__MK20DX256__)
  if(value2!=ADC_ERROR_VALUE) {

      Serial.print("Pin: ");
      Serial.print(readPin2);
      Serial.print(", value ADC1: ");
      Serial.println(value2*3.3/adc->getMaxValue(ADC_1), DEC);
  } else {
      Serial.println("ADC1 Comparison failed");
  }
  #endif


  GPIOC_PTOR = 1<<5;


  delay(1000);
}


