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
    namespace Internal
    {
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

        [[nodiscard]] Coefficients GenerateCoefficientsLowPass(const float sampleRate, const float freq, const float q) noexcept;
        [[nodiscard]] Coefficients GenerateCoefficientsHighPass(const float sampleRate, const float freq, const float q) noexcept;
        [[nodiscard]] Coefficients GenerateCoefficientsBandPass(const float sampleRate, const float freq, const float q) noexcept;
        [[nodiscard]] Coefficients GenerateCoefficientsBandPassFlat(const float sampleRate, const float freq, const float q) noexcept;
        [[nodiscard]] Coefficients GenerateCoefficientsBandStop(const float sampleRate, const float freq, const float q) noexcept;
        [[nodiscard]] Coefficients GenerateCoefficientsPeak(const float sampleRate, const float freq, const float gain, const float q) noexcept;
        [[nodiscard]] Coefficients GenerateCoefficientsLowShelf(const float sampleRate, const float freq, const float gain, const float q) noexcept;
        [[nodiscard]] Coefficients GenerateCoefficientsHighShelf(const float sampleRate, const float freq, const float gain, const float q) noexcept;
        [[nodiscard]] Coefficients GenerateCoefficientsAllPass(const float sampleRate, const float freq, const float q) noexcept;

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
    using Cache = std::array<float, (Form == Internal::Form::Direct1 || Form == Internal::Form::Transposed1) ? 4 : 2>;
    using KeyCache = std::array<Cache, KeyCount>;

    /** @brief Default constructor */
    Filter(void) = default;

    /** Get the internal biquad coefficients */
    [[nodiscard]] const Internal::Coefficients &coefficients(void) const noexcept { return _coefs; }

    void setup(const Internal::Specs &specs) noexcept { _coefs = GenerateCoefficients(specs); }

    /** @brief Process a sample into the biquad */
    template<typename Type>
    [[nodiscard]] Type processSample(const Type in, const Key key, const DB outGain) noexcept;
    // [[nodiscard]] float processSample1(const float in) noexcept;

    /** Process a block of samples */
    template<typename Type>
    void filterBlock(const Type *input, const std::size_t size, Type *output, const Key key, const DB outGain = 1.0) noexcept;
    // void processBlock1(Type *block, std::size_t len) noexcept;


    void reset(void) noexcept
    {
        for (auto &reg : _regs)
            reg.fill(0.0f);
    }
    void resetKey(const Key key) noexcept { _regs[key].fill(0.0f); }

protected:
    Internal::Coefficients   _coefs;
    KeyCache _regs { 0.0 };

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
