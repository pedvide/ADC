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

  for (auto pin : adc::adc0::pin_info::pins) {
    value1 = adc::adc0::analogRead(pin);
    Serial.printf("Pin A%d: %d \n", adc::adc0::pin2int(pin), value1);
  }

  // value1 = adc::adc0::analogRead(adc::adc0::pin_t::A1);
  // value2 = adc::adc0::pin2int(adc::adc0::pin_t::A1);

  // Serial.print("ADC1 ADC_CFG_ADSTS: ");
  // Serial.println((ADC1_CFG & ADC_CFG_ADSTS(0x03)) >> 8, HEX);

  // Serial.print("ADC1 ADC_CFG_ADSTS: ");
  // Serial.println(adc::adc_module_reg_t<0>::adsts::read(), HEX);

  digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));

  delay(500);
}
