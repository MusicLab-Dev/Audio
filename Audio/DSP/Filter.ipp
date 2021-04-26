/**
 * @ Author: Pierre Veysseyre
 * @ Description: Window.ipp
 */

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>


// template<Audio::DSP::Filter::BasicType Filter, bool ProcessWindow>
// inline void Audio::DSP::Filter::GenerateFilter(const FIRSpec specs, float *window) noexcept
// {

// }
// // compile-time filter & window
// template<Audio::DSP::Filter::BasicType Filter, bool ProcessWindow>
// inline void Audio::DSP::Filter::GenerateFilter(const FIRSpec specs, float *window) noexcept
// {
//     return GenerateFilter<Window, ProcessWindow>(FIRSpec {
//         Filter,
//         specs.windowType,
//         Window,
//         specs.sampleRate,
//         { specs.cutoffs[0], specs.cutoffs[1] },
//         specs.gain
//     }, window, true);
// }

// compile-time filter type
template<Audio::DSP::Filter::BasicType Filter>
inline void Audio::DSP::Filter::GenerateFilter(const FIRSpec specs, float *window, const bool symetric) noexcept
{
    switch (Filter) {
    case BasicType::LowPass:
        return GenerateFilterLowPass<true>(specs, window, symetric);
    case BasicType::HighPass:
        return GenerateFilterHighPass<true>(specs, window, symetric);
    case BasicType::BandPass:
        return GenerateFilterBandPass<true>(specs, window, symetric);
    case BasicType::BandStop:
        return GenerateFilterBandStop<true>(specs, window, symetric);
    default:
        return GenerateFilterLowPass<true>(specs, window, symetric);
    }
}

// run-time filter type
inline void Audio::DSP::Filter::GenerateFilter(const FIRSpec specs, float *window, const bool symetric) noexcept
{
    switch (specs.filterType) {
    case BasicType::LowPass:
        return GenerateFilter<BasicType::LowPass>(specs, window, symetric);
    case BasicType::HighPass:
        return GenerateFilter<BasicType::HighPass>(specs, window, symetric);
    case BasicType::BandPass:
        return GenerateFilter<BasicType::BandPass>(specs, window, symetric);
    case BasicType::BandStop:
        return GenerateFilter<BasicType::BandStop>(specs, window, symetric);
    default:
        return GenerateFilter<BasicType::LowPass>(specs, window, symetric);
    }
}

template<bool ProcessWindow>
inline void Audio::DSP::Filter::GenerateFilterLowPass(const FIRSpec specs, float *window, const bool symetric) noexcept
{
    // cutoffNorm/pi*sinc(x*cutoffNorm/pi)
    const std::size_t size = specs.size;
    const float cutoffRateBegin = 2.0f * specs.cutoffs[0] / specs.sampleRate;
    const std::size_t first = symetric ? (size / 2) : size - 1;

    for (auto i = 0ul; i < size; ++i) {
        float idx = static_cast<float>(i - first);
        if constexpr (ProcessWindow)
            *window = ComputeWindow(specs.windowType, i, size);
        *window++ *= (cutoffRateBegin * Utils::sinc<true>(idx * cutoffRateBegin)) * specs.gain;
    }
}

template<bool ProcessWindow>
inline void Audio::DSP::Filter::GenerateFilterHighPass(const FIRSpec specs, float *window, const bool symetric) noexcept
{
    // sinc(x) - cutoffNorm/pi*sinc(x*cutoffNorm/pi)
    const std::size_t size = specs.size;
    const float cutoffRateBegin = 2.0f * specs.cutoffs[0] / specs.sampleRate;
    const std::size_t first = symetric ? (size / 2) : size - 1;

    for (auto i = 0ul; i < size; ++i) {
        float idx = static_cast<float>(i - first);
        if constexpr (ProcessWindow)
            *window = ComputeWindow(specs.windowType, i, size);
        *window++ *= Utils::sinc<true>(idx) - (cutoffRateBegin * Utils::sinc<true>(idx * cutoffRateBegin)) * specs.gain;
    }
}

template<bool ProcessWindow>
inline void Audio::DSP::Filter::GenerateFilterBandPass(const FIRSpec specs, float *window, const bool symetric) noexcept
{
    // cutoffNormHigh/pi*sinc(x*cutoffNormHigh/pi) - cutoffNormLow/pi*sinc(x*cutoffNormLow/pi)
    const std::size_t size = specs.size;
    const float cutoffRateBegin = 2.0f * specs.cutoffs[0] / specs.sampleRate;
    const float cutoffRateEnd = 2.0f * specs.cutoffs[1] / specs.sampleRate;
    const std::size_t first = symetric ? (size / 2) : size - 1;

    for (auto i = 0ul; i < size; ++i) {
        float idx = static_cast<float>(i - first);
        if constexpr (ProcessWindow)
            *window = ComputeWindow(specs.windowType, i, size);
        *window++ *= (cutoffRateEnd * Utils::sinc<true>(idx * cutoffRateEnd)) - (cutoffRateBegin * Utils::sinc<true>(idx * cutoffRateBegin)) * specs.gain;
    }
}

template<bool ProcessWindow>
inline void Audio::DSP::Filter::GenerateFilterBandStop(const FIRSpec specs, float *window, const bool symetric) noexcept
{
    UNUSED(window);
    UNUSED(specs);
    UNUSED(symetric);

    // Not sure at all !
    // lowPass(cutoffLow) + highPass(cutoffHigh)
    // const std::size_t first = symetric ? (size / 2) : size - 1;
    // const float cutoffRateBegin = 2.0 * cutoffRateBegin;
    // const float cutoffRateEnd = 2.0 * cutoffRateEnd;
    // for (auto i = 0u; i < size; ++i) {
    //     int idx = i - first;
        // if constexpr (ProcessWindow)
        //     *window =  ComputeWindow(specs.windowType, i, size);//
        // *window++ *= (cutoffRateBegin * Utils::sinc<true>(idx * cutoffRateBegin)) - (cutoffRateBegin * Utils::sinc<true>(idx * cutoffRateBegin)) * specs.gain;
    // }
}
