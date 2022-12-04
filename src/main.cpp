/* Example for readPin
 *  It measures continuously the voltage on pin A9,
 *  Write a pin number to measure that instead.
 *  If the pin is wrong, an error message will appear, change the pin and then
 * write -1 to clear error.
 */

#include <adc.hpp>

int readPin = A2; // ADC0

void print_ADC_regs() {
  Serial.print("ADC1 H0: ");
  Serial.println(ADC1_HC0, HEX);

  Serial.print("ADC1 HS: ");
  Serial.println(ADC1_HS, HEX);

  Serial.print("ADC1 R0: ");
  Serial.println(ADC1_R0, HEX);

  Serial.print("ADC1 CFG: ");
  Serial.println(ADC1_CFG, HEX);

  Serial.print("ADC1 GC: ");
  Serial.println(ADC1_GC, HEX);

  Serial.print("ADC1 GS: ");
  Serial.println(ADC1_GS, HEX);
}

void setup() {

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(readPin, INPUT);

  Serial.begin(9600);

  delay(500);
}

int value1 = 0, value2 = 0;
int pin = A0;

void loop() {

  //   print_ADC_regs();

  for (adc::adc0::pin_t pin = adc::adc0::pin_t::begin;
       pin <= adc::adc0::pin_t::end;
       pin = static_cast<adc::adc0::pin_t>((size_t)pin + 1)) {
    value1 = adc::adc0::analogRead(pin);
    Serial.printf("Pin A%d: %d \n", (int)pin - (int)adc::adc0::pin_t::A0,
                  value1);
  }

  digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));

  delay(500);
}
