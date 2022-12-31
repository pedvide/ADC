#pragma once

#include "common_defs.hpp"
#include <type_traits>

namespace adc {

template <board_t board, uint8_t adc_num> struct adc_module_t {
  using adc_module_reg = adc_module_reg_t<board, adc_num>;

  using pin_info = pin_info_t<board, adc_num>;
  using pin_t = typename pin_info::pin_t;
  using diff_pin_t = typename pin_info::diff_pin_t;
  using traits = traits_t<board>;

  static constexpr uint8_t pin2int(pin_t pin) {
    return pin_info::pin2int[static_cast<uint8_t>(pin)];
  }

  static void init() {}

  static void start_measurement(pin_t pin) {
    adc_module_reg::adch::write(static_cast<uint8_t>(pin));
  }

  static value_t read_measurement() { return adc_module_reg::value0::read(); }

  static bool measurement_ready() { return adc_module_reg::coco0::read() == 1; }

  // continuous vs single-shot
  // sync (wait for value) vs async (return inmediately)
  template <typename return_policy_t, typename shot_policy_t,
            typename ended_policy_t>
  static int measure(pin_t pin) {
    ended_policy_t::single_or_differential();
    shot_policy_t::one_or_continuous();
    start_measurement(pin);
    return return_policy_t::return_or_wait();
  }

  static int analogRead(pin_t pin) {
    return measure<wait_for_measurement_t, single_shot_t, single_ended_t>(pin);
  }

  template <typename traits = traits_t<board>>
  static typename std::enable_if<traits::has_differential, void>::type
  differentialMode() {
    adc_module_reg::diff::set();
    return;
  }

  template <typename traits = traits_t<board>>
  static typename std::enable_if<traits::has_differential, int>::type
  analogReadDifferential(diff_pin_t pin) {
    return measure<wait_for_measurement_t, single_shot_t, differential_t>(
        static_cast<pin_t>(pin));
  }

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
};

}; // namespace adc
