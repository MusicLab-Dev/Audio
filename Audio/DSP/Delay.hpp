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
        DelayLineUnique<Type, InternalPath::Feedback> feedback;
        DelayLineUnique<Type, InternalPath::Default> feedforward;
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
        ProcessCache inputCache;
        ProcessCache outputCache;
        Buffer input;
        Buffer lastOut;
        // Delay time in samples
        ProcessIndex delayTime { 0u };
        // Read index in samples
        ProcessIndex inputReadIndex { 0u };
        ProcessIndex outputReadIndex { 0u };
        // Write index in samples
        ProcessIndex inputWriteIndex { 0u };
        ProcessIndex outputWriteIndex { 0u };

        float delayRate { 0.0f };
        float inputRate { 0.0f };
        float feedbackAmount { 0.0f };

        ProcessIndex inIdx { 0u };
        Type lastOutFeedback { 0 };
        ProcessIndex outIdx { 0u };

        ProcessIndex blockSize { 0u };
    };

    using Cache = std::array<InternalCache, Count>;

    DelayLineBase(void) = default;

    void sendDataAll(const Type *input, const std::size_t inputSize) noexcept
        { sendDataAllImpl<0u>(input, inputSize); }

    template<unsigned Index = 0u>
    void sendData(const Type *input, const std::size_t inputSize) noexcept;

    template<bool Accumulate>
    void receiveDataAll(Type *output, const std::size_t outputSize, const float outGain = 1.0f) noexcept
        { receiveDataAllImpl<0u, Accumulate>(output, outputSize, outGain); }

    template<bool Accumulate, unsigned Index = 0u>
    void receiveData(Type *output, const std::size_t outputSize, const float outGain = 1.0f) noexcept;


    /** @brief Reset internal cache and indexes */
    void resetAll(const AudioSpecs &audioSpecs, const float maxDelaySize, const float delaySize) noexcept
        { resetAllImpl<0u>(audioSpecs, maxDelaySize, delaySize); }

    template<unsigned Index = 0u>
    void reset(const AudioSpecs &audioSpecs, const float maxDelaySize, const float delaySize) noexcept;

    template<unsigned Index = 0u>
    void setInternalRate(const float feedback, const float feedforward) noexcept;

    template<unsigned Index = 0u>
    void setInternalRateDefault(void) noexcept;

    template<unsigned Index = 0u>
    void setFeedbackAmount(const float amount) noexcept;

    template<unsigned Index = 0u>
    void setDelayTime(const float sampleRate, const float delayTime) noexcept
    {
        const ProcessIndex newDelayTime = getDelayTime(sampleRate, delayTime);
        if (_cache[Index].delayTime == newDelayTime)
            return;
        _cache[Index].delayTime = newDelayTime;
        const int delta = static_cast<int>(newDelayTime) - static_cast<int>(_cache[Index].delayTime);
        const int newInputReadIndex = static_cast<int>(_cache[Index].inputReadIndex) + delta;
        const int newOutputReadIndex = static_cast<int>(_cache[Index].outputReadIndex) + delta;
        const int newInIdx = static_cast<int>(_cache[Index].inIdx) + delta;
        // Overflow
        if (newInputReadIndex < 0) {
            _cache[Index].inputReadIndex = static_cast<ProcessIndex>(_cache[Index].inputCache.size()) + static_cast<ProcessIndex>(-newInputReadIndex);
        } else {
            _cache[Index].inputReadIndex = static_cast<ProcessIndex>(newInputReadIndex);
        }
        if (newOutputReadIndex < 0) {
            _cache[Index].outputReadIndex = static_cast<ProcessIndex>(_cache[Index].outputCache.size()) + static_cast<ProcessIndex>(-newOutputReadIndex);
        } else {
            _cache[Index].outputReadIndex = static_cast<ProcessIndex>(newOutputReadIndex);
        }
        if (newInIdx < 0) {
            _cache[Index].inIdx = static_cast<ProcessIndex>(_cache[Index].outputCache.size()) + static_cast<ProcessIndex>(-newInIdx);
        } else {
            _cache[Index].inIdx = static_cast<ProcessIndex>(newInIdx);
        }
    }

private:
    Cache _cache;

    template<unsigned Index>
    ProcessIndex getNextIndex(const ProcessIndex index) const noexcept;
    template<unsigned Index>
    ProcessIndex incrementInputReadIndex(void) noexcept
        { return _cache[Index].inputReadIndex = getNextIndex<Index>(_cache[Index].inputReadIndex); }
    template<unsigned Index>
    ProcessIndex incrementInputWriteIndex(void) noexcept
        { return _cache[Index].inputWriteIndex = getNextIndex<Index>(_cache[Index].inputWriteIndex); }
    template<unsigned Index>
    ProcessIndex incrementOutputReadIndex(void) noexcept
        { return _cache[Index].outputReadIndex = getNextIndex<Index>(_cache[Index].outputReadIndex); }
    template<unsigned Index>
    ProcessIndex incrementOutputWriteIndex(void) noexcept
        { return _cache[Index].outputWriteIndex = getNextIndex<Index>(_cache[Index].outputWriteIndex); }


    template<unsigned Index>
    Type processFeedforwardSample(const Type x) noexcept
    {
        // GOOD
        auto &line = _cache[Index];
        const auto delay = getDelaySample<Index>();

        line.inputCache.data()[line.inIdx] = x + delay * line.feedbackAmount;
        if (++line.inIdx >= line.delayTime)
            line.inIdx = 0u;
        return x * line.inputRate + delay * line.delayRate;
    }

    template<unsigned Index>
    Type processFeedbackSample(const Type x) noexcept
    {
        auto &line = _cache[Index];
        const auto delay = getDelaySample<Index>();
        const auto out = delay * -line.feedbackAmount;

        line.inputCache.data()[line.inIdx] = out + x;
        if (++line.inIdx >= line.delayTime)
            line.inIdx = 0u;
        return x * line.inputRate + out * line.delayRate;
    }

    template<unsigned Index>
    Type processBothSample(const Type x) noexcept
    {
        auto &line = _cache[Index];
        const auto delay = getDelaySample<Index>();
        const auto out = (x - delay) * line.feedbackAmount;

        line.inputCache.data()[line.inIdx] = out;
        if (++line.inIdx >= line.delayTime)
            line.inIdx = 0u;
        // return out * (line.feedbackAmount * line.delayRate) + x * line.inputRate;
        return out * line.feedbackAmount + x;
    }

    template<unsigned Index>
    Type getDelaySample(void) const noexcept
    {
        return _cache[Index].inputCache.data()[_cache[Index].inIdx];
    }

    template<unsigned Index>
    void resetInDelay(void) noexcept
    {
        _cache[Index].inIdx = 0u;
    }


    ProcessIndex getDelayTime(const float sampleRate, const float time) const noexcept;

    template<unsigned Index>
    void sendDataAllImpl(const Type *input, const std::size_t inputSize) noexcept;

    template<bool Accumulate, unsigned Index>
    void receiveDataAllImpl(Type *output, const std::size_t outputSize, const float outGain) noexcept;

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

    void incrementIndex(void) noexcept;
    void incrementinputWriteIndex(void) noexcept;

    typename Lines::ProcessIndex getDelayTime(const float sampleRate, const float time) const noexcept;
};

#include "Delay.ipp"
