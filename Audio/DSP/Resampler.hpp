/**
 * @ Author: Pierre V
 * @ Description: Resampler
 */

#pragma once

#include <cmath>
#include <Audio/Buffer.hpp>

namespace Audio::DSP
{
    template<typename Type>
    struct Resampler;
};

/**
 * Interpolate (stretch the signal): insert L zero-sample between each pair of sample, then filter
 * Decimate (compress the signal): filter, then remove M sample between each pair of sample
 * Resample (change signal pitch): it's combine both interpolation & decimation procedures
 */
template<typename Type>
struct Audio::DSP::Resampler
{
    /** @brief Interpolation factor of 1 semitone */
    static constexpr auto L_Factor = 185; //7450u;
    /** @brief Decimation factor of 1 semitone */
    static constexpr auto M_Factor = 196; //7893u;

    static constexpr std::int8_t RootKeyDefaultOctave = 6; // Within the range [0, 12[

    struct Internal
    {
        /** @brief Interpolate the inputBuffer into the outputBuffer, outputBuffer size must be nOctave * inputSize */
        static void InterpolateOctave(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const std::uint8_t nOctave) noexcept_ndebug;
    };


    /** @brief Interpolate the inputBuffer into the outputBuffer, outputBuffer size must fit with interpolationRatio */
    static void Interpolate(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const std::size_t interpolationRatio) noexcept_ndebug;
    /** @brief Decimate the inputBuffer into the outputBuffer, outputBuffer size must fit with decimationRatio */
    static void Decimate(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const std::size_t decimationRatio) noexcept_ndebug;

    /** @brief Resample the inputBuffer into outputBuffer, outputBuffer size must fit the semitone, call GetResamplingSizeSemitone to get the outputBuffer size */
    static void ResampleSemitone(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const Semitone semitone) noexcept_ndebug;
    /** @brief Resample the inputBuffer into outputBuffer, outputBuffer size must fit the outSampleRate, call GetResamplingSize to get the outputBuffer size */
    static void ResampleSampleRate(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const SampleRate outSampleRate) noexcept_ndebug;




    [[nodiscard]] static Buffer ResampleBySemitone(const BufferView &inputBuffer, const Semitone semitone) noexcept_ndebug;

    [[nodiscard]] static Buffer ResampleBySamplerate(const BufferView &inputBuffer, const SampleRate newSampleRate) noexcept_ndebug;

    /** @brief Resample by a specific in & out sampleRate */
    [[nodiscard]] static Buffer ResampleSpecificSampleRate(const BufferView &inputBuffer, const SampleRate inSampleRate, const SampleRate outSampleRate) noexcept_ndebug;


    static void GenerateDefaultOctave(const BufferView &inputBuffer, BufferViews &outBuffers) noexcept;


    /** @brief Call these funtions to get the outputBuffer size used for the Interpolate/Decimate/Resample functions */
    [[nodiscard]] static std::size_t GetInterpolationSize(const std::size_t inputSize, const std::size_t interpolationRatio) noexcept;
    [[nodiscard]] static std::size_t GetDecimationSize(const std::size_t inputSize, const std::size_t decimationRatio) noexcept;
    [[nodiscard]] static std::size_t GetInterpolationOctaveSize(const std::size_t inputSize, const std::uint8_t nOctave) noexcept;
    [[nodiscard]] static std::size_t GetDecimationOctaveSize(const std::size_t inputSize, const std::uint8_t nOctave) noexcept;
    [[nodiscard]] static std::size_t GetResamplingSizeSemitone(const std::size_t inputSize, const Semitone semitone) noexcept;
    [[nodiscard]] static std::size_t GetResamplingSize(const std::size_t inputSize, const SampleRate inSampleRate, const SampleRate outSampleRate) noexcept;

private:
    [[nodiscard]] static std::size_t GetOptimalResamplingSize(const std::size_t inputSize, const Semitone semitone) noexcept;
    // [[nodiscard]] static std::size_t GetResamplingSize(const std::size_t inputSize, const std::size_t newSize) noexcept;

    /** @brief Resample by a one semitone implementation */
    static void ResampleClosestSemitoneImpl(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, bool upScale) noexcept;
    /** @brief Resample by semitones implementation within an octave */
    static void ResampleSemitoneOctaveImpl(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const Semitone semitone) noexcept;
};

#include "Resampler.ipp"
