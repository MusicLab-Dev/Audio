/**
 * @file Delay.hpp
 * @brief Delay instances
 *
 * @author Pierre V
 * @date 2021-05-05
 */

#pragma once

#include <Core/Vector.hpp>
#include <Audio/Buffer.hpp>

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
    BasicDelay(const float sampleRate, const std::size_t blockSize, const float maxDelaySize, const float delaySize) { reset(sampleRate, blockSize, delaySize); }

    // void setMaxDelaySize(const SampleRate sampleRate, const float maxDelay) noexcept;
    // void setDelaySize(const SampleRate sampleRate, const float delay) noexcept;
    void sendData(const Type *input, const std::size_t inputSize, const float feedbackRate) noexcept;
    void receiveData(Type *output, const std::size_t outputSize, const float mixRate) noexcept;

    /** @brief Reset internal cache and indexes */
    void reset(const AudioSpecs &audioSpecs, const float maxDelaySize, const float delaySize) noexcept;

    void setDelayTime(const float sampleRate, const float delayTime) noexcept
    {
        const Index newDelayTime = static_cast<Index>(sampleRate * delayTime);
        if (_delayTime == newDelayTime)
            return;
        _delayTime = newDelayTime;
        const int delta = static_cast<int>(newDelayTime) - static_cast<int>(_delayTime);
        const int newReadIndex = static_cast<int>(_readIndex) + delta;
        // Overflow
        if (newReadIndex < 0) {
            _readIndex = static_cast<Index>(_delayCache.size()) + static_cast<Index>(-newReadIndex);
        } else {
            _readIndex = static_cast<Index>(newReadIndex);
        }
    }

    // Buffer &inputCache(void) noexcept { return _inputCache; }
    // const Buffer &inputCache(void) const noexcept { return _inputCache; }

private:
    // Internal delay cache
    Cache _delayCache;
    Buffer _lastIn;
    Buffer _lastOut;
    // Delay time in samples
    Index _delayTime { 0u };
    // Read index in samples
    Index _readIndex { 0u };
    // Write index in samples
    Index _writeIndex { 0u };

    /** @todo REMOVE THIS MEMBER ! */
    std::size_t _blockSize { 0u };

    Index getNextReadIndex(void) noexcept;
    Index getNextWriteIndex(void) noexcept;
};

#include "Delay.ipp"
