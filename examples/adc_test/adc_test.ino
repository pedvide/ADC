/* Test suite for the ADC library
*/

#include <ADC.h>

ADC *adc = new ADC(); // adc object

#if defined(ADC_TEENSY_LC) // teensy LC
#define PINS 13
#define DIG_PINS 13
#define PINS_DIFF 2
uint8_t adc_pins[] = {A0,A1,A2,A3,A4,A5,A6,A7,A8, A9,A10,A11, A12};
uint8_t adc_pins_dig[] = {A0,A1,A2,A3,A4,A5,A6,A7,A8, A9,A10,A11, A12};
uint8_t adc_pins_diff[] = {A10, A11};

#elif defined(ADC_TEENSY_3_0)  // teensy 3.0
#define PINS 14
#define DIG_PINS 10
#define PINS_DIFF 4
uint8_t adc_pins[] = {A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13};
uint8_t adc_pins_dig[] = {A0,A1,A2,A3,A4,A5,A6,A7,A8,A9};
uint8_t adc_pins_diff[] = {A10, A11, A12, A13};

#elif defined(ADC_TEENSY_3_1) || defined(ADC_TEENSY_3_2)  // teensy 3.1/3.2
#define PINS 21
#define DIG_PINS 16
#define PINS_DIFF 4
uint8_t adc_pins[] = {A0,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,
                      A14, A15,A16,A17,A18,A19,A20 };
uint8_t adc_pins_dig[] = {A0,A1,A2,A3,A4,A5,A6,A7,A8,A9, A15,A16,A17,A18,A19,A20 };
uint8_t adc_pins_diff[] = {A10, A11, A12, A13};

#elif defined(ADC_TEENSY_3_5)  // Teensy 3.5
#define PINS 27
#define DIG_PINS 21
#define PINS_DIFF 2
uint8_t adc_pins[] = {A0,A1,A2,A3,A4,A5,A6,A7,A8, A9, A10,
                      A11,A12,A13,A14,A15,A16,A17,A18,A19,A20,A21,A22,A23,A24, A25, A26};
uint8_t adc_pins_dig[] = {A0,A1,A2,A3,A4,A5,A6,A7,A8, A9, A12,A13,A14,A15,A16,A17,A18,A19,A20,A23,A24};
uint8_t adc_pins_diff[] = {A10, A11};

#elif defined(ADC_TEENSY_3_6)  // Teensy 3.6
#define PINS 25
#define DIG_PINS 21
#define PINS_DIFF 2
uint8_t adc_pins[] = {A0,A1,A2,A3,A4,A5,A6,A7,A8, A9, A10,
                      A11,A12,A13,A14,A15,A16,A17,A18,A19,A20,A21,A22,A23,A24};
uint8_t adc_pins_dig[] = {A0,A1,A2,A3,A4,A5,A6,A7,A8, A9, A12,A13,A14,A15,A16,A17,A18,A19,A20,A23,A24};
uint8_t adc_pins_diff[] = {A10, A11};
#endif // defined


int value = 0;

// test the analog pins that also have a digital function
bool test_pullup_down(bool pullup) {
    uint8_t mode = pullup ? INPUT_PULLUP : INPUT_PULLDOWN;

    const int max_val = adc->getMaxValue(ADC_0);

    bool pass_test = true;

    for (int i=0;i<DIG_PINS;i++) {
        pinMode(adc_pins_dig[i], mode);
        delay(50); // settle time
        value = adc->analogRead(adc_pins_dig[i]);
        bool fail_condition = pullup ? (value < 0.95*max_val) : (value > 0.05*max_val);
        if (fail_condition) {
            Serial.print("A"); Serial.print(i); Serial.print(": ");
            Serial.print("FAILED "); Serial.print(pullup ? "INPUT_PULLUP" : "INPUT_PULLDOWN");
            Serial.print("("); Serial.print(value); Serial.print(")");
            Serial.println(". ");
            pass_test = false;
        }
    }

    return pass_test;
}

