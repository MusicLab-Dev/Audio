/**
 * @ Author: Pierre Veysseyre
 * @ Description: Hat implementation
 */

#include <iomanip>

inline void Audio::Hat::onAudioGenerationStarted(const BeatRange &range)
{
    UNUSED(range);
    _noteManager.reset();
    _fm.reset();

    _cache.clear();

    _bandPassFilter.setup(DSP::Biquad::Internal::Specs {
        DSP::Filter::AdvancedType::BandPass,
        static_cast<float>(audioSpecs().sampleRate),
        { 8'000.0f, 8'000.0f },
        1.0f,
        0.707f
    });
    _bandPassFilter.reset();
    _highPassFilter.setup(DSP::Biquad::Internal::Specs {
        DSP::Filter::AdvancedType::HighPass,
        static_cast<float>(audioSpecs().sampleRate),
        { 7'000.0f, 7'000.0f },
        1.0f,
        0.707f
    });
    _highPassFilter.reset();
}

inline void Audio::Hat::onAudioParametersChanged(void)
{
    _noteManager.reset();
    _noteManager.envelope().setSampleRate(audioSpecs().sampleRate);
    _fm.reset();
    _fm.setSampleRate(audioSpecs().sampleRate);

        _bandPassFilter.setup(DSP::Biquad::Internal::Specs {
        DSP::Filter::AdvancedType::BandPassFlat,
        static_cast<float>(audioSpecs().sampleRate),
        { 10'000.0f, 10'000.0f },
        1.0f,
        0.707f
    });

    _cache.resize(GetFormatByteLength(audioSpecs().format) * audioSpecs().processBlockSize, audioSpecs().sampleRate, audioSpecs().channelArrangement, audioSpecs().format);

    _combFilter.reset(audioSpecs(), 1.0f, static_cast<float>(combSpread()));

    _bandPassFilter.reset();
    _highPassFilter.setup(DSP::Biquad::Internal::Specs {
        DSP::Filter::AdvancedType::HighPass,
        static_cast<float>(audioSpecs().sampleRate),
        { 7'000.0f, 7'000.0f },
        1.0f,
        0.707f
    });
    _highPassFilter.reset();
}

inline void Audio::Hat::setExternalPaths(const ExternalPaths &paths)
{
    UNUSED(paths);
}

inline void Audio::Hat::sendNotes(const NoteEvents &notes, const BeatRange &range)
{
    UNUSED(range);
    if (notes.size()) {
        // std::cout << range << std::endl;
        _noteManager.feedNotes(notes);
    }
}

inline Audio::DSP::FM::Internal::Operator MakeOperator(Audio::DSP::FM::Internal::Operator base, const float ratio)
{
    base.frequencyDelta *= ratio / 4.0f;
    return base;
}

inline void Audio::Hat::receiveAudio(BufferView output)
{
    const DB outGain = ConvertDecibelToRatio(static_cast<float>(outputVolume()));
    const auto channels = static_cast<std::size_t>(audioSpecs().channelArrangement);
    const auto outChannelSize = audioSpecs().processBlockSize;
    float *out = reinterpret_cast<float *>(output.byteData());

    _noteManager.envelopeGain().resize(outChannelSize);
    _noteManager.setEnvelopeSpecs(DSP::EnvelopeSpecs {
        0.0f,
        static_cast<float>(attack()), // A
        1.0f,
        0.0f,
        static_cast<float>(duration()), // D
        0.0f, // S
        static_cast<float>(duration()) // R
    });


    _noteManager.processRetriggerNotes(
        output,
        [this, outGain](const Key key, const std::uint32_t readIndex, const NoteModifiers &modifiers, float *realOutput, const std::uint32_t realOutSize, const std::size_t channelCount) -> std::pair<std::uint32_t, std::uint32_t>
        {
            UNUSED(modifiers);
            UNUSED(channelCount);
            const float rootFrequency = std::pow(2.f, static_cast<float>(static_cast<int>(key) - RootKey) / KeysPerOctave) * RootKeyFrequency;
            const DSP::FM::Internal::Operator ref {
                DSP::Generator::Waveform::SquareAnalog,
                1.0f,
                static_cast<float>(attack()),
                static_cast<float>(duration() * 0.75f),
                0.0f,
                static_cast<float>(duration() * 0.75f),
                1.0f,
                0.0f,
                0u,
                static_cast<Key>(69u),
                0.0f,
                0.0f
            };

            _noteManager.generateEnvelopeGains(key, readIndex, realOutSize);
            _fm.process<true>(realOutput, realOutSize, outGain, readIndex, key, rootFrequency, {
                DSP::FM::Internal::Operator {
                    DSP::Generator::Waveform::Noise,
                    1.0f,
                    static_cast<float>(attack()),
                    static_cast<float>(duration()),
                    0.0f,
                    static_cast<float>(duration()),
                    1.0f,
                    0.0f,
                    0u,
                    static_cast<Key>(69u),
                    0.0f,
                    0.0f
                },
                MakeOperator(ref, 3.45787f),
                MakeOperator(ref, 4.14567f),
                MakeOperator(ref, 5.09843f),
                MakeOperator(ref, 6.74659f),
                MakeOperator(ref, 8.64921f)
            });
            return std::make_pair(realOutSize, 0u);
        },
        [this] (const Key key) -> bool
        {
            return !_noteManager.envelope().lastGain(key);
            return _fm.isKeyEnded(key);
        },
        [this] (const Key key) -> void
        {
            _fm.resetKey(key);
        }
    );

    _combFilter.setInputAmount(0.0f);
    _combFilter.setDelayAmount(static_cast<float>(combLevel()));
    _combFilter.setDelayTime(static_cast<float>(audioSpecs().sampleRate), static_cast<float>(combSpread()));

    float *cache = _cache.data<float>();
    std::memcpy(cache, out, sizeof(float) * outChannelSize);
    _combFilter.process<0u, false>(_cache.data<float>(), out, outChannelSize);


    _bandPassFilter.setup(DSP::Biquad::Internal::Specs {
        DSP::Filter::AdvancedType::BandPass,
        static_cast<float>(audioSpecs().sampleRate),
        { static_cast<float>(bandFreq()), static_cast<float>(bandFreq()) },
        1.0f,
        0.707f
    });
    _bandPassFilter.filterBlock(out, audioSpecs().processBlockSize, out, 0u, 1.0f);

    _highPassFilter.setup(DSP::Biquad::Internal::Specs {
        DSP::Filter::AdvancedType::HighPass,
        static_cast<float>(audioSpecs().sampleRate),
        { static_cast<float>(highFreq()), static_cast<float>(highFreq()) },
        1.0f,
        0.707f
    });
    _highPassFilter.filterBlock(out, audioSpecs().processBlockSize, out, 0u, 1.0f);

    const float gainFrom = ConvertDecibelToRatio(static_cast<float>(getControlPrev((0u))));
    const float gainTo = ConvertDecibelToRatio(static_cast<float>(outputVolume()));

    DSP::Gain::Apply<float>(out, outChannelSize, gainFrom, gainTo);
    if (channels - 1u) {
        std::memcpy(out + outChannelSize, out, outChannelSize * sizeof(float));
    }
}
