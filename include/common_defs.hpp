#pragma once
/*
Common structures
*/

#include "Arduino.h"

namespace adc {

enum class board_t : uint8_t { TEENSY_4, TEENSY_3, TEENSY_LC };

template <board_t board, uint8_t adc_num> struct pin_info_t {
  static_assert(0 <= adc_num && adc_num <= 1, "invalid adc_num");
};

template <board_t board, int adc_num> struct adc_base_addr {
  static_assert(0 <= adc_num && adc_num <= 1, "invalid adc_num");
};

template <board_t board, int adc_num> struct adc_module_reg_t {
  static_assert(0 <= adc_num && adc_num <= 1, "invalid adc_num");
};

}; // namespace adc