const uint8_t pin_cmp = A0;

bool test_compare() {
    bool pass_test = true;

    // measurement will be ready if value < 1.0V
    adc->enableCompare(1.0/3.3*adc->getMaxValue(ADC_0), 0, ADC_0);

    pinMode(pin_cmp, INPUT_PULLUP); // set to max
    // this should fail
    value = adc->analogRead(pin_cmp, ADC_0);
    if (adc->adc0->fail_flag != ADC_ERROR::COMPARISON) {
        Serial.println("Comparison didn't fail.");
        adc->adc0->printError();
        pass_test = false;
    }
    adc->adc0->resetError();

    pinMode(pin_cmp, INPUT_PULLDOWN); // set to min
    // this should be ok
    value = adc->analogRead(pin_cmp, ADC_0);
    if(adc->adc0->fail_flag != ADC_ERROR::CLEAR) {
        Serial.println("Some other error happened when comparison should have succeeded.");
        adc->adc0->printError();
        pass_test = false;
    }
    adc->adc0->resetError();

    return pass_test;
}

bool test_compare_range() {
    bool pass_test = true;

    // ready if value lies out of [1.0,2.0] V
    adc->enableCompareRange(1.0*adc->getMaxValue(ADC_0)/3.3, 2.0*adc->getMaxValue(ADC_0)/3.3, 0, 1, ADC_0);


    pinMode(pin_cmp, INPUT_PULLUP); // set to max
    // this should be ok
    value = adc->analogRead(pin_cmp, ADC_0);
    if(adc->adc0->fail_flag != ADC_ERROR::CLEAR) {
        Serial.println("Some other error happened when comparison should have succeeded.");
        adc->adc0->printError();
        pass_test = false;
    }

    pinMode(pin_cmp, INPUT_PULLDOWN); // set to min
    adc->adc0->resetError();
    // this should be ok
    value = adc->analogRead(pin_cmp, ADC_0);
    if(adc->adc0->fail_flag != ADC_ERROR::CLEAR) {
        Serial.println("Some other error happened when comparison should have succeeded.");
        adc->adc0->printError();
        pass_test = false;
    }
    adc->adc0->resetError();


    return pass_test;
}


bool test_averages() {
    elapsedMicros timeElapsed;
    bool pass_test = true;
    const uint32_t num_samples = 100;
    const uint8_t averages[] = {4, 8, 16, 32};
    float avg_times[4];

    adc->setAveraging(1);
    timeElapsed = 0;
    for(uint32_t i=0; i<num_samples; i++) {
        adc->analogRead(A0, ADC_0);
    }
    float one_avg_time = timeElapsed/num_samples;
    //Serial.print("1: "); Serial.println(one_avg_time);


    for(uint8_t i=0; i<4; i++) {
        adc->setAveraging(averages[i]);
        timeElapsed = 0;
        for(uint32_t j=0; j<num_samples; j++) {
            adc->analogRead(A0, ADC_0);
        }
        float time = (float)timeElapsed/num_samples;
        avg_times[i] = time;
        //Serial.print(averages[i]); Serial.print(": "); Serial.print(time);
        //Serial.print(", "); Serial.println(time/one_avg_time);
    }

    // the 4 averages is not 4 times as long as the 1 average because
    // the first sample always takes longer, therefore the 4 avgs take about 3.5 times the first.
    // 8 avgs take twice as long as 4 and so on.
    // this is even worse for Teensy LC, where 4 avgs take about 2.6 more than none.
    if((avg_times[0] < 2*one_avg_time) || (avg_times[0] > 4*one_avg_time)) {
        pass_test = false;
        Serial.print("4 averages should take about 4 times longer than one, but it took ");
        Serial.print(avg_times[0]/one_avg_time); Serial.println(" times longer");
    }
    // check that the times are between 90% and 110% their theoretical value.
    for(uint8_t i=1; i<4; i++) {
        if((avg_times[i] < 2*0.9*avg_times[i-1]) || (avg_times[i] > 2*1.1*avg_times[i-1])) {
            pass_test = false;
            Serial.print(averages[i]); Serial.print(" averages should take about twice as long as "); Serial.print(pow(2, i-1), 0);
            Serial.print(", but it took "); Serial.print(avg_times[i]/avg_times[i-1]); Serial.println(" times longer.");
        }
    }

    return pass_test;
}

