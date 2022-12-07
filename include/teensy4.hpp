#pragma once
/**
Specializations for Teensy 4 and 4.1
*/

#include "common_defs.hpp"
#include "register.hpp"

namespace adc {

template <> struct pin_info_t<board_t::TEENSY_4, 0> {
  static constexpr uint8_t num_pins = 12;

  enum class pin_t : uint8_t {
    A0 = 0,
    A1,
    A2,
    A3,
    A4,
    A5,
    A6,
    A7,
    A8,
    A9,
    A10,
    A11,
  };

  static constexpr pin_t pins[num_pins] = {
      pin_t::A0, pin_t::A1, pin_t::A2, pin_t::A3, pin_t::A4,  pin_t::A5,
      pin_t::A6, pin_t::A7, pin_t::A8, pin_t::A9, pin_t::A10, pin_t::A11};

  static constexpr uint8_t channel2sc1a[num_pins] = {
      7, 8, 12, 11, 6, 5, 15, 0, 13, 14, // A0-A9
      1, 2                               // A10-A11
  };
};
// make linker happy
constexpr uint8_t pin_info_t<board_t::TEENSY_4, 0>::channel2sc1a[];
constexpr pin_info_t<board_t::TEENSY_4, 0>::pin_t
    pin_info_t<board_t::TEENSY_4, 0>::pins[];

template <> struct pin_info_t<board_t::TEENSY_4, 1> {
  static constexpr uint8_t num_pins = 12;

  enum class pin_t : uint8_t {
    A0 = 0,
    A1,
    A2,
    A3,
    A4,
    A5,
    A6,
    A7,
    A8,
    A9,
    A12,
    A13,
  };
  static constexpr pin_t pins[num_pins] = {
      pin_t::A0, pin_t::A1, pin_t::A2, pin_t::A3, pin_t::A4,  pin_t::A5,
      pin_t::A6, pin_t::A7, pin_t::A8, pin_t::A9, pin_t::A12, pin_t::A13};

  static constexpr uint8_t channel2sc1a[num_pins] = {
      7, 8, 12, 11, 6, 5, 15, 0, 13, 14, // A0-A9
      3, 4                               // A12, A13
  };
};
// make linker happy
constexpr uint8_t pin_info_t<board_t::TEENSY_4, 1>::channel2sc1a[];
constexpr pin_info_t<board_t::TEENSY_4, 1>::pin_t
    pin_info_t<board_t::TEENSY_4, 1>::pins[];

// Register definitions
template <> struct adc_base_addr<board_t::TEENSY_4, 0> {
  static constexpr address_t value = 0x400C4000;
};
template <> struct adc_base_addr<board_t::TEENSY_4, 1> {
  static constexpr address_t value = 0x400C8000;
};

template <int adc_num> struct adc_module_reg_t<board_t::TEENSY_4, adc_num> {
  static constexpr address_t base_addr =
      adc_base_addr<board_t::TEENSY_4, adc_num>::value;

  struct hc0 {
    using aien = reg_t<rw_t, direct_access_t, base_addr + 0x00, ADC_HC_AIEN>;
    using adch =
        reg_t<rw_t, direct_access_t, base_addr + 0x00, ADC_HC_ADCH(31)>;
  };
  using aien = typename hc0::aien;
  using adch = typename hc0::adch;

  struct hs {
    static constexpr address_t addr = base_addr + 0x20;
    using coco0 = reg_t<ro_t, direct_access_t, addr, ADC_HS_COCO0>;
  };
  using coco0 = typename hs::coco0;

  struct r0 {
    static constexpr address_t addr = base_addr + 0x24;
    using value = reg_t<ro_t, direct_access_t, addr, 0xFFFFFFFF, 0>;
  };
  using value0 = typename r0::value;

  struct cfg {
    static constexpr address_t addr = base_addr + 0x44;
    using ovwren = reg_t<rw_t, direct_access_t, addr, ADC_CFG_OVWREN>;
    using avgs = reg_t<rw_t, direct_access_t, addr, ADC_CFG_AVGS(3)>;
    using adtrg = reg_t<rw_t, direct_access_t, addr, ADC_CFG_ADTRG>;
    using refsel = reg_t<rw_t, direct_access_t, addr, ADC_CFG_REFSEL(3)>;
    using adhsc = reg_t<rw_t, direct_access_t, addr, ADC_CFG_ADHSC>;
    using adsts = reg_t<rw_t, direct_access_t, addr, ADC_CFG_ADSTS(3)>;
    using adlpc = reg_t<rw_t, direct_access_t, addr, ADC_CFG_ADLPC>;
    using adiv = reg_t<rw_t, direct_access_t, addr, ADC_CFG_ADIV(3)>;
    using adlsmp = reg_t<rw_t, direct_access_t, addr, ADC_CFG_ADLSMP>;
    using mode = reg_t<rw_t, direct_access_t, addr, ADC_CFG_MODE(3)>;
    using adiclk = reg_t<rw_t, direct_access_t, addr, ADC_CFG_ADICLK(3)>;
  };
  using ovwren = typename cfg::ovwren;
  using avgs = typename cfg::avgs;
  using adtrg = typename cfg::adtrg;
  using refsel = typename cfg::refsel;
  using adhsc = typename cfg::adhsc;
  using adsts = typename cfg::adsts;
  using adlpc = typename cfg::adlpc;
  using adiv = typename cfg::adiv;
  using adlsmp = typename cfg::adlsmp;
  using mode = typename cfg::mode;
  using adiclk = typename cfg::adiclk;

  struct gc {
    static constexpr address_t addr = base_addr + 0x48;
    using cal = reg_t<rw_t, direct_access_t, addr, ADC_GC_CAL>;
    using adco = reg_t<rw_t, direct_access_t, addr, ADC_GC_ADCO>;
    using avge = reg_t<rw_t, direct_access_t, addr, ADC_GC_AVGE>;
    using acfe = reg_t<rw_t, direct_access_t, addr, ADC_GC_ACFE>;
    using acfgt = reg_t<rw_t, direct_access_t, addr, ADC_GC_ACFGT>;
    using acren = reg_t<rw_t, direct_access_t, addr, ADC_GC_ACREN>;
    using dmaen = reg_t<rw_t, direct_access_t, addr, ADC_GC_DMAEN>;
    using adacken = reg_t<rw_t, direct_access_t, addr, ADC_GC_ADACKEN>;
  };
  using cal = typename gc::cal;
  using adco = typename gc::adco;
  using avge = typename gc::avge;
  using acfe = typename gc::acfe;
  using acfgt = typename gc::acfgt;
  using acren = typename gc::acren;
  using dmaen = typename gc::dmaen;
  using adacken = typename gc::adacken;

  struct gs {
    static constexpr address_t addr = base_addr + 0x4C;
    using awkst = reg_t<rw_t, direct_access_t, addr, ADC_GS_AWKST>;
    using calf = reg_t<rw_t, direct_access_t, addr, ADC_GS_CALF>;
    using adact = reg_t<rw_t, direct_access_t, addr, ADC_GS_ADACT>;
  };
  using awkst = typename gs::awkst;
  using calf = typename gs::calf;
  using adact = typename gs::adact;

  struct cv {
    static constexpr address_t addr = base_addr + 0x50;
    using cv1 = reg_t<rw_t, direct_access_t, addr, ADC_CV_CV1(0xFFF)>;
    using cv2 = reg_t<rw_t, direct_access_t, addr, ADC_CV_CV2(0xFFF)>;
  };

  struct ofs {
    static constexpr address_t addr = base_addr + 0x54;
    using sign = reg_t<rw_t, direct_access_t, addr, ADC_OFS_SIGN>;
    using offset = reg_t<rw_t, direct_access_t, addr, ADC_OFS_OFS(0xFFF)>;
  };

  struct cal_data {
    static constexpr address_t addr = base_addr + 0x58;
    using code = reg_t<rw_t, direct_access_t, addr, ADC_CAL_CAL_CODE(0x0F)>;
  };

}; // struct adc_module

}; // namespace adc
