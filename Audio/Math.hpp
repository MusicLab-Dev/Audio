/**
 * @ Author: Pierre Veysseyre
 * @ Description: Math.hpp
 */

#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

#include <array>

#include "MathConstants.hpp"

namespace Audio::Utils
{
    struct RandomDataSet
    {
        static constexpr std::size_t Size = 65'536u;

        static std::array<std::size_t, Size> Array;

        /** @brief Generate a fast random number from a static data set */
        [[nodiscard]] static inline std::size_t FastRand(const std::size_t index) noexcept
            { return RandomDataSet::Array[index % RandomDataSet::Size]; }
    };

    /** @brief Return the sinus cardinal of x */
    template<bool NormalizePi = false, typename Type>
    [[nodiscard]] std::enable_if_t<std::is_floating_point_v<Type>, Type>
        Sinc(Type x) noexcept
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
        Cot(Type x) noexcept
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
        ExpInterpUnsafe(Type from, Type to, Type x) noexcept
    {
        const auto range = to - from;
        const auto normX = (x - from) / range;

        return std::pow(normX, M_E) * range + from;
    }

    /**
     * @brief Exponential interpolation from 0 to 1
     */
    template<typename Type>
    [[nodiscard]] std::enable_if_t<std::is_floating_point_v<Type>, Type> ExpInterpSafe(Type x) noexcept
        { return std::pow(x, M_E); }


    class RandomGenerator
    {
    public:
        using ProcessType = std::uint32_t;

        static constexpr ProcessType DefaultMult = 1664525u << 2u;
        static constexpr ProcessType DefaultInc = 1013904223u << 2u ;
        static constexpr ProcessType DefaultMod = 1u << 16u;

        RandomGenerator(void) = default;

        [[nodiscard]] ProcessType last(void) const noexcept { return _last; }
        void setLast(const ProcessType last) noexcept { _last = last; }
        void setSeed(const ProcessType mult = DefaultMult, const ProcessType inc = DefaultInc, const ProcessType mod = DefaultMod, const ProcessType last = 0u) noexcept { _last = last; }

        [[nodiscard]] ProcessType rand(void) noexcept
        {
            _last = (_multiplier * _last + _increment) % _modulus;
            return _last;
        }

        template<typename Type>
        [[nodiscard]] Type randAs(void) noexcept
        {
            if constexpr (std::is_signed_v<Type>) {
                if constexpr (std::is_floating_point_v<Type>) {
                    return static_cast<Type>(static_cast<double>(rand()) / static_cast<double>(_modulus) * 2.0 - 1.0);
                } else {
                    return static_cast<Type>(
                        static_cast<double>(rand()) / static_cast<double>(_modulus) * 2.0 * static_cast<double>(std::numeric_limits<Type>::max() - static_cast<double>(std::numeric_limits<Type>::max()))
                    );
                }
            } else {
                return static_cast<Type>(static_cast<double>(rand()) / static_cast<double>(_modulus) * static_cast<double>(std::numeric_limits<Type>::max()));
            }
        }

    private:
        ProcessType _multiplier = DefaultMult;
        ProcessType _increment = DefaultMod;
        ProcessType _modulus = DefaultMult;
        ProcessType _last { 0u };
    };
}
