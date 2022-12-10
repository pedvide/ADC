#pragma once
/**
Specializations for Teensy 3.x: 3.0, 3.1, 3.2, 3.4, 3.5
*/

#include "common_defs.hpp"
#include "register.hpp"

namespace adc {

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
    TEMP_SENSOR = 26,
    VREF_OUT = 22,
    BANDGAP = 27,
    VREFH = 29,
    VREFL = 30
  };

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

// Register definitions
template <> struct adc_base_addr<board_t::TEENSY_3_0, 0> {
  static constexpr address_t value = 0x4003B000;
};

template <int adc_num> struct adc_module_reg_t<board_t::TEENSY_3_0, adc_num> {
  static_assert(adc_num == 0, "invalid adc_num");

  static constexpr address_t base_addr =
      adc_base_addr<board_t::TEENSY_3_0, adc_num>::value;

  struct sc1a {
    static constexpr address_t addr = base_addr + 0x00;
    using coco0 = reg_t<ro_t, direct_access_t, addr, ADC_SC1_COCO>;
    using aien = reg_t<rw_t, direct_access_t, addr, ADC_SC1_AIEN>;
    using diff = reg_t<rw_t, direct_access_t, addr, ADC_SC1_DIFF>;
    using adch = reg_t<rw_t, direct_access_t, addr, ADC_SC1_ADCH(31)>;
  };
  using aien = typename sc1a::aien;
  using adch = typename sc1a::adch;
  using coco0 = typename sc1a::coco0;

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

// All regs and addresses asre the same as Teensy 3.0
template <int adc_num>
struct adc_module_reg_t<board_t::TEENSY_3_2, adc_num>
    : adc_module_reg_t<board_t::TEENSY_3_0, adc_num> {};

}; // namespace adc
