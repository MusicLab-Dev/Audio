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
#include "Generator.hpp"

namespace Audio::DSP::FM
{
    namespace Internal
    {
        using ProcessSizeType = std::uint16_t;
        using OperatorCountType = std::uint8_t;
        using CacheList = Core::FlatVector<float, ProcessSizeType>;
        using PitchIndexList = std::array<std::uint32_t, KeyCount>;

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
            Generator::Waveform waveform;
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

        struct PitchOperator
        {
            float attack;
            float peak;
            float decay;
            float sustain;
            float release;
            float volume;
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

    using EnvelopeList = EnvelopeClipExp<EnvelopeType::ADSR, OperatorCount + PitchEnv>;
    using EnvelopeCache = Core::TinyVector<float>;
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
        UNUSED(frequencyNorm);
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

        const DB outGain = op.volume * outputGain * keyAmount;

        // std::cout << OperatorIndex << std::endl;
        // std::cout << "outputGain: " << outputGain << std::endl;
        // std::cout << "volume: " << op.volume << std::endl;
        // std::cout << "key amount: " << keyAmount << std::endl;
        // std::cout << "final gain: " << outGain << std::endl;
        // std::cout << std::endl;

//         for (Internal::ProcessSizeType i = 0u; i < processSize; ++i) {
//             const auto index = i + phaseIndex;
//             const auto realOutGain = outGain * _envelopes.template getGain<OperatorIndex>(key, index);
//             auto freq = 2.0f * static_cast<float>(M_PI) * (static_cast<float>(index));
//             if constexpr (PitchEnv && IsCarrier)
//                 freq *= Audio::GetNoteFrequencyDelta(frequencyNorm, _pitchCache[i]) / static_cast<float>(_sampleRate);
//             else
//                 freq *= frequencyNorm / static_cast<float>(_sampleRate);

// // triangle
// // (1.0f - std::acos(std::cos(2.0f * static_cast<float>(i) * frequencyNorm)) * static_cast<float>(M_2_PI))
//             // if constexpr (Modulate) {
//             //     if constexpr (Accumulate)
//             //         output[i] += realOutGain * (1.0f - std::acos(std::cos(2.0f * static_cast<float>(i) * frequencyNorm + input[i])) * static_cast<float>(M_2_PI));
//             //     else
//             //         output[i] = realOutGain * std::sin(freq + static_cast<float>(M_PI) * input[i]);
//             // } else {
//             //     if constexpr (Accumulate)
//             //         output[i] += realOutGain * processGeneratio>(freq, op.feedback);
//             //     else
//             //         output[i] = realOutGain * processGeneratio>(freq, op.feedback);
//             // }


//             if constexpr (Modulate) {
//                 if constexpr (Accumulate)
//                     output[i] += realOutGain * std::sin(freq + static_cast<float>(M_PI) * input[i]);
//                 else
//                     output[i] = realOutGain * std::sin(freq + static_cast<float>(M_PI) * input[i]);
//             } else {
//                 if constexpr (Accumulate)
//                     output[i] += realOutGain * generateWaveform(freq, op.feedback);
//                 else
//                     output[i] = realOutGain * generateWaveform(freq, op.feedback);
//             }
//         }


//         return;
        _envelopes.template generateGains<false, OperatorIndex>(key, phaseIndex, _envelopeGain.data(), processSize);
        // std::fill(_envelopeGain.begin(), _envelopeGain.end(), 1.0f);
        if constexpr (Modulate) {
            DSP::Generator::Modulate<Accumulate>(
                op.waveform,
                output,
                _envelopeGain.data(),
                input,
                processSize,
                Audio::GetFrequencyNorm(frequencyNorm, _sampleRate),
                phaseIndex,
                outGain
            );
        } else {
            DSP::Generator::Generate<Accumulate>(
                op.waveform,
                output,
                _envelopeGain.data(),
                processSize,
                Audio::GetFrequencyNorm(frequencyNorm, _sampleRate),
                phaseIndex,
                outGain
            );
        }
    }

    /** @brief Get the envelope list */
    [[nodiscard]] EnvelopeList &envelopes(void) noexcept { return _envelopes; }
    [[nodiscard]] const EnvelopeList &envelopes(void) const noexcept { return _envelopes; }

    void envelopeResetKey(const Key key) noexcept
    {
        _pitchIndexes[key] = 0u;
        _envelopes.resetKey(key);
    }
    void envelopeSetTriggerIndex(const Key key, const std::uint32_t index) noexcept { _envelopes.setTriggerIndex(key, index); }
    void envelopeResetTriggerIndex(const Key key) noexcept { _envelopes.resetTriggerIndex(key); }
    void envelopeResetInternalGain(const Key key) noexcept { _envelopes.resetInternalGain(key); }

