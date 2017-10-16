/* Example for the use of the internal voltage reference VREF
*   VREF can vary between 1.173 V and 1.225 V, so it can be adjusted by software.
*   This example shows how to find the optimum value for this adjustment (trim).
*   You need a good multimeter to measure the real voltage difference between AGND and 3.3V,
*   change voltage_target accordingly and run the program. The optimum trim value is printed at the end.
*   The bisection algorithm prints some diagnostics while running.
*   Additionally, the bandgap voltage is printed too, it should lie between 0.97 V and 1.03 V, with 1.00 V being the typical value.
*/

#include <ADC.h>
#include <VREF.h>

ADC* adc = new ADC();

//! change this value to your real input value, measured between AGND and 3.3V
float voltage_target = 3.3;

//! change the TOL (tolerance, minimum difference between voltage and target that matters)
//! to refine even more, but not to less than 0.001 or 0.002
const float TOL = 0.002;
// Maximum iterations of the algorithm, no need to change it.
const uint8_t MAX_ITER = 100;


// Get the voltage of VREF using the trim value
float get_voltage(uint8_t trim) {
    VREF::trim(trim);
    VREF::waitUntilStable();
    return 1.20/adc->adc1->analogRead(ADC_INTERNAL_SOURCE::VREF_OUT)*(adc->getMaxValue(ADC_1));
}

// Simple bisection method to get the optimum trim value
// Electrical datasheet: "The Voltage Reference (VREF) is intended to supply an accurate voltage output
// that can be trimmed in 0.5 mV steps."
// But it seems that the resolution of the ADC doesn't allow such fine tuning, 1 mV is the minimum
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
        cur_diff = get_voltage(midpoint) - voltage_target;

        Serial.print(niter, DEC); Serial.print(",\t");
        Serial.print(a, DEC); Serial.print(",\t");
        Serial.print(b, DEC); Serial.print(",\t");
        Serial.print(midpoint, DEC); Serial.print(",\t\t");
        Serial.println(cur_diff*1000, 2);

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

    adc->setAveraging(32);
    adc->setResolution(16);
    adc->setConversionSpeed(ADC_CONVERSION_SPEED::VERY_LOW_SPEED);
    adc->setSamplingSpeed(ADC_SAMPLING_SPEED::VERY_LOW_SPEED);

    delay(2000);

    uint8_t VREF_trim = optimumTrimValue(voltage_target);
    Serial.print("Optimal trim value: "); Serial.println(VREF_trim);

    VREF::start(VREF_SC_MODE_LV_HIGHPOWERBUF, VREF_trim);
    VREF::waitUntilStable();

    Serial.print("VREF value: ");
    Serial.print(1.20/adc->adc1->analogRead(ADC_INTERNAL_SOURCE::VREF_OUT)*adc->getMaxValue(ADC_1), 5);
    Serial.println(" V.");


    Serial.print("Bandgap value: ");
    Serial.print(3.3*adc->adc0->analogRead(ADC_INTERNAL_SOURCE::BANDGAP)/adc->getMaxValue(ADC_0), 5);
    Serial.println(" V.");

    VREF::stop();
}


void loop() {}

/* My output with a Teensy 3.6 connected to my laptop's USB.

With a cheap multimeter I measured voltage_target = 3.29 V, the results are:

niter,	a,	b,	midpoint,	diff (mV)
0,		0,	63,	31,			27.84
1,		31,	63,	47,			10.00
2,		47,	63,	55,			1.15
Optimal trim value: 55
VREF value: 3.29115 V.
Bandgap value: 1.00271 V.


Using voltage_target = 3.3, which should be true:

niter,	a,	b,	midpoint,	diff (mV)
0,	    0,	63,	31,	    	26.83
1,  	31,	63,	47,	    	0.00
Optimal trim value: 47
VREF value: 3.30000 V.
Bandgap value: 1.00282 V.


*/
