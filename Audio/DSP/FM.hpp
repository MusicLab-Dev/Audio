/**
 * @file FM.hpp
 * @brief
 *
 * @author Pierre V
 * @date 2021-06-01
 */

#pragma once

#include <Core/FlatVector.hpp>

#include <Audio/Math.hpp>
#include "EnvelopeGenerator.hpp"
#include "Oscillator.hpp"

namespace Audio::DSP::FM
{
    namespace Internal
    {
        using CacheList = Core::TinyFlatVector<float>;
        using PitchIndexArray = std::array<std::uint32_t, KeyCount>;

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
            float attack { 0.0f };
            float peak { 0.0f };
            float decay { 0.0f };
            float sustain { 0.0f };
            float release { 0.0f };
            float volume { 0.0f };
        };

        template<unsigned InstanceCount>
        using OperatorArray = std::array<Internal::Operator, InstanceCount>;

        static constexpr unsigned HelperSpecs[] = {
            1u,
            4u,
            4u,
            6u
        };
    }

    enum class AlgorithmType : std::uint8_t {
        Default = 0u,
        Piano,
        Drum,
        Hat
    };

    template<unsigned OperatorCount, AlgorithmType Algo, bool PitchEnv>
    class Schema;

    template<AlgorithmType Algo, bool PitchEnv>
    using SchemaHelper = Schema<Internal::HelperSpecs[static_cast<unsigned>(Algo)], Algo, PitchEnv>;
}

template<unsigned OperatorCount, Audio::DSP::FM::AlgorithmType Algo, bool PitchEnv = false>
class Audio::DSP::FM::Schema
{
public:
    using EnvelopeList = EnvelopeDefaultExp<EnvelopeType::ADSR, OperatorCount + PitchEnv>;
    // using EnvelopeList = EnvelopeDefaultLinear<EnvelopeType::ADSR, OperatorCount + PitchEnv>;
    using EnvelopeCache = Core::TinyVector<float>;

    void reset(void) noexcept
    {
        resetEnvelopePitch();
        _envelopes.resetKeys();
    }

    void resetKey(const Key key) noexcept
    {
        _pitchIndexes[key] = 0u;
        _envelopes.resetKey(key);
        _oscillator.resetKey(key);
    }

    void resetEnvelopePitch(void) noexcept { _pitchIndexes.fill(0u); }

    void setSampleRate(const SampleRate sampleRate) noexcept { _sampleRate = sampleRate; _envelopes.setSampleRate(sampleRate); }

    /** @brief Get the envelope list */
    [[nodiscard]] EnvelopeList &envelopes(void) noexcept { return _envelopes; }
    [[nodiscard]] const EnvelopeList &envelopes(void) const noexcept { return _envelopes; }

    void envelopeSetTriggerIndex(const Key key, const std::uint32_t index) noexcept { _envelopes.setTriggerIndex(key, index); }
    void envelopeResetTriggerIndex(const Key key) noexcept { _envelopes.resetTriggerIndex(key); }
    void envelopeResetInternalGain(const Key key) noexcept { _envelopes.resetInternalGain(key); }

    [[nodiscard]] bool isKeyEnded(const Key key) const noexcept;

    template<bool Accumulate>
    void process(
            float *output, const std::uint32_t processSize, const float outputGain,
            const std::uint32_t phaseIndex, const Key key, const float rootFrequency,
            const Internal::OperatorArray<OperatorCount> &operators,
            const Internal::PitchOperator &pitchOp = Internal::PitchOperator(),
            const ChannelArrangement channels = ChannelArrangement::Mono
    ) noexcept;

    [[nodiscard]] DSP::Oscillator<OperatorCount> &osc(void) noexcept { return _oscillator; }
private:
    // Internal operators envelope
    EnvelopeList _envelopes;
    // Internal output cache
    Internal::CacheList _cache;
    // Internal envelope output cache
    EnvelopeCache _envelopeGain;
    // Internal sample rate
    SampleRate _sampleRate;
    // Pitch envelope output cache
    Internal::CacheList _pitchCache;
    // Pitch envelope read indexes cache
    Internal::PitchIndexArray _pitchIndexes;
    // Internal random generator for transients
    Utils::RandomGenerator _random;
    // Internal Oscillator
    DSP::Oscillator<OperatorCount> _oscillator;

