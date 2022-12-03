#include "adc_registers.hpp"

template <std::size_t N, class T> constexpr std::size_t countof(T (&)[N]) {
  return N;
}

namespace adc {
enum class pin_t {
  A0 = PIN_A0,
  A1,
  A2,
  A3,
  A4,
  A5,
  A6,
  A7,
  A8,
  A9,
  A10,
  A11,
  A12,
  A13
};

const uint8_t channel2sc1aADC0[] = {
    7, 8, 12, 11, 6, 5, 15, 0, 13, 14, // 0-9, we treat them as A0-A9
    1, 2, 31, 31,                      // 9-13, we treat them as A9-A13
};

template <int adc_num> struct adc_module_t {
  static void init() {}

  static bool start_measurement(pin_t pin) {
    const uint8_t sc1a_channel = channel2sc1aADC0[static_cast<uint8_t>(pin)];
    adc_module_reg_t<adc_num>::hc0::adch::write(sc1a_channel);

    return true;
  }

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

  static int analogRead(uint8_t pin) {
    return measure<wait_for_measurement_t, single_shot_t>(
        static_cast<pin_t>(pin - static_cast<uint8_t>(pin_t::A0)));
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
