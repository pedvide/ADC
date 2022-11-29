/* Teensy 4.x, 3.x, LC ADC library
 * https://github.com/pedvide/ADC
 * Copyright (c) 2020 Pedro Villanueva
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/* TODO
 * - Function to measure more that 1 pin consecutively (stream?)
 *
 * bugs:
 * - comparison values in 16 bit differential mode are twice what they should be
 */

/*! \mainpage ADC

Teensy 4.x, 3.x, LC ADC library

This manual is divided in the following sections:
- \subpage adc_doc "ADC"
- \subpage adc_module "ADC Module"
- \subpage settings "ADC settings"
- \subpage error "ADC error codes"
- \subpage util "ADC util"

*/

/*! \page adc_doc ADC
Make Analog to Digital conversions using the ADC interface.
See the ADC class for all methods.
*/

#ifndef ADC_H
#define ADC_H

#define ADC_0 0
#define ADC_1 1
// enum class ADC_NUM {ADC_0, ADC_1}; // too verbose, but it'd avoid some
// mistakes

// include ADC module class
#include "ADC_Module.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Class ADC: Controls the Teensy 3.x, 4 ADC
 *
 */
class ADC {
protected:
private:
  // ADCs objects
  ADC_Module adc0_obj;
#ifdef ADC_DUAL_ADCS
  ADC_Module adc1_obj;
#endif

  //! Number of ADC objects
  const uint8_t num_ADCs = ADC_NUM_ADCS;

public:
  /** Default constructor */
  ADC();

  // create both adc objects

  //! Object to control the ADC0
  ADC_Module *const adc0 = &adc0_obj; // adc object pointer
#ifdef ADC_DUAL_ADCS
  //! Object to control the ADC1
  ADC_Module *const adc1 = &adc1_obj; // adc object pointer
#endif

#ifdef ADC_SINGLE_ADC
  //! Array with the ADC Modules
  ADC_Module *const adc[ADC_NUM_ADCS] = {adc0};
#else
  //! Array with the ADC Modules
  ADC_Module *const adc[ADC_NUM_ADCS] = {adc0, adc1};
#endif

  /////////////// METHODS TO SET/GET SETTINGS OF THE ADC ////////////////////
  /** @name ADC settings
   */
  ///@{

  /**
   * @brief Set the voltage reference for all ADCs, default is vcc
   * @param type Any of @ref ADC_settings::ADC_REFERENCE "ADC_REFERENCE"
   */
  inline void setReference(ADC_REFERENCE type) {
    adc0->setReference(type);
#ifdef ADC_DUAL_ADCS
    adc1->setReference(type);
#endif
  }

  /**
   * @brief Change the resolution for all ADCs.
   * @param bits is the number of bits of resolution.
   *  For single-ended measurements: 8, 10, 12 or 16 bits.
   *  For differential measurements: 9, 11, 13 or 16 bits.
   *  If you want something in between (11 bits single-ended for example) select
   * the immediate higher and shift the result one to the right. Whenever you
   * change the resolution, change also the comparison values (if you use them).
   */
  inline void setResolution(uint8_t bits) {
    adc0->setResolution(bits);
#ifdef ADC_DUAL_ADCS
    adc1->setResolution(bits);
#endif
  }

  /**
   * @brief Sets the conversion speed for all ADCs
   * @param speed can be any from the @ref ADC_settings::ADC_CONVERSION_SPEED
   * "ADC_CONVERSION_SPEED" enum.
   */
  inline void setConversionSpeed(ADC_CONVERSION_SPEED speed) {
    adc0->setConversionSpeed(speed);
#ifdef ADC_DUAL_ADCS
    adc1->setConversionSpeed(speed);
#endif
  }

  /**
   * @brief Sets the sampling speed for all ADCs
   *
   * Increase the sampling speed for low impedance sources, decrease it for
   * higher impedance ones.
   * @param speed can be any of the @ref ADC_settings::ADC_SAMPLING_SPEED
   * "ADC_SAMPLING_SPEED".
   */
  inline void setSamplingSpeed(ADC_SAMPLING_SPEED speed) {
    adc0->setSamplingSpeed(speed);
#ifdef ADC_DUAL_ADCS
    adc1->setSamplingSpeed(speed);
#endif
  }

