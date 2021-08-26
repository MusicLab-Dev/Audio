/**
 * @file Delay.ipp
 * @brief Delay implementation
 *
 * @author Pierre V
 * @date 2021-05-05
 */

#include <Core/Assert.hpp>

template<typename Type, Audio::DSP::InternalPath Path, unsigned Count>
template<unsigned Index>
inline void Audio::DSP::DelayLineBase<Type, Path, Count>::resetAllImpl(const AudioSpecs &audioSpecs, const float maxDelaySize, const float delaySize) noexcept
{
    if constexpr (Index < Count) {
        reset<Index>(audioSpecs, maxDelaySize, delaySize);
        resetAllImpl<Index + 1u>(audioSpecs, maxDelaySize, delaySize);
    }
}

template<typename Type, Audio::DSP::InternalPath Path, unsigned Count>
template<unsigned Index>
inline void Audio::DSP::DelayLineBase<Type, Path, Count>::reset(const AudioSpecs &audioSpecs, const float maxDelaySize, const float delaySize) noexcept
{

    _cache[Index].inIdx = 0u;
    _cache[Index].lastOutFeedback = 0;

    _cache[Index].delayRate = 0.0f;
    _cache[Index].blockSize = audioSpecs.processBlockSize;
    _cache[Index].delayTime = static_cast<ProcessIndex>(static_cast<float>(audioSpecs.sampleRate) * delaySize);
    const std::uint32_t delayCacheSize = static_cast<std::uint32_t>(static_cast<float>(audioSpecs.sampleRate) * maxDelaySize) + static_cast<std::uint32_t>(audioSpecs.processBlockSize);
    _cache[Index].inputCache.clear();
    _cache[Index].inputCache.resize(delayCacheSize);
    _cache[Index].inputReadIndex = 0u;
    _cache[Index].inputWriteIndex = _cache[Index].delayTime;

    _cache[Index].outputCache.clear();
    _cache[Index].outputCache.resize(delayCacheSize);
    _cache[Index].outputReadIndex = 0u;
    _cache[Index].outputWriteIndex = _cache[Index].delayTime;


    const auto cacheSize = GetFormatByteLength(audioSpecs.format) * audioSpecs.processBlockSize;
    _cache[Index].input.resize(cacheSize, audioSpecs.sampleRate, audioSpecs.channelArrangement, audioSpecs.format);
    _cache[Index].input.clear();

    _cache[Index].lastOut.resize(cacheSize, audioSpecs.sampleRate, audioSpecs.channelArrangement, audioSpecs.format);
    _cache[Index].lastOut.clear();
}

template<typename Type, Audio::DSP::InternalPath Path, unsigned Count>
template<unsigned Index>
inline void Audio::DSP::DelayLineBase<Type, Path, Count>::sendDataAllImpl(const Type *input, const std::size_t inputSize) noexcept
{
    if constexpr (Index < Count) {
        sendData<Index>(input, inputSize);
        sendDataAllImpl<Index + 1u>(input, inputSize);
    }
}

template<typename Type, Audio::DSP::InternalPath Path, unsigned Count>
template<bool Accumulate, unsigned Index>
inline void Audio::DSP::DelayLineBase<Type, Path, Count>::receiveDataAllImpl(Type *output, const std::size_t outputSize, const float outGain) noexcept
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

template<typename Type, Audio::DSP::InternalPath Path, unsigned Count>
template<unsigned Index>
inline void Audio::DSP::DelayLineBase<Type, Path, Count>::sendData(const Type *input, const std::size_t inputSize) noexcept
{
    auto &line = _cache[Index];

    for (auto i = 0u; i < inputSize; ++i) {
        const auto in = input[i];
        line.input.template data<Type>()[i] = in;
        if constexpr (Path == InternalPath::Default) {
            // _cache[Index].inputCache.data()[_cache[Index].inputWriteIndex] = in + _cache[Index].delayRate * _cache[Index].lastOut.template data<Type>()[i];
            // incrementInputWriteIndex<Index>();
        } else if constexpr (Path == InternalPath::Feedback) {
        } else {
            // _cache[Index].inputCache.data()[_cache[Index].inputWriteIndex] = in + 1 * _cache[Index].lastOut.template data<Type>()[i];
            // incrementInputWriteIndex<Index>();
        }
    }
}

