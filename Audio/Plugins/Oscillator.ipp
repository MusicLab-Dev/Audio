/**
 * @ Author: Pierre Veysseyre
 * @ Description: Oscillator implementation
 */

#include <Audio/DSP/Gain.hpp>

inline void Audio::Oscillator::onAudioGenerationStarted(const BeatRange &range)
{
    UNUSED(range);
    _noteManager.reset();
    _oscillator.reset();

    // _filter.setup(DSP::Biquad::Internal::Specs {
    //     DSP::Filter::AdvancedType::LowPass,
    //     static_cast<float>(audioSpecs().sampleRate),
    //     { static_cast<float>(filterCutoff()), 0.0f },
    //     1.0f,
    //     static_cast<float>(filterResonance()) * 0.707f
    // });
    // _filter.reset();
}

inline void Audio::Oscillator::onAudioParametersChanged(void)
{
    _noteManager.reset();
    _noteManager.envelope().setSampleRate(audioSpecs().sampleRate);
}

inline void Audio::Oscillator::setExternalPaths(const ExternalPaths &paths)
{
    UNUSED(paths);
}

inline void Audio::Oscillator::sendNotes(const NoteEvents &notes, const BeatRange &range)
{
    UNUSED(range);
    if (notes.size()) {
        // std::cout << "RANGE: " << range << std::endl;
        _noteManager.feedNotes(notes);
    }
}