  /**
   * @brief Set the number of averages for all ADCs
   * @param num can be 0, 4, 8, 16 or 32.
   */
  inline void setAveraging(uint8_t num) {
    adc0->setAveraging(num);
#ifdef ADC_DUAL_ADCS
    adc1->setAveraging(num);
#endif
  }

  /**
   * @brief Enable interrupts for all ADCs
   *
   * An IRQ_ADCx Interrupt will be raised when the conversion is completed
   * (including hardware averages and if the comparison (if any) is true).
   * @param isr function (returns void and accepts no arguments) that will be
   * executed after an interrupt.
   * @param priority Interrupt priority, highest is 0, lowest is 255.
   */
  inline void enableInterrupts(void (*isr)(void), uint8_t priority = 255) {
    adc0->enableInterrupts(isr, priority);
#ifdef ADC_DUAL_ADCS
    adc1->enableInterrupts(isr, priority);
#endif
  }

  /**
   * @brief Disable interrupts for all ADCs
   */
  inline void disableInterrupts() {
    adc0->disableInterrupts();
#ifdef ADC_DUAL_ADCS
    adc1->disableInterrupts();
#endif
  }

#ifdef ADC_USE_DMA
  /**
   * @brief Enable DMA request for all ADCs
   *
   * An ADC DMA request will be raised when the conversion is completed
   * (including hardware averages and if the comparison (if any) is true).
   */
  inline void enableDMA() {
    adc0->enableDMA();
#ifdef ADC_DUAL_ADCS
    adc1->enableDMA();
#endif
  }

  /**
   * @brief Disable DMA request for all ADCs
   */
  inline void disableDMA() {
    adc0->disableDMA();
#ifdef ADC_DUAL_ADCS
    adc1->disableDMA();
#endif
  }
#endif

  /**
   * @brief Enable the compare function to a single value
   *
   * A conversion will be completed only when the ADC value
   * is >= compValue (greaterThan=1) or < compValue (greaterThan=0)
   * Call it after changing the resolution
   * Use with interrupts or poll conversion completion with isComplete()
   * @param compValue value to compare
   * @param greaterThan true or false
   */
  inline void enableCompare(int16_t compValue, bool greaterThan) {
    adc0->enableCompare(compValue, greaterThan);
#ifdef ADC_DUAL_ADCS
    adc1->enableCompare(compValue, greaterThan);
#endif
  }

  /**
   * @brief Enable the compare function to a range
   *
   * A conversion will be completed only when the ADC value is inside
   * (insideRange=1) or outside (=0) the range given by (lowerLimit,
   * upperLimit),including (inclusive=1) the limits or not (inclusive=0). See
   * Table 31-78, p. 617 of the freescale manual. Call it after changing the
   * resolution Use with interrupts or poll conversion completion with
   * isComplete()
   * @param lowerLimit lower value to compare
   * @param upperLimit upper value to compare
   * @param insideRange true or false
   * @param inclusive true or false
   */
  inline void enableCompareRange(int16_t lowerLimit, int16_t upperLimit,
                                 bool insideRange, bool inclusive) {
    adc0->enableCompareRange(lowerLimit, upperLimit, insideRange, inclusive);
#ifdef ADC_DUAL_ADCS
    adc1->enableCompareRange(lowerLimit, upperLimit, insideRange, inclusive);
#endif
  }

  /**
   * @brief Disable the compare function
   *
   */
  inline void disableCompare() {
    adc0->disableCompare();
#ifdef ADC_DUAL_ADCS
    adc1->disableCompare();
#endif
  }

#ifdef ADC_USE_PGA
  /**
   * @brief Enable and set PGA
   *
   * Enables the PGA and sets the gain
   * Use only for signals lower than 1.2 V and only in differential mode
   * @param gain can be 1, 2, 4, 8, 16, 32 or 64
   */
  inline void enablePGA(uint8_t gain) {
    adc0->enablePGA(gain);
#ifdef ADC_DUAL_ADCS
    adc1->enablePGA(gain);
#endif
  }

  //! Disable PGA
  inline void disablePGA() {
    adc0->disablePGA();
#ifdef ADC_DUAL_ADCS
    adc1->disablePGA();
#endif
  }
#endif

