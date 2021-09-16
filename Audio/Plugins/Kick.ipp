/**
 * @ Author: Pierre Veysseyre
 * @ Description: Kick implementation
 */

#include <iomanip>

inline void Audio::Kick::onAudioGenerationStarted(const BeatRange &range)
{
    UNUSED(range);
    _noteManager.reset();
    _fm.reset();
}

inline void Audio::Kick::onAudioParametersChanged(void)
{
    _noteManager.reset();
    _noteManager.envelope().setSampleRate(audioSpecs().sampleRate);
    _fm.reset();
    _fm.setSampleRate(audioSpecs().sampleRate);
}

inline void Audio::Kick::setExternalPaths(const ExternalPaths &paths)
{
    UNUSED(paths);
}

inline void Audio::Kick::sendNotes(const NoteEvents &notes, const BeatRange &range)
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

inline void Audio::Kick::receiveAudio(BufferView output)
{
    const DB outGain = ConvertDecibelToRatio(static_cast<float>(outputVolume()) + DefaultVoiceGain);
    const auto outSize = static_cast<std::uint32_t>(output.channelSampleCount());
    const auto channels = static_cast<std::size_t>(output.channelArrangement());
    float *out = reinterpret_cast<float *>(output.byteData());

    _noteManager.setEnvelopeSpecs(DSP::EnvelopeSpecs {
        0.0f,
        0.0f, // A
        1.0f,
        0.0f,
        static_cast<float>(duration()), // D
        0.8f,// S
        static_cast<float>(duration()) // R
    });
    _noteManager.envelopeGain().resize(outSize);
    _noteManager.processNotes(
        output,
        [this, outGain, channels](const Key key, const std::uint32_t readIndex, const NoteModifiers &modifiers, float *realOutput, const std::uint32_t realOutSize, const std::size_t channelCount) -> std::pair<std::uint32_t, std::uint32_t>
        {
            UNUSED(modifiers);
            UNUSED(channelCount);
            const float rootFrequency = std::pow(2.f, static_cast<float>(static_cast<int>(key) - RootKey) / KeysPerOctave) * RootKeyFrequency;

            _noteManager.generateEnvelopeGains(key, readIndex, realOutSize);
            _fm.process<true>(realOutput, realOutSize, 1.0f, readIndex, key, rootFrequency, {
               DSP::FM::Internal::Operator {
                    DSP::Generator::Waveform::Sine,
                    0.5f,
                    0.0f,
                    static_cast<float>(duration()),
                    0.8f,
                    static_cast<float>(duration()),
                    1.0f,
                    0.0f,
                    0u,
                    static_cast<Key>(69u),
                    0.0f,
                    0.0f
               },
               DSP::FM::Internal::Operator {
                    boost() ? DSP::Generator::Waveform::Triangle : DSP::Generator::Waveform::Sine,
                    0.5f,
                    0.0f,
                    static_cast<float>(duration()),
                    0.8f,
                    static_cast<float>(duration()),
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
                    0.0f,
                    static_cast<float>(duration() * 2.0f),
                    0.0f,
                    static_cast<float>(duration() * 2.0f),
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
                    0.0f,
                    static_cast<float>(clic() * 0.01),
                    0.0f,
                    0.001f,
                    1.0f,
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
                static_cast<float>(sweepDuration() * 0.1 + 0.01),
                0.0f,
                0.01f,
                static_cast<float>(sweepImpact() * 0.9 + 0.1)
            });
            return std::make_pair(realOutSize, 0u);
        },
        [this] (const Key key) -> bool
        {
            return _fm.isKeyEnded(key);
            // return !_noteManager.envelope().lastGain(key);
        },
        [this] (const Key key) -> void
        {
            _fm.resetKey(key);
        }
    );

    // Process overdrive in mono
    static constexpr float MaxDriveRatio = 5.0f;
    const float drive = static_cast<float>(overdrive()) * MaxDriveRatio + 1.0f;
    for (auto i = 0u; i < outSize; ++i) {
        out[i] = std::tanh(drive * out[i]) * outGain;
    }

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
