/**
Register definitions for the ADC

*/

#include "register.hpp"

// #define ADC_HC_AIEN ((uint32_t)(1 << 7))
// #define ADC_HC_ADCH(n) ((uint32_t)(((n)&0x1F) << 0))
// #define ADC_HS_COCO0 ((uint32_t)(1 << 0))
// #define ADC_CFG_OVWREN ((uint32_t)(1 << 16))
// #define ADC_CFG_AVGS(n) ((uint32_t)(((n)&0x03) << 14))
// #define ADC_CFG_ADTRG ((uint32_t)(1 << 13))
// #define ADC_CFG_REFSEL(n) ((uint32_t)(((n)&0x03) << 11))
// #define ADC_CFG_ADHSC ((uint32_t)(1 << 10))
// #define ADC_CFG_ADSTS(n) ((uint32_t)(((n)&0x03) << 8))
// #define ADC_CFG_ADLPC ((uint32_t)(1 << 7))
// #define ADC_CFG_ADIV(n) ((uint32_t)(((n)&0x03) << 5))
// #define ADC_CFG_ADLSMP ((uint32_t)(1 << 4))
// #define ADC_CFG_MODE(n) ((uint32_t)(((n)&0x03) << 2))
// #define ADC_CFG_ADICLK(n) ((uint32_t)(((n)&0x03) << 0))
// #define ADC_GC_CAL ((uint32_t)(1 << 7))
// #define ADC_GC_ADCO ((uint32_t)(1 << 6))
// #define ADC_GC_AVGE ((uint32_t)(1 << 5))
// #define ADC_GC_ACFE ((uint32_t)(1 << 4))
// #define ADC_GC_ACFGT ((uint32_t)(1 << 3))
// #define ADC_GC_ACREN ((uint32_t)(1 << 2))
// #define ADC_GC_DMAEN ((uint32_t)(1 << 1))
// #define ADC_GC_ADACKEN ((uint32_t)(1 << 0))
// #define ADC_GS_AWKST ((uint32_t)(1 << 2))
// #define ADC_GS_CALF ((uint32_t)(1 << 1))
// #define ADC_GS_ADACT ((uint32_t)(1 << 0))
// #define ADC_CV_CV2(n) ((uint32_t)(((n)&0xFFF) << 16))
// #define ADC_CV_CV1(n) ((uint32_t)(((n)&0xFFF) << 0))
// #define ADC_OFS_SIGN ((uint32_t)(1 << 12))
// #define ADC_OFS_OFS(n) ((uint32_t)(((n)&0xFFF) << 0))
// #define ADC_CAL_CAL_CODE(n) ((uint32_t)(((n)&0x0F) << 0))

namespace adc {

template <int adc_num> struct adc_module_base_addr {
  static_assert(0 <= adc_num && adc_num <= 1, "invalid adc_num");
};

template <> struct adc_module_base_addr<0> {
  static constexpr address_t value = 0x400C4000;
};
template <> struct adc_module_base_addr<1> {
  static constexpr address_t value = 0x400C8000;
};

template <int adc_num> struct adc_module_reg_t {
  static constexpr address_t base_addr = adc_module_base_addr<adc_num>::value;

  struct hc0 {
    static constexpr address_t addr = base_addr + 0x00;
    using aien = reg_t<rw_t, addr, ADC_HC_AIEN>;
    using adch = reg_t<rw_t, addr, ADC_HC_ADCH(31)>;
  };

  struct hs {
    static constexpr address_t addr = base_addr + 0x20;
    using coco0 = reg_t<ro_t, addr, ADC_HS_COCO0>;
  };

  struct r0 {
    static constexpr address_t addr = base_addr + 0x24;
    using value = reg_t<ro_t, addr, 0xFFFFFFFF, 0>;
  };

  struct cfg {
    static constexpr address_t addr = base_addr + 0x44;
    using ovwren = reg_t<rw_t, addr, ADC_CFG_OVWREN>;
    using avgs = reg_t<rw_t, addr, ADC_CFG_AVGS(3)>;
    using adtrg = reg_t<rw_t, addr, ADC_CFG_ADTRG>;
    using refsel = reg_t<rw_t, addr, ADC_CFG_REFSEL(3)>;
    using adhsc = reg_t<rw_t, addr, ADC_CFG_ADHSC>;
    using adsts = reg_t<rw_t, addr, ADC_CFG_ADSTS(3)>;
    using adlpc = reg_t<rw_t, addr, ADC_CFG_ADLPC>;
    using adiv = reg_t<rw_t, addr, ADC_CFG_ADIV(3)>;
    using adlsmp = reg_t<rw_t, addr, ADC_CFG_ADLSMP>;
    using mode = reg_t<rw_t, addr, ADC_CFG_MODE(3)>;
    using adiclk = reg_t<rw_t, addr, ADC_CFG_ADICLK(3)>;
  };

  struct gc {
    static constexpr address_t addr = base_addr + 0x48;
    using cal = reg_t<rw_t, addr, ADC_GC_CAL>;
    using adco = reg_t<rw_t, addr, ADC_GC_ADCO>;
    using avge = reg_t<rw_t, addr, ADC_GC_AVGE>;
    using acfe = reg_t<rw_t, addr, ADC_GC_ACFE>;
    using acfgt = reg_t<rw_t, addr, ADC_GC_ACFGT>;
    using acren = reg_t<rw_t, addr, ADC_GC_ACREN>;
    using dmaen = reg_t<rw_t, addr, ADC_GC_DMAEN>;
    using adacken = reg_t<rw_t, addr, ADC_GC_ADACKEN>;
  };

  struct gs {
    static constexpr address_t addr = base_addr + 0x4C;
    using awkst = reg_t<rw_t, addr, ADC_GS_AWKST>;
    using calf = reg_t<rw_t, addr, ADC_GS_CALF>;
    using adact = reg_t<rw_t, addr, ADC_GS_ADACT>;
  };

  struct cv {
    static constexpr address_t addr = base_addr + 0x50;
    using cv1 = reg_t<rw_t, addr, ADC_CV_CV1(0xFFF)>;
    using cv2 = reg_t<rw_t, addr, ADC_CV_CV2(0xFFF)>;
  };

  struct ofs {
    static constexpr address_t addr = base_addr + 0x54;
    using sign = reg_t<rw_t, addr, ADC_OFS_SIGN>;
    using offset = reg_t<rw_t, addr, ADC_OFS_OFS(0xFFF)>;
  };

  struct cal {
    static constexpr address_t addr = base_addr + 0x58;
    using code = reg_t<rw_t, addr, ADC_CAL_CAL_CODE(0x0F)>;
  };

}; // struct adc_module

} // namespace adc
