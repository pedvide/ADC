#pragma once
/**
Specializations for Teensy 3.x: 3.0, 3.1, 3.2, 3.4, 3.5
*/

#include "common_defs.hpp"
#include "register.hpp"
#include "teensy30_pins.hpp"
#include "teensy32_pins.hpp"

namespace adc {

template <board_t board, int adc_num>
struct adc_module_reg_t<
    board, adc_num,
    typename std::enable_if<board == board_t::TEENSY_3_0 ||
                            board == board_t::TEENSY_3_2>::type> {
  static_assert((board == board_t::TEENSY_3_0 && adc_num == 0) ||
                    (board == board_t::TEENSY_3_2 &&
                     (0 <= adc_num && adc_num <= 1)),
                "invalid adc_num");

  static constexpr address_t base_addr = adc_base_addr<board, adc_num>::value;

  struct sc1a {
    static constexpr address_t addr = base_addr + 0x00;
    using coco0 = reg_t<ro_t, direct_access_t, addr, ADC_SC1_COCO>;
    using aien = reg_t<rw_t, direct_access_t, addr, ADC_SC1_AIEN>;
    using diff = reg_t<rw_t, direct_access_t, addr, ADC_SC1_DIFF>;
    using adch = reg_t<rw_t, direct_access_t, addr, ADC_SC1_ADCH(31)>;
  };
  using coco0 = typename sc1a::coco0;
  using aien = typename sc1a::aien;
  using diff = typename sc1a::diff;
  using adch = typename sc1a::adch;

  struct cfg1 {
    static constexpr address_t addr = base_addr + 0x08;
    using adlpc = reg_t<ro_t, direct_access_t, addr, ADC_CFG1_ADLPC>;
    using adiv = reg_t<rw_t, direct_access_t, addr, ADC_CFG1_ADIV(3)>;
    using adlsmp = reg_t<rw_t, direct_access_t, addr, ADC_CFG1_ADLSMP>;
    using mode = reg_t<rw_t, direct_access_t, addr, ADC_CFG1_MODE(3)>;
    using adiclk = reg_t<rw_t, direct_access_t, addr, ADC_CFG1_ADICLK(3)>;
  };
  using adlpc = typename cfg1::adlpc;
  using adiv = typename cfg1::adiv;
  using adlsmp = typename cfg1::adlsmp;
  using mode = typename cfg1::mode;
  using adiclk = typename cfg1::adiclk;

  struct cfg2 {
    static constexpr address_t addr = base_addr + 0x0C;
    using muxsel = reg_t<rw_t, direct_access_t, addr, ADC_CFG2_MUXSEL>;
    using adacken = reg_t<rw_t, direct_access_t, addr, ADC_CFG2_ADACKEN>;
    using adhsc = reg_t<rw_t, direct_access_t, addr, ADC_CFG2_ADHSC>;
    using adsts = reg_t<rw_t, direct_access_t, addr, ADC_CFG2_ADLSTS(3)>;
  };
  using muxsel = typename cfg2::muxsel;
  using adacken = typename cfg2::adacken;
  using adhsc = typename cfg2::adhsc;
  using adsts = typename cfg2::adsts;

  struct ra {
    static constexpr address_t addr = base_addr + 0x10;
    using value = reg_t<ro_t, direct_access_t, addr, 0xFFFF>;
  };
  using value0 = typename ra::value;

  struct cv1 {
    static constexpr address_t addr = base_addr + 0x18;
    using cv = reg_t<rw_t, direct_access_t, addr, 0xFFFF>;
  };

  struct cv2 {
    static constexpr address_t addr = base_addr + 0x1C;
    using cv = reg_t<rw_t, direct_access_t, addr, 0xFFFF>;
  };

  struct sc2 {
    static constexpr address_t addr = base_addr + 0x20;
    using adact = reg_t<rw_t, direct_access_t, addr, ADC_SC2_ADACT>;
    using adtrg = reg_t<rw_t, direct_access_t, addr, ADC_SC2_ADTRG>;
    using acfe = reg_t<rw_t, direct_access_t, addr, ADC_SC2_ACFE>;
    using acfgt = reg_t<rw_t, direct_access_t, addr, ADC_SC2_ACFGT>;
    using acren = reg_t<rw_t, direct_access_t, addr, ADC_SC2_ACREN>;
    using dmaen = reg_t<rw_t, direct_access_t, addr, ADC_SC2_DMAEN>;
    using refsel = reg_t<rw_t, direct_access_t, addr, ADC_SC2_REFSEL(3)>;
  };
  using adact = typename sc2::adact;
  using adtrg = typename sc2::adtrg;
  using acfe = typename sc2::acfe;
  using acfgt = typename sc2::acfgt;
  using acren = typename sc2::acren;
  using dmaen = typename sc2::dmaen;
  using refsel = typename sc2::refsel;

  struct sc3 {
    static constexpr address_t addr = base_addr + 0x24;
    using cal = reg_t<rw_t, direct_access_t, addr, ADC_SC3_CAL>;
    using calf = reg_t<rw_t, direct_access_t, addr, ADC_SC3_CALF>;
    using adco = reg_t<rw_t, direct_access_t, addr, ADC_SC3_ADCO>;
    using avge = reg_t<rw_t, direct_access_t, addr, ADC_SC3_AVGE>;
    using avgs = reg_t<rw_t, direct_access_t, addr, ADC_SC3_AVGS(3)>;
  };
  using cal = typename sc3::cal;
  using calf = typename sc3::calf;
  using adco = typename sc3::adco;
  using avge = typename sc3::avge;
  using avgs = typename sc3::avgs;

  struct ofs {
    static constexpr address_t addr = base_addr + 0x28;
    using offset = reg_t<rw_t, direct_access_t, addr, 0xFFF>;
  };

}; // struct adc_module

}; // namespace adc
