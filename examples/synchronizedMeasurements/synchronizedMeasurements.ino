/* Example for synchonized measurements using both ADC present in Teensy 3.1
*  You can change the number of averages, bits of resolution and also the comparison value or range.
*/


#include <ADC.h>

#define ADC_0 0
#define ADC_1 1

// Teensy 3.0 has the LED on pin 13
const int ledPin = 13;
const int readPin = A0;
const int readPin2 = A2;

ADC *adc = new ADC(); // adc object

elapsedMicros time;

void setup() {


  pinMode(ledPin, OUTPUT);
  pinMode(readPin, INPUT);
  pinMode(readPin2, INPUT);

  Serial.begin(9600);

  adc->setAveraging(32); // set number of averages
  adc->setResolution(12); // set bits of resolution

  adc->setAveraging(32, ADC_1); // set number of averages
  adc->setResolution(12, ADC_1); // set bits of resolution

  // always call the compare functions after changing the resolution!
  //adc->enableCompare(1.0/3.3*adc.getMaxValue(), 0, ADC_0); // measurement will be ready if value < 1.0V
  //adc->enableCompareRange(1.0*adc.getMaxValue(ADC_1)/3.3, 2.0*adc.getMaxValue(ADC_1)/3.3, 0, 1, ADC_1); // ready if value lies out of [1.0,2.0] V

  delay(500);
  Serial.println("end setup");
}

int value = 0;
int value2 = 0;

ADC::Sync_result result;

void loop() {

  result = adc->analogSynchronizedRead(readPin, readPin2);

  if( (result.result_adc0 !=ADC_ERROR_VALUE) && (result.result_adc1 !=ADC_ERROR_VALUE) ) {

      // the test results below were obtained commenting out all Serial.print*() and the delay() lines
      //GPIOC_PTOR = 1<<5;

      //Serial.print("Value ADC0: ");
      Serial.print(time, DEC);
      Serial.print(" ");
      Serial.print(result.result_adc0*3.3/adc->adc0->getMaxValue(), DEC);
      Serial.print(" ");
      Serial.println(result.result_adc1*3.3/adc->adc1->getMaxValue(), DEC);
  } else {
      Serial.println("Comparison failed");
  }

    GPIOC_PTOR = 1<<5;


  delay(50);
}

/*
 With a AWG I generated a sine wave of 1 Hz and 2 V amplitude.
 I measure synchronously on pins A0 (ADC0) and A2 (ADC1), sampling at 20 Hz (every 50ms).
 The relative error: mean(value(A0)-value(A2))/mean(value(A0)) is approx 0.02%


*/

