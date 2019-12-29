/* Test suite for the ADC library
*  Do not connect any pin of the board to anything while it's running
*  It uses the internal pulldown and pullup resistors to expose the pins to VDD or GND.
*/

#include <ADC.h>
#include <ADC_util.h>

ADC *adc = new ADC(); // adc object

#if defined(ADC_TEENSY_LC) // teensy LC
#define PINS 13
#define DIG_ADC_0_PINS 13
#define DIG_ADC_1_PINS 0
#define PINS_DIFF 2
uint8_t adc_pins[] = {A0,A1,A2,A3,A4,A5,A6,A7,A8, A9,A10,A11, A12};
uint8_t adc_pins_dig_ADC_0[] = {A0,A1,A2,A3,A4,A5,A6,A7,A8, A9,A10,A11, A12};
uint8_t adc_pins_dig_ADC_1[] = {};
uint8_t adc_pins_diff[] = {A10, A11};

#elif defined(ADC_TEENSY_3_0)  // teensy 3.0
#define PINS 14
#define DIG_ADC_0_PINS 10
#define DIG_ADC_1_PINS 0
#define PINS_DIFF 4
uint8_t adc_pins[] = {A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13};
uint8_t adc_pins_dig_ADC_0[] = {A0,A1,A2,A3,A4,A5,A6,A7,A8,A9};
uint8_t adc_pins_dig_ADC_1[] = {};
uint8_t adc_pins_diff[] = {A10, A11, A12, A13};

#elif defined(ADC_TEENSY_3_1) || defined(ADC_TEENSY_3_2)  // teensy 3.1/3.2
#define PINS 21
#define DIG_ADC_0_PINS 10
#define DIG_ADC_1_PINS 8
#define PINS_DIFF 4
uint8_t adc_pins_dig_ADC_0[] = {A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,};
uint8_t adc_pins_dig_ADC_1[] = {A2,A3, A15,A16,A17,A18,A19,A20 };
uint8_t adc_pins_diff[] = {A10, A11, A12, A13};

#elif defined(ADC_TEENSY_3_5)  // Teensy 3.5
#define PINS 27
#define DIG_ADC_0_PINS 12
#define DIG_ADC_1_PINS 11
#define PINS_DIFF 2
uint8_t adc_pins_dig_ADC_0[] = {A0,A1,A2,A3,A4,A5,A6,A7,A8, A9, 
                                A14,A15};
uint8_t adc_pins_dig_ADC_1[] = {A2,A3, A12,A13, A16,A17,A18,A19, A20,A23,A24};
uint8_t adc_pins_diff[] = {A10, A11};

#elif defined(ADC_TEENSY_3_6)  // Teensy 3.6
#define PINS 25
#define DIG_ADC_0_PINS 12
#define DIG_ADC_1_PINS 11
#define PINS_DIFF 2
uint8_t adc_pins_dig_ADC_0[] = {A0,A1,A2,A3,A4,A5,A6,A7,A8, A9, 
                                A14,A15};
uint8_t adc_pins_dig_ADC_1[] = {A2,A3, A12,A13, A16,A17,A18,A19, A20,A23,A24};
uint8_t adc_pins_diff[] = {A10, A11};

#elif defined(ADC_TEENSY_4)  // Teensy 4
#define PINS 14
#define DIG_ADC_0_PINS 10
#define DIG_ADC_1_PINS 10
#define PINS_DIFF 0
uint8_t adc_pins_dig_ADC_0[] = {A0,A1,A2,A3,A4,A5,A6,A7,A8, A9};
uint8_t adc_pins_dig_ADC_1[] = {A0,A1,A2,A3,A4,A5,A6,A7,A8, A9};
uint8_t adc_pins_diff[] = {};
#endif // defined


long int value = 0;

#ifdef ADC_TEENSY_4
const uint32_t num_samples = 10000;
#else
const uint32_t num_samples = 100;
#endif

