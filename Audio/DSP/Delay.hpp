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
    static constexpr float DelayLineMaxRate = 0.999'999'97f;

    enum class DelayLineDesign : std::uint8_t
    {
        Default,            // Feedforward (with feedback loop) comb filter
        Feedforward,        // Feedforward comb filter
        FeedforwardNorm,    // Feedforward comb filter normalize
        Feedback,           // Feedback comb filter
        FeedbackNorm,       // Feedback comb filter normalize
        AllPass             // All-pass (feedforward -> feedback) filter
    };

    template<typename Type, DelayLineDesign Design = DelayLineDesign::Default, unsigned Count = 1u>
    class DelayLineBase;

    template<typename Type, DelayLineDesign Design = DelayLineDesign::Default, unsigned Count = 2u>
    class DelayLineParallel;

    template<typename Type, DelayLineDesign Design = DelayLineDesign::Default>
    using DelayLineUnique = DelayLineBase<Type, Design, 1u>;

    template<typename Type, DelayLineDesign Design = DelayLineDesign::Default, unsigned Count = 2u>
    class DelayLineSerie;

    template<typename Type>
    struct DelayLineAllPass
    {
        DelayLineUnique<Type, DelayLineDesign::Feedback> feedback;
        DelayLineUnique<Type, DelayLineDesign::Feedforward> feedforward;
    };

}

template<typename Type, Audio::DSP::DelayLineDesign, unsigned Count>
class Audio::DSP::DelayLineBase
{
public:
    using ProcessIndex = std::size_t;
    using ProcessCache = Core::TinyVector<Type>;

    struct InternalCache
    {
        // Internal delay cache
        ProcessCache cache;
        Buffer input;
        // Delay time in samples
        ProcessIndex delayTime { 0u };

        float inputRate { 1.0f };
        float delayRate { 0.0f };

        ProcessIndex index { 0u };
        Type lastOutFeedback { 0 };
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

    template<unsigned Index, bool Accumulate>
    void process(const Type *input, Type *output, const std::size_t processSize, const DB outGain = 1.0f);


    /** @brief Reset internal cache and indexes */
    void resetAll(const AudioSpecs &audioSpecs, const float maxDelayDuration, const float delayDuration) noexcept
        { resetAllImpl<0u>(audioSpecs, maxDelayDuration, delayDuration); }

    template<unsigned Index = 0u>
    void reset(const AudioSpecs &audioSpecs, const float maxDelayDuration, const float delayDuration) noexcept;

    template<unsigned Index = 0u, bool Clip = false>
    void setInputAmount(const float amount) noexcept;

    template<unsigned Index = 0u, bool Clip = false>
    void setDelayAmount(const float amount) noexcept;

    template<unsigned Index = 0u>
    void setDelayTime(const float sampleRate, const float delayTime) noexcept
    {
        const ProcessIndex newDelayTime = getDelayTime(sampleRate, delayTime);
        if (_cache[Index].delayTime == newDelayTime)
            return;
        _cache[Index].delayTime = newDelayTime;
    }

private:
    Cache _cache;

    template<unsigned Index>
    Type processDefaultSample(const Type x) noexcept
    {
        // GOOD
        auto &line = _cache[Index];
        const auto delay = getDelaySample<Index>();

        line.cache.data()[line.index] = x + line.delayRate * delay;
        if (++line.index >= line.delayTime)
            line.index = 0u;
        return x * line.inputRate + delay;
    }

    template<unsigned Index>
    Type processFeedforwardSample(const Type x) noexcept
    {
        // GOOD
        auto &line = _cache[Index];
        const auto delay = getDelaySample<Index>();

        line.cache.data()[line.index] = x;
        if (++line.index >= line.delayTime)
            line.index = 0u;
        return x * line.inputRate + delay * line.delayRate;
    }

    template<unsigned Index>
    Type processFeedforwardNormSample(const Type x) noexcept
    {
        // GOOD
        auto &line = _cache[Index];
        const auto delay = getDelaySample<Index>();

        line.cache.data()[line.index] = x;
        if (++line.index >= line.delayTime)
            line.index = 0u;
        return x + delay * line.delayRate;
    }

    template<unsigned Index>
    Type processFeedbackSample(const Type x) noexcept
    {
        // GOOD
        auto &line = _cache[Index];
        const auto delay = getDelaySample<Index>();
        const auto out = x * line.inputRate + delay * -line.delayRate;

        line.cache.data()[line.index] = out;
        if (++line.index >= line.delayTime)
            line.index = 0u;
        return out;
    }

