/**
 * @ Author: Pierre Veysseyre
 * @ Description: Math.hpp
 */

#pragma once

#include <cmath>

namespace Utils
{
    template<bool NormalizePi = false, typename Type>
    std::enable_if_t<std::is_floating_point_v<Type>, Type>
        sinc(const Type x) noexcept
    {
        if (!x)
            return 1.0;
        // if constexpr (NormalizePi)
        //     x *= M_PI;
        return std::sin(x) / x;
    }
}