/**
 * @ Author: Pierre V
 * @ Description: Resampler
 */

#pragma once

#include <cmath>
#include <numeric>
#include <bitset>

#include <Audio/Base.hpp>
// #include <Audio/Buffer.hpp>

#include "Biquad.hpp"

namespace Audio::DSP
{
    struct Resampler;

    /** @brief Call these funtions to get the outputBuffer size used for the Interpolate/Decimate/Resample functions */
    [[nodiscard]] static std::size_t GetInterpolationOctaveSize(const std::size_t inputSize, const std::uint8_t nOctave) noexcept;
    [[nodiscard]] static std::size_t GetDecimationOctaveSize(const std::size_t inputSize, const std::uint8_t nOctave) noexcept;

    [[nodiscard]] static std::size_t GetInterpolationSize(const std::size_t inputSize, const std::size_t interpolationRatio) noexcept;
    [[nodiscard]] static std::size_t GetDecimationSize(const std::size_t inputSize, const std::size_t decimationRatio) noexcept;

    [[nodiscard]] static std::size_t GetResamplingSizeOneSemitone(const std::size_t inputSize, const bool upScale) noexcept;
    [[nodiscard]] static std::size_t GetResamplingSizeSemitone(const std::size_t inputSize, const Semitone semitone) noexcept;
    [[nodiscard]] static std::size_t GetResamplingSizeOctave(const std::size_t inputSize, const std::int8_t nOctave) noexcept;
    [[nodiscard]] static std::size_t GetResamplingSizeSampleRate(const std::size_t inputSize, const SampleRate inSampleRate, const SampleRate outSampleRate) noexcept;

    [[nodiscard]] static std::size_t GetOptimalResamplingSize(const std::size_t inputSize, const Semitone semitone) noexcept;
    // [[nodiscard]] static std::size_t GetResamplingSize(const std::size_t inputSize, const std::size_t newSize) noexcept;
};

/**
 * Interpolate (stretch the signal): insert L zero-sample between each pair of sample, then filter
 * Decimate (compress the signal): filter, then remove M sample between each pair of sample
 * Resample (change signal pitch): it's combine both interpolation & decimation procedures
 */
struct Audio::DSP::Resampler
{
    /** @brief Interpolation factor of 1 semitone */
    static constexpr auto L_Factor = 185; //7450u;  --> 5 iterations of ratio 37
    /** @brief Decimation factor of 1 semitone */
    static constexpr auto M_Factor = 196; //7893u;  --> 7 iterations of ratio 28

    static constexpr std::int8_t RootKeyDefaultOctave = 6; // Within the range [0, 12[

    struct Internal
    {
        /** @brief Interpolate the inputBuffer into the outputBuffer, outputBuffer size must be inputSize * nOctave */
        template<typename Type>
        static void InterpolateOctave(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const std::uint8_t nOctave);
        /** @brief Decimate the inputBuffer into the outputBuffer, outputBuffer size must be inputSize / nOctave */
        template<typename Type>
        static void DecimateOctave(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const std::uint8_t nOctave);
    };


    /** @brief Interpolate the inputBuffer into the outputBuffer, outputBuffer size must fit with interpolationRatio */
    template<typename Type>
    static void Interpolate(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const std::size_t interpolationRatio);
    /** @brief Decimate the inputBuffer into the outputBuffer, outputBuffer size must fit with decimationRatio */
    template<typename Type>
    static void Decimate(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const std::size_t decimationRatio);

    /** @brief Resample the inputBuffer into outputBuffer, outputBuffer size must fit the semitone, call GetResamplingSizeSemitone to get the outputBuffer size */
    template<bool Accumulate, typename Type>
    static void ResampleSemitone(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const SampleRate inSampleRate, const bool upScale);
    /** @brief Resample the inputBuffer into outputBuffer, outputBuffer size must fit the outSampleRate, call GetResamplingSize to get the outputBuffer size */
    template<typename Type>
    static void ResampleSampleRate(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const SampleRate inSampleRate, const SampleRate outSampleRate);

    template<bool Accumulate, typename Type>
    static void ResampleOctave(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const SampleRate sampleRate, const std::int8_t nOctave, const std::size_t offset = 0u);



    // [[nodiscard]] static Buffer ResampleBySemitone(const BufferView &inputBuffer, const Semitone semitone) noexcept_ndebug;

    // [[nodiscard]] static Buffer ResampleBySamplerate(const BufferView &inputBuffer, const SampleRate newSampleRate) noexcept_ndebug;

    // /** @brief Resample by a specific in & out sampleRate */
    // [[nodiscard]] static Buffer ResampleSpecificSampleRate(const BufferView &inputBuffer, const SampleRate inSampleRate, const SampleRate outSampleRate) noexcept_ndebug;
};

#include "Interpolation.ipp"
#include "Decimation.ipp"
#include "Resampler.ipp"
