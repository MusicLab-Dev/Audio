/**
 * @file Delay.ipp
 * @brief Delay implementation
 *
 * @author Pierre V
 * @date 2021-05-05
 */

#include <Core/Assert.hpp>

template<typename Type, Audio::DSP::DelayLineDesign Design, unsigned Count>
template<unsigned Index>
inline void Audio::DSP::DelayLineBase<Type, Design, Count>::resetAllImpl(const AudioSpecs &audioSpecs, const float maxDelayDuration, const float delaySize) noexcept
{
    if constexpr (Index < Count) {
        reset<Index>(audioSpecs, maxDelayDuration, delaySize);
        resetAllImpl<Index + 1u>(audioSpecs, maxDelayDuration, delaySize);
    }
}

template<typename Type, Audio::DSP::DelayLineDesign Design, unsigned Count>
template<unsigned Index>
inline void Audio::DSP::DelayLineBase<Type, Design, Count>::reset(const AudioSpecs &audioSpecs, const float maxDelayDuration, const float delaySize) noexcept
{
    const auto inputCacheSize = GetFormatByteLength(audioSpecs.format) * audioSpecs.processBlockSize;
    _cache[Index].input.resize(inputCacheSize, audioSpecs.sampleRate, audioSpecs.channelArrangement, audioSpecs.format);
    _cache[Index].input.clear();

    const auto delayCacheSize = static_cast<std::uint32_t>(maxDelayDuration * static_cast<float>(audioSpecs.sampleRate));
    _cache[Index].cache.resize(delayCacheSize);
    _cache[Index].cache.clear();

    _cache[Index].index = 0u;

    _cache[Index].delayRate = 0.0f;
    _cache[Index].delayTime = static_cast<ProcessIndex>(static_cast<float>(audioSpecs.sampleRate) * delaySize);
}

template<typename Type, Audio::DSP::DelayLineDesign Design, unsigned Count>
template<unsigned Index>
inline void Audio::DSP::DelayLineBase<Type, Design, Count>::sendDataAllImpl(const Type *input, const std::size_t inputSize) noexcept
{
    if constexpr (Index < Count) {
        sendData<Index>(input, inputSize);
        sendDataAllImpl<Index + 1u>(input, inputSize);
    }
}

template<typename Type, Audio::DSP::DelayLineDesign Design, unsigned Count>
template<bool Accumulate, unsigned Index>
inline void Audio::DSP::DelayLineBase<Type, Design, Count>::receiveDataAllImpl(Type *output, const std::size_t outputSize, const float outGain) noexcept
{
    if constexpr (Index < Count) {
        receiveData<Accumulate, Index>(output, outputSize, outGain);
        receiveDataAllImpl<true, Index + 1u>(output, outputSize, outGain);
    }
    if constexpr (Index == Count) {
        for (auto i = 0u; i < outputSize; ++i)
            output[i] /= static_cast<Type>(static_cast<float>(Count));
    }
}

template<typename Type, Audio::DSP::DelayLineDesign Design, unsigned Count>
template<unsigned Index>
inline void Audio::DSP::DelayLineBase<Type, Design, Count>::sendData(const Type *input, const std::size_t inputSize) noexcept
{
    auto &line = _cache[Index];

    for (auto i = 0u; i < inputSize; ++i) {
        const auto in = input[i];
        line.input.template data<Type>()[i] = in;
    }
}

template<typename Type, Audio::DSP::DelayLineDesign Design, unsigned Count>
template<bool Accumulate, unsigned Index>
inline void Audio::DSP::DelayLineBase<Type, Design, Count>::receiveData(Type *output, const std::size_t outputSize, const float outGain) noexcept
{
    auto &line = _cache[Index];

    for (auto i = 0u; i < outputSize; ++i) {
        Type out {};
        const auto in = line.input.template data<Type>()[i];

        if constexpr (Design == DelayLineDesign::Default) {
            out = processDefaultSample<Index>(in);
        } else if constexpr (Design == DelayLineDesign::Feedforward) {
            out = processFeedforwardSample<Index>(in);
        } else if constexpr (Design == DelayLineDesign::FeedforwardNorm) {
            out = processFeedforwardNormSample<Index>(in);
        } else if constexpr (Design == DelayLineDesign::Feedback) {
            out = processFeedbackSample<Index>(in);
        } else if constexpr (Design == DelayLineDesign::FeedbackNorm) {
            out = processFeedbackNormSample<Index>(in);
        } else {
            out = processAllPassSample<Index>(in);
        }

        if constexpr (Accumulate) {
            output[i] += out * outGain;
        } else {
            output[i] = out * outGain;
        }
    }
}

template<typename Type, Audio::DSP::DelayLineDesign Design, unsigned Count>
template<unsigned Index, bool Accumulate>
inline void Audio::DSP::DelayLineBase<Type, Design, Count>::process(const Type *input, Type *output, const std::size_t processSize, const DB outGain)
{
    for (auto i = 0u; i < processSize; ++i) {
        Type out {};
        const auto in = input[i];

        if constexpr (Design == DelayLineDesign::Default) {
            out = processDefaultSample<Index>(in);
        } else if constexpr (Design == DelayLineDesign::Feedforward) {
            out = processFeedforwardSample<Index>(in);
        } else if constexpr (Design == DelayLineDesign::FeedforwardNorm) {
            out = processFeedforwardNormSample<Index>(in);
        } else if constexpr (Design == DelayLineDesign::Feedback) {
            out = processFeedbackSample<Index>(in);
        } else if constexpr (Design == DelayLineDesign::FeedbackNorm) {
            out = processFeedbackNormSample<Index>(in);
        } else {
            out = processAllPassSample<Index>(in);
        }

        if constexpr (Accumulate) {
            output[i] += out * outGain;
        } else {
            output[i] = out * outGain;
        }
    }

}

template<typename Type, Audio::DSP::DelayLineDesign Design, unsigned Count>
typename Audio::DSP::DelayLineBase<Type, Design, Count>::ProcessIndex Audio::DSP::DelayLineBase<Type, Design, Count>::getDelayTime(const float sampleRate, const float time) const noexcept
{
    static constexpr auto MinDelaySize = 2u;

    if (time == 0.0f) {
        return MinDelaySize;
    }
    if (const auto index = sampleRate * time; index < static_cast<float>(MinDelaySize)) {
        return MinDelaySize;
    }
    else {
        return static_cast<ProcessIndex>(index);
    }
}

template<typename Type, Audio::DSP::DelayLineDesign Design, unsigned Count>
template<unsigned Index, bool Clip>
inline void Audio::DSP::DelayLineBase<Type, Design, Count>::setInputAmount(const float amount) noexcept
{
    if constexpr (Clip) {
        if (amount >= 1.0f)
            _cache[Index].feedbackAmount = DelayLineMaxRate;
    }
    _cache[Index].inputRate = amount;
}

template<typename Type, Audio::DSP::DelayLineDesign Design, unsigned Count>
template<unsigned Index, bool Clip>
inline void Audio::DSP::DelayLineBase<Type, Design, Count>::setDelayAmount(const float amount) noexcept
{
    if constexpr (Clip) {
        if (amount >= 1.0f)
            _cache[Index].feedbackAmount = DelayLineMaxRate;
    }
    _cache[Index].delayRate = amount;
}
