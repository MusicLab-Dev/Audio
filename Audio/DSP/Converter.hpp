/**
 * @ Author: Pierre Veysseyre
 * @ Description: Convert algorithm
 */

#pragma once

#include <Audio/Base.hpp>

namespace Audio::DSP
{
    template<typename InType, typename OutType>
    class Converter;
}

template<typename InType, typename OutType>
class Audio::DSP::Converter
{
public:
    // Convert sample rate
    void resample(const InType *input, OutType *output, const std::size_t inputSize, const SampleRate inSampleRate, const SampleRate outSampleRate) noexcept;

    // Convert bit depth (sample precision)
    void reformat(const InType *input, OutType *output, const std::size_t inputSize) noexcept;

    // Convert both sample rate and format
    void resampleReformat(const InType *input, OutType *output, const std::size_t inputSize, const SampleRate inSampleRate, const SampleRate outSampleRate) noexcept;

private:
    Resampler<InType> _resamplerIn;
    Resampler<OutType> _resamplerOut;
    Core::TinyVector<OutType> _reformatCache;
};

#include "Converter.ipp"
