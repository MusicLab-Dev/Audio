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

        // 2 -> 1 octave
        static constexpr float KeyFollowRate = 2.0f;

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
            float detune;
            std::uint32_t feedback;
            Key keyBreakPoint;
            float keyAmountLeft;
            float keyAmountRight;
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

    template<bool Accumulate, bool Modulate, bool IsCarrier, unsigned OperatorIndex>
    inline void processOperator(
            const float *input, float *output, const std::uint32_t processSize, const float outputGain,
            const std::uint32_t phaseIndex, const float frequencyNorm, const Key key,
            const Internal::Operator &op
    ) noexcept
    {
        // const auto semitone = (25 - 11) % 12;
        // const auto octave = (25 - 11) / 12;

        // constexpr auto GetKeyAmountRate = [](const float amountLeft, const float amountRight, const bool direction, const Key key, const Key breakPoint)
        // {
        //     if (direction) {
        //         if (!amountRight) {
        //             return 0.0f;
        //         } else if (amountRight < 0.0f) {
        //             return 1.0f / std::pow(2.0f, static_cast<float>(key - breakPoint) / 12.0f) * -amountRight;
        //         } else {
        //             return std::pow(2.0f, static_cast<float>(key - breakPoint) / 12.0f) * amountRight;
        //         }
        //     } else {
        //         if (!amountLeft) {
        //             return 0.0f;
        //         } else if (amountLeft < 0.0f) {
        //             return std::pow(2.0f, static_cast<float>(key - breakPoint) / 12.0f) * -amountLeft;
        //         } else {
        //             return std::pow(2.0f, static_cast<float>(breakPoint - key) / 12.0f) * amountLeft;
        //         }
        //     }
        // };


        constexpr auto GetKeyAmountRate = [](const float amount, const float keyDelta)
        {
            if (!amount) {
                return 1.0f;
            } else if (amount < 0.0f) {
                return -amount * std::pow(2.0f, -keyDelta) + 1.0f + amount;
            } else {
                return amount * std::pow(2.0f, keyDelta) + 1.0f - amount;
            }
        };


        // [0:128]

        const auto keyDelta = static_cast<float>(key - op.keyBreakPoint) / 12.0f;
        const auto keyAmountDirection = keyDelta >= 0;
        auto keyAmount = keyAmountDirection ? GetKeyAmountRate(op.keyAmountRight, keyDelta) : GetKeyAmountRate(op.keyAmountLeft, -keyDelta);

        // if constexpr (IsCarrier)
        //     keyAmount = 1.0f;

        const auto outGain = op.volume * outputGain * keyAmount;

        std::cout << OperatorIndex << std::endl;
        std::cout << "outputGain: " << outputGain << std::endl;
        std::cout << "volume: " << op.volume << std::endl;
        std::cout << "key amount: " << keyAmount << std::endl;
        std::cout << "final gain: " << outGain << std::endl;
        std::cout << std::endl;

        for (auto i = 0u; i < processSize; ++i) {
            const auto index = i + phaseIndex;
            const auto realOutGain = outGain * _envelopes.template getGain<OperatorIndex>(key, index, 0.0f, op.attack, 0.0f, op.decay, op.sustain, op.release, _sampleRate);
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
        } else if constexpr (OperatorCount == 4u) {
            twoCM<Accumulate>(output, processSize, outputGain, phaseIndex, key, rootFrequency, operators);
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
        processOperator<Accumulate, false, true, 0>(nullptr, output, processSize, outputGain, phaseIndex, (rootFrequency * operators[0].frequencyDelta) / static_cast<float>(_sampleRate), key, operators[0]);
    }

    template<bool Accumulate>
    void twoCM(
            float *output, const std::uint32_t processSize, const float outputGain,
            const std::uint32_t phaseIndex, const Key key, const float rootFrequency,
            const Internal::OperatorArray<OperatorCount> &operators
    ) noexcept
    {
        std::cout << "ICI" << std::endl;
        std::cout << outputGain << std::endl << std::endl;
        oneModulatorToCarrier_impl<Accumulate>(output, processSize, outputGain, phaseIndex, rootFrequency, key, operators);
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

        oneModulatorToCarrier_impl<Accumulate>(output, processSize, realOutputGain, phaseIndex, rootFrequency, key, operators);
    }

    template<bool Accumulate, unsigned Index = 0u>
    void oneModulatorToCarrier_impl(
            float *output, const std::uint32_t processSize, const float outputGain,
            const std::uint32_t phaseIndex, const float rootFrequency, const Key key,
            const Internal::OperatorArray<OperatorCount> &operators
    ) noexcept
    {
        if constexpr (Index < OperatorCount) {
            constexpr auto CarrierIdx = Index;
            constexpr auto ModIdx = Index + 1;

            // _cache.clear();
            processOperator<false, false, false, ModIdx>(nullptr, _cache.data(), processSize, 1.0f, phaseIndex, (getDetuneFrequency(rootFrequency, operators[ModIdx].detune) * operators[ModIdx].frequencyDelta) / static_cast<float>(_sampleRate), key, operators[ModIdx]);
            processOperator<Accumulate, true, true, CarrierIdx>(_cache.data(), output, processSize, outputGain, phaseIndex, (getDetuneFrequency(rootFrequency, operators[CarrierIdx].detune) * operators[CarrierIdx].frequencyDelta) / static_cast<float>(_sampleRate), key, operators[CarrierIdx]);
            oneModulatorToCarrier_impl<Accumulate, Index + 2>(output, processSize, outputGain, phaseIndex, rootFrequency, key, operators);
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

    [[nodiscard]] float getDetuneFrequency(const float rootFrequency, const float detune) const noexcept
    {
        if (!detune)
            return rootFrequency;
        const double detuneNorm = static_cast<double>(detune) / 1200.0;
        return rootFrequency * static_cast<float>(std::pow(2.0, detuneNorm));
    }

};

#include "FM.ipp"
