#pragma once
/**
Specializations for Teensy 4.0
*/

#include "common_defs.hpp"

namespace adc {

// ADC0

template <> struct adc_base_addr<board_t::TEENSY_4_0, 0> {
  static constexpr address_t value = 0x400C4000;
};

template <> struct pin_info_t<board_t::TEENSY_4_0, 0> {
  static constexpr uint8_t num_pins = 12;

  enum class pin_t : uint8_t {
    A0 = 7,
    A1 = 8,
    A2 = 12,
    A3 = 11,
    A4 = 6,
    A5 = 5,
    A6 = 15,
    A7 = 0,
    A8 = 13,
    A9 = 14,
    A10 = 1,
    A11 = 2,
  };

  static constexpr pin_t pins[num_pins] = {
      pin_t::A0, pin_t::A1, pin_t::A2, pin_t::A3, pin_t::A4,  pin_t::A5,
      pin_t::A6, pin_t::A7, pin_t::A8, pin_t::A9, pin_t::A10, pin_t::A11};

  static constexpr uint8_t pin2int[] = {7, 10,  11,  255, 255, 5, 4, 0,
                                        1, 255, 255, 3,   2,   8, 9};
};
// make linker happy
constexpr uint8_t pin_info_t<board_t::TEENSY_4_0, 0>::pin2int[];
constexpr pin_info_t<board_t::TEENSY_4_0, 0>::pin_t
    pin_info_t<board_t::TEENSY_4_0, 0>::pins[];

// ADC1

template <> struct adc_base_addr<board_t::TEENSY_4_0, 1> {
  static constexpr address_t value = 0x400C8000;
};

template <> struct pin_info_t<board_t::TEENSY_4_0, 1> {
  static constexpr uint8_t num_pins = 12;

  enum class pin_t : uint8_t {
    A0 = 7,
    A1 = 8,
    A2 = 12,
    A3 = 11,
    A4 = 6,
    A5 = 5,
    A6 = 15,
    A7 = 0,
    A8 = 13,
    A9 = 14,
    A12 = 3,
    A13 = 4,
  };
  static constexpr pin_t pins[num_pins] = {
      pin_t::A0, pin_t::A1, pin_t::A2, pin_t::A3, pin_t::A4,  pin_t::A5,
      pin_t::A6, pin_t::A7, pin_t::A8, pin_t::A9, pin_t::A12, pin_t::A13};

  static constexpr uint8_t pin2int[] = {7, 255, 255, 12, 13, 5, 4, 0,
                                        1, 255, 255, 3,  2,  8, 9};
};
// make linker happy
constexpr uint8_t pin_info_t<board_t::TEENSY_4_0, 1>::pin2int[];
constexpr pin_info_t<board_t::TEENSY_4_0, 1>::pin_t
    pin_info_t<board_t::TEENSY_4_0, 1>::pins[];

}; // namespace adc
