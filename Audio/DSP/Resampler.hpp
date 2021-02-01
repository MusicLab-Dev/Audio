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
 * @brief
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

    /**
     * @brief Interpolate a buffer with a specific sample number
     *
     * @param inputBuffer Buffer to interpolate
     * @param interpolationSamples Number of sample interpolated
     * @return Buffer Interpolated buffer
     */
    static Buffer Interpolate(const BufferView &inputBuffer, const std::size_t interpolationSamples) noexcept;

    /**
     * @brief Decimate a buffer with a specific sample number
     *
     * @param inputBuffer Buffer to decimate
     * @param interpolationSamples Number of sample decimated
     * @return Buffer Decimated buffer
     */
    static Buffer Decimate(const BufferView &inputBuffer, const std::size_t decimationSamples) noexcept;

    static Buffer InterpolateOctave(const BufferView &inputBuffer, const std::uint8_t nOctave) noexcept;
    static Buffer DecimateOctave(const BufferView &inputBuffer, const std::uint8_t nOctave) noexcept;

    /**
     * @brief

     * @param inputBuffer Buffer to resample
     * @param semitone Semiton
     * @return Buffer Resampled buffer
     */
    static Buffer ResampleSemitone(const BufferView &inputBuffer, const Semitone semitone) noexcept;

private:
    static std::size_t GetOptimalResamplingSize(const std::size_t inputSize, const Semitone semitone) noexcept;
    static std::size_t GetRealResamplingSize(const std::size_t inputSize, const Semitone semitone) noexcept;

    static Buffer ResampleOneSemitone(const BufferView &inputBuffer, bool upScale) noexcept;
};

#include "Resampler.ipp"
