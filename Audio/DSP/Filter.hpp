/**
 * @ Author: Pierre Veysseyre
 * @ Description: Window.ipp
 */

#pragma once

#include "Audio/Math.hpp"

namespace Audio::DSP::Filter
{
    // https://technobyte.org/windowing-design-fir-filter-method/
    enum class WindowType : std::uint8_t
    {
        Hanning = 0,
        Hamming
    };

    /** @brief Describe type used for basic filter */
    enum class BasicType : std::uint8_t
    {
        LowPass = 0,
        BandPass,
        BandStop,
        HighPass
    };

    /** @brief Describe type used for a second-order IIR filter section */
    enum class AdvancedType : uint8_t
    {
        LowPass = 0u,
        HighPass,
        BandPass,
        BandPass2,
        BandStop, /* Notch */
        Peak,
        LowShelf,
        HighShelf
    };

    struct Specs
    {
        BasicType filterType;
        WindowType windowType;
        std::size_t windowSize;
        float sampleRate;
        float cutoffs[2];
    };

    void GenerateFilterCoefficients(const WindowType type, const std::size_t size, float *windowCoefficients, const bool isSymetric = true) noexcept;
    void DesignFilter(const Specs specs, float *windowCoefficients, const std::size_t windowSize, const bool centered) noexcept;
    void DesignFilterLowPass(float *windowCoefficients, const std::size_t size, const double cutoffRate, const bool centered) noexcept;
    void DesignFilterHighPass(float *windowCoefficients, const std::size_t size, const double cutoffRate, const bool centered) noexcept;
    void DesignFilterBandPass(float *windowCoefficients, const std::size_t size, const double cutoffRateBegin, const double cutoffRateEnd, const bool centered) noexcept;
    void DesignFilterBandStop(float *windowCoefficients, const std::size_t size, const double cutoffRateBegin, const double cutoffRateEnd, const bool centered) noexcept;

    void Hanning(const std::size_t size, float *windowCoefficients, const bool isSymetric = true) noexcept;
    void Hamming(const std::size_t size, float *windowCoefficients, const bool isSymetric = true) noexcept;


}

#include "Filter.ipp"