/**
 * @ Author: Pierre Veysseyre
 * @ Description: Math.hpp
 */

#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

namespace Utils
{
    /** @brief Return the sinus cardinal of x */
    template<bool NormalizePi = false, typename Type>
    std::enable_if_t<std::is_floating_point_v<Type>, Type>
        sinc(Type x) noexcept
    {
        if (!x)
            return 1.0;
        if constexpr (NormalizePi)
            x *= M_PI;
        return std::sin(x) / x;
    }

    /** @brief Return the cotangent of x */
    template<bool NormalizePi = false, typename Type>
    std::enable_if_t<std::is_floating_point_v<Type>, Type>
        cot(Type x) noexcept
    {
        if constexpr (NormalizePi)
            x *= M_PI;
        // return std::cos(x) / std::sin(x);
        return 1.f / std::tan(x);
    }

    /**
     * @brief Exponential interpolation.
     * @warning Only efficient on positive values !
     */
    template<typename Type>
    std::enable_if_t<std::is_floating_point_v<Type>, Type>
        expInterpUnsafe(Type from, Type to, Type x) noexcept
    {
        const auto range = to - from;
        const auto normX = (x - from) / range;

        return std::pow(normX, M_E) * range + from;
    }

    /**
     * @brief Exponential interpolation from 0 to 1
     */
    template<typename Type>
    std::enable_if_t<std::is_floating_point_v<Type>, Type>
        expInterpSafe(Type x) noexcept
    {
        return std::pow(x, M_E);
    }
}
