/* Example for readPin
 *  It measures continuously the voltage on pin A9,
 *  Write a pin number to measure that instead.
 *  If the pin is wrong, an error message will appear, change the pin and then
 * write -1 to clear error.
 */

#include <adc.hpp>

int readPin = A2; // ADC0

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(readPin, INPUT);

  Serial.begin(9600);

  delay(500);
}

int value1 = 0, value2 = 0;
int pin = A0;

void loop() {

  value1 = adc::adc_t::adc0::analogRead(A0);
  Serial.print("ADC H0: ");
  Serial.print(ADC1_HC0, HEX);
  Serial.print(", value: ");
  Serial.println(value1, DEC);
  value1 = adc::adc_t::adc0::analogRead(A1);
  Serial.print("ADC H0: ");
  Serial.print(ADC1_HC0, HEX);
  Serial.print(", value: ");
  Serial.println(value1, DEC);
  //   adc::adc_t::adc0::stopADC();

  digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));

  delay(200);
}
