/**
 * @file FM.hpp
 * @brief
 *
 * @author Pierre V
 * @date 2021-06-01
 */

#pragma once

#include <Core/FlatVector.hpp>

#include "EnvelopeGenerator.hpp"

namespace Audio::DSP::FM
{
    namespace Internal
    {
        using ProcessSizeType = std::uint16_t;
        using OperatorCountType = std::uint8_t;
        using CacheList = Core::FlatVector<float, ProcessSizeType>;

        struct OperatorFlat
        {
            float attack;
            float decay;
            float sustain;
            float release;
            float volume;
        };

        struct Operator
        {
            float frequencyDelta;
            float attack;
            float decay;
            float sustain;
            float release;
            float volume;
            std::int32_t detune;
            std::uint32_t feedback;
        };

        template<unsigned InstanceCount>
        using OperatorArray = std::array<Internal::Operator, InstanceCount>;

        class Specs
        {

        };

    }

    template<unsigned OperatorCount>
    class Schema;
}

template<unsigned OperatorCount>
class Audio::DSP::FM::Schema
{
public:

    using EnvelopeList = EnvelopeBase<EnvelopeType::ADSR, OperatorCount>;
    // using FrequencyRateList = Core::SmallVector<float, OperatorCount, std::uint8_t>;


    // template<bool Modulate>
    // inline float processOperator(const float *input, const float phaseIndex, const float frequency, const float modulationAmount) noexcept
    // {
    //     if constexpr (Modulate)
    //         return std::sin(2.0f * static_cast<float>(M_PI) * phaseIndex * frequency + (*input * modulationAmount));
    //     else
    //         return std::sin(2.0f * static_cast<float>(M_PI) * phaseIndex * frequency);
    // }

    template<bool Accumulate, bool Modulate, unsigned OperatorIndex>
    inline void processOperator(
            const float *input, float *output, const std::uint32_t processSize, const float outputGain,
            const std::uint32_t phaseIndex, const float frequencyNorm, const Key key,
            const Internal::Operator &op
    ) noexcept
    {
        const auto outGain = op.volume * outputGain;

        for (auto i = 0u; i < processSize; ++i) {
            const auto index = i + phaseIndex;
            const auto realOutGain = outGain * _envelopes.template adsr<OperatorIndex>(key, index, false, op.attack, op.decay, op.sustain, op.release, _sampleRate);
            const auto freq = 2.0f * static_cast<float>(M_PI) * (static_cast<float>(index)) * frequencyNorm;

            if constexpr (Modulate) {
                if constexpr (Accumulate)
                    output[i] += realOutGain * std::sin(freq + static_cast<float>(M_PI) * input[i]);
                else
                    output[i] = realOutGain * std::sin(freq + static_cast<float>(M_PI) * input[i]);
            } else {
                if constexpr (Accumulate)
                    output[i] += realOutGain * processFeedback(freq, op.feedback);
                else
                    output[i] = realOutGain * processFeedback(freq, op.feedback);
            }
        }
    }

    /** @brief Get the envelope list */
    [[nodiscard]] EnvelopeList &envelopes(void) noexcept { return _envelopes; }
    [[nodiscard]] const EnvelopeList &envelopes(void) const noexcept { return _envelopes; }

    void setSampleRate(const SampleRate sampleRate) noexcept { _sampleRate = sampleRate; }

    template<bool Accumulate>
    void process(
            float *output, const std::uint32_t processSize, const float outputGain,
            const std::uint32_t phaseIndex, const Key key, const float rootFrequency,
            const Internal::OperatorArray<OperatorCount> &operators
    ) noexcept
    {
        _cache.resize(static_cast<Internal::ProcessSizeType>(processSize));
        if constexpr (OperatorCount == 2u) {
            oneCarrierOneModulator<Accumulate>(output, processSize, outputGain, phaseIndex, key, rootFrequency, operators);
        } else if constexpr (OperatorCount == 6u) {
            dx7_05<Accumulate>(output, processSize, outputGain, phaseIndex, key, rootFrequency, operators);
        }
    }

private:
    EnvelopeList _envelopes;
    SampleRate _sampleRate;
    Internal::CacheList _cache;

