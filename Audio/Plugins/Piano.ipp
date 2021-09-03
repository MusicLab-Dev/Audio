/**
 * @ Author: Pierre Veysseyre
 * @ Description: Piano implementation
 */

#include <iomanip>

inline void Audio::Piano::onAudioGenerationStarted(const BeatRange &range)
{
    UNUSED(range);
    _noteManager.reset();
    _oscillators.reset();
    _filter.setup(DSP::Biquad::Internal::Specs {
        DSP::Filter::AdvancedType::LowPass,
        static_cast<float>(audioSpecs().sampleRate),
        { 8000.0f, 0.0f },
        1.0f,
        0.707f
    });
    _filterEnv.resetKeys();
    _cache.resize(GetFormatByteLength(audioSpecs().format) * audioSpecs().processBlockSize, audioSpecs().sampleRate, audioSpecs().channelArrangement, audioSpecs().format);
    _cache.clear();
}

inline void Audio::Piano::onAudioParametersChanged(void)
{
    _noteManager.reset();
    _noteManager.envelope().setSampleRate(audioSpecs().sampleRate);
    _filterEnv.setSampleRate(audioSpecs().sampleRate);
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
    const DB outGain = ConvertDecibelToRatio(static_cast<float>(outputVolume()) + DefaultVoiceGain);
    const auto outSize = static_cast<std::uint32_t>(output.size<float>());
    float *out = reinterpret_cast<float *>(output.byteData());

    _noteManager.envelopeGain().resize(outSize);
    _noteManager.processNotes(
        [this, outGain, outSize, out](const Key key, const std::uint32_t readIndex, const NoteModifiers &modifiers) -> std::pair<std::uint32_t, std::uint32_t> {
            auto realOut = out;
            auto realOutSize = outSize;
            if (modifiers.sampleOffset && !readIndex) {
                realOut += modifiers.sampleOffset;
                realOutSize -= modifiers.sampleOffset;
            }
            UNUSED(modifiers);
            const auto freq = GetNoteFrequency(key);
            const auto freqNorm = GetFrequencyNorm(freq, audioSpecs().sampleRate);


            static constexpr DSP::Generator::Waveform PianoTypeWaveforms[3u][4u] = {
                { DSP::Generator::Waveform::PulseThirdAnalog,   DSP::Generator::Waveform::SawAnalog,            DSP::Generator::Waveform::Triangle,     DSP::Generator::Waveform::PulseQuarterAnalog },
                { DSP::Generator::Waveform::SquareAnalog,       DSP::Generator::Waveform::PulseQuarterAnalog,   DSP::Generator::Waveform::SquareAnalog, DSP::Generator::Waveform::SawAnalog },
                { DSP::Generator::Waveform::SquareAnalog,       DSP::Generator::Waveform::PulseQuarterAnalog,   DSP::Generator::Waveform::SquareAnalog, DSP::Generator::Waveform::SawAnalog }
            };

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

            const auto keyDecay = GetKeyDecay(key);
            // std::cout << "Key: " << static_cast<int>(key) << " -> " << keyDecay << std::endl;

            // Generate envelope for osc
            constexpr auto DefaultAttack = 0.01f;
            constexpr auto DefaultDecay = 5.5f;
            constexpr auto DefaultSustain = 0.01f;
            // constexpr auto DefaultRelease = 1.5f;
            _noteManager.envelope().setSpecs<0u>(DSP::EnvelopeSpecs {
                0.0f,
                DefaultAttack, // A
                1.0f,
                0.0f,
                keyDecay / 1.0f,// D
                DefaultSustain,// S
                keyDecay / 4.0f,// R
            });
            _noteManager.generateEnvelopeGains(key, readIndex, realOutSize);
            // Generate osc
            const auto DetuneDelta = static_cast<float>(detune() / 10.0);
            const auto DetuneFreqUp = GetNoteFrequencyDelta(freqNorm * 2.0f, 7.0f + DetuneDelta);
            const auto DetuneFreqDown = GetNoteFrequencyDelta(freqNorm * 2.0f, 7.0f - DetuneDelta);
            const auto pianoType = static_cast<std::uint32_t>(type());
            _oscillators.generate<false, 0u>(
                PianoTypeWaveforms[0u][pianoType],
                _cache.data<float>(),
                _noteManager.envelopeGain().data(),
                realOutSize,
                key,
                freqNorm,
                readIndex,
                outGain
            );
            // Reset phase of detuned osc (sync)
            if (!readIndex) {
                _oscillators.resetKey<1u>(key);
                _oscillators.resetKey<2u>(key);
            }
            _oscillators.generate<true, 1u>(
                PianoTypeWaveforms[1u][pianoType],
                _cache.data<float>(),
                _noteManager.envelopeGain().data(),
                realOutSize,
                key,
                DetuneFreqUp,
                readIndex,
                outGain / 2.0f
            );
            _oscillators.generate<true, 2u>(
                PianoTypeWaveforms[2u][pianoType],
                _cache.data<float>(),
                _noteManager.envelopeGain().data(),
                realOutSize,
                key,
                DetuneFreqDown,
                readIndex,
                outGain / 2.0f
            );

            // Get the real cutoff frequency
            constexpr auto FilterCutOffMin = 400.0f;
            constexpr auto FilterCutOffMax = 1000.0f;
            constexpr auto FilterCutOffDelta = FilterCutOffMax - FilterCutOffMin;
            const auto filterCutOff = static_cast<float>(color() * FilterCutOffDelta + FilterCutOffMin);

            // Generate envelope for filter
            _filterEnv.setSpecs<0u>(DSP::EnvelopeSpecs {
                0.0f,
                DefaultDecay / 2.0f, // A
                1.0f,
                0.0f,
                DefaultDecay,// D
                0.0f,// S
                1.0f,// R
            });
            // _filterEnv.generateEnvelopeGains(key, readIndex, realOutSize);
            float envF = 0.0f;
            constexpr auto EnvAmountBrightScale = 15.0f;
            const auto envAmount = static_cast<float>((1.0 - brightness()) * EnvAmountBrightScale);
            for (auto i = 0u; i < realOutSize; ++i) {
                envF = _filterEnv.getGain(key, readIndex + i);
                float cutOff = 1.0f / (envF * envAmount + 1.0f) * filterCutOff;

                if (const auto max = static_cast<float>(audioSpecs().sampleRate) / 2.0f; cutOff > max)
                    cutOff = max;
                _filter.setup(DSP::Biquad::Internal::Specs {
                    DSP::Filter::AdvancedType::LowPass,
                    static_cast<float>(audioSpecs().sampleRate),
                    { cutOff, 0.0f },
                    1.0f,
                    0.707f
                });
                realOut[i] += _filter.processSample(_cache.data<float>()[i], key, 1.0f);
            }
            // Reset filter envelope cache if last gain is zero
            if (!envF) {
                _filterEnv.resetKey(key);
            }


            return std::make_pair(realOutSize, 0u);
        }
    );


}
