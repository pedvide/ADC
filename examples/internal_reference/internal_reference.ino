/* Example for the use of the internal voltage reference VREF
*   VREF can vary between 1.173 V and 1.225 V, so it can be adjusted by software.
*   This example shows how to find the optimum value for this adjustment (trim).
*   You need a good multimeter to measure the real voltage difference between AGND and 3.3V,
*   change voltage_target accordingly and run the program. The optimum trim value is printed at the end.
*   The bisection algorithm prints some diagnostics while running.
*   Additionally, the bandgap voltage is printed too, it should lie between 0.97 V and 1.03 V, with 1.00 V being the typical value.
*   Because of noise, the trim value is not accurate to 1 step, it fluctuates +- 1 or 2 steps.
*/

#ifdef ADC_USE_INTERNAL_VREF

#include <ADC.h>
#include <VREF.h>

ADC* adc = new ADC();

//! change this value to your real input value, measured between AGND and 3.3V
float voltage_target = 3.29;

//! change the TOL (tolerance, minimum difference  in mV between voltage and target that matters)
//! to refine even more, but not to less than 0.5 mV
const float TOL = 2.0;
// Maximum iterations of the algorithm, no need to change it.
const uint8_t MAX_ITER = 100;


float average = 0;
const int NUM_AVGS = 100;
// Get the voltage of VREF using the trim value
float get_voltage(uint8_t trim) {
    average = 0;
    VREF::trim(trim);
    VREF::waitUntilStable();
    delay(50);
    for(int i=0; i<NUM_AVGS; i++) {
        average += 1.195/adc->analogRead(ADC_INTERNAL_SOURCE::VREF_OUT)*(adc->adc0->getMaxValue());
    }
    return average/NUM_AVGS;
}

// Simple bisection method to get the optimum trim value
// This method is not the bests for noisy functions...
// Electrical datasheet: "The Voltage Reference (VREF) is intended to supply an accurate voltage output
// that can be trimmed in 0.5 mV steps."
int8_t optimumTrimValue(float voltage_target) {
    // https://en.wikipedia.org/wiki/Bisection_method#Algorithm
    // INPUT: Function f, endpoint values a, b, tolerance TOL, maximum iterations NMAX
    // CONDITIONS: a < b, either f(a) < 0 and f(b) > 0 or f(a) > 0 and f(b) < 0
    // OUTPUT: value which differs from a root of f(x)=0 by less than TOL
    //
    // N ← 1
    // While N ≤ NMAX # limit iterations to prevent infinite loop
    //   c ← (a + b)/2 # new midpoint
    //   If f(c) = 0 or (b – a)/2 < TOL then # solution found
    //     Output(c)
    //     Stop
    //   EndIf
    //   N ← N + 1 # increment step counter
    //   If sign(f(c)) = sign(f(a)) then a ← c else b ← c # new interval
    // EndWhile
    // Output("Method failed.") # max number of steps exceeded
    const uint8_t MAX_VREF_trim = 63;
    const uint8_t MIN_VREF_trim = 0;

    uint8_t niter = 0;
    uint8_t a = MIN_VREF_trim, b = MAX_VREF_trim, midpoint = (a + b)/2;
    float cur_diff, diff_a;

    // start VREF
    VREF::start(VREF_SC_MODE_LV_HIGHPOWERBUF, midpoint);

    Serial.println("niter,\ta,\tb,\tmidpoint,\tdiff (mV)");

    while (niter < MAX_ITER) {
        midpoint = (a + b)/2;
        cur_diff = (get_voltage(midpoint) - voltage_target)*1000;

        Serial.print(niter, DEC); Serial.print(",\t");
        Serial.print(a, DEC); Serial.print(",\t");
        Serial.print(b, DEC); Serial.print(",\t");
        Serial.print(midpoint, DEC); Serial.print(",\t\t");
        Serial.println(cur_diff, 2);

        if (abs(cur_diff) <= TOL || b-a < 1) {
            return midpoint;
        }
        niter++;
        diff_a = get_voltage(a) - voltage_target;
        if ((cur_diff < 0 && diff_a < 0) || (cur_diff > 0 && diff_a > 0)) {
            a = midpoint;
        }
        else {
            b = midpoint;
        }

    }
    return -1;
}

