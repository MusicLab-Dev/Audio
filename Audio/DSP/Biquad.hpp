/**
 * @ Author: Pierre Veysseyre
 * @ Description: Biquad.hpp
 */

#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

#include <cstdint>
#include <type_traits>

#include <Audio/Base.hpp>
#include "Filter.hpp"

// Dummy MSVC doesn't find M_PI
#ifndef M_PI
# define M_PI       3.14159265358979323846   // pi
#endif

namespace Audio::DSP::Biquad
{
    /** @brief Describe the parameters used for a second-order IIR filter section */
    namespace Internal {
        struct Coefficients
        {
            float a[3] { 0.0 };
            float b[3] { 0.0 };
        };
        // static_assert(sizeof(Coefficients) == 24, "Coefficients must take 24 bytes !");

        struct Specs
        {
            DSP::Filter::AdvancedType filterType { DSP::Filter::AdvancedType::LowPass };
            float sampleRate;
            float cutoffs[2];
            float gain;
            float qFactor;
            bool qAsBandWidth;
        };

        /** @brief Select the internal implementation: classic form or optimized (for memory) form */
        enum class Optimization : std::uint8_t {
            Classic = 0u,
            Optimized
        };

        /** @brief Different possible implementation */
        enum class Form : std::uint8_t {
            Direct1 = 0u,        // 4 registers, 3 addOp -> better for fixed-points
            Direct2,        // 2 registers, 4 addOp -> better for fixed-points
            Transposed1,    // 4 registers, 4 addOp -> better for floating-points
            Transposed2     // 2 registers, 3 addOp -> better for floating-points
        };

        [[nodiscard]] Coefficients GenerateCoefficientsLowPass(const float sampleRate, const float freq, const float q, const bool qAsBandWidth) noexcept;
        [[nodiscard]] Coefficients GenerateCoefficientsHighPass(const float sampleRate, const float freq, const float q, const bool qAsBandWidth) noexcept;
        [[nodiscard]] Coefficients GenerateCoefficientsBandPass(const float sampleRate, const float freq, const float q, const bool qAsBandWidth) noexcept;
        [[nodiscard]] Coefficients GenerateCoefficientsBandStop(const float sampleRate, const float freq, const float q, const bool qAsBandWidth) noexcept;
        [[nodiscard]] Coefficients GenerateCoefficientsPeak(const float sampleRate, const float freq, const float gain, const float q, const bool qAsBandWidth) noexcept;
        [[nodiscard]] Coefficients GenerateCoefficientsLowShelf(const float sampleRate, const float freq, const float gain, const float q) noexcept;
        [[nodiscard]] Coefficients GenerateCoefficientsHighShelf(const float sampleRate, const float freq, const float gain, const float q) noexcept;

    }

    [[nodiscard]] Internal::Coefficients GenerateCoefficients(const Internal::Specs &specs) noexcept;

    // template<Internal::Optimization Opti>
    // struct BiquadMaker;

    // template<typename T, Internal::Form Form>
    // template<Internal::Form Form, typename T>
    template<Internal::Form Form>
    class Filter;
}

template<Audio::DSP::Biquad::Internal::Form Form>
class Audio::DSP::Biquad::Filter
{
public:
    /** @brief Default constructor */
    Filter(void) = default;

    /** Get the internal biquad coefficients */
    [[nodiscard]] const Internal::Coefficients &coefficients(void) const noexcept { return _coefs; }
    void setupCoefficients(const Internal::Coefficients &coefficients) noexcept { _coefs = coefficients; }

    /** Process a block of samples */
    template<typename Type>
    void filterBlock(const Type *input, const std::size_t size, Type *output, const DB outGain = 1.0) noexcept;
    // void processBlock1(Type *block, std::size_t len) noexcept;

    float foo1(const float a, const float b, const float c) noexcept {
        return a * b + c;
    }
    float foo2(const float a, const float b, const float c) noexcept {
        return std::fma(a, b, c);
    }

    void resetRegisters(void) noexcept;

protected:
    Internal::Coefficients   _coefs;
    float _regs[(Form == Internal::Form::Direct1 || Form == Internal::Form::Transposed1) ? 4 : 2] { 0.0 };

    /** @brief Process a sample into the biquad */
    template<typename Type>
    [[nodiscard]] Type process(const Type in, const DB outGain) noexcept;
    // [[nodiscard]] float process1(const float in) noexcept;
};


// template<Audio::DSP::Internal::Optimization Opti>
// struct Audio::DSP::BiquadMaker
// {
//     template<typename Type>
//     [[nodiscard]] static auto MakeBiquad(void)
//             // const double sampleRate, const double freq, const double gain = 3.0, const double q = 0.70710678118654752440, bool qAsBandWidth = false
//     noexcept {
//         if constexpr (std::is_floating_point_v<Type>) {
//             if constexpr (Opti == DSP::Internal::Optimization::Classic)
//                 return Biquad<Internal::Form::Transposed1>();
//             else if constexpr (Opti == Internal::Optimization::Optimized)
//                 return Biquad<Internal::Form::Transposed2>();
//         } else if constexpr (std::is_integral_v<Type>) {
//             if constexpr (Opti == Internal::Optimization::Classic)
//                 return Biquad<Internal::Form::Direct1>();
//             else if constexpr (Opti == Internal::Optimization::Optimized)
//                 return Biquad<Internal::Form::Direct2>();
//         }
//     }
// };

#include "Biquad.ipp"

// // static_assert(alignof(DSP::Biquad) == 8, "Biquad must be aligned to 8 bytes !");
// // static_assert(sizeof(DSP::Biquad) == 80, "Biquad must take 80 bytes !");