template<typename Type, Audio::DSP::InternalPath Path, unsigned Count>
template<bool Accumulate, unsigned Index>
inline void Audio::DSP::DelayLineBase<Type, Path, Count>::receiveData(Type *output, const std::size_t outputSize, const float outGain) noexcept
{
    auto &line = _cache[Index];

    for (auto i = 0u; i < outputSize; ++i) {
        Type out {};

        if constexpr (Path == InternalPath::Default) {
            // const auto delay = line.inputCache.data()[line.inputReadIndex];

            // out = delay;
            // line.lastOut.template data<Type>()[i] = delay;
            // incrementInputReadIndex<Index>();

            out = processFeedforwardSample<Index>(line.input.template data<Type>()[i]);

        } else if constexpr (Path == InternalPath::Feedback) {
            // const auto lastOut = line.outputCache.data()[line.outputReadIndex];
            // const auto in = line.input.template data<Type>()[i];

            // const auto ret = in + lastOut * line.delayRate;
            // line.outputCache.data()[line.outputWriteIndex] = ret;
            // line.lastOut.template data<Type>()[i] = ret;
            // out = ret - in;
            // incrementOutputReadIndex<Index>();
            // incrementOutputWriteIndex<Index>();

            out = processFeedbackSample<Index>(line.input.template data<Type>()[i]);

        } else {
            // const auto delay = line.inputCache.data()[line.inputReadIndex];
            // const auto lastOut = line.outputCache.data()[line.outputReadIndex];
            // const auto in = line.input.template data<Type>()[i];

            // const auto retDelay = -in * line.delayRate + delay * line.inputRate + lastOut * line.delayRate;
            // // const auto ret = delay * line.inputRate + lastOut * line.delayRate;
            // const auto ret = retDelay;
            // line.outputCache.data()[line.outputWriteIndex] = ret;
            // line.lastOut.template data<Type>()[i] = ret;
            // out = ret + in * line.delayRate;
            // incrementInputReadIndex<Index>();
            // incrementOutputReadIndex<Index>();
            // incrementOutputWriteIndex<Index>();

            out = processBothSample<Index>(line.input.template data<Type>()[i]);
        }

        if constexpr (Accumulate) {
            output[i] += out * outGain;
        } else {
            output[i] = out * outGain;
        }
    }
}

template<typename Type, Audio::DSP::InternalPath Path, unsigned Count>
template<unsigned Index>
typename Audio::DSP::DelayLineBase<Type, Path, Count>::ProcessIndex Audio::DSP::DelayLineBase<Type, Path, Count>::getNextIndex(const ProcessIndex index) const noexcept
{
    if (const auto idx = index + 1u; idx >= _cache[Index].delayTime + static_cast<ProcessIndex>(_cache[Index].blockSize))
        return 0u;
    else
        return idx;
}


template<typename Type, Audio::DSP::InternalPath Path, unsigned Count>
typename Audio::DSP::DelayLineBase<Type, Path, Count>::ProcessIndex Audio::DSP::DelayLineBase<Type, Path, Count>::getDelayTime(const float sampleRate, const float time) const noexcept
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

template<typename Type, Audio::DSP::InternalPath Path, unsigned Count>
template<unsigned Index>
void Audio::DSP::DelayLineBase<Type, Path, Count>::setInternalRate(const float delay, const float input) noexcept
{
    _cache[Index].delayRate = delay;
    _cache[Index].inputRate = input;
}

template<typename Type, Audio::DSP::InternalPath Path, unsigned Count>
template<unsigned Index>
void Audio::DSP::DelayLineBase<Type, Path, Count>::setInternalRateDefault(void) noexcept
{
    if constexpr (Path == InternalPath::Default) {
        _cache[Index].delayRate = 1.0f;
        _cache[Index].inputRate = 0.0f;
    } else if constexpr (Path == InternalPath::Feedback) {
        _cache[Index].delayRate = 1.0f;
        _cache[Index].inputRate = 0.0f;
    } else {
        _cache[Index].delayRate = 1.0f;
        _cache[Index].inputRate = 0.0f;
    }
}

template<typename Type, Audio::DSP::InternalPath Path, unsigned Count>
template<unsigned Index>
void Audio::DSP::DelayLineBase<Type, Path, Count>::setFeedbackAmount(const float amount) noexcept
{
    // if (amount >= 0.9f)
    //     _cache[Index].feedbackAmount = 0.9f;
    // else
        _cache[Index].feedbackAmount = amount;
    _cache[Index].inputRate = -_cache[Index].feedbackAmount;
}
