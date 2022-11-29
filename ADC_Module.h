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

/* ADC_Module.h: Declarations of the fuctions of a Teensy 3.x, LC ADC module
 *
 */

/**
 * @page adc_module ADC Module
 * Control each ADC_Module independently.
 * See the ADC_Module class for all methods.
 */

#ifndef ADC_MODULE_H
#define ADC_MODULE_H

#include <Arduino.h>
#include <atomic.h>
#include <settings_defines.h>

using ADC_Error::ADC_ERROR;
using namespace ADC_settings;

// debug mode: blink the led light
#define ADC_debug 0

/**
 * @brief Implements all functions of the Teensy 3.x, LC, 4.x analog to digital
 * converter
 *
 */
class ADC_Module {

public:
#if ADC_DIFF_PAIRS > 0
  //! \cond internal
  //! Dictionary with the differential pins as keys and the SC1A number as
  //! values
  /** Internal, do not use.
   */
  struct ADC_NLIST {
    //! Pin and corresponding SC1A value.
    uint8_t pin, sc1a;
  };
#endif
  //! \endcond

#if ADC_DIFF_PAIRS > 0
  /**
   * @brief Pass the ADC number and the Channel number to SC1A number arrays.
   * @param ADC_number Number of the ADC module, from 0.
   * @param a_channel2sc1a contains an index that pairs each pin to its SC1A
   * number (used to start a conversion on that pin)
   * @param a_diff_table is similar to a_channel2sc1a, but for differential
   * pins.
   * @param a_adc_regs pointer to start of the ADC registers
   */
  ADC_Module(uint8_t ADC_number, const uint8_t *const a_channel2sc1a,
             const ADC_NLIST *const a_diff_table, ADC_REGS_t &a_adc_regs);
#else
  /**
   * @brief Pass the ADC number and the Channel number to SC1A number arrays.
   * @param ADC_number Number of the ADC module, from 0.
   * @param a_channel2sc1a contains an index that pairs each pin to its SC1A
   * number (used to start a conversion on that pin)
   * @param a_adc_regs pointer to start of the ADC registers
   */
  ADC_Module(uint8_t ADC_number, const uint8_t *const a_channel2sc1a,
             ADC_REGS_t &a_adc_regs);
#endif

  /** @name Calibration functions
   */
  ///@{

  /**
   * @brief Starts the calibration sequence, waits until it's done and writes
   * the results.
   *
   * Usually it's not necessary to call this function directly, but
   * do it if the "environment" changed significantly since the program was
   * started.
   */
  void recalibrate();

  //! Starts the calibration sequence
  void calibrate();

  //! Waits until calibration is finished and writes the corresponding registers
  void wait_for_cal();

  ///@}

  /////////////// METHODS TO SET/GET SETTINGS OF THE ADC ////////////////////
  /** @name ADC settings
   */
  ///@{

  /**
   * @brief Set the voltage reference you prefer, default is vcc
   * @param ref_type any of the @ref ADC_settings::ADC_REFERENCE
   * "ADC_REFERENCE" enum.
   *
   *  It recalibrates at the end.
   */
  void setReference(ADC_REFERENCE ref_type);

  /**
   * @brief Change the resolution of the measurement.
   * @param bits is the number of bits of resolution.
   *  - For single-ended measurements: 8, 10, 12 or 16 bits.
   *  - For differential measurements: 9, 11, 13 or 16 bits.
   *  If you want something in between (11 bits single-ended for example) select
   * the immediate higher and shift the result one to the right.
   *
   *  Whenever you change the resolution, change also the comparison values (if
   * you use them).
   */
  void setResolution(uint8_t bits);

  /**
   * @brief Returns the resolution of the ADC_Module.
   * @return the resolution of the ADC_Module.
   */
  uint8_t getResolution();

  /**
   * @brief Returns the maximum value for a measurement: 2^res-1.
   * @return the maximum value for a measurement: 2^res-1.
   */
  uint32_t getMaxValue();

  /**
   * @brief Sets the conversion speed (changes the ADC clock, ADCK)
   * @param speed can be any from the @ref ADC_settings::ADC_CONVERSION_SPEED
   * "ADC_CONVERSION_SPEED" enum.
   *
   */
  void setConversionSpeed(ADC_CONVERSION_SPEED speed);

