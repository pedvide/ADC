/* This example uses the internal 1.2 V reference to measure the Teensy's supply voltage
 *  
 * Hardware: just the teensy, USB Serial, no external connections.
 */

#include <ADC.h>
#include <VREF.h>

ADC* adc;

void setup() {
  Serial.begin(9600);
  adc = new ADC();
  VREF::start(VREF_SC_MODE_LV_LOWPOWERBUF);
  VREF::waitUntilStable();
}

void loop() {
  int value = adc->analogRead(ADC_INTERNAL_SOURCE::VREF_OUT, ADC_1);
  if (value == ADC_ERROR_VALUE)
  {
    Serial.println("error");
  }
  uint32_t voltage_mV = (1200*adc->getMaxValue(ADC_1))/value;
  Serial.printf("Vdd = %04d mV\n", voltage_mV);
  delay(1000);
}
