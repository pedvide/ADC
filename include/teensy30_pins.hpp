#pragma once
/**
Specializations for Teensy 3.0
*/

#include "common_defs.hpp"

namespace adc {

template <> struct traits_t<board_t::TEENSY_3_0> {
  static constexpr bool has_differential = true;
};

template <> struct pin_info_t<board_t::TEENSY_3_0, 0> {
  static constexpr uint8_t num_pins = 12;

  enum class pin_t : uint8_t {
    A0 = 5,
    A1 = 14,
    A2 = 8,
    A3 = 9,
    A4 = 13,
    A5 = 12,
    A6 = 6,
    A7 = 7,
    A8 = 15,
    A9 = 4,
    A10 = 0,
    A11 = 19,
    A12 = 3,
    A13 = 21,
    // Other sources
    TEMP_SENSOR = 26,
    VREF_OUT = 22,
    BANDGAP = 27,
    VREFH = 29,
    VREFL = 30
  };

  enum class diff_pin_t : uint8_t { A10_11 = 0, A12_13 = 3 };

  static constexpr pin_t pins[] = {
      pin_t::A0,  pin_t::A1,  pin_t::A2,  pin_t::A3, pin_t::A4,
      pin_t::A5,  pin_t::A6,  pin_t::A7,  pin_t::A8, pin_t::A9,
      pin_t::A10, pin_t::A11, pin_t::A12, pin_t::A13};

  static constexpr uint8_t pin2int[] = {10,  255, 255, 12,  9,   0, 6, 7,
                                        2,   3,   255, 255, 5,   4, 1, 8,
                                        255, 255, 255, 11,  255, 13};
};
// make linker happy
constexpr uint8_t pin_info_t<board_t::TEENSY_3_0, 0>::pin2int[];
constexpr pin_info_t<board_t::TEENSY_3_0, 0>::pin_t
    pin_info_t<board_t::TEENSY_3_0, 0>::pins[];

}; // namespace adc
