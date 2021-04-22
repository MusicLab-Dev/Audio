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

inline void Audio::DSP::Filter::DesignFilter(const Specs specs, float *windowCoefficients, const std::size_t windowSize, const bool centered) noexcept
{
    const float cutoffRateBegin = specs.cutoffs[0] / static_cast<float>(specs.sampleRate);
    switch (specs.filterType) {
    case BasicType::LowPass:
        return DesignFilterLowPass(windowCoefficients, windowSize, cutoffRateBegin, centered);
    case BasicType::HighPass:
        return DesignFilterHighPass(windowCoefficients, windowSize, cutoffRateBegin, centered);
    case BasicType::BandPass:
    {
        const float cutoffRateEnd = specs.cutoffs[1] / static_cast<float>(specs.sampleRate);
        return DesignFilterBandPass(windowCoefficients, windowSize, cutoffRateBegin, cutoffRateEnd, centered);
    }
    case BasicType::BandStop:
    {
        const float cutoffRateEnd = specs.cutoffs[1] / static_cast<float>(specs.sampleRate);
        return DesignFilterBandStop(windowCoefficients, windowSize, cutoffRateBegin, cutoffRateEnd, centered);
    }
    default:
        return DesignFilterLowPass(windowCoefficients, windowSize, cutoffRateBegin, centered);
    }
}

inline void Audio::DSP::Filter::DesignFilterLowPass(float *windowCoefficients, const std::size_t size, const double cutoffRate, const bool centered) noexcept
{
    // cutoffNorm/pi*sinc(cutoffNorm*x/pi)
    const std::size_t first = centered ? (size / 2) : size - 1;
    const double realRate = 2.0 * cutoffRate;
    for (auto i = 0u; i < size; ++i) {
        int idx = i - first;
        *windowCoefficients *= (realRate * Utils::sinc<true>(idx * realRate));
        ++windowCoefficients;
    }
}

inline void Audio::DSP::Filter::DesignFilterHighPass(float *windowCoefficients, const std::size_t size, const double cutoffRate, const bool centered) noexcept
{
    // sinc(x) - cutoffNorm/pi*sinc(cutoffNorm*x/pi)
    const std::size_t first = centered ? (size / 2) : size - 1;
    const double realRate = 2.0 * cutoffRate;
    for (auto i = 0u; i < size; ++i) {
        int idx = i - first;
        *windowCoefficients *= Utils::sinc<true>(static_cast<float>(idx)) - (realRate * Utils::sinc<true>(idx * realRate));
        ++windowCoefficients;
    }
}

inline void Audio::DSP::Filter::DesignFilterBandPass(float *windowCoefficients, const std::size_t size, const double cutoffRateBegin, const double cutoffRateEnd, const bool centered) noexcept
{
    // sinc(x) - cutoffNorm/pi*sinc(cutoffNorm*x/pi)
    const std::size_t first = centered ? (size / 2) : size - 1;
    const double realRateBegin = 2.0 * cutoffRateBegin;
    // const double realRateEnd = 2.0 * cutoffRateEnd;
    for (auto i = 0u; i < size; ++i) {
        int idx = i - first;
        *windowCoefficients *= Utils::sinc<true>(static_cast<float>(idx)) - (realRateBegin * Utils::sinc<true>(idx * realRateBegin));
        ++windowCoefficients;
    }
}

inline void Audio::DSP::Filter::DesignFilterBandStop(float *windowCoefficients, const std::size_t size, const double cutoffRateBegin, const double cutoffRateEnd, const bool centered) noexcept
{
    // sinc(x) - cutoffNorm/pi*sinc(cutoffNorm*x/pi)
    const std::size_t first = centered ? (size / 2) : size - 1;
    const double realRateBegin = 2.0 * cutoffRateBegin;
    // const double realRateEnd = 2.0 * cutoffRateEnd;
    for (auto i = 0u; i < size; ++i) {
        int idx = i - first;
        *windowCoefficients *= Utils::sinc<true>(static_cast<float>(idx)) - (realRateBegin * Utils::sinc<true>(idx * realRateBegin));
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