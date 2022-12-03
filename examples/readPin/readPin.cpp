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
int pin = 0;

void loop() {

  if (Serial.available()) {
    pin = Serial.parseInt();
    if (pin > 0) {
      Serial.print("Changing to pin ");
      Serial.println(pin);
      readPin = pin;
    }
  }

  //   Serial.print("Pin: ");
  //   Serial.print(readPin);
  //   Serial.print(", ADC0 value: ");
  //   value1 = adc->adc0->analogRead(readPin);
  //   Serial.print(value1 * 3.3 / adc->adc0->getMaxValue(), DEC);
  // #ifdef ADC_DUAL_ADCS
  //   Serial.print(", ADC1 value: ");
  //   value2 = adc->adc1->analogRead(readPin);
  //   Serial.println(value2 * 3.3 / adc->adc0->getMaxValue(), DEC);
  // #else
  //   Serial.println();
  // #endif

  digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));

  delay(200);
}
