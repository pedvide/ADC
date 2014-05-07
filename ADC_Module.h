 /* ADC Library created by Pedro Villanueva
 * It implements all functions of the Teensy 3.0 internal ADC
 * Some of the code was extracted from the file analog.c included in the distribution of Teensduino by PJRC.COM
 *
 */


#ifndef ADC_MODULE_H
#define ADC_MODULE_H

#include <stdint.h>
#include "core_pins.h"
#include "mk20dx128.h"
// include pow()
#include <math.h>

// include new and delete
#include <Arduino.h>

// the alternate clock is connected to OSCERCLK (16 MHz).
// datasheet says ADC clock should be 2 to 12 MHz for 16 bit mode
// datasheet says ADC clock should be 1 to 18 MHz for 8-12 bit mode
// calibration works best when averages are 32 and speed is less than 4 MHz
#if F_BUS == 48000000
  #define ADC_CFG1_3MHZ   ADC_CFG1_ADIV(3) + ADC_CFG1_ADICLK(1) // Clock divide select: 3=div8 + Input clock: 1=bus/2
  #define ADC_CFG1_6MHZ   ADC_CFG1_ADIV(2) + ADC_CFG1_ADICLK(1) // Clock divide select: 2=div4 + Input clock: 1=bus/2
  #define ADC_CFG1_12MHZ  ADC_CFG1_ADIV(1) + ADC_CFG1_ADICLK(1) // Clock divide select: 1=div2 Input clock: 1=bus/2
  #define ADC_CFG1_24MHZ  ADC_CFG1_ADIV(0) + ADC_CFG1_ADICLK(1) // Clock divide select: 0=direct + Input clock: 1=bus/2
#elif F_BUS == 24000000
  #define ADC_CFG1_3MHZ   ADC_CFG1_ADIV(3) + ADC_CFG1_ADICLK(0) // Clock divide select: 3=div8 + Input clock: 1=bus
  #define ADC_CFG1_6MHZ   ADC_CFG1_ADIV(2) + ADC_CFG1_ADICLK(0) // Clock divide select: 2=div4 + Input clock: 0=bus
  #define ADC_CFG1_12MHZ  ADC_CFG1_ADIV(1) + ADC_CFG1_ADICLK(0) // Clock divide select: 1=div2 + Input clock: 0=bus
  #define ADC_CFG1_24MHZ  ADC_CFG1_ADIV(0) + ADC_CFG1_ADICLK(0) // Clock divide select: 0=direct + Input clock: 0=bus
#else
#error
#endif

// VREF options that don't appear on mk20dx128.h
#define VREF_TRM_CHOPEN (uint8_t)0x40 // Chop oscillator enable

#define VREF_SC_VREFEN (uint8_t)0x80 // Internal Voltage Reference enable
#define VREF_SC_REGEN (uint8_t)0x40 // Regulator enable
#define VREF_SC_ICOMPEN (uint8_t)0x20 // Second order curvature compensation enable
#define VREF_SC_VREFST (uint8_t)0x04 // Internal Voltage Reference stable flag
#define VREF_SC_MODE_LV(n) (uint8_t)(((n) & 3) << 0) // Buffer Mode selection: 0=Bandgap on only, 1=High power buffer mode,
                                                     // 2=Low-power buffer mode


// ADCx_SC2[REFSEL] bit selects the voltage reference sources for ADC.
//   VREFH/VREFL - connected as the primary reference option
//   1.2 V VREF_OUT - connected as the VALT reference option
#define ADC_REF_DEFAULT         0
#define ADC_REF_EXTERNAL        0
#define ADC_REF_INTERNAL        2
#define ADC_REF_INTERNAL1V2     2
#define ADC_REF_INTERNAL1V1     2


// Error codes for analogRead and analogReadDifferential
#define ADC_ERROR_DIFF_VALUE -70000
#define ADC_ERROR_VALUE ADC_ERROR_DIFF_VALUE

// Other things to measure with the ADC that don't use external pins
// In my Teensy I read 1.22 V for the ADC_VREF_OUT, random values for ADC_BANDGAP,
// 3.3 V for ADC_VREFH and 0.0 V for ADC_VREFL.
#define ADC_TEMP_SENSOR 38
#define ADC_VREF_OUT 39
#define ADC_BANDGAP 41
#define ADC_VREFH 42
#define ADC_VREFL 43


// debug mode: blink the led light
#define ADC_debug 0


/** Class ADC_Module: Implements all functions of the Teensy 3.0 analog to digital converter
*
*/
class ADC_Module
{
    public:

        /** Constructor
        *
        */
        ADC_Module(uint8_t ADC_number = 0);

        /** Destructor
        *
        */
        ~ADC_Module();


