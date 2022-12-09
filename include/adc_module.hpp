#pragma once

#include "common_defs.hpp"

namespace adc {

template <board_t board, uint8_t adc_num> struct adc_module_t {
  using adc_module_reg = adc_module_reg_t<board, adc_num>;

  using pin_info = pin_info_t<board, adc_num>;
  using pin_t = typename pin_info::pin_t;

  static void init() {}

  static void start_measurement(pin_t pin) {
    const uint8_t sc1a_channel =
        pin_info::channel2sc1a[static_cast<uint8_t>(pin)];
    adc_module_reg::adch::write(sc1a_channel);
  }

  static value_t read_measurement() { return adc_module_reg::value0::read(); }

  static bool measurement_ready() { return adc_module_reg::coco0::read() == 1; }

  // continuous vs single-shot
  // sync (wait for value) vs async (return inmediately)
  template <typename return_policy_t, typename shot_policy_t>
  static int measure(pin_t pin) {
    shot_policy_t::single_or_continuous();
    start_measurement(pin);
    return return_policy_t::return_or_wait();
  }

  static int analogRead(pin_t pin) {
    return measure<wait_for_measurement_t, single_shot_t>(pin);
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
    static void single_or_continuous() {
      // set single shot
    }
  };
  struct continuous_shot_t {
    static void single_or_continuous() {
      // set continuous shot
    }
  };
};

}; // namespace adc