  /**
   * @brief Sets the sampling speed
   *
   * Increase the sampling speed for low impedance sources, decrease it for
   * higher impedance ones.
   * @param speed can be any of the @ref ADC_settings::ADC_SAMPLING_SPEED
   * "ADC_SAMPLING_SPEED" enum.
   */
  void setSamplingSpeed(ADC_SAMPLING_SPEED speed);

  /**
   * @brief Set the number of averages
   * @param num can be 0, 4, 8, 16 or 32.
   *
   *  It doesn't recalibrate at the end.
   */
  void setAveraging(uint8_t num);

  /**
   * @brief Enable interrupts.
   *
   * An IRQ_ADCx interrupt will be raised when the conversion is completed
   * (including hardware averages and if the comparison (if any) is true).
   * @param isr function (returns void and accepts no arguments) that will be
   * executed after an interrupt.
   * @param priority Interrupt priority, highest is 0, lowest is 255.
   */
  void enableInterrupts(void (*isr)(void), uint8_t priority = 255);

  //! Disable interrupts
  void disableInterrupts();

#ifdef ADC_USE_DMA
  /**
   * @brief Enable DMA request.
   *
   * An ADC DMA request will be raised when the conversion is completed
   * (including hardware averages and if the comparison (if any) is true).
   */
  void enableDMA();

  //! Disable ADC DMA request
  void disableDMA();
#endif

  /**
   * @brief Enable the compare function to a single value
   *
   * A conversion will be completed only when the ADC value
   * is >= compValue (greaterThan=1) or < compValue (greaterThan=0)
   *
   * Call it after changing the resolution. Use with interrupts or poll
   * conversion completion with @ref isComplete().
   * @param compValue value to compare
   * @param greaterThan true or false
   */
  void enableCompare(int16_t compValue, bool greaterThan);

  /**
   * @brief Enable the compare function to a range
   *
   * A conversion will be completed only when the ADC value is inside
   * (insideRange=1) or outside (=0) the range given by (lowerLimit,
   * upperLimit), including (inclusive=1) the limits or not (inclusive=0).
   *
   * See Table 31-78, p. 617 of the freescale manual. Call it after changing the
   * resolution. Use with interrupts or poll conversion completion with
   * @ref isComplete()
   * @param lowerLimit lower value to compare
   * @param upperLimit upper value to compare
   * @param insideRange true or false
   * @param inclusive true or false
   */
  void enableCompareRange(int16_t lowerLimit, int16_t upperLimit,
                          bool insideRange, bool inclusive);

  //! Disable the compare function
  void disableCompare();

#ifdef ADC_USE_PGA
  /**
   * @brief Enables the PGA and sets the gain
   *
   *  Use only for signals lower than 1.2 V and only in differential mode
   *  @param gain can be 1, 2, 4, 8, 16, 32 or 64
   */
  void enablePGA(uint8_t gain);

  /**
   * @brief Returns the PGA level
   * @return PGA level from 1 to 64
   */
  uint8_t getPGA();

  //! Disable PGA
  void disablePGA();
#endif

  //! Set continuous conversion mode
  void continuousMode() __attribute__((always_inline)) {
#ifdef ADC_TEENSY_4
    atomic::setBitFlag(adc_regs.GC, ADC_GC_ADCO);
#else
    atomic::setBitFlag(adc_regs.SC3, ADC_SC3_ADCO);
#endif
  }
  //! Set single-shot conversion mode
  void singleMode() __attribute__((always_inline)) {
#ifdef ADC_TEENSY_4
    atomic::clearBitFlag(adc_regs.GC, ADC_GC_ADCO);
#else
    atomic::clearBitFlag(adc_regs.SC3, ADC_SC3_ADCO);
#endif
  }

  //! Set single-ended conversion mode
  void singleEndedMode() __attribute__((always_inline)) {
#ifdef ADC_TEENSY_4
// Teensy 4 is always single-ended
#else
    atomic::clearBitFlag(adc_regs.SC1A, ADC_SC1_DIFF);
#endif
  }
#if ADC_DIFF_PAIRS > 0
  //! Set differential conversion mode
  void differentialMode() __attribute__((always_inline)) {
    atomic::setBitFlag(adc_regs.SC1A, ADC_SC1_DIFF);
  }
#endif

  //! Use software to trigger the ADC, this is the most common setting
  void setSoftwareTrigger() __attribute__((always_inline)) {
#ifdef ADC_TEENSY_4
    atomic::clearBitFlag(adc_regs.CFG, ADC_CFG_ADTRG);
#else
    atomic::clearBitFlag(adc_regs.SC2, ADC_SC2_ADTRG);
#endif
  }