    void resetEnvelopePitch(void) noexcept { _pitchIndexes.fill(0u); }

    void setSampleRate(const SampleRate sampleRate) noexcept { _sampleRate = sampleRate; _envelopes.setSampleRate(sampleRate); }

    template<bool Accumulate>
    void process(
            float *output, const std::uint32_t processSize, const float outputGain,
            const std::uint32_t phaseIndex, const Key key, const float rootFrequency,
            const Internal::OperatorArray<OperatorCount> &operators,
            const Internal::PitchOperator &pitchOp = Internal::PitchOperator()
    ) noexcept
    {
        _cache.resize(static_cast<Internal::ProcessSizeType>(processSize));
        _envelopeGain.resize(static_cast<Internal::ProcessSizeType>(processSize));
        // Update envelopes specs
        updateEnvelopesSpecs<OperatorCount>(operators);

        if constexpr (PitchEnv)
            processPitchOperator(key, rootFrequency, processSize, pitchOp);

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
    Internal::CacheList _pitchCache;
    Internal::PitchIndexList _pitchIndexes;
    EnvelopeCache _envelopeGain;


    void processPitchOperator(const Key key, const float rootFrequency, const std::uint32_t processSize, const Internal::PitchOperator &pitchOp) noexcept
    {
        UNUSED(rootFrequency);
        constexpr float MaxSemitonePeak = 72.0f;

        _pitchCache.resize(static_cast<Internal::ProcessSizeType>(processSize));
        auto &pitchIdx = _pitchIndexes[key];
        for (Internal::ProcessSizeType i = 0u; i < processSize; ++i, pitchIdx++) {
            const float semitoneDelta = MaxSemitonePeak * pitchOp.volume * (_envelopes.template getGain<OperatorCount>(key, pitchIdx));
            // const float ret = Audio::GetNoteFrequencyDelta(rootFrequency, semitoneDelta);
            _pitchCache[i] = semitoneDelta;
        }

        // std::cout << "pitchCache:   " << _pitchCache[0] << std::endl;
        // std::cout << "pitchCache_:  " << _pitchCache[processSize / 3] << std::endl;
        // std::cout << "pitchCache__: " << _pitchCache[2 * processSize / 3] << std::endl;
        // std::cout << std::endl;

    }

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

        // Sub (A)
        processOperator<true, false, true, 0u>(nullptr, output, processSize, realOutputGain, phaseIndex, getDetuneFrequency(rootFrequency, operators[0u]), key, operators[0u]);
        // Impact (B+C->D)
        processOperator<false, false, false, 2u>(nullptr, _cache.data(), processSize, 1.0f, phaseIndex, getDetuneFrequency(rootFrequency, operators[2u]), key, operators[2u]);
        processOperator<true, false, false, 3u>(nullptr, _cache.data(), processSize, 1.0f, phaseIndex, getDetuneFrequency(rootFrequency, operators[3u]), key, operators[3u]);
        processOperator<true, true, true, 1u>(_cache.data(), output, processSize, realOutputGain, phaseIndex, getDetuneFrequency(rootFrequency, operators[1u]), key, operators[1u]);
    }


    [[nodiscard]] float generateWaveform(const float freq, const std::uint32_t feedbackAmount) const noexcept
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


    template<unsigned Count = OperatorCount>
    void updateEnvelopesSpecs(const Internal::OperatorArray<OperatorCount> &operators) noexcept
    {
        if constexpr (Count) {
            updateEnvelopesSpecs<Count - 1u>(operators[Count - 1u]);
            updateEnvelopesSpecs<Count - 1u>(operators);
        }
    }

    template<unsigned Index>
    void updateEnvelopesSpecs(const Internal::Operator &op) noexcept
    {
        _envelopes.template setSpecs<Index>(DSP::EnvelopeSpecs {
            0.0f,
            op.attack,
            1.0f,
            0.0f,
            op.decay,
            op.sustain,
            op.release
        });

    }


    [[nodiscard]] float getDetuneFrequency(const float rootFrequency, const Internal::Operator &op) const noexcept
    {
        if (!op.detune)
            return rootFrequency * op.frequencyDelta; // / static_cast<float>(_sampleRate);
        const double detuneNorm = static_cast<double>(op.detune) / 1200.0;
        return rootFrequency * static_cast<float>(std::pow(2.0, detuneNorm)) * op.frequencyDelta;// / static_cast<float>(_sampleRate);
    }

};

#include "FM.ipp"
