/**
 * @ Author: Pierre Veysseyre
 * @ Description: Piano implementation
 */

#include <Audio/DSP/Gain.hpp>

inline void Audio::Piano::onAudioGenerationStarted(const BeatRange &range)
{
    UNUSED(range);
    _noteManager.reset();
    _oscillators.reset();
    _filter[0u].setup(DSP::Biquad::Internal::Specs {
        DSP::Filter::AdvancedType::LowPass,
        static_cast<float>(audioSpecs().sampleRate),
        { 8000.0f, 0.0f },
        1.0f,
        0.707f
    });
    _filter[1u].setup(DSP::Biquad::Internal::Specs {
        DSP::Filter::AdvancedType::LowPass,
        static_cast<float>(audioSpecs().sampleRate),
        { 8000.0f, 0.0f },
        1.0f,
        0.707f
    });
    _filterEnv[0u].resetKeys();
    _filterEnv[1u].resetKeys();
    _cache.resize(GetFormatByteLength(audioSpecs().format) * audioSpecs().processBlockSize, audioSpecs().sampleRate, audioSpecs().channelArrangement, audioSpecs().format);
    _cache.clear();
}

inline void Audio::Piano::onAudioParametersChanged(void)
{
    _noteManager.reset();
    _noteManager.envelope().setSampleRate(audioSpecs().sampleRate);
    _filterEnv[0u].setSampleRate(audioSpecs().sampleRate);
    _filterEnv[1u].setSampleRate(audioSpecs().sampleRate);
}

inline void Audio::Piano::setExternalPaths(const ExternalPaths &paths)
{
    UNUSED(paths);
}

inline void Audio::Piano::sendNotes(const NoteEvents &notes, const BeatRange &range)
{
    UNUSED(range);
    if (notes.size()) {
        _noteManager.feedNotes(notes);
    }
}

