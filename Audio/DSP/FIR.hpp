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

    template<typename Type>
    using VoidType = std::enable_if_t<std::is_floating_point_v<Type>, void>;
    template<typename Type>
    using ProcessType = std::enable_if_t<std::is_floating_point_v<Type>, Type>;
}

template<typename Type>
class Audio::DSP::FIR
{
public:
    using Cache = Core::TinyVector<Type>;

    /** @brief Perform filtering using convolution. */
    template<bool UseLastInput>
    VoidType<Type> filter(const Type *input, const std::size_t inputSize, Type *output) noexcept;

    /** @brief Get the internal cache coefficients */
    [[nodiscard]] const Cache &coefficients(void) const noexcept { return _coefficients; }
    [[nodiscard]] Cache &coefficients(void) noexcept { return _coefficients; }

    /** @brief Get the internal cache for the last input */
    [[nodiscard]] const Cache &lastInput(void) const noexcept { return _lastInputCache; }
    [[nodiscard]] Cache &lastInput(void) noexcept { return _lastInputCache; }

private:
    template<bool UseLastInput>
    ProcessType<Type> filterImpl(const Type *input, const std::size_t size, const std::size_t zeroPad = 0ul) noexcept;

    /** @brief Filter cache coefficients */
    Cache _coefficients;
    /** @brief Last input cache used for block processing */
    Cache _lastInputCache;
};

template<typename Type>
class Audio::DSP::FIRFilter
{
public:
    FIRFilter(void) = default;

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
    template<bool UseLastInput>
    VoidType<Type> filter(const Type *input, const std::size_t inputSize, Type *output) noexcept;

private:
    /** @brief FIR instance */
    FIR<Type> _instance;
    /** @brief Filter specs */
    Filter::FIRSpecs _specs;
};

#include "FIR.ipp"