inline void Audio::Oscillator::receiveAudio(BufferView output)
{
    const DB outGain = ConvertDecibelToRatio(DefaultVoiceGain);
    const auto outChannelSize = audioSpecs().processBlockSize;
    const auto channels = static_cast<std::size_t>(audioSpecs().channelArrangement);

    _noteManager.setEnvelopeSpecs(DSP::EnvelopeSpecs {
        0.0f,
        static_cast<float>(envelopeAttack()),
        1.0f,
        0.0f,
        static_cast<float>(envelopeDecay()),
        static_cast<float>(envelopeSustain()),
        static_cast<float>(envelopeRelease()),
    });
    _noteManager.envelopeGain().resize(outChannelSize);
    _noteManager.processNotesEnvelope(
        output,
        [this, outGain, outChannelSize](const Key key, const std::uint32_t readIndex, const NoteModifiers &modifiers, float *realOutput, const std::uint32_t realOutSize, const std::size_t channelCount) -> std::pair<std::uint32_t, std::uint32_t>
        {
            UNUSED(modifiers);
            const auto channelsMinusOne = channelCount - 1u;
            const auto outGainNorm = channelsMinusOne ? outGain / 2.0f : outGain / 4.0f;

            auto realOutLeft = realOutput;
            auto realOutRight = realOutput + (channelsMinusOne * outChannelSize);

            const auto freq = GetNoteFrequency(key);
            const auto freqNorm = GetFrequencyNorm(freq, audioSpecs().sampleRate);

            constexpr auto MaxDetuneSemitone = 0.5f;
            const float det = static_cast<float>(detune());
            const DB pan = channelsMinusOne ? static_cast<float>(panning()) : 0.0f;
            const DB gainL = pan <= 0.0f ? outGainNorm : (1.0f - pan) * outGainNorm;
            const DB gainR = pan >= 0.0f ? outGainNorm : (1.0f + pan) * outGainNorm;

            // Reset phase index -> move this shit
            // if (!readIndex)
            //     _oscillator.resetKey<0u>(key);
            // Sync voices when no detune (unison)
            if (!readIndex) {
                const auto rootPhase = _oscillator.phase<0u>(key);
                _oscillator.setPhase<1u>(key, rootPhase);
                _oscillator.setPhase<2u>(key, rootPhase);
            }
            const auto wave = static_cast<DSP::Generator::Waveform>(waveform());
            const auto env = _noteManager.envelopeGain().data();

            auto GenerateVoice = [&osc = _oscillator, env, wave, realOutSize, key, readIndex]
                    <unsigned VoiceIdx, bool IncrementIdx = true, bool Accumulate = true>
                    (float *out, const float freq, const float gain) -> void
            {
                osc.generate<Accumulate, VoiceIdx, IncrementIdx>(
                    wave,
                    out,
                    env,
                    realOutSize,
                    key,
                    freq,
                    readIndex,
                    gain
                );
            };

            GenerateVoice.operator()<0u, false>(realOutLeft, freqNorm, gainL);
            GenerateVoice.operator()<0u>(realOutRight, freqNorm, gainR);
            GenerateVoice.operator()<1u>(realOutLeft, GetNoteFrequencyDelta(freqNorm, -det * MaxDetuneSemitone), gainL);
            GenerateVoice.operator()<2u>(realOutRight, GetNoteFrequencyDelta(freqNorm, det * MaxDetuneSemitone), gainR);


            /** @brief Envelope filter */
            // const float cutoffRate = static_cast<float>((filterCutoff() - 50.0) / 22'000.0);
            // constexpr auto EnvMaxRate = 4.0f; // In octave
            // constexpr auto EnvRateNorm = std::pow(2.0f, EnvMaxRate) - 1.0f; // In octave
            // const auto cut = Utils::LogFrequency::GetLog(cutoffRate);
            // const auto env = _noteManager.envelopeGain().data();
            // const auto res = static_cast<float>(filterResonance() + 1.0f) * 0.707f;
            // const auto sr = static_cast<float>(audioSpecs().sampleRate);
            // const auto amount = static_cast<float>(filterEnv());

            // for (auto i = 0u; i < realOutSize; ++i) {
            //     _filter.setup(DSP::Biquad::Internal::Specs {
            //         DSP::Filter::AdvancedType::LowPass,
            //         sr,
            //         { (amount * env[i] + 1.0f) * cut, 0.0f },
            //         1.0f,
            //         res
            //     });
            //     realOutLeft[i] = _filter.processSample(realOutLeft[i], key, 1.0f);
            // }


            return std::make_pair(realOutSize, 0u);
        },
        [this] (const Key key) -> bool
        {
            return !_noteManager.envelope().lastGain(key);
        },
        [this] (const Key key) -> void
        {
            _filter.resetKey(key);
            _oscillator.resetKey(key);
        }
    );

    float *out = output.data<float>();
    const float gainFrom = ConvertDecibelToRatio(static_cast<float>(getControlPrev((0u))));
    const float gainTo = ConvertDecibelToRatio(static_cast<float>(outputVolume()));

    // _filter.filterBlock(output.data<float>(), output.size<float>(), output.data<float>(), 0u, 1.0f);
    DSP::Gain::ApplyStereo<float>(out, outChannelSize, channels, gainFrom, gainTo);

    // To benchmark, must be slower
    // Modifier<float>::ApplyIndexFunctor(outLeft, outChannelSize, 0u, [&, outGain](const std::size_t index) -> float {
    //     UNUSED(index);
    //     float sample {};
    //     const auto activeNote = _noteManager.getActiveNote();
    //     const auto activeNoteSize = activeNote.size();
    //     // std::cout << "Receive <audio>: " << activeNote.size() << std::endl;
    //     for (auto iKey = 0u; iKey < activeNoteSize; ++iKey) {
    //         const auto key = activeNote[iKey];
    //         const auto trigger = _noteManager.trigger(key);
    //         // Phase index
    //         const auto phaseIndex = _noteManager.readIndex(key);
    //         const float frequency = std::pow(2.f, static_cast<float>(static_cast<int>(key) - RootKey) / KeysPerOctave) * RootKeyFrequency;
    //         const float frequencyNorm = 2.f * static_cast<float>(M_PI) * frequency / static_cast<float>(audioSpecs().sampleRate);
    //         sample += std::sin(static_cast<float>(index) * frequencyNorm) *
    //         _noteManager.getEnvelopeGain(key, static_cast<std::uint32_t>(phaseIndex));
    //     }
    //     return sample * outGain;
    //         // std::sin(static_cast<float>(index) * frequencyNorm) *
    //         // _noteManager.getEnvelopeGain(key, static_cast<std::uint32_t>(index)) * gain;
    // });

}