        //! Set the voltage reference you prefer, default is vcc
        /*!
        * \param type can be DEFAULT, EXTERNAL or INTERNAL.
        * It needs to recalibrate.
        */
        void setReference(uint8_t type);


        //! Change the resolution of the measurement.
        /*!
        *  \param bits is the number of bits of resolution.
        *  For single-ended measurements: 8, 10, 12 or 16 bits.
        *  For differential measurements: 9, 11, 13 or 16 bits.
        *  If you want something in between (11 bits single-ended for example) select the inmediate higher
        *  and shift the result one to the right.
        *  If you select, for example, 9 bits and then do a single-ended reading, the resolution will be adjusted to 8 bits
        *  In this case the comparison values will still be correct for analogRead and analogReadDifferential, but not
        *  for startSingle* or startContinous*, so whenever you change the resolution, change also the comparison values.
        */
        void setResolution(uint8_t bits);

        //! Returns the resolution of the ADC_Module.
        uint8_t getResolution();

        //! Returns the maximum value for a measurement.
        uint32_t getMaxValue();


        //! Set the number of averages
        /*!
        * \param num can be 0, 4, 8, 16 or 32.
        */
        void setAveraging(uint8_t num);


        //! Enable interrupts
        /** An IRQ_ADC0 Interrupt will be raised when the conversion is completed
        *  (including hardware averages and if the comparison (if any) is true).
        */
        void enableInterrupts();

        //! Disable interrupts
        void disableInterrupts();


        //! Enable DMA request
        /** An ADC DMA request will be raised when the conversion is completed
        *  (including hardware averages and if the comparison (if any) is true).
        */
        void enableDMA();

        //! Disable ADC DMA request
        void disableDMA();


        //! Enable the compare function to a single value
        /** A conversion will be completed only when the ADC value
        *  is >= compValue (greaterThan=1) or < compValue (greaterThan=0)
        *  Call it after changing the resolution
        *  Use with interrupts or poll conversion completion with isComplete()
        */
        void enableCompare(int16_t compValue, int greaterThan);

        //! Enable the compare function to a range
        /** A conversion will be completed only when the ADC value is inside (insideRange=1) or outside (=0)
        *  the range given by (lowerLimit, upperLimit),including (inclusive=1) the limits or not (inclusive=0).
        *  See Table 31-78, p. 617 of the freescale manual.
        *  Call it after changing the resolution
        *  Use with interrupts or poll conversion completion with isComplete()
        */
        void enableCompareRange(int16_t lowerLimit, int16_t upperLimit, int insideRange, int inclusive);

        //! Disable the compare function
        void disableCompare();


        //! Enable and set PGA
        /** Enables the PGA and sets the gain
        *   Use only for signals lower than 1.2 V
        *   \param gain From 0 to 6, set PGA to 2^gain
        *
        */
        void enablePGA(uint8_t gain);
        void setPGA(uint8_t gain) {enablePGA(gain);}

        //! Returns the PGA level
        /** PGA level = 2^gain, from 0 to 64
        */
        uint8_t getPGA();

        //! Disable PGA
        void disablePGA();


        //! Returns the analog value of the pin.
        /** It waits until the value is read and then returns the result.
        * If a comparison has been set up and fails, it will return ADC_ERROR_VALUE.
        * This function is interrupt safe, so it will restore the adc to the state it was before being called
        */
        int analogRead(uint8_t pin);

        //! Reads the differential analog value of two pins (pinP - pinN).
        /** It waits until the value is read and then returns the result.
        * If a comparison has been set up and fails, it will return ADC_ERROR_DIFF_VALUE.
        * \param pinP must be A10 or A12.
        * \param pinN must be A11 (if pinP=A10) or A13 (if pinP=A12).
        * In Teensy 3.1, if PGA is enabled A10-A11 are connected to it.
        * Other pins will return ADC_ERROR_DIFF_VALUE.
        * This function is interrupt safe, so it will restore the adc to the state it was before being called
        */
        int analogReadDifferential(uint8_t pinP, uint8_t pinN);

        //! Starts an analog measurement on the pin and enables interrupts.
        /** It returns inmediately, get value with readSingle().
        *   If the pin is incorrect it returns ADC_ERROR_VALUE
        *   This function is interrupt safe. The ADC interrupt will restore the adc to its previous settings and
        *   restart the adc if it stopped a measurement. If you modify the adc_isr then this won't happen.
        */
        int startSingleRead(uint8_t pin);
        int startSingleReadFast(uint8_t pin);

