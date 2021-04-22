/**
 * @ Author: Pierre Veysseyre
 * @ Description: Biquad.hpp
 */

#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#include <cstdint>
#include <type_traits>

#include <Audio/Base.hpp>
#include "Filter.hpp"

namespace Audio::DSP
{
    /** @brief Describe the parameters used for a second-order IIR filter section */
    struct BiquadParam {
        struct Coefficients
        {
            float a[3] { 0.0 };
            float b[3] { 0.0 };
        };
        // static_assert(sizeof(Coefficients) == 24, "Coefficients must take 24 bytes !");

        struct Specs
        {
            SampleRate sampleRate;
            SampleRate cutoff;
            double gain;
            double qFactor;
            bool qAsBandWidth;
        };

        /** @brief Select the internal implementation: classic form or optimized (for memory) form */
        enum class Optimization : uint8_t {
            Classic = 0u,
            Optimized
        };

        /** @brief Different possible implementation */
        enum class InternalForm : uint8_t {
            Direct1 = 0u,        // 4 registers, 3 addOp -> better for fixed-points
            Direct2,        // 2 registers, 4 addOp -> better for fixed-points
            Transposed1,    // 4 registers, 4 addOp -> better for floating-points
            Transposed2     // 2 registers, 3 addOp -> better for floating-points
        };

        template<Filter::AdvancedType Filter>
        [[nodiscard]] static Coefficients GenerateCoefficients(
                const SampleRate sampleRate, const double freq, const double gain, const double q, bool qAsBandWidth) noexcept;

    };

    template<BiquadParam::Optimization Opti>
    struct BiquadMaker;

    // template<typename T, BiquadParam::InternalForm Form>
    // template<BiquadParam::InternalForm Form, typename T>
    template<BiquadParam::InternalForm Form>
    class Biquad;

};

template<Audio::DSP::BiquadParam::InternalForm Form>
class Audio::DSP::Biquad
{
public:
    static constexpr auto FormX = Form;

    /** @brief Default constructor */
    Biquad(void) = default;

    /** Get the internal biquad coefficients */
    [[nodiscard]] const BiquadParam::Coefficients &coefficients(void) const noexcept { return _coefs; }
    void setupCoefficients(const BiquadParam::Coefficients &coefficients) noexcept { _coefs = coefficients; }

    /** Process a block of samples */
    template<typename Type>
    void processBlock(Type *block, std::size_t len) noexcept;
    // void processBlock1(Type *block, std::size_t len) noexcept;

    float foo1(const float a, const float b, const float c) noexcept {
        return a * b + c;
    }
    float foo2(const float a, const float b, const float c) noexcept {
        return std::fma(a, b, c);
    }

    void resetRegisters(void) noexcept;

protected:
    BiquadParam::Coefficients   _coefs;
    float                      _regs[(Form == BiquadParam::InternalForm::Direct1 || Form == BiquadParam::InternalForm::Transposed1) ? 4 : 2] { 0.0 };

    /** @brief Process a sample into the biquad */
    template<typename Type>
    [[nodiscard]] Type process(const Type in) noexcept;
    // [[nodiscard]] float process1(const float in) noexcept;
};


template<Audio::DSP::BiquadParam::Optimization Opti>
struct Audio::DSP::BiquadMaker
{
    template<typename Type>
    [[nodiscard]] static auto MakeBiquad(void)
            // const double sampleRate, const double freq, const double gain = 3.0, const double q = 0.70710678118654752440, bool qAsBandWidth = false
    noexcept {
        if constexpr (std::is_floating_point_v<Type>) {
            if constexpr (Opti == DSP::BiquadParam::Optimization::Classic)
                return Biquad<BiquadParam::InternalForm::Transposed1>();
            else if constexpr (Opti == BiquadParam::Optimization::Optimized)
                return Biquad<BiquadParam::InternalForm::Transposed2>();
        } else if constexpr (std::is_integral_v<Type>) {
            if constexpr (Opti == BiquadParam::Optimization::Classic)
                return Biquad<BiquadParam::InternalForm::Direct1>();
            else if constexpr (Opti == BiquadParam::Optimization::Optimized)
                return Biquad<BiquadParam::InternalForm::Direct2>();
        }
    }
};

#include "Biquad.ipp"

// // static_assert(alignof(DSP::Biquad) == 8, "Biquad must be aligned to 8 bytes !");
// // static_assert(sizeof(DSP::Biquad) == 80, "Biquad must take 80 bytes !");