    template<unsigned Index>
    [[nodiscard]] bool isKeyEnded(const Key key) const noexcept;

    template<bool Accumulate>
    void processImpl(
            float *output, const std::uint32_t processSize, const float outputGain,
            const std::uint32_t phaseIndex, const Key key, const float freqNorm,
            const Internal::OperatorArray<OperatorCount> &operators,
            const ChannelArrangement channels
    ) noexcept;

    template<bool Accumulate, bool Modulate, bool IsCarrier, unsigned OperatorIndex>
    inline void processOperator(
            const float *input, float *output, const std::uint32_t processSize, const float outputGain,
            const std::uint32_t phaseIndex, const float frequencyNorm, const Key key,
            const Internal::Operator &op, const ChannelArrangement channels
    ) noexcept;

    void processPitchOperator(
            const Key key, const std::uint32_t processSize, const Internal::PitchOperator &pitchOp
    ) noexcept;

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
        if constexpr (Count != 0u) {
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

/** @brief FM algorithms implementation */
private:
    template<bool Accumulate>
    void oneCarrierOneModulator(
            float *output, const std::uint32_t processSize, const float outputGain,
            const std::uint32_t phaseIndex, const Key key, const float rootFrequency,
            const Internal::OperatorArray<OperatorCount> &operators,
            const ChannelArrangement channels
    ) noexcept
    {
        // Op_1 modulate Op_0
        // processOperator<Accumulate, false, 1>(nullptr, _cache.data(), processSize, 1.0f, phaseIndex, (rootFrequency * operators[1].frequencyDelta) / static_cast<float>(_sampleRate), key, operators[1], channels);
        // processOperator<Accumulate, true, 0>(_cache.data(), output, processSize, outputGain, phaseIndex, (rootFrequency * operators[0].frequencyDelta) / static_cast<float>(_sampleRate), key, operators[0], channels);

        // Op_0 only
        processOperator<Accumulate, false, true, 0>(nullptr, output, processSize, outputGain, phaseIndex, (rootFrequency * operators[0].frequencyDelta) / static_cast<float>(_sampleRate), key, operators[0], channels);
    }

    template<bool Accumulate>
    void twoCM(
            float *output, const std::uint32_t processSize, const float outputGain,
            const std::uint32_t phaseIndex, const Key key, const float rootFrequency,
            const Internal::OperatorArray<OperatorCount> &operators,
            const ChannelArrangement channels
    ) noexcept
    {
        oneModulatorToCarrier_impl<Accumulate>(output, processSize, outputGain, phaseIndex, rootFrequency, key, operators, channels);
    }


    template<bool Accumulate>
    void dx7_05(
            float *output, const std::uint32_t processSize, const float outputGain,
            const std::uint32_t phaseIndex, const Key key, const float rootFrequency,
            const Internal::OperatorArray<OperatorCount> &operators,
            const ChannelArrangement channels
    ) noexcept
    {
        constexpr auto CarrierCount = 3u;
        const auto realOutputGain = outputGain / static_cast<float>(CarrierCount);

        oneModulatorToCarrier_impl<Accumulate>(output, processSize, realOutputGain, phaseIndex, rootFrequency, key, operators, channels);
    }

    template<bool Accumulate, unsigned Index = 0u>
    void oneModulatorToCarrier_impl(
            float *output, const std::uint32_t processSize, const float outputGain,
            const std::uint32_t phaseIndex, const float rootFrequency, const Key key,
            const Internal::OperatorArray<OperatorCount> &operators,
            const ChannelArrangement channels
    ) noexcept
    {
        if constexpr (Index < OperatorCount) {
            constexpr auto CarrierIdx = Index;
            constexpr auto ModIdx = Index + 1;

            processOperator<false, false, false, ModIdx>(nullptr, _cache.data(), processSize, 1.0f, phaseIndex, getDetuneFrequency(rootFrequency, operators[ModIdx]), key, operators[ModIdx], channels);
            processOperator<Accumulate, true, true, CarrierIdx>(_cache.data(), output, processSize, outputGain, phaseIndex, getDetuneFrequency(rootFrequency, operators[CarrierIdx]), key, operators[CarrierIdx], channels);
            oneModulatorToCarrier_impl<Accumulate, Index + 2>(output, processSize, outputGain, phaseIndex, rootFrequency, key, operators, channels);
        }
    }

    template<bool Accumulate>
    void drum_impl(
            float *output, const std::uint32_t processSize, const float outputGain,
            const std::uint32_t phaseIndex, const Key key, const float rootFrequency,
            const Internal::OperatorArray<OperatorCount> &operators,
            const ChannelArrangement channels
    ) noexcept
    {
        const auto realOutputGain = outputGain / static_cast<float>(2);

        // // Sub (A)
        // processOperator<true, false, true, 0u>(nullptr, output, processSize, realOutputGain, phaseIndex, getDetuneFrequency(rootFrequency, operators[0u]), key, operators[0u], channels);
        // // Impact (B+C->D)
        // processOperator<false, false, false, 2u>(nullptr, _cache.data(), processSize, 1.0f, phaseIndex, getDetuneFrequency(rootFrequency, operators[2u]), key, operators[2u], channels);
        // processOperator<true, false, false, 3u>(nullptr, _cache.data(), processSize, 1.0f, phaseIndex, getDetuneFrequency(rootFrequency, operators[3u]), key, operators[3u], channels);
        // processOperator<true, true, true, 1u>(_cache.data(), output, processSize, realOutputGain, phaseIndex, getDetuneFrequency(rootFrequency, operators[1u]), key, operators[1u], channels);

        // Noise (D) for clicking sound
        processOperator<Accumulate, false, false, 3u>(nullptr, output, processSize, realOutputGain, phaseIndex, getDetuneFrequency(rootFrequency, operators[3u]), key, operators[3u], channels);
        // Sub (A)
        processOperator<true, false, true, 0u>(nullptr, output, processSize, realOutputGain, phaseIndex, getDetuneFrequency(rootFrequency, operators[0u]), key, operators[0u], channels);
        // Color (C->B)
        processOperator<false, false, false, 2u>(nullptr, _cache.data(), processSize, 1.0f, phaseIndex, getDetuneFrequency(rootFrequency, operators[2u]), key, operators[2u], channels);
        processOperator<true, true, true, 1u>(_cache.data(), output, processSize, realOutputGain, phaseIndex, getDetuneFrequency(rootFrequency, operators[1u]), key, operators[1u], channels);
    }

    template<bool Accumulate>
    void hat_impl(
            float *output, const std::uint32_t processSize, const float outputGain,
            const std::uint32_t phaseIndex, const Key key, const float rootFrequency,
            const Internal::OperatorArray<OperatorCount> &operators,
            const ChannelArrangement channels
    ) noexcept
    {
        const auto defOutGain = outputGain / 2.0f;
        const auto oscOutGain = defOutGain / static_cast<float>(Internal::HelperSpecs[static_cast<unsigned>(Algo)] - 1u);

        processOperator<true, false, true, 0u>(nullptr, output, processSize, defOutGain, phaseIndex, getDetuneFrequency(rootFrequency, operators[0u]), key, operators[0u], channels);
        processOperator<true, false, true, 1u>(nullptr, output, processSize, oscOutGain, phaseIndex, getDetuneFrequency(rootFrequency, operators[1u]), key, operators[1u], channels);
        processOperator<true, false, true, 2u>(nullptr, output, processSize, oscOutGain, phaseIndex, getDetuneFrequency(rootFrequency, operators[2u]), key, operators[2u], channels);
        processOperator<true, false, true, 3u>(nullptr, output, processSize, oscOutGain, phaseIndex, getDetuneFrequency(rootFrequency, operators[3u]), key, operators[3u], channels);
        processOperator<true, false, true, 4u>(nullptr, output, processSize, oscOutGain, phaseIndex, getDetuneFrequency(rootFrequency, operators[4u]), key, operators[4u], channels);
        processOperator<true, false, true, 5u>(nullptr, output, processSize, oscOutGain, phaseIndex, getDetuneFrequency(rootFrequency, operators[5u]), key, operators[5u], channels);
    }};

#include "FM.ipp"
