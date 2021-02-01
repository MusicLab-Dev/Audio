/**
 * @ Author: Pierre V
 * @ Description: Resampler
 */

#pragma once

#include <Audio/Buffer.hpp>

namespace Audio::DSP
{
    template<typename T>
    struct Resampler;
};

/**
 * Interpolate (stretch the signal): insert L zero-sample between each pair of sample, then filter
 * Decimate (compress the signal): filter, then remove M sample between each pair of sample
 * Resample (change signal pitch): it's combine both interpolation & decimation procedures
 */
template<typename T>
struct Audio::DSP::Resampler
{
    /** @brief Interpolation factor of 1 semitone */
    static constexpr auto L_Factor = 185; //7450u;
    /** @brief Decimation factor of 1 semitone */
    static constexpr auto M_Factor = 196; //7893u;


    /** @brief Interpolate a buffer with a specific sample number */
    static Buffer Interpolate(const BufferView &inputBuffer, const std::size_t interpolationSamples) noexcept;
    /** @brief Interpolate a buffer with nOctave */
    static Buffer InterpolateOctave(const BufferView &inputBuffer, const std::uint8_t nOctave) noexcept;

    /** @brief Decimate a buffer with a specific sample number */
    static Buffer Decimate(const BufferView &inputBuffer, const std::size_t decimationSamples) noexcept;
    /** @brief Decimate a buffer with nOctave */
    static Buffer DecimateOctave(const BufferView &inputBuffer, const std::uint8_t nOctave) noexcept;


    /**
     * @brief Resample a buffer by semitone
     * @param semitone Semitone within the range [-11, 11] and != 0
     */
    static Buffer ResampleSemitone(const BufferView &inputBuffer, const Semitone semitone) noexcept;


    static void GenerateDefaultOctave(const BufferView &inputBuffer, BufferViews &outBuffers) noexcept;

private:
    static std::size_t GetOptimalResamplingSize(const std::size_t inputSize, const Semitone semitone) noexcept;
    static std::size_t GetRealResamplingSize(const std::size_t inputSize, const Semitone semitone) noexcept;

    static Buffer ResampleOneSemitone(const BufferView &inputBuffer, bool upScale) noexcept;
};

#include "Resampler.ipp"
