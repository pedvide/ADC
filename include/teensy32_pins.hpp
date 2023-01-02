#pragma once
/**
Specializations for Teensy 3.2
*/

#include "common_defs.hpp"

namespace adc {

template <> struct traits_t<board_t::TEENSY_3_2> {
  static constexpr bool differential = false;
};

// ADC0

template <> struct adc_base_addr<board_t::TEENSY_3_2, 0> {
  static constexpr address_t value = 0x4003B000;
};

template <> struct pin_info_t<board_t::TEENSY_3_2, 0> {
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
    // A13 not connected as a single-ended pin
    A14 = 23,
    // Other sources
    TEMP_SENSOR = 26,
    VREF_OUT = 22,
    BANDGAP = 27,
    VREFH = 29,
    VREFL = 30
  };

  static constexpr pin_t pins[] = {
      pin_t::A0,  pin_t::A1,  pin_t::A2,  pin_t::A3, pin_t::A4,
      pin_t::A5,  pin_t::A6,  pin_t::A7,  pin_t::A8, pin_t::A9,
      pin_t::A10, pin_t::A11, pin_t::A12, pin_t::A14};

  static constexpr uint8_t pin2int[] = {10,  255, 255, 12,  9,   0,   6,   7,
                                        2,   3,   255, 255, 5,   4,   1,   8,
                                        255, 255, 255, 11,  255, 255, 255, 14};
};
// make linker happy
constexpr uint8_t pin_info_t<board_t::TEENSY_3_2, 0>::pin2int[];
constexpr pin_info_t<board_t::TEENSY_3_2, 0>::pin_t
    pin_info_t<board_t::TEENSY_3_2, 0>::pins[];

}; // namespace adc