  ///@}

  //////////////// BLOCKING CONVERSION METHODS //////////////////
  /** @name Blocking conversion methods
   */
  ///@{

  //! Returns the analog value of the pin.
  /** It waits until the value is read and then returns the result.
   * If a comparison has been set up and fails, it will return ADC_ERROR_VALUE.
   * This function is interrupt safe, so it will restore the adc to the state it
   * was before being called If more than one ADC exists, it will select the
   * module with less workload, you can force a selection using adc_num. If you
   * select ADC1 in a board with just one ADC, it will return ADC_ERROR_VALUE.
   * @param pin can be any of the analog pins
   * @param adc_num ADC_X ADC module
   * @return the analog value of the pin.
   */
  int analogRead(uint8_t pin, int8_t adc_num = -1);

  /** Returns the analog value of the special internal source, such as the
   * temperature sensor.
   *   \param pin ADC_INTERNAL_SOURCE to read.
   *   \param adc_num ADC_X ADC module
   *   \return the value of the pin.
   */
  int analogRead(ADC_INTERNAL_SOURCE pin, int8_t adc_num = -1)
      __attribute__((always_inline)) {
    return analogRead(static_cast<uint8_t>(pin), adc_num);
  }

#if ADC_DIFF_PAIRS > 0
  //! Reads the differential analog value of two pins (pinP - pinN).
  /** It waits until the value is read and then returns the result.
   * This function is interrupt safe, so it will restore the adc to the state it
   * was before being called If more than one ADC exists, it will select the
   * module with less workload, you can force a selection using adc_num
   * @param pinP must be A10 or A12.
   * @param pinN must be A11 (if pinP=A10) or A13 (if pinP=A12).
   * @param adc_num ADC_X ADC module
   * @return the differential value of the pins, invalid pins return
   * ADC_ERROR_VALUE. If a comparison has been set up and fails, it will return
   * ADC_ERROR_VALUE.
   */
  int analogReadDifferential(uint8_t pinP, uint8_t pinN, int8_t adc_num = -1);
#endif

  ///@}

  /////////////// NON-BLOCKING CONVERSION METHODS //////////////
  /** @name Non-blocking conversion methods
   */
  ///@{

  //! Starts an analog measurement on the pin and enables interrupts.
  /** It returns immediately, get value with readSingle().
   *  If this function interrupts a measurement, it stores the settings in
   * adc_config
   * @param pin can be any of the analog pins
   * @param adc_num ADC_X ADC module
   * @return true if the pin is valid, false otherwise.
   */
  bool startSingleRead(uint8_t pin, int8_t adc_num = -1);

#if ADC_DIFF_PAIRS > 0
  //! Start a differential conversion between two pins (pinP - pinN) and enables
  //! interrupts.
  /** It returns immediately, get value with readSingle().
   *   If this function interrupts a measurement, it stores the settings in
   * adc_config
   * @param pinP must be A10 or A12.
   * @param pinN must be A11 (if pinP=A10) or A13 (if pinP=A12).
   * @param adc_num ADC_X ADC module
   * @return true if the pins are valid, false otherwise.
   */
  bool startSingleDifferential(uint8_t pinP, uint8_t pinN, int8_t adc_num = -1);
#endif

  //! Reads the analog value of a single conversion.
  /** Set the conversion with with startSingleRead(pin) or
   * startSingleDifferential(pinP, pinN).
   * @param adc_num ADC_X ADC module
   * @return the converted value.
   */
  int readSingle(int8_t adc_num = -1);

  ///@}

  ///////////// CONTINUOUS CONVERSION METHODS ////////////
  /** @name Continuous conversion methods
   */
  ///@{

  //! Starts continuous conversion on the pin.
  /** It returns as soon as the ADC is set, use analogReadContinuous() to read
   * the value.
   * @param pin can be any of the analog pins
   * @param adc_num ADC_X ADC module
   * @return true if the pin is valid, false otherwise.
   */
  bool startContinuous(uint8_t pin, int8_t adc_num = -1);

#if ADC_DIFF_PAIRS > 0
  //! Starts continuous conversion between the pins (pinP-pinN).
  /** It returns as soon as the ADC is set, use analogReadContinuous() to read
   * the value.
   * @param pinP must be A10 or A12.
   * @param pinN must be A11 (if pinP=A10) or A13 (if pinP=A12).
   * @param adc_num ADC_X ADC module
   * @return true if the pins are valid, false otherwise.
   */
  bool startContinuousDifferential(uint8_t pinP, uint8_t pinN,
                                   int8_t adc_num = -1);
#endif