// test the analog pins that also have a digital function
// Teensy LC has 20-50 kOhm pullup, no idea about pulldown (do they exist?)
// Teensy 3.0 has 22-50 kOhm pullup and pulldown
// Teensy 3.1/3.2 has 20-50 kOhm pullup and pulldown
// Teensy 3.5 has 20-50 kOhm pullup and pulldown
// Teensy 3.6 has 20-50 kOhm pullup and pulldown
// Teensy 4 has 100 kOhm pulldow, and 22 kOhm pullup, so pulldown will fail faster
bool test_pullup_down(bool pullup, bool debug=false) {
    uint8_t mode = pullup ? INPUT_PULLUP : INPUT_PULLDOWN;

    const int max_val = adc->getMaxValue(ADC_0);

    bool pass_test = true;

    for (auto pin : adc_pins_dig_ADC_0) {
      pinMode(pin, mode);
      delay(10); // settle time

        value = adc->adc0->analogRead(pin);
        bool fail_condition = pullup ? (value < 0.95*max_val) : (value > 0.05*max_val);
        if (fail_condition) {
          if (debug) {
            Serial.print("Pin "); Serial.print(pin); Serial.print(": ");
            Serial.print("FAILED "); Serial.print(pullup ? "INPUT_PULLUP" : "INPUT_PULLDOWN");
            Serial.print("("); Serial.print(value); Serial.print(")");
            Serial.print("max_val: "); Serial.print(pullup ? (0.95*max_val) : (0.05*max_val));
            Serial.println(". ");
          }
            pass_test = false;
        }
    }
    #if ADC_NUM_ADCS>1
    for (auto pin : adc_pins_dig_ADC_1) {
      pinMode(pin, mode);
      delay(10); // settle time

        value = adc->adc1->analogRead(pin);
        bool fail_condition = pullup ? (value < 0.95*max_val) : (value > 0.05*max_val);
        if (fail_condition) {
          if (debug) {
            Serial.print("Pin "); Serial.print(pin); Serial.print(": ");
            Serial.print("FAILED "); Serial.print(pullup ? "INPUT_PULLUP" : "INPUT_PULLDOWN");
            Serial.print("("); Serial.print(value); Serial.print(")");
            Serial.print("max_val: "); Serial.print(pullup ? (0.95*max_val) : (0.05*max_val));
            Serial.println(". ");
          }
            pass_test = false;
        }
    }
    #endif

    return pass_test;
}

const uint8_t pin_cmp = A0;

bool test_compare(bool debug=false) {
    bool pass_test = true;

    // measurement will be ready if value < 1.0V
    adc->enableCompare((int)(1.0/3.3*adc->getMaxValue(ADC_0)), 0, ADC_0);

    pinMode(pin_cmp, INPUT_PULLUP); // set to max
    delay(10);
    // this should fail
    value = adc->analogRead(pin_cmp, ADC_0);
    if (adc->adc0->fail_flag != ADC_ERROR::COMPARISON) {
      if (debug) {
        Serial.println("Comparison didn't fail.");
        Serial.println(getStringADCError(adc->adc0->fail_flag));
      }
        pass_test = false;
    }
    adc->adc0->resetError();

    pinMode(pin_cmp, INPUT_PULLDOWN); // set to min
    delay(50);
    // this should be ok
    value = adc->analogRead(pin_cmp, ADC_0);
    if(adc->adc0->fail_flag == ADC_ERROR::COMPARISON) {
      if (debug) {
        Serial.println("Comparison didn't succeed.");
        Serial.println(getStringADCError(adc->adc0->fail_flag));
        Serial.println(value);
      }
        pass_test = false;
    }
    adc->adc0->resetError();

    adc->adc0->disableCompare();

    return pass_test;
}

bool test_compare_range(bool debug=false) {
    bool pass_test = true;

    // ready if value lies out of [1.0,2.0] V
    adc->enableCompareRange(1.0*adc->getMaxValue(ADC_0)/3.3, 2.0*adc->getMaxValue(ADC_0)/3.3, 0, 1, ADC_0);


    pinMode(pin_cmp, INPUT_PULLUP); // set to max
    // this should be ok
    value = adc->analogRead(pin_cmp, ADC_0);
    if(adc->adc0->fail_flag != ADC_ERROR::CLEAR) {
      if (debug) {
        Serial.println("Some other error happened when comparison should have succeeded.");
        Serial.println(getStringADCError(adc->adc0->fail_flag));
      }
        pass_test = false;
    }

    pinMode(pin_cmp, INPUT_PULLDOWN); // set to min
    adc->adc0->resetError();
    // this should be ok
    value = adc->analogRead(pin_cmp, ADC_0);
    if(adc->adc0->fail_flag != ADC_ERROR::CLEAR) {
      if (debug) {
        Serial.println("Some other error happened when comparison should have succeeded.");
        Serial.println(getStringADCError(adc->adc0->fail_flag));
      }
        pass_test = false;
    }
    adc->adc0->resetError();

    adc->adc0->disableCompare();

    return pass_test;
}


