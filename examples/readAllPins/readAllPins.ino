/* Example for analogContinuousRead
*  It measures continuously the voltage on pin A9,
*  Write v and press enter on the serial console to get the value
*  Write c and press enter on the serial console to check that the conversion is taking place,
*  Write t to check if the voltage agrees with the comparison in the setup()
*  Write s to stop the conversion, you can restart it writing r.
*/

#include <ADC.h>
#include <ADC_util.h>

ADC *adc = new ADC(); // adc object

#if defined(ADC_TEENSY_LC) // teensy LC
#define PINS 13
#define PINS_DIFF 2
uint8_t adc_pins[] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12};
uint8_t adc_pins_diff[] = {A10, A11};

#elif defined(ADC_TEENSY_3_0) // teensy 3.0
#define PINS 14
#define PINS_DIFF 4
uint8_t adc_pins[] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13};
uint8_t adc_pins_diff[] = {A10, A11, A12, A13};

#elif defined(ADC_TEENSY_3_1) || defined(ADC_TEENSY_3_2) // teensy 3.1/3.2
#define PINS 21
#define PINS_DIFF 4
uint8_t adc_pins[] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13,
                      A14, A15, A16, A17, A18, A19, A20};
uint8_t adc_pins_diff[] = {A10, A11, A12, A13};

#elif defined(ADC_TEENSY_3_5) // Teensy 3.5
#define PINS 27
#define PINS_DIFF 2
uint8_t adc_pins[] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10,
                      A11, A12, A13, A14, A15, A16, A17, A18, A19, A20, A21, A22, A23, A24, A25, A26};
uint8_t adc_pins_diff[] = {A10, A11};

#elif defined(ADC_TEENSY_3_6) // Teensy 3.6
#define PINS 25
#define PINS_DIFF 2
uint8_t adc_pins[] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10,
                      A11, A12, A13, A14, A15, A16, A17, A18, A19, A20, A21, A22, A23, A24};
uint8_t adc_pins_diff[] = {A10, A11};

#elif defined(ADC_TEENSY_4_0) // Teensy 4.0
#define PINS 14
#define DIG_PINS 10
#define PINS_DIFF 0
uint8_t adc_pins[] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13};
uint8_t adc_pins_dig[] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9};
uint8_t adc_pins_diff[] = {};

#elif defined(ADC_TEENSY_4_1) // Teensy 4.1
#define PINS 18
#define DIG_PINS 10
#define PINS_DIFF 0
uint8_t adc_pins[] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, A12, A13, A14, A15, A16, A17};
uint8_t adc_pins_dig[] = {A0, A1, A2, A3, A4, A5, A6, A7, A8, A9};
uint8_t adc_pins_diff[] = {};
#endif // defined

void setup()
{

    pinMode(LED_BUILTIN, OUTPUT);

    for (int i = 0; i < PINS; i++)
    {
        pinMode(adc_pins[i], INPUT);
    }

    Serial.begin(9600);

    ///// ADC0 ////
    adc->adc0->setAveraging(16);                                    // set number of averages
    adc->adc0->setResolution(12);                                   // set bits of resolution
    adc->adc0->setConversionSpeed(ADC_CONVERSION_SPEED::MED_SPEED); // change the conversion speed
    adc->adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED);     // change the sampling speed

////// ADC1 /////
#ifdef ADC_DUAL_ADCS
    adc->adc1->setAveraging(16);                                    // set number of averages
    adc->adc1->setResolution(12);                                   // set bits of resolution
    adc->adc1->setConversionSpeed(ADC_CONVERSION_SPEED::MED_SPEED); // change the conversion speed
    adc->adc1->setSamplingSpeed(ADC_SAMPLING_SPEED::MED_SPEED);     // change the sampling speed
#endif

    delay(500);
}

int value = 0;
int pin = 0;

void loop()
{

    for (int i = 0; i < PINS; i++)
    {
        value = adc->analogRead(adc_pins[i]); // read a new value, will return ADC_ERROR_VALUE if the comparison is false.
        Serial.print("A");
        Serial.print(i);
        Serial.print(": ");
        Serial.print(value * 3.3 / adc->adc0->getMaxValue(), 2);
        Serial.print(". ");
        if (i == 9)
        {
            Serial.println();
        }
        else if (i == 11)
        {
            Serial.print("\t");
        }
        else if (i == 13)
        {
            Serial.print("\t");
        }
        else if (i == 22)
        {
            Serial.println();
        }
    }
    Serial.println();
#if ADC_DIFF_PAIRS > 0
    Serial.print("Differential pairs: ");
    for (int i = 0; i < PINS_DIFF; i += 2)
    {
        value = adc->analogReadDifferential(adc_pins_diff[i], adc_pins_diff[i + 1]); // read a new value, will return ADC_ERROR_VALUE if the comparison is false.
        Serial.print(i);
        Serial.print(": ");
        Serial.print(value * 3.3 / adc->adc0->getMaxValue(), 2);
        Serial.print(". ");
    }
    Serial.println();
#endif
    // the actual parameters for the temperature sensor depend on the board type and
    // on the actual batch. The printed value is only an approximation
    //Serial.print("Temperature sensor (approx.): ");
    //value = adc->analogRead(ADC_INTERNAL_SOURCE::TEMP_SENSOR); // read a new value, will return ADC_ERROR_VALUE if the comparison is false.
    //Serial.print(": ");
    //float volts = value*3.3/adc->adc0->getMaxValue();
    //Serial.print(25-(volts-0.72)/1.7*1000, 2); // slope is 1.6 for T3.0
    //Serial.println(" C.");

    // Print errors, if any.
    if (adc->adc0->fail_flag != ADC_ERROR::CLEAR)
    {
        Serial.print("ADC0: ");
        Serial.println(getStringADCError(adc->adc0->fail_flag));
    }
#ifdef ADC_DUAL_ADCS
    if (adc->adc1->fail_flag != ADC_ERROR::CLEAR)
    {
        Serial.print("ADC1: ");
        Serial.println(getStringADCError(adc->adc1->fail_flag));
    }
#endif
    adc->resetError();

    Serial.println();
    Serial.println();

    //digitalWriteFast(LED_BUILTIN, !digitalReadFast(LED_BUILTIN));

    delay(50);
}
