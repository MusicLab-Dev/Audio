/**
 * @file FM.ipp
 * @brief FM implementation
 *
 * @author Pierre V
 * @date 2021-06-10
 */

template<unsigned OperatorCount, Audio::DSP::FM::AlgorithmType Algo, bool PitchEnv>
template<bool Accumulate>
inline void Audio::DSP::FM::Schema<OperatorCount, Algo, PitchEnv>::process(
        float *output, const std::uint32_t processSize, const float outputGain,
        const std::uint32_t phaseIndex, const Key key, const float rootFrequency,
        const Internal::OperatorArray<OperatorCount> &operators,
        const Internal::PitchOperator &pitchOp
) noexcept
{
    const auto freqNorm = Audio::GetFrequencyNorm(rootFrequency, _sampleRate);

    _random.setLast(0u);
    if (_cache.capacity() != processSize)
        _cache.resize(processSize);
    std::fill_n(_cache.begin(), processSize, 0.0f);
    if (_envelopeGain.capacity() != processSize)
        _envelopeGain.resize(processSize);
    // Update envelopes specs
    updateEnvelopesSpecs<OperatorCount>(operators);

    if constexpr (PitchEnv)
        processPitchOperator(key, processSize, pitchOp);

    if constexpr (Algo == AlgorithmType::Default){
        if constexpr (OperatorCount == 2u) {
            oneCarrierOneModulator<Accumulate>(output, processSize, outputGain, phaseIndex, key, freqNorm, operators);
        } else if constexpr (OperatorCount == 4u) {
            twoCM<Accumulate>(output, processSize, outputGain, phaseIndex, key, freqNorm, operators);
        } else if constexpr (OperatorCount == 6u) {
            dx7_05<Accumulate>(output, processSize, outputGain, phaseIndex, key, freqNorm, operators);
        }
    } else
        processImpl<Accumulate>(output, processSize, outputGain, phaseIndex, key, freqNorm, operators);
}

template<unsigned OperatorCount, Audio::DSP::FM::AlgorithmType Algo, bool PitchEnv>
template<bool Accumulate>
inline void Audio::DSP::FM::Schema<OperatorCount, Algo, PitchEnv>::processImpl(
        float *output, const std::uint32_t processSize, const float outputGain,
        const std::uint32_t phaseIndex, const Key key, const float freqNorm,
        const Internal::OperatorArray<OperatorCount> &operators
) noexcept
{
    if constexpr (Algo == AlgorithmType::Drum) {
        static_assert(OperatorCount == 4u, "Audio::DSP::FM::Schema<OperatorCount, Drum>::processImpl: OperatorCount must be equal to 4");
        drum_impl<Accumulate>(output, processSize, outputGain, phaseIndex, key, freqNorm, operators);
    } else if constexpr (Algo == AlgorithmType::Hat) {
        static_assert(OperatorCount == Internal::HelperSpecs[static_cast<unsigned>(Algo)], "Audio::DSP::FM::Schema<OperatorCount, Hat>::processImpl: OperatorCount must be equal to 2");
        hat_impl<Accumulate>(output, processSize, outputGain, phaseIndex, key, freqNorm, operators);
    } else if constexpr (Algo == AlgorithmType::Piano) {
    }
}

template<unsigned OperatorCount, Audio::DSP::FM::AlgorithmType Algo, bool PitchEnv>
inline bool Audio::DSP::FM::Schema<OperatorCount, Algo, PitchEnv>::isKeyEnded(const Key key) const noexcept
{
    if constexpr (Algo == AlgorithmType::Drum) {
        // 0: sub, 1: transient 3: noise
        return isKeyEnded<0u>(key) && isKeyEnded<1u>(key) && isKeyEnded<3u>(key);
    } else if constexpr (Algo == AlgorithmType::Hat) {
        return isKeyEnded<0u>(key);
    }
}

template<unsigned OperatorCount, Audio::DSP::FM::AlgorithmType Algo, bool PitchEnv>
template<unsigned Index>
inline bool Audio::DSP::FM::Schema<OperatorCount, Algo, PitchEnv>::isKeyEnded(const Key key) const noexcept
{
    return _envelopes.template isGainEnded<Index>(key);
}

template<unsigned OperatorCount, Audio::DSP::FM::AlgorithmType Algo, bool PitchEnv>
template<bool Accumulate, bool Modulate, bool IsCarrier, unsigned OperatorIndex>
inline void Audio::DSP::FM::Schema<OperatorCount, Algo, PitchEnv>::processOperator(
        const float *input, float *output, const std::uint32_t processSize, const float outputGain,
        const std::uint32_t phaseIndex, const float frequencyNorm, const Key key,
        const Internal::Operator &op
) noexcept
{
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
    const auto keyDelta = static_cast<float>(key - op.keyBreakPoint) / 12.0f;
    const auto keyAmountDirection = keyDelta >= 0;
    auto keyAmount = keyAmountDirection ? GetKeyAmountRate(op.keyAmountRight, keyDelta) : GetKeyAmountRate(op.keyAmountLeft, -keyDelta);

    // if constexpr (IsCarrier)
    //     keyAmount = 1.0f;

    const DB outGain = op.volume * outputGain * keyAmount;

    _envelopeGain.clear();
    _envelopes.template generateGains<false, OperatorIndex>(key, phaseIndex, _envelopeGain.data(), processSize);
    if constexpr (PitchEnv) {
        if constexpr (Modulate) {
            _oscillator.template modulateSemitoneShift<Accumulate, OperatorIndex>(
                op.waveform,
                output,
                _envelopeGain.data(),
                input,
                _pitchCache.data(),
                processSize,
                key,
                frequencyNorm,
                phaseIndex,
                outGain
            );
        } else {
            _oscillator.template semitoneShift<Accumulate, OperatorIndex>(
                op.waveform,
                output,
                _envelopeGain.data(),
                _pitchCache.data(),
                processSize,
                key,
                frequencyNorm,
                phaseIndex,
                outGain
            );
        }
    } else {
        if constexpr (Modulate) {
            _oscillator.template modulate<Accumulate, OperatorIndex>(
                op.waveform,
                output,
                _envelopeGain.data(),
                input,
                processSize,
                key,
                frequencyNorm,
                phaseIndex,
                outGain
            );
        } else {
            _oscillator.template generate<Accumulate, OperatorIndex>(
                op.waveform,
                output,
                _envelopeGain.data(),
                processSize,
                key,
                frequencyNorm,
                phaseIndex,
                outGain
            );
        }
    }
}

template<unsigned OperatorCount, Audio::DSP::FM::AlgorithmType Algo, bool PitchEnv>
inline void Audio::DSP::FM::Schema<OperatorCount, Algo, PitchEnv>::processPitchOperator(
        const Key key, const std::uint32_t processSize, const Internal::PitchOperator &pitchOp
) noexcept
{
    constexpr float MaxSemitonePeak = 12.0f * 6.0f; // 6 octaves

    if (_pitchCache.capacity() != processSize)
        _pitchCache.resize(processSize);
    auto &pitchIdx = _pitchIndexes[key];

    _envelopes.template setSpecs<OperatorCount>(EnvelopeSpecs {
        0.0f,
        pitchOp.attack,
        pitchOp.peak,
        0.0f,
        pitchOp.decay,
        pitchOp.sustain,
        pitchOp.release,
    });
    for (auto i = 0u; i < processSize; ++i, pitchIdx++) {
        const float semitoneDelta = MaxSemitonePeak * pitchOp.volume * (_envelopes.template getGain<OperatorCount>(key, pitchIdx));

        _pitchCache[i] = semitoneDelta;
    }
}
