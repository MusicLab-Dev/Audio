/**
 * @ Author: Antoine la Mache
 * @ Description: Conversion algorithm
 */

#pragma once


#include <Core/Utils.hpp>

namespace Audio::DSP
{
    template<typename InputUnit, typename OutputUnitDSP, std::size_t BufferSize>
    void Convert(InputUnit *in, OutputUnitDSP *out) noexcept;

    template<typename InputUnit, typename OutputUnitDSP, std::size_t Left>
    void UnrolledConvert(OutputUnitDSP * const output, const InputUnit * const input) noexcept;

    template<typename From, typename To>
    struct ConvertLimits
    {
        static constexpr To Min = std::numeric_limits<To>::min();
        static constexpr To Max = std::numeric_limits<To>::max();
    };

    template<>
    struct ConvertLimits<float, std::int32_t>
    {
        static constexpr std::int32_t Min = std::numeric_limits<std::int32_t>::min();
        static constexpr std::int32_t Max = std::numeric_limits<std::int32_t>::max() - 128;
    };
}


#include "Convert.ipp"
