/**
 * @ Author: Pierre Veysseyre
 * @ Description: Snare implementation
 */

#include <Audio/DSP/Gain.hpp>

inline void Audio::Snare::onAudioGenerationStarted(const BeatRange &range)
{
    UNUSED(range);
    _noteManager.reset();
    _fm.reset();
}

inline void Audio::Snare::onAudioParametersChanged(void)
{
    _noteManager.reset();
    _noteManager.envelope().setSampleRate(audioSpecs().sampleRate);
    _fm.reset();
    _fm.setSampleRate(audioSpecs().sampleRate);
}

inline void Audio::Snare::setExternalPaths(const ExternalPaths &paths)
{
    UNUSED(paths);
}

inline void Audio::Snare::sendNotes(const NoteEvents &notes, const BeatRange &range)
{
    UNUSED(range);
    if (notes.size()) {
        // std::cout << range << std::endl;
        _noteManager.feedNotesRetrigger(notes,
            [this] (const Key key) -> void
            {
                _fm.resetKey(key);
            }
        );
    }
}

inline void Audio::Snare::receiveAudio(BufferView output)
{
    const DB outGain = ConvertDecibelToRatio(DefaultVoiceGain);
    const auto outSize = static_cast<std::uint32_t>(output.channelSampleCount());
    const auto channels = static_cast<std::size_t>(output.channelArrangement());
    float *out = reinterpret_cast<float *>(output.byteData());

    _noteManager.envelopeGain().resize(outSize);
    // _noteManager.setEnvelopeSpecs(DSP::EnvelopeSpecs {
    //     0.0f,
    //     0.0f, // A
    //     1.0f,
    //     0.0f,
    //     0.0f,// D
    //     1.0f,// S
    //     1.5f,// R
    // });


    _noteManager.processNotes(
        output,
        [this, outGain, channels](const Key key, const std::uint32_t readIndex, const NoteModifiers &modifiers, float *realOutput, const std::uint32_t realOutSize, const std::size_t channelCount) -> std::pair<std::uint32_t, std::uint32_t>
        {
            UNUSED(modifiers);
            UNUSED(channelCount);
            const float rootFrequency = std::pow(2.f, static_cast<float>(static_cast<int>(key) - RootKey) / KeysPerOctave) * RootKeyFrequency;

            const auto globalDuration = static_cast<float>(duration());
            const auto snapLvl = static_cast<float>(snapLevel());
            const auto snapDec = static_cast<float>(snapDuration());
            const auto snapTn = static_cast<float>(snapTone());
            const auto pitchDec = static_cast<float>(pitchDecay());
            const auto pitchLvl = static_cast<float>(pitchLevel());

            UNUSED(snapTn);
            _noteManager.generateEnvelopeGains(key, readIndex, realOutSize);
            _fm.process<true>(realOutput, realOutSize, outGain, readIndex, key, rootFrequency, {
               DSP::FM::Internal::Operator {
                    DSP::Generator::Waveform::Triangle,
                    1.0f,
                    0.0f,
                    static_cast<float>(attack() * 2.0f),
                    0.0f,
                    static_cast<float>(attack() * 2.0f),
                    1.0f,
                    0.0f,
                    0u,
                    static_cast<Key>(69u),
                    0.0f,
                    0.0f
               },
               DSP::FM::Internal::Operator {
                    DSP::Generator::Waveform::Sine,
                    1.0f,
                    static_cast<float>(attack()),
                    globalDuration,
                    0.0f,
                    globalDuration,
                    1.0f,
                    0.0f,
                    0u,
                    static_cast<Key>(69u),
                    0.0f,
                    0.0f
               },
               DSP::FM::Internal::Operator {
                    DSP::Generator::Waveform::Sine,
                    1.0f,
                    static_cast<float>(attack()),
                    globalDuration * 2.0f,
                    0.0f,
                    globalDuration * 2.0f,
                    static_cast<float>(color()),
                    0.0f,
                    0u,
                    static_cast<Key>(69u),
                    0.0f,
                    0.0f
               },
               DSP::FM::Internal::Operator {
                    DSP::Generator::Waveform::Noise,
                    1.0f,
                    static_cast<float>(attack()),
                    snapDec,
                    0.0f,
                    0.001f,
                    snapLvl,
                    0.0f,
                    0u,
                    static_cast<Key>(69u),
                    0.0f,
                    0.0f
               }
            },
            DSP::FM::Internal::PitchOperator {
                0.0f, // Attack
                1.0f, // peak
                pitchDec,
                0.0f,
                0.01f,
                pitchLvl
            });
            return std::make_pair(realOutSize, 0u);
        },
        [this] (const Key key) -> bool
        {
            return _fm.isKeyEnded(key);
        },
        [this] (const Key key) -> void
        {
            _fm.resetKey(key);
        }
    );

    // // Process overdrive in mono
    // static constexpr float MaxDriveRatio = 5.0f;
    // const float drive = static_cast<float>(overdrive()) * MaxDriveRatio + 1.0f;
    // for (auto i = 0u; i < outSize; ++i) {
    //     out[i] = std::tanh(drive * out[i]);
    // }

    const float gainFrom = ConvertDecibelToRatio(static_cast<float>(getControlPrev((0u))));
    const float gainTo = ConvertDecibelToRatio(static_cast<float>(outputVolume()));

    DSP::Gain::Apply<float>(output.data<float>(), output.size<float>(), gainFrom, gainTo);

    // Re-arrange the buffer in case of multiple channels
    const auto channelsMinusOne = channels - 1u;
    if (channelsMinusOne) {

        // ABCD___ -> ABCDABCD
        for (auto ch = 1u; ch < channels; ++ch)
            std::memcpy(out + (ch * outSize), out, outSize * sizeof(float));

        // ABCDABCD -> AABBCCDD
        for (auto i = 0u; i < outSize; ++i) {
            for (auto ch = 0u; ch < channels; ++ch) {
                out[i * channels + ch] = out[i + channelsMinusOne * outSize];
            }
        }
    }
}
