/**
 * @ Author: Pierre Veysseyre
 * @ Description: Window.ipp
 */

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

// compile-time filter type
// template<bool Accumulate, Audio::DSP::Filter::BasicType Filter>
// inline void Audio::DSP::Filter::GenerateFilter(const FIRSpec specs, float *window) noexcept
// {
//     switch (Filter) {
//     case BasicType::LowPass:
//         return GenerateFilterLowPass<true, Accumulate>(specs, window);
//     case BasicType::HighPass:
//         return GenerateFilterHighPass<true, Accumulate>(specs, window);
//     case BasicType::BandPass:
//         return GenerateFilterBandPass<true, Accumulate>(specs, window);
//     case BasicType::BandStop:
//         return GenerateFilterBandStop<true, Accumulate>(specs, window);
//     default:
//         return GenerateFilterLowPass<true, Accumulate>(specs, window);
//     }
// }

// run-time filter type
template<bool ProcessWindow, bool Accumulate>
inline void Audio::DSP::Filter::GenerateFilter(const FIRSpec specs, float *window) noexcept
{
    switch (specs.filterType) {
    case BasicType::LowPass:
        return GenerateFilterLowPass<ProcessWindow, Accumulate>(specs, window);
    case BasicType::HighPass:
        return GenerateFilterHighPass<ProcessWindow, Accumulate>(specs, window);
    case BasicType::BandPass:
        return GenerateFilterBandPass<ProcessWindow, Accumulate>(specs, window);
    case BasicType::BandStop:
        return GenerateFilterBandStop<ProcessWindow, Accumulate>(specs, window);
    default:
        return GenerateFilterLowPass<ProcessWindow, Accumulate>(specs, window);
    }
}

#include <iostream>

template<bool ProcessWindow, bool Accumulate>
inline void Audio::DSP::Filter::GenerateFilterLowPass(const FIRSpec specs, float *window) noexcept
{
    // cutoffNorm/pi*sinc(x*cutoffNorm/pi)
    // ONLY use type1 (symetric & odd size) || type2 (symetric & even size)
    // const std::size_t size = (specs.size & 1u) ? specs.size : specs.size - 1;
    const std::size_t size = specs.size;
    const float cutoffRateBegin = 2.0f * specs.cutoffs[0] / specs.sampleRate;
    const std::size_t first = size / 2;

    // std::cout << "Low-pass gain: " << specs.gain << std::endl;

    for (auto i = 0ul; i < size; ++i) {
        float idx = static_cast<float>(static_cast<int>(i) - static_cast<int>(first));
        float coef = *window;
        if constexpr (ProcessWindow)
            coef = ComputeWindow<false>(specs.windowType, i, size, true);
        if constexpr (Accumulate)
            *window++ += coef * (cutoffRateBegin * Utils::sinc<true>(idx * cutoffRateBegin)) * specs.gain;
        else
            *window++ = coef * (cutoffRateBegin * Utils::sinc<true>(idx * cutoffRateBegin)) * specs.gain;
    }
}

template<bool ProcessWindow, bool Accumulate>
inline void Audio::DSP::Filter::GenerateFilterHighPass(const FIRSpec specs, float *window) noexcept
{
    // sinc<pi>(x) - cutoffNorm/pi*sinc(x*cutoffNorm/pi)
    // ONLY use type4 (anti-symetric & even size)
    // const std::size_t size = (specs.size & 1u) ? specs.size - 1 : specs.size;
    const std::size_t size = specs.size;
    const float cutoffRateBegin = 2.0f * specs.cutoffs[0] / specs.sampleRate;
    const std::size_t first = size / 2;

    std::cout << "High-pass gain: " << specs.gain << std::endl;

    for (auto i = 0ul; i < size; ++i) {
        float idx = static_cast<float>(static_cast<int>(i) - static_cast<int>(first));
        float coef = *window;
        if constexpr (ProcessWindow)
            coef = ComputeWindow<false>(specs.windowType, i, size, true);
        if constexpr (Accumulate)
            *window++ += coef * (Utils::sinc<true>(idx) - (cutoffRateBegin * Utils::sinc<true>(idx * cutoffRateBegin))) * specs.gain;
        else
            *window++ = coef * (Utils::sinc<true>(idx) - (cutoffRateBegin * Utils::sinc<true>(idx * cutoffRateBegin))) * specs.gain;
    }
}

template<bool ProcessWindow, bool Accumulate>
inline void Audio::DSP::Filter::GenerateFilterBandPass(const FIRSpec specs, float *window) noexcept
{
    // cutoffNormHigh/pi*sinc(x*cutoffNormHigh/pi) - cutoffNormLow/pi*sinc(x*cutoffNormLow/pi)
    // ONLY use type3 (anti-symetric & odd size) | type4 (anti-symetric & even size)
    // type4 actually
    // const std::size_t size = (specs.size & 1u) ? specs.size - 1 : specs.size;
    const std::size_t size = specs.size;
    const float cutoffRateBegin = 2.0f * specs.cutoffs[0] / specs.sampleRate;
    const float cutoffRateEnd = 2.0f * specs.cutoffs[1] / specs.sampleRate;
    const std::size_t first = size / 2;

    for (auto i = 0ul; i < size; ++i) {
        float idx = static_cast<float>(static_cast<int>(i) - static_cast<int>(first));
        float coef = *window;
        if constexpr (ProcessWindow)
            coef = ComputeWindow<false>(specs.windowType, i, size, true);
        if constexpr (Accumulate)
            *window++ += coef * ((cutoffRateEnd * Utils::sinc<true>(idx * cutoffRateEnd)) - (cutoffRateBegin * Utils::sinc<true>(idx * cutoffRateBegin)) * specs.gain);
        else
            *window++ = coef * ((cutoffRateEnd * Utils::sinc<true>(idx * cutoffRateEnd)) - (cutoffRateBegin * Utils::sinc<true>(idx * cutoffRateBegin)) * specs.gain);
    }
}

template<bool ProcessWindow, bool Accumulate>
inline void Audio::DSP::Filter::GenerateFilterBandStop(const FIRSpec specs, float *window) noexcept
{
    // sinc<pi>(x) - (cutoffNormHigh/pi*sinc(x*cutoffNormHigh/pi) - cutoffNormLow/pi*sinc(x*cutoffNormLow/pi))
    const std::size_t size = specs.size;
    const float cutoffRateBegin = 2.0f * specs.cutoffs[0] / specs.sampleRate;
    const float cutoffRateEnd = 2.0f * specs.cutoffs[1] / specs.sampleRate;
    const std::size_t first = size / 2;

    for (auto i = 0ul; i < size; ++i) {
        float idx = static_cast<float>(static_cast<int>(i) - static_cast<int>(first));
        float coef = *window;
        if constexpr (ProcessWindow)
            coef = ComputeWindow<false>(specs.windowType, i, size, true);
        if constexpr (Accumulate)
            *window++ += coef * (Utils::sinc<true>(idx) - ((cutoffRateEnd * Utils::sinc<true>(idx * cutoffRateEnd)) - (cutoffRateBegin * Utils::sinc<true>(idx * cutoffRateBegin)) * specs.gain));
        else
            *window++ = coef * (Utils::sinc<true>(idx) - ((cutoffRateEnd * Utils::sinc<true>(idx * cutoffRateEnd)) - (cutoffRateBegin * Utils::sinc<true>(idx * cutoffRateBegin)) * specs.gain));
    }
}
