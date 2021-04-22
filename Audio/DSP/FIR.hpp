/**
 * @file FIR.hpp
 * @brief FIR filtering
 *
 * @author Pierre V
 * @date 2021-04-22
 */

#pragma once

#include <Core/Vector.hpp>

#include "Filter.hpp"

namespace Audio::DSP
{
    /** FIR instance */
    template<typename Type>
    class FIR;

    /** FIR filter using an FIR instance */
    template<typename Type>
    class FIRFilter;

    // struct Filter::FIRSpecs
    // {
    //     Filter::BasicType filterType;
    //     Filter::WindowType windowType;
    //     std::size_t size;
    //     float sampleRate;
    //     float cutoffs[2] { 0.0f, 0.0f };

    //     bool operator==(const Filter::FIRSpecs other) {
    //         return (
    //             filterType == other.filterType &&
    //             windowType == other.windowType &&
    //             size == other.size &&
    //             sampleRate == other.sampleRate &&
    //             cutoffs[0] == other.cutoffs[0] &&
    //             cutoffs[1] == other.cutoffs[1]
    //         );
    //     }
    // };
}

template<typename Type>
class Audio::DSP::FIR
{
public:
    using Cache = Core::TinyVector<Type>;

    using VoidType = std::enable_if_t<std::is_floating_point_v<Type>, void>;
    using ProcessType = std::enable_if_t<std::is_floating_point_v<Type>, Type>;

    /** @brief Perform filtering using convolution. ZeroPad is used to 'add' zero before the input during processing stage */
    VoidType filter(const Type *input, const std::size_t inputSize, Type *output, const bool useLastInput) noexcept;

    /** @brief Perform filtering using convolution. LastInput is used instead of zero padding */
    VoidType filter(
            const Type *input, const std::size_t inputSize,
            const Type *filterCoefficients, const std::size_t filterSize,
            Type *output,
            const Type *lastInput, const std::size_t lastInputSize) noexcept;

    /** @brief Get the internal cache coefficients */
    [[nodiscard]] const Cache &coefficients(void) const noexcept { return _coefficients; }
    [[nodiscard]] Cache &coefficients(void) noexcept { return _coefficients; }

    /** @brief Get the internal cache for the last input */
    [[nodiscard]] const Cache &lastInput(void) const noexcept { return _lastInputCache; }
    [[nodiscard]] Cache &lastInput(void) noexcept { return _lastInputCache; }

private:
    // ProcessType filterImpl(const Type *input, const Type *window, const std::size_t size, const std::size_t zeroPadSize) noexcept;

    /** @brief Filter cache coefficients */
    Cache _coefficients;
    /** @brief Last input cache used for block processing */
    Cache _lastInputCache;
};

template<typename Type>
class Audio::DSP::FIRFilter
{
public:
    FIRFilter(const Filter::FIRSpecs specs) : _specs(specs) { setSpecs(specs); }

    /** @brief Set the internal specs. It will recompute the instance coefficients */
    bool setSpecs(const Filter::FIRSpecs specs) noexcept;
    /** @brief Set the internal cutoffs */
    bool setCutoffs(const float cutoffFrom, const float cutoffTo = 0.0f) noexcept;
    /** @brief Set the internal sampleRate */
    bool setSampleRate(const float sampleRate) noexcept;
    /** @brief Set the internal sampleRate */
    bool setFilterType(const Filter::BasicType filterType) noexcept;

    /** @brief Reset the internal last input cache */
    void resetLastInputCache(void) noexcept { _instance.lastInput().clear(); }

    /** @brief Filter an input signal */
    void filter(const Type *input, const std::size_t inputSize, Type *output, const bool useLastInput) noexcept { _instance.filter(input, inputSize, output, useLastInput); }

private:
    /** @brief FIR instance */
    FIR<Type> _instance;
    /** @brief Filter specs */
    Filter::FIRSpecs _specs;
};

#include "FIR.ipp"