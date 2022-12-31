#pragma once

#include "common_defs.hpp"
#include <type_traits>

namespace adc {

/**
 * @brief ADC module
 *
 * @tparam board
 * @tparam adc_num
 */
template <board_t board, uint8_t adc_num> struct adc_module_t {

  //! \cond internal
  using adc_module_reg = adc_module_reg_t<board, adc_num>;
  using pin_info = pin_info_t<board, adc_num>;
  //! \cond differential
  using diff_pin_t = typename pin_info::diff_pin_t;
  //! \endcond
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
  static int measure(pin_t pin) {
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
    return measure<wait_for_measurement_t, single_shot_t, single_ended_t>(pin);
  }

  //! \cond differential
  /**
   * @brief Start one differential measurement and return it
   *
   * @param pin
   * @return The value
   */
  static int analogReadDifferential(diff_pin_t pin) {
    static_assert(traits::has_differential,
                  "This board doesn't have differential capabilities");
    return measure<wait_for_measurement_t, single_shot_t, differential_t>(
        static_cast<pin_t>(pin));
  }
  //! \endcond

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

  struct single_shot_t {
    static void one_or_continuous() { adc_module_reg::adco::clear(); }
  };
  struct continuous_shot_t {
    static void one_or_continuous() { adc_module_reg::adco::set(); }
  };

  struct single_ended_t {
    static void single_or_differential() { adc_module_reg::diff::clear(); }
  };

  struct differential_t {
    template <typename traits = traits_t<board>>
    static typename std::enable_if<traits::has_differential, void>::type
    single_or_differential() {
      adc_module_reg::diff::set();
    }
  };
  //! \endcond
};

}; // namespace adc