void setup() {
    Serial.begin(9600);

    // Best measurement conditions
    adc->adc0->setAveraging(32);
    adc->adc0->setResolution(16);
    adc->adc0->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_LOW_SPEED);
    adc->adc0->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_LOW_SPEED);
    #ifdef ADC_DUAL_ADCS
    adc->adc1->setAveraging(32);
    adc->adc1->setResolution(16);
    adc->adc1->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_LOW_SPEED);
    adc->adc1->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_LOW_SPEED);
    #endif // ADC_DUAL_ADCS

    delay(2000);

    int8_t VREF_trim = optimumTrimValue(voltage_target);
    Serial.print("Optimal trim value: "); Serial.println(VREF_trim);

    VREF::start(VREF_SC_MODE_LV_HIGHPOWERBUF, VREF_trim);
    VREF::waitUntilStable();

    Serial.print("3.3V pin value: ");
    Serial.print(1.195/adc->adc0->analogRead(ADC_INTERNAL_SOURCE::VREF_OUT)*adc->adc0->getMaxValue(), 5);
    Serial.println(" V.");


    Serial.print("Bandgap value: ");
    //Serial.print(3.3*adc->adc0->analogRead(ADC_INTERNAL_SOURCE::BANDGAP)/adc->adc0->getMaxValue(), 5);
    adc->adc0->analogRead(ADC_INTERNAL_SOURCE::BANDGAP);
    adc->adc0->differentialMode(); // Use differential mode for better precision.
    Serial.print(voltage_target*adc->adc0->readSingle()/adc->adc0->getMaxValue(), 5);
    Serial.println(" V. (Should be between 0.97 and 1.03 V.)");

    // VREF::stop(); // you can stop it to save power.
}


void loop() {

    // If you now run your teensy from a battery you can check the charge by looking at the 3.3V pin voltage
    // it should be 3.3V, but it will fall as the battery discharges.
    Serial.print("3.3V pin value: ");
    Serial.print(1.195/adc->adc0->analogRead(ADC_INTERNAL_SOURCE::VREF_OUT)*adc->adc0->getMaxValue(), 5);
    Serial.println(" V.");

    delay(3000);
}

/*

My output with a Teensy 3.6 connected to my laptop's USB.

With a cheap multimeter I measured voltage_target = 3.29 V, the results are:

niter,	a,	b,	midpoint,	diff (mV)
0,  	0,	63,	31,	    	24.26
1,  	31,	63,	47,	    	2.54
2,	    47,	63,	55,	    	-7.90
3,  	47,	55,	51,	    	-2.58
4,  	47,	51,	49,	    	0.03
Optimal trim value: 49
VREF value: 3.29024 V.
Bandgap value: 0.99948 V.




For Teensy 3.5 connected to my laptop's USB.
voltage_target = 3.28 V, the results are:

niter,	a,	b,	midpoint,	diff (mV)
0,  	0,	63,	31,	    	21.97
1,	    31,	63,	47,	    	0.27
Optimal trim value: 47
VREF value: 3.28154 V.
Bandgap value: 1.00387 V.




My output with a (quite old) Teensy 3.0 connected to my laptop's USB.
voltage_target = 3.22 V, the results are:

niter,	a,	b,	midpoint,	diff (mV)
0,  	0,	63,	31,		    37.34
1,  	31,	63,	47,	    	16.56
2,	    47,	63,	55,	    	5.54
3,	    55,	63,	59,	    	-0.00
Optimal trim value: 59
VREF value: 3.21947 V.
Bandgap value: 1.01978 V.


*/

#else // make sure the example can run for any boards (automated testing)
void setup() {}
void loop() {}
#endif // ADC_USE_INTERNAL_VREF