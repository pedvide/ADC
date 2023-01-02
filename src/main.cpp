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
// int pin = A0;

void loop() {

  //   print_ADC_regs();

  // for (auto pin : adc::adc0::pin_info::pins) {
  //   value1 = adc::adc0::analogRead(pin);
  //   Serial.printf("Pin A%d: %d \n", adc::adc0::pin2int(pin), value1);
  // }

#if defined(__MK20DX128__)
  value2 = adc::adc0::analogReadDifferential(adc::adc0::diff_pin_t::A12_13);
#endif

  value1 = adc::adc0::analogRead(adc::adc0::pin_t::A1);
  // value2 = adc::adc0::pin2int(adc::adc0::pin_t::A1);

  // digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));

  // delay(500);
}
