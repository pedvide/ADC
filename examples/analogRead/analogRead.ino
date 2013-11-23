/* Example for analogRead
*  You can change the number of averages, bits of resolution and also the comparison value or range.
*/


#include <ADC.h>

// Teensy 3.0 has the LED on pin 13
const int ledPin = 13;
const int readPin = A9;

ADC adc; // adc object

void setup() {

  pinMode(ledPin, OUTPUT);
  pinMode(readPin, INPUT); //pin 23 single ended

  Serial.begin(9600);

  adc.setAveraging(4); // set number of averages
  adc.setResolution(16); // set bits of resolution

  // always call the compare functions after changing the resolution!
  adc.enableCompare(1.0/3.3*adc.getMaxValue(), 0); // measurement will be ready if value < 1.0V
  //adc.enableCompareRange(1.0*adc.getMaxValue()/3.3, 2.0*adc.getMaxValue()/3.3, 0, 1); // ready if value lies out of [1.0,2.0] V

  delay(500);
}

int value = 0;

void loop() {

  value = adc.analogRead(readPin); // read a new value, will return ADC_ERROR_VALUE if the comparison is false.

  if(value!=ADC_ERROR_VALUE) {

      // the test results below were obtained commenting out all Serial.print*() and the delay() lines
      //GPIOC_PTOR = 1<<5;

      Serial.print("Value: ");
      Serial.println(value*3.3/adc.getMaxValue(), DEC);
  } else {
      Serial.println("Comparison failed");
  }


  delay(100);
}

// RESULTS OF THE TEST
// Measure in a loop the voltage on a voltage divider. Result true if less than 1.0 V
// Measurement pin A9 (23). Clock speed 96 Mhz. Default options for analog_init. Only toggling led code on loop(){}, no Serial.print() nor delay.

// ADC resolution     Measurement frequency                  Num. averages
//     16  bits            99.960 kHz                               1
//     12  bits           107.002 kHz                               1
//     10  bits           115.226 kHz                               1
//      8  bits           166.320 kHz                               1
