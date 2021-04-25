/**
 * @ Author: Pierre Veysseyre
 * @ Description: Math.hpp
 */

#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

namespace Utils
{
    template<typename Type>
    std::enable_if_t<std::is_floating_point_v<Type>, Type>
        sinc(const Type x) noexcept
    {
        if (!x)
            return 1.0;
        return std::sin(x) / x;
    }
}
