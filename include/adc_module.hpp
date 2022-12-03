#include "adc_registers.hpp"

namespace adc {
enum class pin_t { A0, A1, A2 };

template <int adc_num> struct adc_module_t {
  static void init() {}

  static bool start_measurement(pin_t pin) { return true; }

  static value_t read_measurement() {
    return adc_module_reg_t<adc_num>::r0::value::read();
  }

  static bool measurement_ready() {
    return adc_module_reg_t<adc_num>::hs::coco0::read() == 1;
  }

  // sync (wait for value) vs async (return inmediately)
  // continuous vs sinlge-shot
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
        // yield();
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