  //! Use hardware to trigger the ADC
  void setHardwareTrigger() __attribute__((always_inline)) {
#ifdef ADC_TEENSY_4
    atomic::setBitFlag(adc_regs.CFG, ADC_CFG_ADTRG);
#else
    atomic::setBitFlag(adc_regs.SC2, ADC_SC2_ADTRG);
#endif
  }

  ///@}

  ////////////// INFORMATION ABOUT THE STATE OF THE ADC /////////////////
  /** @name State of the ADC
   */
  ///@{

  //!
  /**
   * @brief Is the ADC converting at the moment?
   * @return true or false
   */
  volatile bool isConverting() __attribute__((always_inline)) {
#ifdef ADC_TEENSY_4
    return atomic::getBitFlag(adc_regs.GS, ADC_GS_ADACT);
#else
    return atomic::getBitFlag(adc_regs.SC2, ADC_SC2_ADACT);
#endif
  }

  /**
   * @brief Is an ADC conversion ready?
   *
   * When a value is read this function returns false until a new value exists,
   * so it only makes sense to call it before @ref analogReadContinuous() or
   * @ref readSingle()
   * @return true if yes, false if not.
   *
   */
  volatile bool isComplete() __attribute__((always_inline)) {
#ifdef ADC_TEENSY_4
    return atomic::getBitFlag(adc_regs.HS, ADC_HS_COCO0);
#else
    return atomic::getBitFlag(adc_regs.SC1A, ADC_SC1_COCO);
#endif
  }

#if ADC_DIFF_PAIRS > 0
  /**
   * @brief Is the ADC in differential mode?
   * @return true or false
   */
  volatile bool isDifferential() __attribute__((always_inline)) {
    return atomic::getBitFlag(adc_regs.SC1A, ADC_SC1_DIFF);
  }
#endif

  /**
   * @brief Is the ADC in continuous mode?
   * @return true or false
   */
  volatile bool isContinuous() __attribute__((always_inline)) {
#ifdef ADC_TEENSY_4
    return atomic::getBitFlag(adc_regs.GC, ADC_GC_ADCO);
#else
    return atomic::getBitFlag(adc_regs.SC3, ADC_SC3_ADCO);
#endif
  }

#ifdef ADC_USE_PGA
  /**
   * @brief Is the PGA function enabled?
   * @return true or false
   */
  volatile bool isPGAEnabled() __attribute__((always_inline)) {
    return atomic::getBitFlag(adc_regs.PGA, ADC_PGA_PGAEN);
  }
#endif

  ///@}

  //////////////// INFORMATION ABOUT VALID PINS //////////////////
  /** @name Valid pins
   */
  ///@{

  /**
   * @brief Check whether the pin is a valid analog pin
   * @param pin to check.
   * @return true if the pin is valid, false otherwise.
   */
  bool checkPin(uint8_t pin);

  //!
  /**
   * @brief Check whether the pins are a valid analog differential pair of pins
   *
   * If PGA is enabled it also checks that this ADCx can use PGA on this pins
   * @param pinP positive pin to check.
   * @param pinN negative pin to check.
   * @return true if the pin is valid, false otherwise.
   */
  bool checkDifferentialPins(uint8_t pinP, uint8_t pinN);

  ///@}

  //////////////// HELPER METHODS FOR CONVERSION /////////////////
  /** @name Conversion helper methods
   */
  ///@{

  /**
   * @brief Starts a single-ended conversion on the pin
   *
   * It sets the mux correctly, doesn't do any of the checks on the pin and
   * doesn't change the continuous conversion bit.
   * @param pin to read.
   */
  void startReadFast(uint8_t pin); // helper method

#if ADC_DIFF_PAIRS > 0
  /**
   * @brief Starts a differential conversion on the pair of pins
   *
   * It sets the mux correctly, doesn't do any of the checks on the pin and
   * doesn't change the continuous conversion bit.
   * @param pinP positive pin to read.
   * @param pinN negative pin to read.
   */
  void startDifferentialFast(uint8_t pinP, uint8_t pinN);
#endif

  ///@}

  //////////////// BLOCKING CONVERSION METHODS //////////////////
  /** @name Blocking conversion methods
   */
  ///@{

  /**
   * @brief Returns the analog value of the pin.
   *
   * It waits until the value is read and then returns the result.
   * If a comparison has been set up and fails, it will return ADC_ERROR_VALUE.
   * This function is interrupt safe, so it will restore the adc to the state it
   * was before being called
   * @param pin pin to read.
   * @return the analog value of the pin.
   */
  int analogRead(uint8_t pin);

