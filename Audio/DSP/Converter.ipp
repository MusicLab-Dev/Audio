/**
 * @ Author: Pierre Veysseyre
 * @ Description: Converter implementation
 */

#include "Resampler.hpp"
#include "Reformater.hpp"

template<typename InType, typename OutType>
inline void Audio::DSP::Converter<InType, OutType>::resample(const InType *input, OutType *output, const std::size_t inputSize, const SampleRate inSampleRate, const SampleRate outSampleRate) noexcept
{
    _resamplerIn.resampleSampleRate(input, output, inputSize, inSampleRate, outSampleRate);
}

template<typename InType, typename OutType>
inline void Audio::DSP::Converter<InType, OutType>::reformat(const InType *input, OutType *output, const std::size_t inputSize) noexcept
{

}

template<typename InType, typename OutType>
inline void Audio::DSP::Converter<InType, OutType>::resampleReformat(const InType *input, OutType *output, const std::size_t inputSize, const SampleRate inSampleRate, const SampleRate outSampleRate) noexcept
{
    _reformatCache.resize(inputSize);
    reformat(input, _reformatCache.data(), inputSize);
    _resamplerOut.resampleSampleRate(_reformatCache.data(), output, inputSize, inSampleRate, outSampleRate);
}
