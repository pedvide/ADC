/* Example for analogReadDifferential
*  You can change the number of averages, bits of resolution and also the comparison value or range.
*/

#include <ADC.h>

// Teensy 3.0 has the LED on pin 13
const int ledPin = 13;

ADC adc; // adc object

void setup() {
  // initialize the digital pin as an output.
  pinMode(ledPin, OUTPUT);
  pinMode(A10, INPUT); //Diff. Channel 0 Positive
  pinMode(A11, INPUT); //Diff. Channel 0 Negative

  Serial.begin(9600);

  adc.setAveraging(4);
  adc.setResolution(13); // call this before the compare functions!!

  // the differential mode includes a bit for the sign, so the range is from -3.3 to 3.3
  //adc.enableCompare(-2.0/6.6*adc.getMaxValue(), 0); // true if value < -1.0V
  adc.enableCompareRange(-2.0*adc.getMaxValue()/6.6, 1.0*adc.getMaxValue()/6.6, 0, 1); // ready if value lies outside [-2.0,1.0] V

  delay(500);
}

int value = 0;

void loop() {

  value = adc.analogReadDifferential(A10,A11);

  if(value!=ADC_ERROR_DIFF_VALUE) {

      // the test results below were obtained commenting out all Serial.print*() and the delay() lines
      //GPIOC_PTOR = 1<<5;

      Serial.print("Value: ");
      Serial.println(value*6.6/adc.getMaxValue(), DEC); // the differential mode includes a bit for the sign, so the range is from -3.3 to 3.3
  } else {
      Serial.println("Comparison failed");
  }

  delay(100);
}

// RESULTS OF THE TEST
// Differential measurement in a loop of the voltage on a voltage divider.
// Measurement pins A10 and A11. Clock speed 96 Mhz. Default options for analog_init. Only toggling led code on loop(){}, no Serial.print() nor delay.

// ADC resolution     Measurement frequency                 Num. averages
//     16  bits           146.990 kHz                               1
//     13  bits           146.020 kHz                               1
//     11  bits           158.728 kHz                               1
//      9  bits           191.736 kHz                               1