bool test_averages(bool debug=false) {
    elapsedMicros timeElapsed;
    bool pass_test = true;
    const uint8_t averages[] = {4, 8, 16, 32};
    float avg_times[4];
    volatile int value = 0;

    adc->setAveraging(1);
    timeElapsed = 0;
    for(uint32_t i=0; i<num_samples; i++) {
        value += adc->analogRead(A0, ADC_0);
    }
    float one_avg_time = (float)timeElapsed/num_samples;
//    Serial.print("1: "); Serial.println(one_avg_time);
//    Serial.print("value: "); Serial.println(value/num_samples);


    for(uint8_t i=0; i<4; i++) {
        adc->setAveraging(averages[i]);
        timeElapsed = 0;
        value = 0;
        for(uint32_t j=0; j<num_samples; j++) {
            value += adc->analogRead(A0, ADC_0);
        }
        float time = (float)timeElapsed/num_samples;
        avg_times[i] = time;
//        Serial.print(averages[i]); Serial.print(": "); Serial.print(time);
//        Serial.print(", "); Serial.println(time/one_avg_time);
//        Serial.print("value: "); Serial.println(value/num_samples);
    }

    // the 4 averages is not 4 times as long as the 1 average because
    // the first sample always takes longer, therefore the 4 avgs take about 3.5 times the first.
    // 8 avgs take twice as long as 4 and so on.
    // this is even worse for Teensy LC, where 4 avgs take about 2.6 more than none.
    if((avg_times[0] < 2*one_avg_time) || (avg_times[0] > 4*one_avg_time)) {
        pass_test = false;
        if (debug) {
          Serial.print("4 averages should take about 4 times longer than one, but it took ");
          Serial.print(avg_times[0]/one_avg_time); Serial.println(" times longer");
        }
    }
    // check that the times are between 80% and 120% their theoretical value.
    for(uint8_t i=1; i<4; i++) {
        if((avg_times[i] < 2*0.8*avg_times[i-1]) || (avg_times[i] > 2*1.2*avg_times[i-1])) {
            pass_test = false;
            if (debug) {
              Serial.print(averages[i]); Serial.print(" averages should take about twice as long as "); Serial.print(averages[i-1]);
              Serial.print(", but it took "); Serial.print(avg_times[i]/avg_times[i-1]); Serial.println(" times longer.");
            }
        }
    }

    return pass_test;
}


bool test_all_combinations(bool debug=false) {
    bool pass_test = true;

    for(auto average : averages_list) {
      adc->setAveraging(average, ADC_0); // set number of averages
      adc->setAveraging(average, ADC_1); // set number of averages
      for (auto resolution : resolutions_list) {
        adc->setResolution(resolution, ADC_0); // set bits of resolution
        adc->setResolution(resolution, ADC_1); // set bits of resolution
        for (auto conv_speed : conversion_speed_list) {
            adc->setConversionSpeed(conv_speed, ADC_0); // change the conversion speed
            adc->setConversionSpeed(conv_speed, ADC_1); // change the conversion speed
            for (auto samp_speed: sampling_speed_list) {
              adc->setSamplingSpeed(samp_speed, ADC_0); // change the sampling speed
              adc->setSamplingSpeed(samp_speed, ADC_1); // change the sampling speed
              
              adc->adc0->wait_for_cal();
              #if ADC_NUM_ADCS>1
              adc->adc1->wait_for_cal();
              #endif

              bool test = test_pullup_down(true, false); // pullups have lower impedance for T4
              if (!test) {
                if (debug) {
                  Serial.print("Average: "); Serial.print(average);
                  Serial.print(", Resolution: "); Serial.print(resolution);
                  Serial.print(", Conversion speed: "); Serial.print(getConversionEnumStr(conv_speed));
                  Serial.print(", Sampling speed: "); Serial.print(getSamplingEnumStr(samp_speed));  
                  Serial.println(". PULLUP FAILED.");
                }
                // VERY_HIGH_SPEED are not guaranteed to work
                if ((conv_speed != ADC_CONVERSION_SPEED::VERY_HIGH_SPEED) || (samp_speed != ADC_SAMPLING_SPEED::VERY_HIGH_SPEED)) {
                  pass_test = false;
                }
                
              }
              
              digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));
   
            }
        }

      }
    }
    return pass_test;
}

