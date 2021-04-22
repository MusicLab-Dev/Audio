/**
 * @ Author: Pierre V
 * @ Description: Resampler
 */

#pragma once

#include <numeric>

#include <Core/Vector.hpp>

#include "Filter.hpp"

namespace Audio::DSP
{
    /** @brief Handler to perform resampling using FIR for filtering stage */
    template<typename Type>
    class Resampler;
}

template<typename Type>
class Audio::DSP::Resampler
{
public:
    /** @brief Interpolation factor of 1 semitone */
    static constexpr std::size_t InterpolationSemitoneFactor = 185; //7450u;  --> 5 iterations of ratio 3

    /** @brief Decimation factor of 1 semitone */
    static constexpr std::size_t DecimationSemitoneFactor = 196; //7893u;  --> 7 iterations of ratio 28


    /** @brief Get the output buffer size for a resample semitone call */
    [[nodiscard]] static std::size_t GetResampleSemitoneBufferSize(const std::size_t inputSize, const bool upScale) noexcept;

    /** @brief Get the output buffer size for a resample sample rate call */
    [[nodiscard]] static std::size_t GetResampleSampleRateBufferSize(const std::size_t inputSize, const SampleRate inSampleRate, const SampleRate outSampleRate) noexcept;

    /** @brief Get the output buffer size for a resample octave call */
    [[nodiscard]] static std::size_t GetResampleOctaveBufferSize(const std::size_t inputSize, const int nOctave) noexcept;


    /** @brief Resample the inputBuffer into outputBuffer, outputBuffer size must fit the semitone, call GetResamplingSizeSemitone to get the outputBuffer size */
    template<bool Accumulate, unsigned ProcessSize>
    void resampleSemitone(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const SampleRate sampleRate, const bool upScale, const std::size_t inputOffset = 0u) noexcept;

    /** @brief Resample the inputBuffer into outputBuffer, shifting by an octave count */
    template<bool Accumulate, unsigned ProcessSize>
    void resampleOctave(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const SampleRate sampleRate, const int nOctave, const std::size_t inputOffset = 0u) noexcept;

    /** @brief Resample the inputBuffer into outputBuffer, outputBuffer size must fit the outSampleRate, call GetResamplingSize to get the outputBuffer size */
    template<unsigned ProcessSize>
    void resampleSampleRate(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const SampleRate inSampleRate, const SampleRate outSampleRate) noexcept;

private:
    Core::TinyVector<Type> _filterCache;
    Core::TinyVector<Type> _interpolationCache;
    Core::TinyVector<Type> _produceCache;

};

#include "Resampler.ipp"