/**
 * @ Author: Pierre V
 * @ Description: Resampler
 */

#pragma once

#include <Audio/Buffer.hpp>

namespace Audio::DSP
{
    struct Resampler;
};

/**
 * @brief
 * Interpolate: insert L zero-sample between each pair of sample, then filter
 * Decimate: remove M sample between each pair of sample, then filter
 */
struct Audio::DSP::Resampler
{
    /**
     * @brief Interpolate a buffer with a specific sample number
     *
     * @param inputBuffer Buffer to interpolate
     * @param interpolationSamples Number of sample interpolated
     * @return Buffer Interpolated buffer
     */
    template<typename T>
    static Buffer Interpolate(const BufferView &inputBuffer, const std::size_t interpolationSamples) noexcept;

    /**
     * @brief Decimate a buffer with a specific sample number
     *
     * @param inputBuffer Buffer to decimate
     * @param interpolationSamples Number of sample decimated
     * @return Buffer Decimated buffer
     */
    template<typename T>
    static Buffer Decimate(const BufferView &inputBuffer, const std::size_t decimationSamples) noexcept;
};