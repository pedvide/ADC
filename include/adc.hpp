#pragma once

#include "adc_module.hpp"
#include "common_defs.hpp"

// Easier names for the boards
#if defined(__MK20DX128__) // Teensy 3.0
#include "teensy3x.hpp"
#elif defined(__MK20DX256__) // Teensy 3.1/3.2
#include "teensy3x.hpp"
#elif defined(__MKL26Z64__)  // Teensy LC
#elif defined(__MK64FX512__) // Teensy 3.5
#elif defined(__MK66FX1M0__) // Teensy 3.6
#elif defined(__IMXRT1062__) // Teensy 4.0/4.1
#include "teensy4x.hpp"
#else
#error "Board not supported!"
#endif

/**
 * @brief Main ADC namespace, contains everything in this library
 *
 */
namespace adc {

#if defined(__MK20DX128__) // Teensy 3.0
static constexpr board_t board = board_t::TEENSY_3_0;
#elif defined(__MK20DX256__) // Teensy 3.1/3.2
static constexpr board_t board = board_t::TEENSY_3_2;
#elif defined(__MKL26Z64__)  // Teensy LC
static constexpr board_t board = board_t::TEENSY_LC;
#elif defined(__MK64FX512__) // Teensy 3.5
static constexpr board_t board = board_t::TEENSY_3_5;
#elif defined(__MK66FX1M0__) // Teensy 3.6
static constexpr board_t board = board_t::TEENSY_3_6;
#elif defined(__IMXRT1062__) // Teensy 4.0/4.1
#ifdef ARDUINO_TEENSY41
static constexpr board_t board = board_t::TEENSY_4_1;
#else
static constexpr board_t board = board_t::TEENSY_4_0;
#endif
#else
#error "Board not supported!"
#endif

//! ADC0 module
using adc0 = adc_module_t<board, 0>;
#if !defined(__MK20DX128__)
//! ADC1 module
using adc1 = adc_module_t<board, 1>;
#endif

}; // namespace adc
