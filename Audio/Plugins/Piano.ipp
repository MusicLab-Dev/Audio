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

            // Generate envelope for osc
            constexpr auto DefaultAttack = 0.01f;
            constexpr auto DefaultDecay = 5.5f;
            constexpr auto DefaultSustain = 0.0f;
            constexpr auto DefaultRelease = 1.5f;
            _noteManager.envelope().setSpecs<0u>(DSP::EnvelopeSpecs {
                0.0f,
                DefaultAttack, // A
                1.0f,
                0.0f,
                DefaultDecay,// D
                DefaultSustain,// S
                DefaultRelease,// R
            });
            _noteManager.generateEnvelopeGains(key, readIndex, realOutSize);
            // Generate osc
            const auto DetuneDelta = static_cast<float>(detuneAmount());
            const auto DetuneFreqUp = GetNoteFrequencyDelta(freqNorm * 2.0f, 7.0f + DetuneDelta);
            const auto DetuneFreqDown = GetNoteFrequencyDelta(freqNorm * 4.0f, 7.0f - DetuneDelta);
            _oscillators.generate<false, 0u>(
                DSP::Generator::Waveform::Saw,
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
                DSP::Generator::Waveform::Saw,
                _cache.data<float>(),
                _noteManager.envelopeGain().data(),
                realOutSize,
                key,
                DetuneFreqUp,
                readIndex,
                outGain / 2.0f
            );
            _oscillators.generate<true, 2u>(
                DSP::Generator::Waveform::Saw,
                _cache.data<float>(),
                _noteManager.envelopeGain().data(),
                realOutSize,
                key,
                freqNorm * 4.0f,
                readIndex,
                outGain / 2.0f
            );

            // Get the real frequency according to: keyFollow, filterCutoff
            const Key CenteredKey = 60u; // C4 (middle C)
            const float CenteredKeyFreq = GetNoteFrequency(CenteredKey);
            const float keyFreqDelta = GetNoteFrequency(key) / CenteredKeyFreq;
            const auto keyFollowMultiplier = std::abs(static_cast<float>(filterKeyFollow()));
            float freqRate { 1.0f };
            if (keyFreqDelta >= 1.0f) {
                freqRate = (keyFreqDelta - 1.0f) * keyFollowMultiplier + 1.0f;
            } else {
                freqRate = 1.0f / ((1.0f / keyFreqDelta - 1.0f) * keyFollowMultiplier + 1.0f);
            }
            if (filterKeyFollow() < 0.0f)
                freqRate = 1.0f / freqRate;
            freqRate *= static_cast<float>(filterCutoff());

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
            for (auto i = 0u; i < realOutSize; ++i) {
                envF = _filterEnv.getGain(key, readIndex + i);
                float cutOff = 1.0f / (envF * static_cast<float>(filterEnvAmount()) + 1.0f) * freqRate;
                // float cutOff = static_cast<float>(filterCutoff());
                // float cutOff = freqRate;

                if (const auto max = static_cast<float>(audioSpecs().sampleRate) / 2.0f; cutOff > max)
                    cutOff = max;
                _filter.setup(DSP::Biquad::Internal::Specs {
                    DSP::Filter::AdvancedType::LowPass,
                    static_cast<float>(audioSpecs().sampleRate),
                    { cutOff, 0.0f },
                    1.0f,
                    static_cast<float>(filterResonance() * 9.0 + 0.707)
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
