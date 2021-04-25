/**
 * @ Author: Pierre Veysseyre
 * @ Description: Window.ipp
 */

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

inline void Audio::DSP::Filter::GenerateFilter(const FIRSpec specs, float *window, const bool centered) noexcept
{
    GenerateWindow(specs.windowType, specs.size, window, centered);
    DesignFilter(specs, window, specs.size, centered);
}

inline void Audio::DSP::Filter::DesignFilter(const FIRSpec specs, float *window, const std::size_t windowSize, const bool centered) noexcept
{
    const double cutoffRateBegin = specs.cutoffs[0] / specs.sampleRate;
    const double cutoffRateEnd = specs.cutoffs[1] / specs.sampleRate;

    switch (specs.filterType) {
    case BasicType::LowPass:
        return DesignFilterLowPass(window, windowSize, cutoffRateBegin, specs.gain, centered);
    case BasicType::HighPass:
        return DesignFilterHighPass(window, windowSize, cutoffRateBegin, specs.gain, centered);
    case BasicType::BandPass:
        return DesignFilterBandPass(window, windowSize, cutoffRateBegin, cutoffRateEnd, specs.gain, centered);
    case BasicType::BandStop:
        return DesignFilterBandStop(window, windowSize, cutoffRateBegin, cutoffRateEnd, specs.gain, centered);
    default:
        return DesignFilterLowPass(window, windowSize, cutoffRateBegin, specs.gain, centered);
    }
}

inline void Audio::DSP::Filter::DesignFilterLowPass(float *window, const std::size_t size, const double cutoffRate, const double gain, const bool centered) noexcept
{
    // cutoffNorm/pi*sinc(x*cutoffNorm/pi)
    const std::size_t first = centered ? (size / 2) : size - 1;
    // Normalize to [0:1]
    const double realRate = 2.0 * cutoffRate;
    for (auto i = 0u; i < size; ++i) {
        int idx = i - first;
        *window++ *= (realRate * Utils::sinc<true>(idx * realRate)) * gain;
    }
}

inline void Audio::DSP::Filter::DesignFilterHighPass(float *window, const std::size_t size, const double cutoffRate, const double gain, const bool centered) noexcept
{
    // sinc(x) - cutoffNorm/pi*sinc(x*cutoffNorm/pi)
    const std::size_t first = centered ? (size / 2) : size - 1;
    // Normalize to [0:1]
    const double realRate = 2.0 * cutoffRate;
    for (auto i = 0u; i < size; ++i) {
        int idx = i - first;
        *window++ *= Utils::sinc<true>(static_cast<float>(idx)) - (realRate * Utils::sinc<true>(idx * realRate)) * gain;
    }
}

inline void Audio::DSP::Filter::DesignFilterBandPass(float *window, const std::size_t size, const double cutoffRateBegin, const double cutoffRateEnd, const double gain, const bool centered) noexcept
{
    // cutoffNormHigh/pi*sinc(x*cutoffNormHigh/pi) - cutoffNormLow/pi*sinc(x*cutoffNormLow/pi)
    const std::size_t first = centered ? (size / 2) : size - 1;
    // Normalize to [0:1]
    const double realRateBegin = 2.0 * cutoffRateBegin;
    const double realRateEnd = 2.0 * cutoffRateEnd;
    for (auto i = 0u; i < size; ++i) {
        int idx = i - first;
        *window++ *= (realRateEnd * Utils::sinc<true>(idx * realRateEnd)) - (realRateBegin * Utils::sinc<true>(idx * realRateBegin)) * gain;
    }
}

inline void Audio::DSP::Filter::DesignFilterBandStop(float *window, const std::size_t size, const double cutoffRateBegin, const double cutoffRateEnd, const double gain, const bool centered) noexcept
{
    // Not sure at all !
    // lowPass(cutoffLow) + highPass(cutoffHigh)
    // const std::size_t first = centered ? (size / 2) : size - 1;
    // const double realRateBegin = 2.0 * cutoffRateBegin;
    // const double realRateEnd = 2.0 * cutoffRateEnd;
    // for (auto i = 0u; i < size; ++i) {
    //     int idx = i - first;
    //     *window++ *= (realRateBegin * Utils::sinc<true>(idx * realRateBegin)) - (realRateBegin * Utils::sinc<true>(idx * realRateBegin)) * gain;
    // }
}