void resetSettings() {
  adc->setAveraging(16); // set number of averages
  adc->setResolution(12); // set bits of resolution
  adc->setConversionSpeed(ADC_CONVERSION_SPEED::MED_SPEED); // change the conversion speed
  adc->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED); // change the sampling speed

  #if ADC_NUM_ADCS>1
  adc->setAveraging(16, ADC_1); // set number of averages
  adc->setResolution(12, ADC_1); // set bits of resolution
  adc->setConversionSpeed(ADC_CONVERSION_SPEED::MED_SPEED, ADC_1); // change the conversion speed
  adc->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED, ADC_1); // change the sampling speed
  adc->adc1->recalibrate();
  #endif
  
}

void setup() {

    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(9600);

    ///// ADC0 ////
    // reference can be ADC_REFERENCE::REF_3V3, ADC_REFERENCE::REF_1V2 (not for Teensy LC) or ADC_REFERENCE::REF_EXT.
    //adc->setReference(ADC_REFERENCE::REF_1V2, ADC_0); // change all 3.3 to 1.2 if you change the reference to 1V2

    adc->setAveraging(16); // set number of averages
    adc->setResolution(12); // set bits of resolution
    adc->setConversionSpeed(ADC_CONVERSION_SPEED::MED_SPEED); // change the conversion speed
    adc->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED); // change the sampling speed
    adc->adc0->recalibrate();

    ////// ADC1 /////
    #if ADC_NUM_ADCS>1
    adc->setAveraging(16, ADC_1); // set number of averages
    adc->setResolution(12, ADC_1); // set bits of resolution
    adc->setConversionSpeed(ADC_CONVERSION_SPEED::MED_SPEED, ADC_1); // change the conversion speed
    adc->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED, ADC_1); // change the sampling speed
    adc->adc1->recalibrate();
    #endif

    delay(2000);

    ////// START TESTS /////////////
    // do the tests
       
    bool pullup_test = test_pullup_down(true, false);
    resetSettings();
    Serial.print("PULLUP TEST "); Serial.println(pullup_test ? "PASS" : "FAIL");
    bool pulldown_test = test_pullup_down(false, false);
    resetSettings();
    Serial.print("PULLDOWN TEST "); Serial.println(pulldown_test ? "PASS" : "FAIL");
    bool compare_test = test_compare(true);
    resetSettings();
    Serial.print("COMPARE TEST "); Serial.println(compare_test ? "PASS" : "FAIL");
    bool compare_range_test = test_compare_range(true);
    resetSettings();
    Serial.print("COMPARE RANGE TEST "); Serial.println(compare_range_test ? "PASS" : "FAIL");
    bool averages_test = test_averages(true);
    resetSettings();
    Serial.print("AVERAGES TEST "); Serial.println(averages_test ? "PASS" : "FAIL");
    bool all_combinations_test = test_all_combinations(true);
    resetSettings();
    Serial.print("ALL COMBINATIONS TEST "); Serial.println(all_combinations_test ? "PASS" : "FAIL");
    if (pullup_test & pulldown_test & compare_test & compare_range_test & averages_test & all_combinations_test) {
    Serial.println("ALL TEST PASSED."); }
    

    
}


void loop() {
    // Print errors, if any.
    if(adc->adc0->fail_flag != ADC_ERROR::CLEAR) {
      Serial.print("ADC0: "); Serial.println(getStringADCError(adc->adc0->fail_flag));
    }
    #if ADC_NUM_ADCS > 1
    if(adc->adc1->fail_flag != ADC_ERROR::CLEAR) {
      Serial.print("ADC1: "); Serial.println(getStringADCError(adc->adc1->fail_flag));
    }
    #endif
    adc->resetError();


    digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));

    delay(500);

}