  //! Reads the analog value of a continuous conversion.
  /** Set the continuous conversion with with analogStartContinuous(pin) or
   * startContinuousDifferential(pinP, pinN). If single-ended and 16 bits it's
   * necessary to typecast it to an unsigned type (like uint16_t), otherwise
   * values larger than 3.3/2 V are interpreted as negative!
   * @param adc_num ADC_X ADC module
   * @return the last converted value.
   */
  int analogReadContinuous(int8_t adc_num = -1);

  //! Stops continuous conversion
  /**
   *   \param adc_num ADC_X ADC module
   */
  void stopContinuous(int8_t adc_num = -1);

  ///@}

/////////// SYNCHRONIZED METHODS ///////////////
///// ONLY FOR BOARDS WITH MORE THAN ONE ADC /////
#ifdef ADC_DUAL_ADCS
  /** @name Synchronized conversion methods
   * ADC0 and ADC1 will measure simultaneously
   */
  ///@{

  //! Struct for synchronous measurements
  /** result_adc0 has the result from ADC0 and result_adc1 from ADC1.
   */
  struct Sync_result {
    int32_t result_adc0; /**< Result in ADC0 */
    int32_t result_adc1; /**< Result in ADC1 */
  };

  //////////////// SYNCHRONIZED BLOCKING METHODS //////////////////

  //! Returns the analog values of both pins, measured at the same time by the
  //! two ADC modules.
  /** It waits until the values are read and then returns the result as a struct
   * Sync_result, use Sync_result.result_adc0 and Sync_result.result_adc1. If a
   * comparison has been set up and fails, it will return ADC_ERROR_VALUE in
   * both fields of the struct. This function is interrupt safe, so it will
   * restore the adc to the state it was before being called
   * @param pin0 pin in ADC0
   * @param pin1 pin in ADC1
   * @return a Sync_result struct with the result of each ADC value.
   */
  Sync_result analogSynchronizedRead(uint8_t pin0, uint8_t pin1);

  //! Same as analogSynchronizedRead
  /**
   *   \param pin0 pin in ADC0
   *   \param pin1 pin in ADC1
   *   \return a Sync_result struct with the result of each ADC value.
   */
  Sync_result analogSyncRead(uint8_t pin0, uint8_t pin1)
      __attribute__((always_inline)) {
    return analogSynchronizedRead(pin0, pin1);
  }

#if ADC_DIFF_PAIRS > 0
  //! Returns the differential analog values of both sets of pins, measured at
  //! the same time by the two ADC modules.
  /** It waits until the values are read and then returns the result as a struct
   * Sync_result, use Sync_result.result_adc0 and Sync_result.result_adc1. If a
   * comparison has been set up and fails, it will return ADC_ERROR_VALUE in
   * both fields of the struct. This function is interrupt safe, so it will
   * restore the adc to the state it was before being called
   * @param pin0P positive pin in ADC0
   * @param pin0N negative pin in ADC0
   * @param pin1P positive pin in ADC1
   * @param pin1N negative pin in ADC1
   * @return a Sync_result struct with the result of each differential ADC
   * value.
   */
  Sync_result analogSynchronizedReadDifferential(uint8_t pin0P, uint8_t pin0N,
                                                 uint8_t pin1P, uint8_t pin1N);

  //! Same as analogSynchronizedReadDifferential
  /**
   *   \param pin0P positive pin in ADC0
   *   \param pin0N negative pin in ADC0
   *   \param pin1P positive pin in ADC1
   *   \param pin1N negative pin in ADC1
   *   \return a Sync_result struct with the result of each differential ADC
   * value.
   */
  Sync_result analogSyncReadDifferential(uint8_t pin0P, uint8_t pin0N,
                                         uint8_t pin1P, uint8_t pin1N)
      __attribute__((always_inline)) {
    return analogSynchronizedReadDifferential(pin0P, pin0N, pin1P, pin1N);
  }
#endif

