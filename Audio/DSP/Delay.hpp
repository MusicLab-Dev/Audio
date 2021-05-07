/**
 * @file Delay.hpp
 * @brief Delay instances
 *
 * @author Pierre V
 * @date 2021-05-05
 */

#pragma once

#include <Core/Vector.hpp>
#include <Audio/Base.hpp>

namespace Audio::DSP
{
    template<typename Type>
    class BasicDelay;
}

template<typename Type>
class Audio::DSP::BasicDelay
{
public:
    using Cache = Core::TinyVector<Type>;
    using Index = std::size_t;

    BasicDelay(void) = default;
    BasicDelay(const float sampleRate, const std::size_t blockSize, const float delaySize) { reset(sampleRate, blockSize, delaySize); }

    // void setMaxDelaySize(const SampleRate sampleRate, const float maxDelay) noexcept;
    // void setDelaySize(const SampleRate sampleRate, const float delay) noexcept;
    void sendData(const Type *input, const std::size_t inputSize, const float feedbackRate) noexcept;
    void receiveData(Type *output, const std::size_t outputSize, const float mixRate) noexcept;

    void reset(const float sampleRate, const std::size_t blockSize, const float delaySize) noexcept;
    // Buffer &inputCache(void) noexcept { return _inputCache; }
    // const Buffer &inputCache(void) const noexcept { return _inputCache; }

private:
    Cache _delayCache;
    Index _delayTime { 0u };
    Index _readIndex { 0u };
    Index _writeIndex { 0u };

    Index getNextReadIndex(void) noexcept;
    Index getNextWriteIndex(void) noexcept;
};

#include "Delay.ipp"
