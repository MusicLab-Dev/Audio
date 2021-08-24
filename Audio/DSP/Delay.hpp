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
    enum class InternalPath : std::uint8_t
    {
        Default,    // Feedforward comb filter
        Feedback,   // Feedback comb filter
        Both        // All-pass filter
    };

    template<typename Type, InternalPath Path = InternalPath::Default, unsigned Count = 1u>
    class DelayLineBase;

    template<typename Type, InternalPath Path = InternalPath::Default, unsigned Count = 1u>
    using DelayLineParallel = DelayLineBase<Type, Path, Count>;

    template<typename Type, InternalPath Path = InternalPath::Default>
    using DelayLineUnique = DelayLineBase<Type, Path, 1u>;

    template<typename Type, InternalPath Path = InternalPath::Default, unsigned Count = 1u>
    class DelayLineSerie;

    template<typename Type>
    struct DelayLineAllPass
    {
        DelayLineUnique<Type, InternalPath::Feedback> _feedback;
        DelayLineUnique<Type, InternalPath::Default> _feedforward;
    };

}

template<typename Type, Audio::DSP::InternalPath, unsigned Count>
class Audio::DSP::DelayLineBase
{
public:
    using ProcessIndex = std::size_t;
    using ProcessCache = Core::TinyVector<Type>;

    struct InternalCache
    {
        // Internal delay cache
        ProcessCache delayCache;
        Buffer lastIn;
        Buffer lastOut;
        // Delay time in samples
        ProcessIndex delayTime { 0u };
        // Read index in samples
        ProcessIndex readIndex { 0u };
        // Write index in samples
        ProcessIndex writeIndex { 0u };

        float feedbackRate { 0.0f };
        float feedforwardRate { 0.0f };

        /** @todo REMOVE THIS MEMBER ! */
        std::size_t blockSize { 0u };
    };

    using Cache = std::array<InternalCache, Count>;

    DelayLineBase(void) = default;

    void sendDataAll(const Type *input, const std::size_t inputSize) noexcept
        { sendDataAllImpl<0u>(input, inputSize); }

    template<unsigned Index = 0u>
    void sendData(const Type *input, const std::size_t inputSize) noexcept;

    template<bool Accumulate>
    void receiveDataAll(Type *output, const std::size_t outputSize, const float mixRate) noexcept
        { receiveDataAllImpl<0u, Accumulate>(output, outputSize, mixRate); }

    template<bool Accumulate, unsigned Index = 0u>
    void receiveData(Type *output, const std::size_t outputSize, const float mixRate) noexcept;


    /** @brief Reset internal cache and indexes */
    void resetAll(const AudioSpecs &audioSpecs, const float maxDelaySize, const float delaySize) noexcept
        { resetAllImpl<0u>(audioSpecs, maxDelaySize, delaySize); }

    template<unsigned Index = 0u>
    void reset(const AudioSpecs &audioSpecs, const float maxDelaySize, const float delaySize) noexcept;

    template<unsigned Index = 0u>
    void setAmount(const float feedback, const float feedforward) noexcept;

    template<unsigned Index = 0u>
    void setDelayTime(const float sampleRate, const float delayTime) noexcept
    {
        const ProcessIndex newDelayTime = getDelayTime(sampleRate, delayTime);
        if (_cache[Index].delayTime == newDelayTime)
            return;
        _cache[Index].delayTime = newDelayTime;
        const int delta = static_cast<int>(newDelayTime) - static_cast<int>(_cache[Index].delayTime);
        const int newReadIndex = static_cast<int>(_cache[Index].readIndex) + delta;
        // Overflow
        if (newReadIndex < 0) {
            _cache[Index].readIndex = static_cast<ProcessIndex>(_cache[Index].delayCache.size()) + static_cast<ProcessIndex>(-newReadIndex);
        } else {
            _cache[Index].readIndex = static_cast<ProcessIndex>(newReadIndex);
        }
    }

private:
    Cache _cache;

    template<unsigned Index>
    void incrementReadIndex(void) noexcept;
    template<unsigned Index>
    void incrementWriteIndex(void) noexcept;

    ProcessIndex getDelayTime(const float sampleRate, const float time) const noexcept;

    template<unsigned Index>
    void sendDataAllImpl(const Type *input, const std::size_t inputSize) noexcept;

    template<bool Accumulate, unsigned Index>
    void receiveDataAllImpl(Type *output, const std::size_t outputSize, const float mixRate) noexcept;

    template<unsigned Index>
    void resetAllImpl(const AudioSpecs &audioSpecs, const float maxDelaySize, const float delaySize) noexcept;

};



template<typename Type, Audio::DSP::InternalPath Path, unsigned Count>
class Audio::DSP::DelayLineSerie
{
public:
    using Lines = DelayLineBase<Type, Path>;

    DelayLineSerie(void) = default;

    void sendData(const Type *input, const std::size_t inputSize) noexcept
    {

    }

    void receiveData(Type *output, const std::size_t outputSize) noexcept
    {

    }


    void reset(const AudioSpecs &audioSpecs, const float maxDelaySize, const float delaySize) noexcept
        { _lines.reset(audioSpecs, maxDelaySize, delaySize); }

    void setFeedback(const float feedback) noexcept
        { _lines.setFeedback(feedback); }

    void setDelayTime(const float sampleRate, const float delayTime) noexcept
        { _lines.setDelayTime(sampleRate, delayTime); }

private:
    Lines _lines;

    void incrementReadIndex(void) noexcept;
    void incrementWriteIndex(void) noexcept;

    typename Lines::ProcessIndex getDelayTime(const float sampleRate, const float time) const noexcept;
};

#include "Delay.ipp"
