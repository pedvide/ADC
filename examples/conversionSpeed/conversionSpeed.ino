/* Example for analogContinuousRead
*  It measures continuously the voltage on pin A9,
*  Write v and press enter on the serial console to get the value
*  Write c and press enter on the serial console to check that the conversion is taking place,
*  Write t to check if the voltage agrees with the comparison in the setup()
*  Write s to stop the conversion, you can restart it writing r.
*/

#include <ADC.h>
#include <ADC_util.h>

const int readPin = A2; // ADC0 or ADC1

volatile uint32_t value = 0;
#if defined(ADC_TEENSY_4)
const uint32_t NUM_SAMPLES = 10000; 
#else
const uint32_t NUM_SAMPLES = 1000; 
#endif
uint32_t num_samples = NUM_SAMPLES; 
elapsedMicros timeElapsed;
volatile uint32_t num_iter = 0;

ADC *adc = new ADC(); // adc object

void setup() {

    pinMode(LED_BUILTIN, OUTPUT);
    pinMode(readPin, INPUT_PULLUP);

    Serial.begin(9600);
    delay(100);

    Serial.print("F_CPU: "); Serial.print(F_CPU/1e6);  Serial.println(" MHz."); 
    Serial.print("ADC_F_BUS: "); Serial.print(ADC_F_BUS/1e6); Serial.println(" MHz."); 

    // Single-shot conversions. The fastest way to do conversions is with continuous mode (see below)
    Serial.println("Single-shot conversion speeds. Value should be 1."); 
    for(auto average : averages_list) {
      adc->adc0->setAveraging(average); // set number of averages
      for (auto resolution : resolutions_list) {
        adc->adc0->setResolution(resolution); // set bits of resolution
        for (auto conv_speed : conversion_speed_list) {
            adc->adc0->setConversionSpeed(conv_speed); // change the conversion speed
            for (auto samp_speed: sampling_speed_list) {
                adc->adc0->setSamplingSpeed(samp_speed); // change the sampling speed
                
                adc->adc0->wait_for_cal();

                num_samples = (int)NUM_SAMPLES/average;

                timeElapsed = 0;
                value = 0;
                for(uint32_t i=0; i<num_samples; i++) {
                    value += adc->adc0->analogRead(readPin);
                }
                double time_us = (float)timeElapsed/num_samples;
                Serial.print("Average: "); Serial.print(average);
                Serial.print(", Resolution: "); Serial.print(resolution);
                Serial.print(", Conversion speed: "); Serial.print(getConversionEnumStr(conv_speed));
                Serial.print(", Sampling speed: "); Serial.print(getSamplingEnumStr(samp_speed));  
                Serial.print(". Time: "); Serial.print(time_us); Serial.print(" us.");
                Serial.print(" Value: "); Serial.println(value/num_samples*1.0/adc->adc0->getMaxValue(), 6);
              }
            }
        }
    }


    // Continuous mode
    Serial.println("Continuous mode conversion speeds. Value should be 1."); 
    adc->adc0->enableInterrupts(adc0_isr);
    for(auto average : averages_list) {
      adc->adc0->setAveraging(average); // set number of averages
      for (auto resolution : resolutions_list) {
        adc->adc0->setResolution(resolution); // set bits of resolution
        for (auto conv_speed : conversion_speed_list) {
            adc->adc0->setConversionSpeed(conv_speed); // change the conversion speed
            for (auto samp_speed: sampling_speed_list) {
                adc->adc0->setSamplingSpeed(samp_speed); // change the sampling speed
                
                adc->adc0->wait_for_cal();

                num_samples = NUM_SAMPLES/average;

                value = 0;
                num_iter = 0;
                timeElapsed = 0;
                adc->adc0->startContinuous(readPin);
                while (num_iter<num_samples) {}
                double time_us = (float)timeElapsed/num_samples;
                adc->adc0->stopContinuous();
                Serial.print("Average: "); Serial.print(average);
                Serial.print(", Resolution: "); Serial.print(resolution);
                Serial.print(", Conversion speed: "); Serial.print(getConversionEnumStr(conv_speed));
                Serial.print(", Sampling speed: "); Serial.print(getSamplingEnumStr(samp_speed));  
                Serial.print(". Time: "); Serial.print(time_us); Serial.print(" us.");
                Serial.print(" Value: "); Serial.println(value/num_samples*1.0/adc->adc0->getMaxValue(), 6);
              }
            }
        }
    }
}


void loop() {
    // Print errors, if any.
    if(adc->adc0->fail_flag != ADC_ERROR::CLEAR) {
      Serial.print("ADC0: "); Serial.println(getStringADCError(adc->adc0->fail_flag));
    }
    #ifdef ADC_DUAL_ADCS
    if(adc->adc1->fail_flag != ADC_ERROR::CLEAR) {
      Serial.print("ADC1: "); Serial.println(getStringADCError(adc->adc1->fail_flag));
    }
    #endif

    digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));

    delay(100);

}

void adc0_isr(void) {
    if(num_iter<num_samples) {
      value += (uint16_t)adc->adc0->analogReadContinuous();
      num_iter++;
    } else { // clear interrupt
      adc->adc0->analogReadContinuous();
    }
    //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));
}