        //! Start a differential conversion between two pins (pinP - pinN) and enables interrupts.
        /** It returns inmediately, get value with readSingle().
        *   \param pinP must be A10 or A12.
        *   \param pinN must be A11 (if pinP=A10) or A13 (if pinP=A12).
        *   Other pins will return ADC_ERROR_DIFF_VALUE.
        *   This function is interrupt safe. The ADC interrupt will restore the adc to its previous settings and
        *   restart the adc if it stopped a measurement. If you modify the adc_isr then this won't happen.
        */
        int startSingleDifferential(uint8_t pinP, uint8_t pinN);

        //! Reads the analog value of a single conversion.
        /** Set the conversion with with startSingleRead(pin) or startSingleDifferential(pinP, pinN).
        *   \return the converted value.
        */
        int readSingle(){ return analogReadContinuous(); }


        //! Starts continuous conversion on the pin.
        /** It returns as soon as the ADC is set, use analogReadContinuous() to read the value.
        */
        void startContinuous(uint8_t pin);

        //! Starts continuous conversion between the pins (pinP-pinN).
        /** It returns as soon as the ADC is set, use analogReadContinuous() to read the value.
        * \param pinP must be A10 or A12.
        * \param pinN must be A11 (if pinP=A10) or A13 (if pinP=A12).
        * Other pins will return ADC_ERROR_DIFF_VALUE.
        */
        void startContinuousDifferential(uint8_t pinP, uint8_t pinN);

        //! Reads the analog value of a continuous conversion.
        /** Set the continuous conversion with with analogStartContinuous(pin) or startContinuousDifferential(pinP, pinN).
        *   \return the last converted value.
        */
        int analogReadContinuous();

        //! Stops continuous conversion
        void stopContinuous();


        //! Is the ADC converting at the moment?
        bool isConverting();

        //! Is an ADC conversion ready?
        /**
        *  \return 1 if yes, 0 if not.
        *  When a value is read this function returns 0 until a new value exists
        *  So it only makes sense to call it before analogReadContinuous() or readSingle()
        */
        bool isComplete();

        //! Is the ADC in differential mode?
        bool isDifferential();

        //! Is the ADC in continuous mode?
        bool isContinuous();


        void startPDB(double period);
        double adc_pdb_period;

        // struct to store the config of the adc
        typedef struct ADC_CONFIG{
            uint32_t savedSC1A, savedSC2, savedSC3, savedCFG1, savedCFG2, savedRes;
            uint8_t diffRes; // is the new resolution different from the old one?
        } ADC_Config;

        // translate pin number to SC1A nomenclature and viceversa
        // we need to create this static const arrays so that we can assign the "normal arrays" to the correct one
        // depending on which ADC module we will be.
        static const uint8_t channel2sc1aADC0[44];
        static const uint8_t channel2sc1aADC1[44];
        static const uint8_t sc1a2channelADC0[30];
        static const uint8_t sc1a2channelADC1[30];

        uint8_t channel2sc1a[44];
        uint8_t sc1a2channel[30];

        ADC_Config adc_config;
        uint8_t adcWasInUse; // was the adc in use before an analog timer call?

        // number of measurements that the ADC is performing
        uint8_t num_measurements = 0;


    private:

        const uint8_t ledPin=13;

        uint8_t calibrating;

        uint8_t analog_config_bits;

        uint32_t analog_max_val;

        uint8_t analog_num_average;

        uint8_t analog_reference_internal;

        uint8_t var_enableInterrupts;

        uint8_t pga_value;

        // which adc is this?
        uint8_t ADC_num;


        /** Initialize ADC
        *
        */
        void analog_init(uint32_t config = 0);


        /** Starts the calibration sequence
        *
        */
        void calibrate();

        /** Waits until calibration is finished and writes the corresponding registers
        *
        */
        void wait_for_cal(void);


        // registers point to the correct ADC module
        typedef volatile uint32_t* reg;

        // registers that control the adc module
        reg ADC_SC1A;
        reg ADC_SC1B;

        reg ADC_CFG1;
        reg ADC_CFG2;

        reg ADC_RA;
        reg ADC_RB;

        reg ADC_CV1;
        reg ADC_CV2;

        reg ADC_SC2;
        reg ADC_SC3;

        reg ADC_PGA;

        reg ADC_OFS;
        reg ADC_PG;
        reg ADC_MG;
        reg ADC_CLPD;
        reg ADC_CLPS;
        reg ADC_CLP4;
        reg ADC_CLP3;
        reg ADC_CLP2;
        reg ADC_CLP1;
        reg ADC_CLP0;
        reg ADC_CLMD;
        reg ADC_CLMS;
        reg ADC_CLM4;
        reg ADC_CLM3;
        reg ADC_CLM2;
        reg ADC_CLM1;
        reg ADC_CLM0;


    protected:


};


#endif // ADC_MODULE_H
