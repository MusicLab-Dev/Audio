/**
 * @file Delay.ipp
 * @brief Delay implementation
 *
 * @author Pierre V
 * @date 2021-05-05
 */

#include <Core/Assert.hpp>

template<typename Type>
inline void Audio::DSP::BasicDelay<Type>::reset(const AudioSpecs &audioSpecs, const float maxDelaySize, const float delaySize) noexcept
{
    _blockSize = audioSpecs.processBlockSize;
    const std::uint32_t cacheSize = static_cast<std::uint32_t>(static_cast<float>(audioSpecs.sampleRate) * maxDelaySize) + static_cast<std::uint32_t>(audioSpecs.processBlockSize);
    _delayCache.resize(cacheSize);
    _delayTime = static_cast<Index>(static_cast<float>(audioSpecs.sampleRate) * delaySize);
    _readIndex = 0u;
    _writeIndex = _delayTime;

    _lastIn.resize(GetFormatByteLength(audioSpecs.format) * audioSpecs.processBlockSize, audioSpecs.sampleRate, audioSpecs.channelArrangement, audioSpecs.format);
    _lastIn.clear();
    _lastOut.resize(GetFormatByteLength(audioSpecs.format) * audioSpecs.processBlockSize, audioSpecs.sampleRate, audioSpecs.channelArrangement, audioSpecs.format);
    _lastOut.clear();
}

// template<typename Type>
// inline void Audio::DSP::BasicDelay<Type>::setMaxDelaySize(const SampleRate sampleRate, const float maxDelay) noexcept
// {
//     _delayCache(static_cast<std::size_t>(static_cast<float>(sampleRate) * maxDelay));
// }

// template<typename Type>
// inline void Audio::DSP::BasicDelay<Type>::setDelaySize(const SampleRate sampleRate, const float delay) noexcept
// {
//     _delayTime = static_cast<Index>(static_cast<float>(sampleRate) * delay);
//     coreAssert(_delayTime <= _delayCache.size(),
//         throw std::logic_error("Audio::DSP::BasicDelay::setDelaySize: Delay size must be in less than maxDelaySize"));
// }

template<typename Type>
inline void Audio::DSP::BasicDelay<Type>::sendData(const Type *input, const std::size_t inputSize, const float feedbackRate) noexcept
{
    // const auto realSize = inputSize > _delayTime ? _delayTime : inputSize;

    for (auto i = 0u; i < inputSize; ++i) {
        const auto lastIn = input[i];
        _lastIn.data<Type>()[i] = lastIn;
        _delayCache.data()[getNextWriteIndex()] = lastIn + feedbackRate * _lastOut.data<Type>()[i];
    }
}

template<typename Type>
inline void Audio::DSP::BasicDelay<Type>::receiveData(Type *output, const std::size_t outputSize, const float mixRate) noexcept
{
    const auto realSize = outputSize > _delayTime ? _delayTime : outputSize;
    float dry { 1.0f };
    float wet { 1.0f };
    if (mixRate != 0.5f) {
        if (mixRate >= 0.5f) {
            wet = 1.0f - mixRate;
        } else {
            dry = mixRate;
        }
    }
    for (auto i = 0u; i < realSize; ++i) {
        const auto lastOut = _delayCache.data()[getNextReadIndex()];
        _lastOut.data<Type>()[i] = lastOut;
        output[i] = lastOut * wet + _lastIn.data<Type>()[i] * dry;
    }
}

template<typename Type>
typename Audio::DSP::BasicDelay<Type>::Index Audio::DSP::BasicDelay<Type>::getNextReadIndex(void) noexcept
{
    if (++_readIndex >= _delayTime + static_cast<Index>(_blockSize))
        _readIndex = 0u;
    return _readIndex;
}

template<typename Type>
typename Audio::DSP::BasicDelay<Type>::Index Audio::DSP::BasicDelay<Type>::getNextWriteIndex(void) noexcept
{
    if (++_writeIndex >= _delayTime + static_cast<Index>(_blockSize))
        _writeIndex = 0u;
    return _writeIndex;
}
