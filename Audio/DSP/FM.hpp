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

    enum class AlgorithmType : std::uint8_t {
        Default,
        Piano,
        KickDrum
    };

    template<unsigned OperatorCount, AlgorithmType Algo, bool PitchEnv>
    class Schema;
}

template<unsigned OperatorCount, Audio::DSP::FM::AlgorithmType Algo, bool PitchEnv = false>
class Audio::DSP::FM::Schema
{
public:

    using EnvelopeList = EnvelopeBase<EnvelopeType::ADSR, OperatorCount + PitchEnv>;
    // using FrequencyRateList = Core::SmallVector<float, OperatorCount, std::uint8_t>;


    // template<bool Modulate>
    // inline float processOperator(const float *input, const float phaseIndex, const float frequency, const float modulationAmount) noexcept
    // {
    //     if constexpr (Modulate)
    //         return std::sin(2.0f * static_cast<float>(M_PI) * phaseIndex * frequency + (*input * modulationAmount));
    //     else
    //         return std::sin(2.0f * static_cast<float>(M_PI) * phaseIndex * frequency);
    // }

    template<bool Accumulate, bool Modulate, bool IsCarrier, unsigned OperatorIndex, bool IsCosine = false>
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
                    output[i] += realOutGain * processGeneration<IsCosine>(freq, op.feedback);
                else
                    output[i] = realOutGain * processGeneration<IsCosine>(freq, op.feedback);
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
        // std::memset(_cache.data(), 0, processSize * sizeof(float));

        if constexpr (Algo == AlgorithmType::Default){
            if constexpr (OperatorCount == 2u) {
                oneCarrierOneModulator<Accumulate>(output, processSize, outputGain, phaseIndex, key, rootFrequency, operators);
            } else if constexpr (OperatorCount == 4u) {
                twoCM<Accumulate>(output, processSize, outputGain, phaseIndex, key, rootFrequency, operators);
            } else if constexpr (OperatorCount == 6u) {
                dx7_05<Accumulate>(output, processSize, outputGain, phaseIndex, key, rootFrequency, operators);
            }
        } else
            processImpl<Accumulate>(output, processSize, outputGain, phaseIndex, key, rootFrequency, operators);

    }
    template<bool Accumulate>
    void processImpl(
            float *output, const std::uint32_t processSize, const float outputGain,
            const std::uint32_t phaseIndex, const Key key, const float rootFrequency,
            const Internal::OperatorArray<OperatorCount> &operators
    ) noexcept
    {
        if constexpr (Algo == AlgorithmType::KickDrum) {
            static_assert(OperatorCount == 4u, "Audio::DSP::FM::Schema<OperatorCount, KickDrum>::processImpl: OperatorCount must be equal to 4");
            kickDrum_impl<Accumulate>(output, processSize, outputGain, phaseIndex, key, rootFrequency, operators);
        } else if constexpr (Algo == AlgorithmType::Piano) {

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
            processOperator<false, false, false, ModIdx>(nullptr, _cache.data(), processSize, 1.0f, phaseIndex, getDetuneFrequency(rootFrequency, operators[ModIdx]), key, operators[ModIdx]);
            processOperator<Accumulate, true, true, CarrierIdx>(_cache.data(), output, processSize, outputGain, phaseIndex, getDetuneFrequency(rootFrequency, operators[CarrierIdx]), key, operators[CarrierIdx]);
            oneModulatorToCarrier_impl<Accumulate, Index + 2>(output, processSize, outputGain, phaseIndex, rootFrequency, key, operators);
        }
    }

    template<bool Accumulate>
    void kickDrum_impl(
            float *output, const std::uint32_t processSize, const float outputGain,
            const std::uint32_t phaseIndex, const Key key, const float rootFrequency,
            const Internal::OperatorArray<OperatorCount> &operators
    ) noexcept
    {
        const auto realOutputGain = outputGain / static_cast<float>(1);

        std::cout << "ICICICICICICICI" << std::endl;

    // template<bool Accumulate, bool Modulate, bool IsCarrier, unsigned OperatorIndex>
        // Sub (A)
        processOperator<false, false, true, 0u>(nullptr, output, processSize, realOutputGain, phaseIndex, getDetuneFrequency(rootFrequency, operators[0u]), key, operators[0u]);
        // Impact (B+C->D)
        processOperator<false, false, false, 2u>(nullptr, _cache.data(), processSize, 1.0f, phaseIndex, getDetuneFrequency(rootFrequency, operators[2u]), key, operators[2u]);
        processOperator<true, false, false, 3u>(nullptr, _cache.data(), processSize, 1.0f, phaseIndex, getDetuneFrequency(rootFrequency, operators[3u]), key, operators[3u]);
        processOperator<true, true, true, 1u>(_cache.data(), output, processSize, realOutputGain, phaseIndex, getDetuneFrequency(rootFrequency, operators[1u]), key, operators[1u]);
    }


    template<bool IsCosine = false>
    [[nodiscard]] float processGeneration(const float freq, const std::uint32_t feedbackAmount) const noexcept
    {
        switch (feedbackAmount) {
        case 0:
            return unrollGeneration<0u, IsCosine>(freq);
        case 1:
            return unrollGeneration<1u, IsCosine>(freq);
        case 2:
            return unrollGeneration<2u, IsCosine>(freq);
        case 3:
            return unrollGeneration<3u, IsCosine>(freq);
        case 4:
            return unrollGeneration<4u, IsCosine>(freq);
        case 5:
            return unrollGeneration<5u, IsCosine>(freq);
        case 6:
            return unrollGeneration<6u, IsCosine>(freq);
        case 7:
            return unrollGeneration<7u, IsCosine>(freq);
        default:
            return unrollGeneration<0u, IsCosine>(freq);
        }
    }

    template<unsigned Feedback, bool IsCosine>
    [[nodiscard]] float unrollGeneration(const float freq) const noexcept
    {
        if constexpr (!Feedback) {
            if constexpr (IsCosine)
                return std::cos(freq);
            else
                return std::sin(freq);
        } else {
            if constexpr (IsCosine)
                return std::cos(freq + unrollGeneration<Feedback - 1, IsCosine>(freq));
            else
                return std::sin(freq + unrollGeneration<Feedback - 1, IsCosine>(freq));
        }
    }

    [[nodiscard]] float getDetuneFrequency(const float rootFrequency, const Internal::Operator &op) const noexcept
    {
        if (!op.detune)
            return rootFrequency * op.frequencyDelta / static_cast<float>(_sampleRate);
        const double detuneNorm = static_cast<double>(op.detune) / 1200.0;
        return rootFrequency * static_cast<float>(std::pow(2.0, detuneNorm)) * op.frequencyDelta / static_cast<float>(_sampleRate);
    }

};

#include "FM.ipp"
