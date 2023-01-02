#pragma once
/**
Specializations for Teensy 3.0
*/

#include "common_defs.hpp"

namespace adc {

template <> struct traits_t<board_t::TEENSY_3_0> {
  static constexpr bool differential = true;
  static constexpr bool pga = true;
};

template <> struct adc_base_addr<board_t::TEENSY_3_0, 0> {
  static constexpr address_t value = 0x4003B000;
};

/**
 * @brief Information about the pins available to this module
 *
 */
template <> struct pin_info_t<board_t::TEENSY_3_0, 0> {
  static constexpr uint8_t num_pins = 12;

  /**
   * @brief Pins available to this module
   */
  enum class pin_t : uint8_t {
    A0 = 5,   ///< A0
    A1 = 14,  ///< A1
    A2 = 8,   ///< A2
    A3 = 9,   ///< A3
    A4 = 13,  ///< A4
    A5 = 12,  ///< A5
    A6 = 6,   ///< A6
    A7 = 7,   ///< A7
    A8 = 15,  ///< A8
    A9 = 4,   ///< A9
    A10 = 0,  ///< A10
    A11 = 19, ///< A11
    A12 = 3,  ///< A12
    A13 = 21, ///< A13
    // Other sources
    TEMP_SENSOR = 26, ///< Temperature sensor
    VREF_OUT = 22,    ///< Voltage reference
    BANDGAP = 27,     ///< Bandgap
    VREFH = 29,       ///< Voltage reference high
    VREFL = 30        ///< Voltage reference low
  };

  //! Differential pins
  enum class diff_pin_t : uint8_t {
    A10_11 = 0, ///< Differential A10-A11
    A12_13 = 3  ///< Differential A12-A13
  };

  //! List of pins
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
