#pragma once

#include "adc_module.hpp"
#include "common_defs.hpp"

namespace adc {

#ifdef __IMXRT1062__
static constexpr board_t board = board_t::TEENSY_4;
#endif

using adc0 = adc_module_t<board, 0>;
using adc1 = adc_module_t<board, 1>;

}; // namespace adc
