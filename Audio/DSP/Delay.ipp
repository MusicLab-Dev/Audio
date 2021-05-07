/**
 * @file Delay.ipp
 * @brief Delay implementation
 *
 * @author Pierre V
 * @date 2021-05-05
 */

#include <Core/Assert.hpp>

template<typename Type>
inline void Audio::DSP::BasicDelay<Type>::reset(const float sampleRate, const std::size_t blockSize, const float delaySize) noexcept
{
    _delayTime = static_cast<Index>(static_cast<float>(sampleRate) * delaySize);
    _delayCache.resize(static_cast<std::uint32_t>(_delayTime + blockSize));
    _readIndex = 0u;
    _writeIndex = _delayTime;
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
    // const auto realSize = inputSize > _delayCache.size() ? _delayCache.size() : inputSize;

    // UNUSED(input);
    // UNUSED(realSize);
    UNUSED(feedbackRate);

    for (auto i = 0u; i < inputSize; ++i) {
        _delayCache.data()[getNextWriteIndex()] = input[i];
    }
}

template<typename Type>
inline void Audio::DSP::BasicDelay<Type>::receiveData(Type *output, const std::size_t outputSize, const float mixRate) noexcept
{
    const auto realSize = outputSize > _delayCache.size() ? _delayCache.size() : outputSize;

    UNUSED(mixRate);
    for (auto i = 0u; i < realSize; ++i) {
        output[i] = _delayCache.data()[getNextReadIndex()];
    }
}

template<typename Type>
typename Audio::DSP::BasicDelay<Type>::Index Audio::DSP::BasicDelay<Type>::getNextReadIndex(void) noexcept
{
    if (++_readIndex >= _delayCache.size())
        _readIndex = 0u;
    return _readIndex;
}

template<typename Type>
typename Audio::DSP::BasicDelay<Type>::Index Audio::DSP::BasicDelay<Type>::getNextWriteIndex(void) noexcept
{
    if (++_writeIndex >= _delayCache.size())
        _writeIndex = 0u;
    return _writeIndex;
}
