/**
 * @ Author: Pierre Veysseyre
 * @ Description: Math.hpp
 */

#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

#include <atomic>

namespace Utils
{
    /** @brief Return the sinus cardinal of x */
    template<bool NormalizePi = false, typename Type>
    [[nodiscard]] std::enable_if_t<std::is_floating_point_v<Type>, Type>
        sinc(Type x) noexcept
    {
        if (!x)
            return 1.0;
        if constexpr (NormalizePi)
            x *= static_cast<Type>(M_PI);
        return std::sin(x) / x;
    }

    /** @brief Return the cotangent of x */
    template<bool NormalizePi = false, typename Type>
    [[nodiscard]] std::enable_if_t<std::is_floating_point_v<Type>, Type>
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
    [[nodiscard]] std::enable_if_t<std::is_floating_point_v<Type>, Type>
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
    [[nodiscard]] std::enable_if_t<std::is_floating_point_v<Type>, Type>
        expInterpSafe(Type x) noexcept
    {
        return std::pow(x, M_E);
    }

    [[nodiscard]] inline std::uint32_t fastRand(void) noexcept
    {
        static std::atomic<std::uint32_t> randState { 1234567890u };
        std::uint32_t out { randState };

        out ^= out << 13u;
        out ^= out >> 17u;
        out ^= out << 5u;
        randState = out;
        return out;
    }

    [[nodiscard]] inline std::uint32_t fastRand(std::uint32_t &last) noexcept
    {
        std::uint32_t out { last };

        out ^= out << 13u;
        out ^= out >> 17u;
        out ^= out << 5u;
        last = out;
        return out;
    }
}