    template<unsigned Index>
    Type processFeedbackNormSample(const Type x) noexcept
    {
        // GOOD
        auto &line = _cache[Index];
        const auto delay = getDelaySample<Index>();
        const auto out = x + delay * line.delayRate;

        line.cache.data()[line.index] = out;
        if (++line.index >= line.delayTime)
            line.index = 0u;
        return out;
    }

    template<unsigned Index>
    Type processAllPassSample(const Type x) noexcept
    {
        auto &line = _cache[Index];
        const auto delay = getDelaySample<Index>();
        const auto next = x + delay * -line.delayRate;
        // const auto out = delay * line.delayRate + x;

        line.cache.data()[line.index] = next;
        if (++line.index >= line.delayTime)
            line.index = 0u;
        return next * line.delayRate + delay;
    }

    template<unsigned Index>
    Type getDelaySample(void) const noexcept
    {
        return _cache[Index].cache.data()[_cache[Index].index];
    }


    ProcessIndex getDelayTime(const float sampleRate, const float time) const noexcept;

    template<unsigned Index>
    void sendDataAllImpl(const Type *input, const std::size_t inputSize) noexcept;

    template<bool Accumulate, unsigned Index>
    void receiveDataAllImpl(Type *output, const std::size_t outputSize, const float outGain) noexcept;

    template<unsigned Index>
    void resetAllImpl(const AudioSpecs &audioSpecs, const float maxDelayDuration, const float delayDuration) noexcept;
};


template<typename Type, Audio::DSP::DelayLineDesign Design, unsigned Count>
class Audio::DSP::DelayLineParallel : public Audio::DSP::DelayLineBase<Type, Design, Count>
{
public:
    using Base = DelayLineBase<Type, Design, Count>;

    template<unsigned Index>
    void reset(const AudioSpecs &audioSpecs, const float maxDelayDuration, const float delayDuration) noexcept
    {
        Base::template reset<Index>(audioSpecs, maxDelayDuration, delayDuration);
        resetCache(audioSpecs);
    }

    void resetAll(const AudioSpecs &audioSpecs, const float maxDelayDuration, const float delayDuration) noexcept
    {
        Base::template resetAll(audioSpecs, maxDelayDuration, delayDuration);
        resetCache(audioSpecs);
    }

    template<bool Accumulate>
    void process(const Type *input, Type *output, const std::size_t processSize, const DB outGain = 1.0f) noexcept
    {
        // Save input buffer
        std::memcpy(_input.template data<Type>(), input, processSize * sizeof(Type));

        processAllImpl<0u, Accumulate>(output, processSize, outGain);
    }


private:
    Buffer _input;

    template<unsigned Index, bool Accumulate>
    void processAllImpl(Type *output, const std::size_t processSize, const DB outGain) noexcept
    {
        if constexpr (Index < Count) {
            const auto outGainNorm = outGain / static_cast<DB>(Count);

            Base::template process<Index, Accumulate>(_input.template data<Type>(), output, processSize, outGainNorm);
            processAllImpl<Index + 1u, true>(output, processSize, outGain);
        }
    }


    void resetCache(const AudioSpecs &audioSpecs) noexcept
    {
        const auto inputCacheSize = GetFormatByteLength(audioSpecs.format) * audioSpecs.processBlockSize;
        _input.resize(inputCacheSize, audioSpecs.sampleRate, audioSpecs.channelArrangement, audioSpecs.format);
        _input.clear();
    }
};

template<typename Type, Audio::DSP::DelayLineDesign Design, unsigned Count>
class Audio::DSP::DelayLineSerie : public Audio::DSP::DelayLineBase<Type, Design, Count>
{
public:
    using Base = DelayLineBase<Type, Design, Count>;

    static_assert(Count >= 2u, "Audio::DSP::DelayLineSerie<Type, Design, Count>: Count must be >= 2.");

    DelayLineSerie(void) = default;

    template<bool Accumulate>
    void process(const Type *input, Type *output, const std::size_t processSize) noexcept
        { processAllImpl<0u, Accumulate>(input, output, processSize); }


    template<unsigned Index>
    void reset(const AudioSpecs &audioSpecs, const float maxDelayDuration, const float delayDuration) noexcept
        { Base::template reset<Index>(audioSpecs, maxDelayDuration, delayDuration); }

    void resetAll(const AudioSpecs &audioSpecs, const float maxDelayDuration, const float delayDuration) noexcept
        { Base::template resetAll(audioSpecs, maxDelayDuration, delayDuration); }

private:
    template<unsigned Index, bool Accumulate>
    void processAllImpl(const Type *input, Type *output, const std::size_t processSize) noexcept
    {
        if constexpr (Index < Count) {
            Base::template process<Index, Accumulate>(input, output, processSize, 0.707f);
            processAllImpl<Index + 1u, true>(output, output, processSize);
        }
    }
};

#include "Delay.ipp"