  /**
   * @brief Returns the analog value of the special internal source, such as the
   * temperature sensor.
   * @param pin @ref ADC_settings::ADC_INTERNAL_SOURCE "ADC_INTERNAL_SOURCE" to
   * read.
   * @return the analog value of the pin.
   */
  int analogRead(ADC_INTERNAL_SOURCE pin) __attribute__((always_inline)) {
    return analogRead(static_cast<uint8_t>(pin));
  }

#if ADC_DIFF_PAIRS > 0
  /**
   * @brief Reads the differential analog value of two pins (pinP - pinN).
   *
   * It waits until the value is read and then returns the result.
   * If a comparison has been set up and fails, it will return
   * ADC_ERROR_DIFF_VALUE.
   *
   * This function is interrupt safe, so it will restore the adc to the state it
   * was before being called
   * @param pinP must be A10 or A12.
   * @param pinN must be A11 (if pinP=A10) or A13 (if pinP=A12).
   * @return the difference between the pins if they are valid, othewise returns
   * ADC_ERROR_DIFF_VALUE.
   */
  int analogReadDifferential(uint8_t pinP, uint8_t pinN);
#endif

  ///@}

  /////////////// NON-BLOCKING CONVERSION METHODS //////////////
  /** @name Non-blocking conversion methods
   */
  ///@{

  /**
   * @brief Starts an analog measurement on the pin.
   * It returns immediately, get value with @ref readSingle().
   * If this function interrupts a measurement, it stores the settings in
   * adc_config
   * @param pin pin to read.
   * @return true if the pin is valid, false otherwise.
   */
  bool startSingleRead(uint8_t pin);

#if ADC_DIFF_PAIRS > 0
  /**
   * @brief Start a differential conversion between two pins (pinP - pinN)
   * It returns immediately, get value with @ref readSingle().
   * If this function interrupts a measurement, it stores the settings in
   * adc_config
   * @param pinP must be A10 or A12.
   * @param pinN must be A11 (if pinP=A10) or A13 (if pinP=A12).
   * @return true if the pins are valid, false otherwise.
   */
  bool startSingleDifferential(uint8_t pinP, uint8_t pinN);
#endif

  /**
   * @brief Reads the analog value of a single conversion.
   * Set the conversion with with @ref startSingleRead(uint8_t pin) or
   * startSingleDifferential(uint8_t pinP, uint8_t pinN).
   * @return the converted value.
   */
  int readSingle() __attribute__((always_inline)) {
    return analogReadContinuous();
  }

  ///@}

  ///////////// CONTINUOUS CONVERSION METHODS ////////////
  /** @name Continuous conversion methods
   */
  ///@{

  /**
   * @brief Starts continuous conversion on the pin.
   *
   * It returns as soon as the ADC is set, use @ref analogReadContinuous() to
   * read the value.
   * @param pin can be any of the analog pins
   * @return true if the pin is valid, false otherwise.
   */
  bool startContinuous(uint8_t pin);

#if ADC_DIFF_PAIRS > 0
  /**
   * @brief Starts continuous conversion between the pins (pinP-pinN).
   *
   * It returns as soon as the ADC is set, use @ref analogReadContinuous() to
   * read the value.
   * @param pinP must be A10 or A12.
   * @param pinN must be A11 (if pinP=A10) or A13 (if pinP=A12).
   * @return true if the pins are valid, false otherwise.
   */
  bool startContinuousDifferential(uint8_t pinP, uint8_t pinN);
#endif

  /**
   * @brief Reads the analog value of a continuous conversion.
   *
   * If single-ended and 16 bits it's necessary to typecast it to an unsigned
   * type (like uint16_t), otherwise values larger than 3.3/2 V are interpreted
   * as negative!
   *
   * Set the continuous conversion with with @ref startContinuous(uint8_t pin)
   * or startContinuousDifferential(uint8_t pinP, uint8_t pinN) (if exists).
   * @return the last converted value.
   *
   */
  int analogReadContinuous() __attribute__((always_inline)) {
#ifdef ADC_TEENSY_4
    return (int16_t)(int32_t)adc_regs.R0;
#else
    return (int16_t)(int32_t)adc_regs.RA;
#endif
  }

  //! Stops continuous conversion
  void stopContinuous();

  ///@}

