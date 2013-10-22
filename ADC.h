 /* ADC Library created by Pedro Villanueva
 * It implements all functions of the Teensy 3.0 internal ADC
 * Some of the code was extracted from the file analog.c included in the distribution of Teensduino by PJRC.COM
 *
 */


#ifndef ADC_H
#define ADC_H

#include <stdint.h>
#include "core_pins.h"
#include "mk20dx128.h"
// include pow()
#include <math.h>

// the alternate clock is connected to OSCERCLK (16 MHz).
// datasheet says ADC clock should be 2 to 12 MHz for 16 bit mode
// datasheet says ADC clock should be 1 to 18 MHz for 8-12 bit mode
#if F_BUS == 48000000
  #define ADC0_CFG1_6MHZ   ADC_CFG1_ADIV(2) + ADC_CFG1_ADICLK(1) // Clock divide select: 2=div4 + Input clock: 1=bus/2
  #define ADC0_CFG1_12MHZ  ADC_CFG1_ADIV(1) + ADC_CFG1_ADICLK(1) // Clock divide select: 1=div2 Input clock: 1=bus/2
  #define ADC0_CFG1_24MHZ  ADC_CFG1_ADIV(0) + ADC_CFG1_ADICLK(1) // Clock divide select: 0=direct + Input clock: 1=bus/2
#elif F_BUS == 24000000
  #define ADC0_CFG1_6MHZ   ADC_CFG1_ADIV(2) + ADC_CFG1_ADICLK(0) // Clock divide select: 2=div4 + Input clock: 0=bus
  #define ADC0_CFG1_12MHZ  ADC_CFG1_ADIV(1) + ADC_CFG1_ADICLK(0) // Clock divide select: 1=div2 + Input clock: 0=bus
  #define ADC0_CFG1_24MHZ  ADC_CFG1_ADIV(0) + ADC_CFG1_ADICLK(0) // Clock divide select: 0=direct + Input clock: 0=bus
#else
#error
#endif


// ADCx_SC2[REFSEL] bit selects the voltage reference sources for ADC.
//   VREFH/VREFL - connected as the primary reference option
//   1.2 V VREF_OUT - connected as the VALT reference option
#define DEFAULT         0
#define INTERNAL        2
#define INTERNAL1V2     2
#define INTERNAL1V1     2
#define EXTERNAL        0


// Error codes for analogRead and analogReadDifferential
#define ADC_ERROR_VALUE -1
#define ADC_ERROR_DIFF_VALUE -70000

/** Class ADC: Implements all functions of the Teensy 3.0 analog to digital converter
*
*/
class ADC
{
    public:

        /** Constructor
        *
        */
        ADC();

        /** Destructor
        *
        */
        ~ADC();


        //! Set the voltage reference you prefer, default is vcc
        /*!
        * \param type can be DEFAULT, EXTERNAL or INTERNAL
        */
        void setReference(uint8_t type);


        //! Change the resolution of the measurement.
        /*!
        *  \param bits is the number of bits of resolution.
        *  For single-ended measurements: 8, 10, 12 or 16 bits.
        *  For differential measurements: 9, 11, 13 or 16 bits.
        *  If you want something in between (11 bits single-ended for example) select the inmediate higher
        *  and shift the result one to the right.
        */
        void setResolution(unsigned int bits);

        //! Returns the resolution of the ADC.
        int getResolution();

        //! Returns the maximum value for a measurement, that is: 2^resolution
        double getMaxValue();


        //! Set the number of averages
        /*!
        * \param num can be 0, 4, 8, 16 or 32.
        */
        void setAveraging(unsigned int num);


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


        //! Returns the analog value of the pin.
        /** It waits until the value is read and then returns the result.
        * If a comparison has been set up and fails, it will return ADC_ERROR_VALUE.
        */
        int analogRead(uint8_t pin);

        //! Reads the differential analog value of two pins (pinP - pinN).
        /** It waits until the value is read and then returns the result.
        * If a comparison has been set up and fails, it will return ADC_ERROR_DIFF_VALUE.
        * \param pinP must be A10 or A12.
        * \param pinN must be A11 (if pinP=A10) or A13 (if pinP=A12).
        * Other pins will return ADC_ERROR_DIFF_VALUE.
        */
        int analogReadDifferential(uint8_t pinP, uint8_t pinN);


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

        //! Reads the analog value of a continuous conversion
        /** Set the continuous conversion with with analogStartContinuous(pin) or startContinuousDifferential(pinP, pinN)
        *   \return the last converted value.
        */
        int analogReadContinuous();

        //! Stops continuous conversion
        void stopContinuous();


        //! Is the ADC converting at the moment?
        /**
        *  \return 1 if yes, 0 if not
        */
        int isConverting();

        //! Is an ADC conversion ready?
        /**
        *  \return 1 if yes, 0 if not.
        *  When a value is read this function returns 0 until a new value exists
        *  So it only makes sense to call it before analogRead(), analogReadContinuous() or analogReadDifferential()
        */
        int isComplete();

    protected:
    private:
        uint8_t calibrating;

        uint8_t analog_config_bits;

        double analog_max_val;

        uint8_t analog_num_average;

        uint8_t analog_reference_internal;

        uint8_t var_enableInterrupts;

        /** Sets up all initial configurations and starts calibration
        *
        */
        void analog_init(void);

        /** Waits until calibration is finished and writes the corresponding registers
        *
        */
        void wait_for_cal(void);

        // translate pin number to SC1A nomenclature
        uint8_t channel2sc1a[16]= {
            5, 14, 8, 9, 13, 12, 6, 7, 15, 4,
            0, 19, 3, 21, 26, 22};

};

#endif // ADC_H