  /////////////// SYNCHRONIZED NON-BLOCKING METHODS //////////////

  //! Starts an analog measurement at the same time on the two ADC modules
  /** It returns immediately, get value with readSynchronizedSingle().
   *   If this function interrupts a measurement, it stores the settings in
   * adc_config
   * @param pin0 pin in ADC0
   * @param pin1 pin in ADC1
   * @return true if the pins are valid, false otherwise.
   */
  bool startSynchronizedSingleRead(uint8_t pin0, uint8_t pin1);

#if ADC_DIFF_PAIRS > 0
  //! Start a differential conversion between two pins (pin0P - pin0N) and
  //! (pin1P - pin1N)
  /** It returns immediately, get value with readSynchronizedSingle().
   *   If this function interrupts a measurement, it stores the settings in
   * adc_config
   * @param pin0P positive pin in ADC0
   * @param pin0N negative pin in ADC0
   * @param pin1P positive pin in ADC1
   * @param pin1N negative pin in ADC1
   * @return true if the pins are valid, false otherwise.
   */
  bool startSynchronizedSingleDifferential(uint8_t pin0P, uint8_t pin0N,
                                           uint8_t pin1P, uint8_t pin1N);
#endif

  //! Reads the analog value of a single conversion.
  /**
   *   \return the converted value.
   */
  Sync_result readSynchronizedSingle();

  ///////////// SYNCHRONIZED CONTINUOUS CONVERSION METHODS ////////////

  //! Starts a continuous conversion in both ADCs simultaneously
  /** Use readSynchronizedContinuous to get the values
   *   \param pin0 pin in ADC0
   *   \param pin1 pin in ADC1
   *   \return true if the pins are valid, false otherwise.
   */
  bool startSynchronizedContinuous(uint8_t pin0, uint8_t pin1);

#if ADC_DIFF_PAIRS > 0
  //! Starts a continuous differential conversion in both ADCs simultaneously
  /** Use readSynchronizedContinuous to get the values
   *   \param pin0P positive pin in ADC0
   *   \param pin0N negative pin in ADC0
   *   \param pin1P positive pin in ADC1
   *   \param pin1N negative pin in ADC1
   *   \return true if the pins are valid, false otherwise.
   */
  bool startSynchronizedContinuousDifferential(uint8_t pin0P, uint8_t pin0N,
                                               uint8_t pin1P, uint8_t pin1N);
#endif

  //! Returns the values of both ADCs.
  /**
   *   \return the converted value.
   */
  Sync_result readSynchronizedContinuous();

  //! Stops synchronous continuous conversion
  void stopSynchronizedContinuous();

  ///@}

#endif

  //////////// ERRORS /////
  //! Resets all errors from all ADCs, if any.
  void resetError() {
    for (int i = 0; i < ADC_NUM_ADCS; i++) {
      adc[i]->resetError();
    }
  }

  //! Translate pin number to SC1A nomenclature
  // should this be a constexpr?
  static const uint8_t channel2sc1aADC0[ADC_MAX_PIN + 1];
#ifdef ADC_DUAL_ADCS
  //! Translate pin number to SC1A nomenclature
  static const uint8_t channel2sc1aADC1[ADC_MAX_PIN + 1];
#endif

  //! Translate pin number to SC1A nomenclature for differential pins
  static const uint8_t sc1a2channelADC0[ADC_MAX_PIN + 1];
#ifdef ADC_DUAL_ADCS
  //! Translate pin number to SC1A nomenclature for differential pins
  static const uint8_t sc1a2channelADC1[ADC_MAX_PIN + 1];
#endif

#if ADC_DIFF_PAIRS > 0
  //! Translate differential pin number to SC1A nomenclature
  static const ADC_Module::ADC_NLIST diff_table_ADC0[ADC_DIFF_PAIRS];
#ifdef ADC_DUAL_ADCS
  //! Translate differential pin number to SC1A nomenclature
  static const ADC_Module::ADC_NLIST diff_table_ADC1[ADC_DIFF_PAIRS];
#endif
#endif
};

#ifdef __cplusplus
}
#endif

#endif // ADC_H