  //////////// FREQUENCY METHODS ////////
  // The general API is:
  // void startTimer(uint32_t freq)
  // void stopTimer()
  // uint32_t getTimerFrequency()
  // For each board the best timer method will be selected

  /** @name Frequency trigger methods
   */
  ///@{

//////////// PDB ////////////////
//// Only works for Teensy 3.x not LC nor Tensy 4.0 (they don't have PDB)
#if defined(ADC_USE_PDB)

  //! Start the default timer (PDB) triggering the ADC at the frequency
  /** The default timer in this board is the PDB, you can also call it directly
   * with startPDB(). Call startSingleRead or startSingleDifferential on the pin
   * that you want to measure before calling this function. See the example
   * adc_pdb.ino. \param freq is the frequency of the ADC conversion, it can't
   * be lower that 1 Hz
   */
  void startTimer(uint32_t freq) __attribute__((always_inline)) {
    startPDB(freq);
  }
  //! Start PDB triggering the ADC at the frequency
  /** Call startSingleRead or startSingleDifferential on the pin that you want
   * to measure before calling this function. See the example adc_pdb.ino.
   *   \param freq is the frequency of the ADC conversion, it can't be lower
   * that 1 Hz
   */
  void startPDB(uint32_t freq);

  //! Stop the default timer (PDB)
  void stopTimer() __attribute__((always_inline)) { stopPDB(); }
  //! Stop the PDB
  void stopPDB();

  //! Return the default timer's (PDB) frequency
  /** The default timer in this board is the PDB, you can also call it directly
   * with getPDBFrequency(). \return the timer's frequency in Hz.
   */
  uint32_t getTimerFrequency() __attribute__((always_inline)) {
    return getPDBFrequency();
  }
  //! Return the PDB's frequency
  /** Return the PDB's frequency
   *   \return the timer's frequency in Hz.
   */
  uint32_t getPDBFrequency();

//////////// TIMER ////////////////
//// Only works for Teensy 3.x and 4 (not LC)
#elif defined(ADC_USE_QUAD_TIMER)
  //! Start the default timer (QuadTimer) triggering the ADC at the frequency
  /** The default timer in this board is the QuadTimer, you can also call it
   * directly with startQuadTimer(). Call startSingleRead or
   * startSingleDifferential on the pin that you want to measure before calling
   * this function. See the example adc_timer.ino. \param freq is the frequency
   * of the ADC conversion, it can't be lower that 1 Hz
   */
  void startTimer(uint32_t freq) __attribute__((always_inline)) {
    startQuadTimer(freq);
  }
  //! Start a Quad timer to trigger the ADC at the frequency
  /** Call startSingleRead or startSingleDifferential on the pin that you want
   * to measure before calling this function. See the example adc_timer.ino.
   *   \param freq is the frequency of the ADC conversion, it can't be lower
   * that 1 Hz
   */
  void startQuadTimer(uint32_t freq);

  //! Stop the default timer (QuadTimer)
  void stopTimer() __attribute__((always_inline)) { stopQuadTimer(); }
  //! Stop the Quad timer
  void stopQuadTimer();

  //! Return the default timer's (QuadTimer) frequency
  /** The default timer in this board is the QuadTimer, you can also call it
   * directly with getQuadTimerFrequency(). \return the timer's frequency in Hz.
   */
  uint32_t getTimerFrequency() __attribute__((always_inline)) {
    return getQuadTimerFrequency();
  }
  //! Return the Quad timer's frequency
  /** Return the Quad timer's frequency
   *   \return the timer's frequency in Hz.
   */
  uint32_t getQuadTimerFrequency();
#endif

  ///@}

  //////// OTHER STUFF ///////////
  /** @name Other methods and variables
   */
  ///@{

  //! Store the config of the adc
  struct ADC_Config {
#ifdef ADC_TEENSY_4
    uint32_t savedHC0; /**< HC0. */
    uint32_t savedCFG; /**< CFG. */
    uint32_t savedGC;  /**< GC. */
    uint32_t savedGS;  /**< GS. */
#else
    uint32_t savedSC1A; /**< SC1A. */
    uint32_t savedSC2;  /**< SC2. */
    uint32_t savedSC3;  /**< SC3. */
    uint32_t savedCFG1; /**< CFG1. */
    uint32_t savedCFG2; /**< CFG2. */
#endif
  } adc_config; /**< Struct with all relevant ADC configs. */

  //! Was the adc in use before a call?
  uint8_t adcWasInUse;

