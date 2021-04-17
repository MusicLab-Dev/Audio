/**
 * @ Author: Pierre Veysseyre
 * @ Description: Window.ipp
 */

#include <cmath>

inline void Audio::DSP::Filter::GenerateFilterCoefficients(const WindowType type, const std::size_t size, float *windowCoefficients, const bool isSymetric) noexcept
{
    switch (type) {
    case WindowType::Hanning:
        return Hanning(size, windowCoefficients, isSymetric);
    case WindowType::Hamming:
        return Hamming(size, windowCoefficients, isSymetric);
    default:
        return Hanning(size, windowCoefficients, isSymetric);
    }
}

inline void Audio::DSP::Filter::DesignFilter(const FilterSpecs specs, float *windowCoefficients, const std::size_t windowSize, const bool centered) noexcept
{
    const float cutoffRate = specs.cutoffs[0] / static_cast<float>(specs.sampleRate);
    switch (specs.filterType) {
    case FilterType::LowPass:
        return DesignFilterLowPass(windowCoefficients, windowSize, cutoffRate, centered);

    default:
        return DesignFilterLowPass(windowCoefficients, windowSize, cutoffRate, centered);
    }
}

inline void Audio::DSP::Filter::DesignFilterLowPass(float *windowCoefficients, const std::size_t size, const double cutoffRate, const bool centered) noexcept
{
    const std::size_t first = centered ? (size / 2) : size - 1;
    const double realRate = 2.0 * cutoffRate;
    for (auto i = 0u; i < size; ++i) {
        int idx = i - first;
        *windowCoefficients *= (realRate * Utils::sinc(idx * realRate));
        ++windowCoefficients;
    }
}

inline void Audio::DSP::Filter::Hanning(const std::size_t size, float *outputWindow, const bool isSymetric) noexcept
{
    if (isSymetric) {
        for (auto i = 0u; i < size; i++) {
            outputWindow[i] = 0.5f - 0.5f * (std::cos(2.0 * M_PI * i / static_cast<float>(size - 1)));
        }
    } else {
    }
}

inline void Audio::DSP::Filter::Hamming(const std::size_t size, float *outputWindow, const bool isSymetric) noexcept
{
    if (isSymetric) {
        for (auto i = 0u; i < size; i++) {
            outputWindow[i] = 0.54f - 0.46f * (std::cos(2.0 * M_PI * i / static_cast<float>(size - 1)));
        }
    } else {
    }
}