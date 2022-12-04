#include "adc_registers.hpp"

namespace adc {

template <uint8_t adc_num> struct pin_info_t {
  static_assert(0 <= adc_num && adc_num <= 1, "invalid adc_num");
};

template <> struct pin_info_t<0> {
  enum class pin_t : uint8_t {
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
    begin = A0,
    end = A11
  };

  static constexpr uint8_t channel2sc1a[] = {
      7, 8, 12, 11, 6, 5, 15, 0, 13, 14, // A0-A9
      1, 2                               // A10-A11
  };
};
// make linker happy
constexpr uint8_t adc::pin_info_t<0>::channel2sc1a[];

template <> struct pin_info_t<1> {
  enum class pin_t : uint8_t {
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
    A12 = PIN_A12,
    A13,
    begin = A0,
    end = A13
  };

  static constexpr uint8_t channel2sc1a[] = {
      7,  8,  12, 11, 6, 5, 15, 0, 13, 14, // A0-A9
      31, 31, 3,  4                        // A10, A11, A12, A13
  };
};
// make linker happy
constexpr uint8_t adc::pin_info_t<1>::channel2sc1a[];

template <uint8_t adc_num> struct adc_module_t {
  static void init() {}

  using pin_t = typename pin_info_t<adc_num>::pin_t;

  static constexpr uint8_t const *channel2sc1a =
      pin_info_t<adc_num>::channel2sc1a;

  static bool start_measurement(pin_t pin) {
    const uint8_t sc1a_channel =
        channel2sc1a[static_cast<uint8_t>(pin) -
                     static_cast<uint8_t>(pin_t::begin)];
    if (sc1a_channel == 31) {
      return false;
    }
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
    if (!start_measurement(pin)) {
      return -1;
    }
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
