/**
 * @ Author: Pierre Veysseyre
 * @ Description: Window.ipp
 */

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

inline Audio::DSP::Filter::FIRSpecs::FIRSpecs(const BasicType filterType_, const WindowType windowType_, const std::uint32_t desiredOrder,
        const float sampleRate_, const float cutoffBegin_, const float cutoffEnd_, const float gain_) noexcept
    : filterType(filterType_), windowType(windowType_), order(desiredOrder + (desiredOrder & 1u)), filterSize(order + 1), sampleRate(sampleRate_),
      cutoffBegin(cutoffBegin_), cutoffEnd(cutoffEnd_), gain(gain_)
{
}

// run-time filter type
template<bool ProcessWindow, bool Accumulate>
inline void Audio::DSP::Filter::GenerateFilter(const FIRSpecs specs, float *window) noexcept
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

template<bool ProcessWindow, bool Accumulate>
inline void Audio::DSP::Filter::GenerateFilterLowPass(const FIRSpecs specs, float *window) noexcept
{
    // cutoffNorm/pi*sinc(x*cutoffNorm/pi)
    // ONLY use type1 (symetric & odd size) || type2 (symetric & even size)

    const std::size_t size = specs.filterSize;
    const float cutoffRateBegin = 2.0f * specs.cutoffBegin / specs.sampleRate;
    const std::size_t first = size / 2ul;

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
inline void Audio::DSP::Filter::GenerateFilterHighPass(const FIRSpecs specs, float *window) noexcept
{
    // sinc<pi>(x) - cutoffNorm/pi*sinc(x*cutoffNorm/pi)
    // ONLY use type4 (anti-symetric & even size)

    const std::size_t size = specs.filterSize;
    const float cutoffRateBegin = 2.0f * specs.cutoffBegin / specs.sampleRate;
    const std::size_t first = size / 2ul;

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
inline void Audio::DSP::Filter::GenerateFilterBandPass(const FIRSpecs specs, float *window) noexcept
{
    // cutoffNormHigh/pi*sinc(x*cutoffNormHigh/pi) - cutoffNormLow/pi*sinc(x*cutoffNormLow/pi)
    // ONLY use type3 (anti-symetric & odd size) | type4 (anti-symetric & even size)
    // type4 actually

    const std::size_t size = specs.filterSize;
    const float cutoffRateBegin = 2.0f * specs.cutoffBegin / specs.sampleRate;
    const float cutoffRateEnd = 2.0f * specs.cutoffEnd / specs.sampleRate;
    const std::size_t first = size / 2ul;

    for (auto i = 0ul; i < size; ++i) {
        float idx = static_cast<float>(static_cast<int>(i) - static_cast<int>(first));
        float coef = *window;
        if constexpr (ProcessWindow)
            coef = ComputeWindow<false>(specs.windowType, i, size, true);
        if constexpr (Accumulate)
            *window++ += coef * ((cutoffRateEnd * Utils::sinc<true>(idx * cutoffRateEnd)) - (cutoffRateBegin * Utils::sinc<true>(idx * cutoffRateBegin))) * specs.gain;
        else
            *window++ = coef * ((cutoffRateEnd * Utils::sinc<true>(idx * cutoffRateEnd)) - (cutoffRateBegin * Utils::sinc<true>(idx * cutoffRateBegin))) * specs.gain;
    }
}

template<bool ProcessWindow, bool Accumulate>
inline void Audio::DSP::Filter::GenerateFilterBandStop(const FIRSpecs specs, float *window) noexcept
{
    // sinc<pi>(x) - (cutoffNormHigh/pi*sinc(x*cutoffNormHigh/pi) - cutoffNormLow/pi*sinc(x*cutoffNormLow/pi))

    const std::size_t size = specs.filterSize;
    const float cutoffRateBegin = 2.0f * specs.cutoffBegin / specs.sampleRate;
    const float cutoffRateEnd = 2.0f * specs.cutoffEnd / specs.sampleRate;
    const std::size_t first = size / 2ul;

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
