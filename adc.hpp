#include "adc_module.hpp"

namespace adc
{
    struct adc_t
    {
        using adc0 = adc_module_t<0>;
        using adc1 = adc_module_t<1>;
    };

}; // namespace adc