    template<bool Accumulate>
    void oneCarrierOneModulator(
            float *output, const std::uint32_t processSize, const float outputGain,
            const std::uint32_t phaseIndex, const Key key, const float rootFrequency,
            const Internal::OperatorArray<OperatorCount> &operators
    ) noexcept
    {
        // Op_1 modulate Op_0
        // processOperator<Accumulate, false, 1>(nullptr, _cache.data(), processSize, 1.0f, phaseIndex, (rootFrequency * operators[1].frequencyDelta) / static_cast<float>(_sampleRate), key, operators[1]);
        // processOperator<Accumulate, true, 0>(_cache.data(), output, processSize, outputGain, phaseIndex, (rootFrequency * operators[0].frequencyDelta) / static_cast<float>(_sampleRate), key, operators[0]);

        // Op_0 only
        processOperator<Accumulate, false, 0>(nullptr, output, processSize, outputGain, phaseIndex, (rootFrequency * operators[0].frequencyDelta) / static_cast<float>(_sampleRate), key, operators[0]);
    }

    template<bool Accumulate>
    void dx7_05(
            float *output, const std::uint32_t processSize, const float outputGain,
            const std::uint32_t phaseIndex, const Key key, const float rootFrequency,
            const Internal::OperatorArray<OperatorCount> &operators
    ) noexcept
    {
        constexpr auto CarrierCount = 3u;
        const auto realOutputGain = outputGain / static_cast<float>(CarrierCount);

        dx7_05_impl<Accumulate>(output, processSize, realOutputGain, phaseIndex, rootFrequency, key, operators);
    }

    template<bool Accumulate, unsigned Index = 0u>
    void dx7_05_impl(
            float *output, const std::uint32_t processSize, const float outputGain,
            const std::uint32_t phaseIndex, const float rootFrequency, const Key key,
            const Internal::OperatorArray<OperatorCount> &operators
    ) noexcept
    {
        if constexpr (Index < OperatorCount) {
            constexpr auto CarrierIdx = Index;
            constexpr auto ModIdx = Index + 1;

            // _cache.clear();
            processOperator<false, false, ModIdx>(nullptr, _cache.data(), processSize, 1.0f, phaseIndex, (getDetuneFrequency(rootFrequency, operators[ModIdx].detune) * operators[ModIdx].frequencyDelta) / static_cast<float>(_sampleRate), key, operators[ModIdx]);
            processOperator<Accumulate, true, CarrierIdx>(_cache.data(), output, processSize, outputGain, phaseIndex, (getDetuneFrequency(rootFrequency, operators[CarrierIdx].detune) * operators[CarrierIdx].frequencyDelta) / static_cast<float>(_sampleRate), key, operators[CarrierIdx]);
            dx7_05_impl<Accumulate, Index + 2>(output, processSize, outputGain, phaseIndex, rootFrequency, key, operators);
        }
    }

    [[nodiscard]] float processFeedback(const float freq, const std::uint32_t feedbackAmount) const noexcept
    {
        switch (feedbackAmount) {
        case 0:
            return unrollFeedback<0u>(freq);
        case 1:
            return unrollFeedback<1u>(freq);
        case 2:
            return unrollFeedback<2u>(freq);
        case 3:
            return unrollFeedback<3u>(freq);
        case 4:
            return unrollFeedback<4u>(freq);
        case 5:
            return unrollFeedback<5u>(freq);
        case 6:
            return unrollFeedback<6u>(freq);
        case 7:
            return unrollFeedback<7u>(freq);
        default:
            return unrollFeedback<0u>(freq);
        }
    }

    template<unsigned Feedback>
    [[nodiscard]] float unrollFeedback(const float freq) const noexcept
    {
        if constexpr (!Feedback) {
            return std::sin(freq);
        } else {
            return std::sin(freq + unrollFeedback<Feedback - 1>(freq));
        }
    }

    [[nodiscard]] float getDetuneFrequency(const float rootFrequency, std::int32_t detune) const noexcept
    {
        if (!detune)
            return rootFrequency;
        const double detuneNorm = static_cast<double>(detune) / 120.0;
        return rootFrequency * static_cast<float>(std::pow(2.0, detuneNorm));
    }

};

#include "FM.ipp"