void setup() {

    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(9600);


    ///// ADC0 ////
    // reference can be ADC_REFERENCE::REF_3V3, ADC_REFERENCE::REF_1V2 (not for Teensy LC) or ADC_REFERENCE::REF_EXT.
    //adc->setReference(ADC_REFERENCE::REF_1V2, ADC_0); // change all 3.3 to 1.2 if you change the reference to 1V2

    adc->setAveraging(16); // set number of averages
    adc->setResolution(16); // set bits of resolution

    // it can be any of the ADC_CONVERSION_SPEED enum: VERY_LOW_SPEED, LOW_SPEED, MED_SPEED, HIGH_SPEED_16BITS, HIGH_SPEED or VERY_HIGH_SPEED
    // see the documentation for more information
    // additionally the conversion speed can also be ADACK_2_4, ADACK_4_0, ADACK_5_2 and ADACK_6_2,
    // where the numbers are the frequency of the ADC clock in MHz and are independent on the bus speed.
    adc->setConversionSpeed(ADC_CONVERSION_SPEED::LOW_SPEED); // change the conversion speed
    // it can be any of the ADC_MED_SPEED enum: VERY_LOW_SPEED, LOW_SPEED, MED_SPEED, HIGH_SPEED or VERY_HIGH_SPEED
    adc->setSamplingSpeed(ADC_SAMPLING_SPEED::LOW_SPEED); // change the sampling speed

    // If you enable interrupts, notice that the isr will read the result, so that isComplete() will return false (most of the time)
    //adc->enableInterrupts(ADC_0);


    ////// ADC1 /////
    #if ADC_NUM_ADCS>1
    adc->setAveraging(16, ADC_1); // set number of averages
    adc->setResolution(16, ADC_1); // set bits of resolution
    adc->setConversionSpeed(ADC_CONVERSION_SPEED::LOW_SPEED, ADC_1); // change the conversion speed
    adc->setSamplingSpeed(ADC_SAMPLING_SPEED::LOW_SPEED, ADC_1); // change the sampling speed

    //adc->setReference(ADC_REFERENCE::REF_1V2, ADC_1);


    // If you enable interrupts, note that the isr will read the result, so that isComplete() will return false (most of the time)
    //adc->enableInterrupts(ADC_1);

    #endif

    adc->adc0->recalibrate();
    #if ADC_NUM_ADCS>1
    adc->adc1->recalibrate();
    #endif

    delay(2000);

    ////// START TESTS /////////////

    bool pullup_test = test_pullup_down(true);
    Serial.print("PULLUP TEST "); Serial.println(pullup_test ? "PASS" : "FAIL");
    bool pulldown_test = test_pullup_down(false);
    Serial.print("PULLDOWN TEST "); Serial.println(pulldown_test ? "PASS" : "FAIL");
    bool compare_test = test_compare();
    Serial.print("COMPARE TEST "); Serial.println(compare_test ? "PASS" : "FAIL");
    bool compare_range_test = test_compare_range();
    Serial.print("COMPARE RANGE TEST "); Serial.println(compare_range_test ? "PASS" : "FAIL");
    bool averages_test = test_averages();
    Serial.print("AVERAGES TEST "); Serial.println(averages_test ? "PASS" : "FAIL");
}




void loop() {
    // Print errors, if any.
    adc->printError();
    adc->resetError();


    digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));

    delay(500);

}
