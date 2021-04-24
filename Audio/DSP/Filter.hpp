/**
 * @ Author: Pierre Veysseyre
 * @ Description: Window.ipp
 */

#pragma once

#include "Audio/Math.hpp"

namespace Audio::DSP::Filter
{
    // http://www.labbookpages.co.uk/audio/firWindowing.html
    // https://dsp.stackexchange.com/questions/9408/fir-filter-with-linear-phase-4-types
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

    struct FIRSpec
    {
        BasicType filterType { BasicType::LowPass };
        WindowType windowType { WindowType::Hanning };
        std::size_t size;
        double sampleRate;
        double cutoffs[2];
        double gain;

        bool operator==(const FIRSpec &other) {
            return (
                filterType == other.filterType &&
                windowType == other.windowType &&
                size == other.size &&
                sampleRate == other.sampleRate &&
                cutoffs[0] == other.cutoffs[0] &&
                cutoffs[1] == other.cutoffs[1] &&
                gain == other.gain
            );
        }
    };

    /**
     * @brief Generate window
     * @warning Window output is [0:1]
     */
    void GenerateWindow(const WindowType type, const std::size_t size, float *window, const bool centered = true) noexcept;

    /**
     * @brief Design filter coefficients within a window
     * @warning Call GenerateWindow before to populate the window for the filter coefficients
     */
    void DesignFilter(const FIRSpec specs, float *window, const std::size_t windowSize, const bool centered) noexcept;

    /** @brief Helper to fully generate filter coefficients */
    void GenerateFilter(const FIRSpec specs, float *window, const bool centered = true) noexcept;

    void DesignFilterLowPass(float *window, const std::size_t size, const double cutoffRate, const double gain, const bool centered) noexcept;
    void DesignFilterHighPass(float *window, const std::size_t size, const double cutoffRate, const double gain, const bool centered) noexcept;
    void DesignFilterBandPass(float *window, const std::size_t size, const double cutoffRateBegin, const double cutoffRateEnd, const double gain, const bool centered) noexcept;
    void DesignFilterBandStop(float *window, const std::size_t size, const double cutoffRateBegin, const double cutoffRateEnd, const double gain, const bool centered) noexcept;

    void Hanning(const std::size_t size, float *window, const bool isSymetric = true) noexcept;
    void Hamming(const std::size_t size, float *window, const bool isSymetric = true) noexcept;
}

#include "Filter.ipp"
#include "Window.ipp"
