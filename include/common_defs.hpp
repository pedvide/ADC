#pragma once
/*
Common structures
*/

#include "Arduino.h"

namespace adc {

using address_t = uint32_t;
using value_t = uint32_t;

enum class board_t : uint8_t {
  TEENSY_3_0,
  TEENSY_3_2,
  TEENSY_LC,
  TEENSY_3_5,
  TEENSY_3_6,
  TEENSY_4_0,
  TEENSY_4_1
};

template <board_t board, uint8_t adc_num> struct pin_info_t {
  static_assert(adc_num <= 1, "invalid adc_num");
};
template <uint8_t adc_num> struct pin_info_t<board_t::TEENSY_3_0, adc_num> {
  static_assert(adc_num == 0, "invalid adc_num");
};

template <board_t board> struct traits_t {
  static constexpr bool has_differential = false;
};

template <board_t board, uint8_t adc_num> struct adc_base_addr {
  static_assert(adc_num <= 1, "invalid adc_num");
};
template <uint8_t adc_num> struct adc_base_addr<board_t::TEENSY_3_0, adc_num> {
  static_assert(adc_num == 0, "invalid adc_num");
};

template <board_t board, uint8_t adc_num, typename = void>
struct adc_module_reg_t {
  static_assert(adc_num <= 1, "invalid adc_num");
};

}; // namespace adc
