/**
 * @ Author: Pierre Veysseyre
 * @ Description: Filter.hpp
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
        Hamming,
        Default = Hanning
    };

    /** @brief Describe type used for basic filter */
    enum class BasicType : std::uint8_t
    {
        LowPass = 0,
        BandPass,
        BandStop,
        HighPass,
        Default = LowPass
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
        HighShelf,
        Default = LowPass
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

    /** @brief Generate window with compile-time window type */
    template<WindowType Window = WindowType::Default>
    void GenerateWindow(float *window, const std::size_t size, const bool symetric) noexcept;
    /** @brief Generate window with run-time window type */
    void GenerateWindow(const WindowType type, const std::size_t size, float *window, const bool symetric = true) noexcept;

    /** @brief Design filter coefficients within a window with a run-time filter type */
    template<BasicType Filter = BasicType::Default>
    void GenerateFilter(const FIRSpec specs, float *window, const bool symetric = true) noexcept;
    void GenerateFilter(const FIRSpec specs, float *window, const bool symetric = true) noexcept;


    template<bool ProcessWindow = false>
    void GenerateFilterLowPass(const FIRSpec specs, float *window, const bool symetric) noexcept;
    template<bool ProcessWindow = false>
    void GenerateFilterHighPass(const FIRSpec specs, float *window, const bool symetric) noexcept;
    template<bool ProcessWindow = false>
    void GenerateFilterBandPass(const FIRSpec specs, float *window, const bool symetric) noexcept;
    template<bool ProcessWindow = false>
    void GenerateFilterBandStop(const FIRSpec specs, float *window, const bool symetric) noexcept;

    void GenerateHanning(float *window, const std::size_t size, const bool symetric = true) noexcept;
    void GenerateHamming(float *window, const std::size_t size, const bool symetric = true) noexcept;


    constexpr auto Hanning = [](const std::size_t index, const std::size_t size) -> float
    {
        return 0.5f - 0.5f * (std::cos(2.0f * static_cast<float>(M_PI) * static_cast<float>(index) / static_cast<float>(size - 1)));
    };

    constexpr auto Hamming = [](const std::size_t index, const std::size_t size) -> float
    {
        return 0.54f - 0.46f * (std::cos(2.0f * static_cast<float>(M_PI) * static_cast<float>(index) / static_cast<float>(size - 1)));
    };

    template<WindowType Window>
    inline float ComputeWindow(const std::size_t index, const std::size_t size) {
        switch (Window) {
        case WindowType::Hanning:
            return Hanning(index, size);
        case WindowType::Hamming:
            return Hamming(index, size);
        default:
            return Hanning(index, size);
        }
    }
    inline float ComputeWindow(WindowType windowType, const std::size_t index, const std::size_t size)
    {
        switch (windowType) {
        case WindowType::Hanning:
            return ComputeWindow<WindowType::Hanning>(index, size);
        case WindowType::Hamming:
            return ComputeWindow<WindowType::Hamming>(index, size);
        default:
            return ComputeWindow<WindowType::Default>(index, size);
        }
    }


}

#include "Window.ipp"
#include "Filter.ipp"
