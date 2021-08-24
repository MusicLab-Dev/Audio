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
    _cache[Index].feedbackRate = 0.0f;
    _cache[Index].blockSize = audioSpecs.processBlockSize;
    const std::uint32_t delayCacheSize = static_cast<std::uint32_t>(static_cast<float>(audioSpecs.sampleRate) * maxDelaySize) + static_cast<std::uint32_t>(audioSpecs.processBlockSize);
    _cache[Index].delayCache.clear();
    _cache[Index].delayCache.resize(delayCacheSize);
    _cache[Index].delayTime = static_cast<ProcessIndex>(static_cast<float>(audioSpecs.sampleRate) * delaySize);
    _cache[Index].readIndex = 0u;
    _cache[Index].writeIndex = _cache[Index].delayTime;

    const auto cacheSize = GetFormatByteLength(audioSpecs.format) * audioSpecs.processBlockSize;
    _cache[Index].lastIn.resize(cacheSize, audioSpecs.sampleRate, audioSpecs.channelArrangement, audioSpecs.format);
    _cache[Index].lastIn.clear();
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
inline void Audio::DSP::DelayLineBase<Type, Path, Count>::receiveDataAllImpl(Type *output, const std::size_t outputSize, const float mixRate) noexcept
{
    if constexpr (Index < Count) {
        receiveData<Accumulate, Index>(output, outputSize, mixRate);
        receiveDataAllImpl<true, Index + 1u>(output, outputSize, mixRate);
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
        const auto lastIn = input[i];
        line.lastIn.template data<Type>()[i] = lastIn;
        if constexpr (Path == InternalPath::Default) {
            _cache[Index].delayCache.data()[_cache[Index].writeIndex] = lastIn + _cache[Index].feedbackRate * _cache[Index].lastOut.template data<Type>()[i];
            incrementWriteIndex<Index>();
        } else if constexpr (Path == InternalPath::Feedback) {
            // _cache[Index].delayCache.data()[_cache[Index].writeIndex] = lastIn + _cache[Index].feedbackRate * _cache[Index].lastOut.data<Type>()[i];
            // incrementWriteIndex();
        }
    }
}

template<typename Type, Audio::DSP::InternalPath Path, unsigned Count>
template<bool Accumulate, unsigned Index>
inline void Audio::DSP::DelayLineBase<Type, Path, Count>::receiveData(Type *output, const std::size_t outputSize, const float mixRate) noexcept
{
    float dry { 1.0f };
    float wet { 1.0f };
    if (mixRate != 0.5f) {
        if (mixRate >= 0.5f) {
            wet = (1.0f - mixRate) * 2.0f;
        } else {
            dry = (mixRate * 2.0f);
        }
    }
    for (auto i = 0u; i < outputSize; ++i) {
        Type out {};

        if constexpr (Path == InternalPath::Default) {
            const auto lastOut = _cache[Index].delayCache.data()[_cache[Index].readIndex];
            out = lastOut * wet + _cache[Index].lastIn.template data<Type>()[i] * dry;
            _cache[Index].lastOut.template data<Type>()[i] = lastOut;
            incrementReadIndex<Index>();
        } else if constexpr (Path == InternalPath::Feedback) {
            const auto lastOut = _cache[Index].delayCache.data()[_cache[Index].readIndex];
            const auto in = _cache[Index].lastIn.template data<Type>()[i];

            const auto delay = in + lastOut * _cache[Index].feedbackRate;
            _cache[Index].delayCache.data()[_cache[Index].writeIndex] = delay;
            _cache[Index].lastOut.template data<Type>()[i] = delay;
            out = in * dry + delay * wet;
            incrementReadIndex<Index>();
            incrementWriteIndex<Index>();
        } else {
        }

        if constexpr (Accumulate) {
            output[i] += out;
        } else {
            output[i] = out;
        }
    }
    // std::cout << _cache[Index].writeIndex - _cache[Index].readIndex << std::endl;
}

template<typename Type, Audio::DSP::InternalPath Path, unsigned Count>
template<unsigned Index>
inline void Audio::DSP::DelayLineBase<Type, Path, Count>::incrementReadIndex(void) noexcept
{
    if (++_cache[Index].readIndex >= _cache[Index].delayTime + static_cast<ProcessIndex>(_cache[Index].blockSize))
        _cache[Index].readIndex = 0u;
}

template<typename Type, Audio::DSP::InternalPath Path, unsigned Count>
template<unsigned Index>
inline void Audio::DSP::DelayLineBase<Type, Path, Count>::incrementWriteIndex(void) noexcept
{
    if (++_cache[Index].writeIndex >= _cache[Index].delayTime + static_cast<ProcessIndex>(_cache[Index].blockSize))
        _cache[Index].writeIndex = 0u;
}

template<typename Type, Audio::DSP::InternalPath Path, unsigned Count>
typename Audio::DSP::DelayLineBase<Type, Path, Count>::ProcessIndex Audio::DSP::DelayLineBase<Type, Path, Count>::getDelayTime(const float sampleRate, const float time) const noexcept
{
    if (time == 0.0f)
        return 1u;
    if (const auto index = sampleRate * time; index < 1.0f)
        return 1u;
    else
        return static_cast<ProcessIndex>(index);
}

template<typename Type, Audio::DSP::InternalPath Path, unsigned Count>
template<unsigned Index>
void Audio::DSP::DelayLineBase<Type, Path, Count>::setAmount(const float feedback, const float feedforward) noexcept
{
    if (feedback >= 0.9f)
        _cache[Index].feedbackRate = 0.9f;
    else
        _cache[Index].feedbackRate = feedback;
    _cache[Index].feedforwardRate = feedforward;
}