inline void Audio::Piano::receiveAudio(BufferView output)
{
    const DB outGain = ConvertDecibelToRatio(DefaultVoiceGain);
    const auto channels = output.channelArrangement();

    const auto outSize = static_cast<std::uint32_t>(output.channelSampleCount());
    _noteManager.envelopeGain().resize(outSize);
    // _noteManager.setEnvelopeSpecs(DSP::EnvelopeSpecs {
    //     0.0f,
    //     0.005f, // A
    //     1.0f,
    //     0.0f,
    //     1.5f,// D
    //     0.0f,// S
    //     1.5f,// R
    // });

    _noteManager.processNotes(
        output,
        [this, outGain, channels](const Key key, const std::uint32_t readIndex, const NoteModifiers &modifiers, float *realOutput, const std::uint32_t realOutSize, const std::size_t channelCount) -> std::pair<std::uint32_t, std::uint32_t>
        {
            UNUSED(modifiers);
            const auto channelsMinusOne = channelCount - 1u;
            auto outGainNorm = channelsMinusOne ? outGain / 2.0f : outGain / 4.0f;
            if (static_cast<std::uint32_t>(type()) == 2u)
                outGainNorm *= 1.5f;

            auto cacheLeft = _cache.data<float>();
            auto cacheRight = _cache.data<float>() + channelsMinusOne;
            const auto freq = GetNoteFrequency(key);
            const auto freqNorm = GetFrequencyNorm(freq, audioSpecs().sampleRate);


            // Decay for each key: higher == shorter
            static constexpr Key DecayDurationRootKey = 48u;    // C3
            static constexpr float DefaultDecayDuration = 9.0f; // duration at DecayDurationRootKey
            static constexpr float DecayDurationDelta = 1.5f;   // duration delta per octave up
            constexpr auto GetKeyDecay = [](const Key key) -> float
            {
                const auto keyDelta = static_cast<float>(static_cast<int>(DecayDurationRootKey) - static_cast<int>(key));
                const auto freqDelta = keyDelta / static_cast<float>(KeysPerOctave);
                // std::cout << "  -dt: " << keyDelta << " - " << freqDelta << std::endl;
                return freqDelta * DecayDurationDelta + DefaultDecayDuration;
            };

            static constexpr auto DurationRateMin = 0.1f;
            static constexpr auto DurationRateMax = 4.0f;
            static constexpr auto DurationRate = DurationRateMax - DurationRateMin;
            const auto durationRate = (static_cast<float>(duration()) * DurationRate + DurationRateMin);
            const auto keyDecay = GetKeyDecay(key) * durationRate;

            // Get real attack
            static constexpr auto AttackMin = 0.01f;
            static constexpr auto AttackMax = 0.05f;
            static constexpr auto AttackDt = AttackMax - AttackMin;

            const auto realAttack = (1.0f - static_cast<float>(impact())) * AttackDt + AttackMin;
            // std::cout << "Key: " << static_cast<int>(key) << " -> " << keyDecay << std::endl;

            // Generate envelope for osc
            constexpr auto DefaultDecay = 5.5f;
            constexpr auto DefaultSustain = 0.0f;
            // constexpr auto DefaultRelease = 1.5f;
            _noteManager.envelope().setSpecs<0u>(DSP::EnvelopeSpecs {
                0.0f,
                realAttack, // A
                1.0f,
                0.0f,
                keyDecay / 1.0f,// D
                DefaultSustain,// S
                keyDecay / 4.0f,// R
            });

            // Sync voices when no detune (unison)
            const auto det = static_cast<float>(detune() / 10.0f);
            const auto detFreqUp = GetNoteFrequencyDelta(freqNorm * 3.0f, det);
            const auto detFreqDown = GetNoteFrequencyDelta(freqNorm * 3.0f, -det);
            if (det == 0.0f && !readIndex) {
                const auto rootPhase = _oscillators.phase<0u>(key);
                _oscillators.setPhase<1u>(key, rootPhase);
                _oscillators.setPhase<2u>(key, rootPhase);
            }
            _noteManager.generateEnvelopeGains(key, readIndex, realOutSize);

            // Generate osc
            const auto pianoType = static_cast<std::uint32_t>(type());

            _cache.clear();
            _oscillators.generate<true, 0u, false>(
                PianoTypeWaveforms[0u][pianoType],
                channels,
                cacheLeft,
                _noteManager.envelopeGain().data(),
                realOutSize,
                key,
                freqNorm,
                readIndex,
                outGainNorm
            );
            _oscillators.generate<true, 0u>(
                PianoTypeWaveforms[0u][pianoType],
                channels,
                cacheRight,
                _noteManager.envelopeGain().data(),
                realOutSize,
                key,
                freqNorm,
                readIndex,
                outGainNorm
            );
            _oscillators.generate<true, 1u>(
                PianoTypeWaveforms[1u][pianoType],
                channels,
                cacheLeft,
                _noteManager.envelopeGain().data(),
                realOutSize,
                key,
                detFreqUp,
                readIndex,
                outGainNorm / 2.0f
            );
            _oscillators.generate<true, 2u>(
                PianoTypeWaveforms[2u][pianoType],
                channels,
                cacheRight,
                _noteManager.envelopeGain().data(),
                realOutSize,
                key,
                detFreqDown,
                readIndex,
                outGainNorm / 2.0f
            );

            // Get the real cutoff frequency
            constexpr auto FilterCutOffMin = 400.0f;
            constexpr auto FilterCutOffMax = 1000.0f;
            constexpr auto FilterCutOffDelta = FilterCutOffMax - FilterCutOffMin;
            const auto filterCutOff = static_cast<float>(color() * FilterCutOffDelta + FilterCutOffMin);

            // Filtering the osc output
            float envF = 0.0f;
            constexpr auto EnvAmountBrightScale = 15.0f;
            const auto envAmount = static_cast<float>((1.0 - brightness()) * EnvAmountBrightScale);
            for (auto ch = 0ul; ch < channelCount; ++ch) {
                // Generate envelope for filter
                _filterEnv[ch].setSpecs<0u>(DSP::EnvelopeSpecs {
                    0.0f,
                    DefaultDecay / 2.0f, // A
                    1.0f,
                    0.0f,
                    DefaultDecay,// D
                    0.0f,// S
                    1.0f,// R
                });

                auto k = 0u;
                for (auto i = 0u; i < realOutSize; ++i, k += static_cast<std::uint32_t>(channelCount)) {
                    envF = _filterEnv[ch].getGain(key, readIndex + i);
                    float cutOff = 1.0f / (envF * envAmount + 1.0f) * filterCutOff;

                    if (const auto max = static_cast<float>(audioSpecs().sampleRate) / 2.0f; cutOff > max)
                        cutOff = max;
                    _filter[ch].setup(DSP::Biquad::Internal::Specs {
                        DSP::Filter::AdvancedType::LowPass,
                        static_cast<float>(audioSpecs().sampleRate),
                        { cutOff, 0.0f },
                        1.0f,
                        0.707f
                    });
                    realOutput[k] += _filter[ch].processSample(cacheLeft[k], key, 1.0f);
                }
                // Reset filter envelope cache if last gain is zero
                // if (!envF) {
                //     _filterEnv[ch].resetKey(key);
                // }
            }
            return std::make_pair(realOutSize, 0u);
        },
        [this] (const Key key) -> bool
        {
            return !_noteManager.envelope().lastGain(key);
        },
        [this] (const Key key) -> void
        {
            _oscillators.resetKey(key);
            _filter[0u].resetKey(key);
            _filter[1u].resetKey(key);
            _filterEnv[0u].resetKey(key);
            _filterEnv[1u].resetKey(key);
        }
    );

    const float gainFrom = ConvertDecibelToRatio(static_cast<float>(getControlPrev((0u))));
    const float gainTo = ConvertDecibelToRatio(static_cast<float>(outputVolume()));

    DSP::Gain::Apply<float>(output.data<float>(), output.size<float>(), gainFrom, gainTo);
}
