#pragma once

#include "common_defs.hpp"
#include <type_traits>

namespace adc {

/**
 * @brief ADC module base functionality
 *
 * @tparam board
 * @tparam adc_num
 */
template <board_t board, uint8_t adc_num> struct adc_module_base_t {

  //! \cond internal
  using adc_module_reg = adc_module_reg_t<board, adc_num>;
  using pin_info = pin_info_t<board, adc_num>;
  using traits = traits_t<board>;
  //! \endcond
  //! Pins available to this module
  using pin_t = typename pin_info::pin_t;

  /**
   * @brief Translate between pin_t and its number
   *  eg: pin_t::A10 -> A10
   * @param pin
   * @return the Arduino pin number
   */
  static constexpr uint8_t pin2int(pin_t pin) {
    return pin_info::pin2int[static_cast<uint8_t>(pin)];
  }

  /**
   * @brief Initialize the adc module
   */
  static void init() {}

  /**
   * @brief Start a measurement on the pin
   *
   * @param pin
   */
  static void start_measurement(pin_t pin) {
    adc_module_reg::adch::write(static_cast<uint8_t>(pin));
  }

  /**
   * @brief Read a measurement
   *
   * @return ADC value
   */
  static value_t read_measurement() { return adc_module_reg::value0::read(); }

  /**
   * @brief Is a new measurement ready?
   *
   * @return true of ready
   */
  static bool measurement_ready() { return adc_module_reg::coco0::read() == 1; }

  //! \cond internal
  /**
   * @brief Make a measurement on the pin using the different strategies
   *
   * @tparam return_policy_t Wait for value or return inmediately?
   * @tparam shot_policy_t One measurement or continuous?
   * @tparam ended_policy_t Single-ended or differential?
   * @param pin
   * @return Depending on return_policy_t either the value at the pin or nothing
   */
  template <typename return_policy_t, typename shot_policy_t,
            typename ended_policy_t>
  static auto measure(pin_t pin)
      -> decltype(return_policy_t::return_or_wait()) {
    ended_policy_t::single_or_differential();
    shot_policy_t::one_or_continuous();
    start_measurement(pin);
    return return_policy_t::return_or_wait();
  }
  //! \endcond

  //! \name Measurement functions
  //!@{

  /**
   * @brief Start one single-ended measurement and return it
   *
   * @param pin
   * @return The value
   */
  static int analogRead(pin_t pin) {
    return measure<wait_for_measurement_t, one_shot_t, single_ended_t>(pin);
  }

  //!@}

  //! \cond internal
  struct wait_for_measurement_t {
    static value_t return_or_wait() {
      while (!measurement_ready()) {
        yield();
      }
      return read_measurement();
    }
  };
  struct return_inmediately_t {
    static void return_or_wait() { return; }
  };

  struct one_shot_t {
    static void one_or_continuous() { adc_module_reg::adco::clear(); }
  };
  struct continuous_shot_t {
    static void one_or_continuous() { adc_module_reg::adco::set(); }
  };

  // TODO: simplify with `if constexpr` with next teensy update to gcc 11
  struct single_ended_t {
    template <typename traits = traits_t<board>,
              std::enable_if_t<traits::differential> * = nullptr>
    static void single_or_differential() {
      adc_module_reg::diff::clear();
    }
    template <typename traits = traits_t<board>,
              std::enable_if_t<!traits::differential> * = nullptr>
    static void single_or_differential() {}
  };

  //! \endcond
};

//! \cond internal
/**
 * @brief Empty implementation for boards without differential
 *
 * @tparam adc_module_t
 * @tparam enable
 */
template <typename adc_module_t, bool enable>
struct adc_differential_t : adc_module_t {};
//! \endcond

/**
 * @brief Implementation of differential mode
 *
 * @tparam adc_module_t
 */
template <typename adc_module_t>
struct adc_differential_t<adc_module_t, true> : adc_module_t {

  //! Pins available to this module
  using diff_pin_t = typename adc_module_t::pin_info::diff_pin_t;

  /**
   * @brief Start one differential measurement and return it
   *
   * @param pin
   * @return The value
   */
  static int analogReadDifferential(diff_pin_t pin) {
    return adc_module_t::template measure<
        typename adc_module_t::wait_for_measurement_t,
        typename adc_module_t::one_shot_t, differential_t>(
        static_cast<typename adc_module_t::pin_t>(pin));
  }

  //! \cond internal
  //! Policy for differential mode.
  struct differential_t {
    static void single_or_differential() {
      adc_module_t::adc_module_reg::diff::set();
    }
  };
  //! \endcond
};

//! \cond internal
/**
 * @brief Empty implementation for boards without differential
 *
 * @tparam adc_module_t
 * @tparam enable
 */
template <typename adc_module_t, bool enable>
struct adc_pga_t : adc_module_t {};
//! \endcond

/**
 * @brief Implementation of PGA
 *
 * @tparam adc_module_t
 */
template <typename adc_module_t>
struct adc_pga_t<adc_module_t, true> : adc_module_t {

  /**
   * @brief Enables the PGA and sets the gain
   *
   *  Use only for signals lower than 1.2 V and only in differential mode
   *  @param gain can be 1, 2, 4, 8, 16, 32 or 64
   */
  static void enablePGA(uint8_t gain) {
    uint8_t setting;
    if (gain <= 1) {
      setting = 0;
    } else if (gain <= 2) {
      setting = 1;
    } else if (gain <= 4) {
      setting = 2;
    } else if (gain <= 8) {
      setting = 3;
    } else if (gain <= 16) {
      setting = 4;
    } else if (gain <= 32) {
      setting = 5;
    } else { // 64
      setting = 6;
    }
    adc_module_t::adc_module_reg::pgaen::set();
    adc_module_t::adc_module_reg::pgag::write(setting);
  }

  /**
   * @brief Returns the PGA level
   * @return PGA level from 1 to 64
   */
  static uint8_t getPGA() {
    const uint8_t setting = adc_module_t::adc_module_reg::pgag::read();
    if (setting == 0) {
      return 1;
    } else if (setting == 1) {
      return 2;
    } else if (setting == 2) {
      return 4;
    } else if (setting == 3) {
      return 8;
    } else if (setting == 4) {
      return 16;
    } else if (setting == 5) {
      return 32;
    } else if (setting == 6) {
      return 64;
    }
    return 0;
  }

  /**
   * @brief Is the PGA function enabled?
   * @return true or false
   */
  static volatile bool isPGAEnabled() {
    return adc_module_t::adc_module_reg::pgaen::read();
  }

  //! Disable PGA
  static void disablePGA() { adc_module_t::adc_module_reg::pgaen::clear(); }
};

/**
 * @brief ADC module
 *
 * @tparam board
 * @tparam adc_num
 */
template <board_t board, uint8_t adc_num>
struct adc_module_t
    : adc_pga_t<adc_module_base_t<board, adc_num>, traits_t<board>::pga>,
      adc_differential_t<adc_module_base_t<board, adc_num>,
                         traits_t<board>::differential> {};

}; // namespace adc
