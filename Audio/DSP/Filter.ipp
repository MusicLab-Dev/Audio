/**
 * @ Author: Pierre Veysseyre
 * @ Description: Window.ipp
 */

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

// compile-time filter type
// template<bool Accumulate, Audio::DSP::Filter::BasicType Filter>
// inline void Audio::DSP::Filter::GenerateFilter(const FIRSpec specs, float *window, const bool symetric) noexcept
// {
//     switch (Filter) {
//     case BasicType::LowPass:
//         return GenerateFilterLowPass<true, Accumulate>(specs, window, symetric);
//     case BasicType::HighPass:
//         return GenerateFilterHighPass<true, Accumulate>(specs, window, symetric);
//     case BasicType::BandPass:
//         return GenerateFilterBandPass<true, Accumulate>(specs, window, symetric);
//     case BasicType::BandStop:
//         return GenerateFilterBandStop<true, Accumulate>(specs, window, symetric);
//     default:
//         return GenerateFilterLowPass<true, Accumulate>(specs, window, symetric);
//     }
// }

// run-time filter type
template<bool ProcessWindow, bool Accumulate>
inline void Audio::DSP::Filter::GenerateFilter(const FIRSpec specs, float *window, const bool symetric) noexcept
{
    switch (specs.filterType) {
    case BasicType::LowPass:
        return GenerateFilterLowPass<ProcessWindow, Accumulate>(specs, window, symetric);
    case BasicType::HighPass:
        return GenerateFilterHighPass<ProcessWindow, Accumulate>(specs, window, symetric);
    case BasicType::BandPass:
        return GenerateFilterBandPass<ProcessWindow, Accumulate>(specs, window, symetric);
    case BasicType::BandStop:
        return GenerateFilterBandStop<ProcessWindow, Accumulate>(specs, window, symetric);
    default:
        return GenerateFilterLowPass<ProcessWindow, Accumulate>(specs, window, symetric);
    }
}

template<bool ProcessWindow, bool Accumulate>
inline void Audio::DSP::Filter::GenerateFilterLowPass(const FIRSpec specs, float *window, const bool symetric) noexcept
{
    // cutoffNorm/pi*sinc(x*cutoffNorm/pi)
    const std::size_t size = specs.size;
    const float cutoffRateBegin = 2.0f * specs.cutoffs[0] / specs.sampleRate;
    const std::size_t first = symetric ? (size / 2) : size - 1;

    for (auto i = 0ul; i < size; ++i) {
        float idx = static_cast<float>(i - first);
        float coef = *window;
        if constexpr (ProcessWindow)
            coef = ComputeWindow<false>(specs.windowType, i, size);
        if constexpr (Accumulate)
            *window++ += coef * (cutoffRateBegin * Utils::sinc<true>(idx * cutoffRateBegin)) * specs.gain;
        else
            *window++ = coef * (cutoffRateBegin * Utils::sinc<true>(idx * cutoffRateBegin)) * specs.gain;
    }
}

template<bool ProcessWindow, bool Accumulate>
inline void Audio::DSP::Filter::GenerateFilterHighPass(const FIRSpec specs, float *window, const bool symetric) noexcept
{
    // sinc(x) - cutoffNorm/pi*sinc(x*cutoffNorm/pi)
    const std::size_t size = specs.size;
    const float cutoffRateBegin = 2.0f * specs.cutoffs[0] / specs.sampleRate;
    const std::size_t first = symetric ? (size / 2) : size - 1;

    for (auto i = 0ul; i < size; ++i) {
        float idx = static_cast<float>(i - first);
        float coef = *window;
        if constexpr (ProcessWindow)
            coef = ComputeWindow<false>(specs.windowType, i, size);
        if constexpr (Accumulate)
            *window++ += coef * Utils::sinc<true>(idx) - (cutoffRateBegin * Utils::sinc<true>(idx * cutoffRateBegin)) * specs.gain;
        else
            *window++ = coef * Utils::sinc<true>(idx) - (cutoffRateBegin * Utils::sinc<true>(idx * cutoffRateBegin)) * specs.gain;
    }
}

template<bool ProcessWindow, bool Accumulate>
inline void Audio::DSP::Filter::GenerateFilterBandPass(const FIRSpec specs, float *window, const bool symetric) noexcept
{
    // cutoffNormHigh/pi*sinc(x*cutoffNormHigh/pi) - cutoffNormLow/pi*sinc(x*cutoffNormLow/pi)
    const std::size_t size = specs.size;
    const float cutoffRateBegin = 2.0f * specs.cutoffs[0] / specs.sampleRate;
    const float cutoffRateEnd = 2.0f * specs.cutoffs[1] / specs.sampleRate;
    const std::size_t first = symetric ? (size / 2) : size - 1;

    for (auto i = 0ul; i < size; ++i) {
        float idx = static_cast<float>(i - first);
        float coef = *window;
        if constexpr (ProcessWindow)
            coef = ComputeWindow<false>(specs.windowType, i, size);
        if constexpr (Accumulate)
            *window++ += coef * (cutoffRateEnd * Utils::sinc<true>(idx * cutoffRateEnd)) - (cutoffRateBegin * Utils::sinc<true>(idx * cutoffRateBegin)) * specs.gain;
        else
            *window++ = coef * (cutoffRateEnd * Utils::sinc<true>(idx * cutoffRateEnd)) - (cutoffRateBegin * Utils::sinc<true>(idx * cutoffRateBegin)) * specs.gain;
    }
}

template<bool ProcessWindow, bool Accumulate>
inline void Audio::DSP::Filter::GenerateFilterBandStop(const FIRSpec specs, float *window, const bool symetric) noexcept
{
    UNUSED(window);
    UNUSED(specs);
    UNUSED(symetric);

    // Not sure at all !
    // lowPass(cutoffLow) + highPass(cutoffHigh)
}
