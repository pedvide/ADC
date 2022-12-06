#include "adc_registers.hpp"

namespace adc {

template <uint8_t adc_num> struct pin_info_t {
  static_assert(0 <= adc_num && adc_num <= 1, "invalid adc_num");
};

template <> struct pin_info_t<0> {
  static constexpr uint8_t num_pins = 12;

  enum class pin_t : uint8_t {
    A0 = 0,
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
  };

  static constexpr pin_t pins[num_pins] = {
      pin_t::A0, pin_t::A1, pin_t::A2, pin_t::A3, pin_t::A4,  pin_t::A5,
      pin_t::A6, pin_t::A7, pin_t::A8, pin_t::A9, pin_t::A10, pin_t::A11};

  static constexpr uint8_t channel2sc1a[num_pins] = {
      7, 8, 12, 11, 6, 5, 15, 0, 13, 14, // A0-A9
      1, 2                               // A10-A11
  };
};
// make linker happy
constexpr uint8_t adc::pin_info_t<0>::channel2sc1a[];
constexpr adc::pin_info_t<0>::pin_t adc::pin_info_t<0>::pins[];

template <> struct pin_info_t<1> {
  static constexpr uint8_t num_pins = 12;

  enum class pin_t : uint8_t {
    A0 = 0,
    A1,
    A2,
    A3,
    A4,
    A5,
    A6,
    A7,
    A8,
    A9,
    A12,
    A13,
  };
  static constexpr pin_t pins[num_pins] = {
      pin_t::A0, pin_t::A1, pin_t::A2, pin_t::A3, pin_t::A4,  pin_t::A5,
      pin_t::A6, pin_t::A7, pin_t::A8, pin_t::A9, pin_t::A12, pin_t::A13};

  static constexpr uint8_t channel2sc1a[num_pins] = {
      7, 8, 12, 11, 6, 5, 15, 0, 13, 14, // A0-A9
      3, 4                               // A12, A13
  };
};
// make linker happy
constexpr uint8_t adc::pin_info_t<1>::channel2sc1a[];
constexpr adc::pin_info_t<1>::pin_t adc::pin_info_t<1>::pins[];

template <uint8_t adc_num> struct adc_module_t {
  using adc_module_reg = adc_module_reg_t<adc_num>;

  using pin_info = pin_info_t<adc_num>;
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
