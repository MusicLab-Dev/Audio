/**
 * @ Author: Pierre Veysseyre
 * @ Description: Window.ipp
 */

#pragma once

#include "Audio/Math.hpp"

namespace Audio::DSP::Filter
{
    enum class WindowType : std::uint8_t
    {
        Hanning = 0,
        Hamming
    };

    enum class FilterType : std::uint8_t
    {
        LowPass = 0,
        BandPass,
        BandStop,
        HighPass
    };

    struct FilterSpecs
    {
        FilterType filterType;
        WindowType windowType;
        std::size_t windowSize;
        float sampleRate;
        float cutoffs[2];
    };

    void GenerateFilterCoefficients(const WindowType type, const std::size_t size, float *windowCoefficients, const bool isSymetric = true) noexcept;
    void DesignFilter(const FilterSpecs specs, float *windowCoefficients, const std::size_t windowSize, const bool centered) noexcept;
    void DesignFilterLowPass(float *windowCoefficients, const std::size_t size, const double cutoffRate, const bool centered) noexcept;

    void Hanning(const std::size_t size, float *windowCoefficients, const bool isSymetric = true) noexcept;
    void Hamming(const std::size_t size, float *windowCoefficients, const bool isSymetric = true) noexcept;


}

#include "Filter.ipp"