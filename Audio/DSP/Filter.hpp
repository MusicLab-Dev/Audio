/**
 * @ Author: Pierre Veysseyre
 * @ Description: Window.ipp
 */

#pragma once

#include <Audio/Math.hpp>
#include <Audio/Modifier.hpp>

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
        std::uint32_t size;
        float sampleRate;
        float cutoffs[2];
        float gain;

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

    using FIRSpecs = Core::TinyVector<FIRSpec>;

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
    void GenerateFilter2(const FIRSpec specs, float *window) noexcept;

    void DesignFilterLowPass(float *window, const std::size_t size, const float cutoffRate, const float gain, const bool centered) noexcept;
    void DesignFilterHighPass(float *window, const std::size_t size, const float cutoffRate, const float gain, const bool centered) noexcept;
    void DesignFilterBandPass(float *window, const std::size_t size, const float cutoffRateBegin, const float cutoffRateEnd, const float gain, const bool centered) noexcept;
    void DesignFilterBandStop(float *window, const std::size_t size, const float cutoffRateBegin, const float cutoffRateEnd, const float gain, const bool centered) noexcept;

    void Hanning(const std::size_t size, float *window, const bool isSymetric = true) noexcept;
    void Hamming(const std::size_t size, float *window, const bool isSymetric = true) noexcept;



    constexpr auto  GenerateHanning = [](const std::size_t index, const std::size_t size)
    {
        return 0.5f - 0.5f * (std::cos(2.0f * static_cast<float>(M_PI) * static_cast<float>(index) / static_cast<float>(size - 1)));
    };

    constexpr auto GenerateHamming = [](const std::size_t index, const std::size_t size) -> float
    {
        return 0.54f - 0.46f * (std::cos(2.0f * static_cast<float>(M_PI) * static_cast<float>(index) / static_cast<float>(size - 1)));
    };

    // template<WindowType Window>
    // constexpr auto GetWindowGenerator(void)
    // {
    //     if constexpr (Window == WindowType::Hanning)
    //         return GenerateHanning;
    //     else if constexpr (Window == WindowType::Hamming)
    //         return GenerateHamming;
    //     else
    //         return GenerateHanning;
    // }

}

#include "Filter.ipp"
#include "Window.ipp"