  /** Save config of the ADC to the ADC_Config struct
   * \param config ADC_Config where the config will be stored
   */
  void saveConfig(ADC_Config *config) {
#ifdef ADC_TEENSY_4
    config->savedHC0 = adc_regs.HC0;
    config->savedCFG = adc_regs.CFG;
    config->savedGC = adc_regs.GC;
    config->savedGS = adc_regs.GS;
#else
    config->savedSC1A = adc_regs.SC1A;
    config->savedCFG1 = adc_regs.CFG1;
    config->savedCFG2 = adc_regs.CFG2;
    config->savedSC2 = adc_regs.SC2;
    config->savedSC3 = adc_regs.SC3;
#endif
  }

  /** Load config to the ADC
   * \param config ADC_Config from where the config will be loaded
   */
  void loadConfig(const ADC_Config *config) {
#ifdef ADC_TEENSY_4
    adc_regs.HC0 = config->savedHC0;
    adc_regs.CFG = config->savedCFG;
    adc_regs.GC = config->savedGC;
    adc_regs.GS = config->savedGS;
#else
    adc_regs.CFG1 = config->savedCFG1;
    adc_regs.CFG2 = config->savedCFG2;
    adc_regs.SC2 = config->savedSC2;
    adc_regs.SC3 = config->savedSC3;
    adc_regs.SC1A = config->savedSC1A; // restore last
#endif
  }

  //! Number of measurements that the ADC is performing
  uint8_t num_measurements;

  //! This flag indicates that some kind of error took place
  /** Use the defines at the beginning of this file to find out what caused the
   * fail.
   */
  volatile ADC_ERROR fail_flag;

  //! Resets all errors from the ADC, if any.
  void resetError() { ADC_Error::resetError(fail_flag); }

  //! Which adc is this?
  const uint8_t ADC_num;

  ///@}

private:
  // is set to 1 when the calibration procedure is taking place
  uint8_t calibrating;

  // the first calibration will use 32 averages and lowest speed,
  // when this calibration is over the averages and speed will be set to
  // default.
  uint8_t init_calib;

  // resolution
  uint8_t analog_res_bits;

  // maximum value possible 2^res-1
  uint32_t analog_max_val;

  // num of averages
  uint8_t analog_num_average;

  // reference can be internal or external
  ADC_REF_SOURCE analog_reference_internal;

#ifdef ADC_USE_PGA
  // value of the pga
  uint8_t pga_value;
#endif

  // conversion speed
  ADC_CONVERSION_SPEED conversion_speed;

  // sampling speed
  ADC_SAMPLING_SPEED sampling_speed;

  // translate pin number to SC1A nomenclature
  const uint8_t *const channel2sc1a;

  // are interrupts on?
  bool interrupts_enabled;

// same for differential pins
#if ADC_DIFF_PAIRS > 0
  const ADC_NLIST *const diff_table;

  //! Get the SC1A value of the differential pair for this pin
  uint8_t getDifferentialPair(uint8_t pin) {
    for (uint8_t i = 0; i < ADC_DIFF_PAIRS; i++) {
      if (diff_table[i].pin == pin) {
        return diff_table[i].sc1a;
      }
    }
    return ADC_SC1A_PIN_INVALID;
  }
#endif

  //! Initialize ADC
  void analog_init();

  //! Switch on clock to ADC
  void startClock() {
#if defined(ADC_TEENSY_4)
    if (ADC_num == 0) {
      CCM_CCGR1 |= CCM_CCGR1_ADC1(CCM_CCGR_ON);
    } else {
      CCM_CCGR1 |= CCM_CCGR1_ADC2(CCM_CCGR_ON);
    }
#else
    if (ADC_num == 0) {
      SIM_SCGC6 |= SIM_SCGC6_ADC0;
    } else {
      SIM_SCGC3 |= SIM_SCGC3_ADC1;
    }
#endif
  }

  // registers point to the correct ADC module
  typedef volatile uint32_t &reg;

  // registers that control the adc module
  ADC_REGS_t &adc_regs;

#ifdef ADC_USE_PDB
  reg PDB0_CHnC1; // PDB channel 0 or 1
#endif
#ifdef ADC_TEENSY_4
  uint8_t XBAR_IN;
  uint8_t XBAR_OUT;
  uint8_t QTIMER4_INDEX;
  uint8_t ADC_ETC_TRIGGER_INDEX;
#endif
  const IRQ_NUMBER_t IRQ_ADC; // IRQ number

protected:
};

#endif // ADC_MODULE_H
