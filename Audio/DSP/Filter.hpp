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
        HighPass,
        BandPass,
        BandStop,
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
        std::uint32_t order;
        float sampleRate;
        float cutoffs[2];
        float gain;

        bool operator==(const FIRSpec &other) {
            return (
                filterType == other.filterType &&
                windowType == other.windowType &&
                order == other.order &&
                sampleRate == other.sampleRate &&
                cutoffs[0] == other.cutoffs[0] &&
                cutoffs[1] == other.cutoffs[1] &&
                gain == other.gain
            );
        }
    };

    struct FIRSpecMulti
    {
        WindowType windowType { WindowType::Hanning };
        float sampleRate;
        std::uint32_t order;
        std::uint32_t instanceCount;
        Core::TinyVector<BasicType> filterType {};
        Core::TinyVector<float> gain;
        Core::TinyVector<float> cutoffs;
    };

    /** @brief Generate window with compile-time window type */
    template<bool Accumulate = false, WindowType Window = WindowType::Default>
    void GenerateWindow(float *window, const std::size_t size, const bool symetric = true) noexcept;
    /** @brief Generate window with run-time window type */
    template<bool Accumulate = false>
    void GenerateWindow(const WindowType type, const std::size_t size, float *window, const bool symetric = true) noexcept;

    /** @brief Design filter coefficients within a window with a run-time filter type */
    // template<bool Accumulate = false, BasicType Filter = BasicType::Default>
    // void GenerateFilter(const FIRSpec specs, float *window) noexcept;
    template<bool ProcessWindow = true, bool Accumulate = false>
    void GenerateFilter(const FIRSpec specs, float *window) noexcept;


    template<bool ProcessWindow = true, bool Accumulate = false>
    void GenerateFilterLowPass(const FIRSpec specs, float *window) noexcept;
    template<bool ProcessWindow = true, bool Accumulate = false>
    void GenerateFilterHighPass(const FIRSpec specs, float *window) noexcept;
    template<bool ProcessWindow = true, bool Accumulate = false>
    void GenerateFilterBandPass(const FIRSpec specs, float *window) noexcept;
    template<bool ProcessWindow = true, bool Accumulate = false>
    void GenerateFilterBandStop(const FIRSpec specs, float *window) noexcept;

    template<bool Accumulate = false>
    void GenerateHanning(float *window, const std::size_t size, const bool symetric) noexcept;
    template<bool Accumulate = false>
    void GenerateHamming(float *window, const std::size_t size, const bool symetric) noexcept;


    constexpr auto Hanning = [](const std::size_t index, const std::size_t size, const bool symetric) -> float
    {
        UNUSED(symetric);
        return 0.5f - 0.5f * (std::cos(2.0f * static_cast<float>(M_PI) * static_cast<float>(index) / static_cast<float>(size - 1)));
    };

    constexpr auto Hamming = [](const std::size_t index, const std::size_t size, const bool symetric) -> float
    {
        UNUSED(symetric);
        return 0.54f - 0.46f * (std::cos(2.0f * static_cast<float>(M_PI) * static_cast<float>(index) / static_cast<float>(size - 1)));
    };

    template<WindowType Window, bool Accumulate = false>
    inline float ComputeWindow(const std::size_t index, const std::size_t size, const bool symetric) {
        switch (Window) {
        case WindowType::Hanning:
            return Hanning(index, size, symetric);
        case WindowType::Hamming:
            return Hamming(index, size, symetric);
        default:
            return Hanning(index, size, symetric);
        }
    }
    template<bool Accumulate = false>
    inline float ComputeWindow(WindowType windowType, const std::size_t index, const std::size_t size, const bool symetric)
    {
        switch (windowType) {
        case WindowType::Hanning:
            return ComputeWindow<WindowType::Hanning, Accumulate>(index, size, symetric);
        case WindowType::Hamming:
            return ComputeWindow<WindowType::Hamming, Accumulate>(index, size, symetric);
        default:
            return ComputeWindow<WindowType::Default, Accumulate>(index, size, symetric);
        }
    }


}

#include "Window.ipp"
#include "Filter.ipp"
