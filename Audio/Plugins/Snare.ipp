/**
 * @ Author: Pierre Veysseyre
 * @ Description: Snare implementation
 */

#include <iomanip>

inline void Audio::Snare::onAudioGenerationStarted(const BeatRange &range)
{
    UNUSED(range);
    _fmManager.reset();
}

inline void Audio::Snare::onAudioParametersChanged(void)
{
    _fmManager.reset();
    _fmManager.schema().setSampleRate(audioSpecs().sampleRate);
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
        _fmManager.feedNotes(notes);
    }
}

inline void Audio::Snare::receiveAudio(BufferView output)
{
    const DB outGain = ConvertDecibelToRatio(static_cast<float>(outputVolume()) + DefaultVoiceGain);
    const auto outSize = static_cast<std::uint32_t>(output.channelSampleCount());
    const auto channels = static_cast<std::size_t>(output.channelArrangement());
    float *out = reinterpret_cast<float *>(output.byteData());

    _fmManager.processNotes(
        [this, outGain, outSize, out](const Key key, const std::uint32_t readIndex, const NoteModifiers &modifiers) -> std::pair<std::uint32_t, std::uint32_t>
        {
            const float rootFrequency = std::pow(2.f, static_cast<float>(static_cast<int>(key) - RootKey) / KeysPerOctave) * RootKeyFrequency;
            auto realOut = out;
            auto realOutSize = outSize;
            if (modifiers.sampleOffset && !readIndex) {
                realOut += modifiers.sampleOffset;
                realOutSize -= modifiers.sampleOffset;
            }
            UNUSED(readIndex);
            UNUSED(rootFrequency);
            UNUSED(modifiers);

            const auto pitchDecay = static_cast<float>(sweepDuration() * 0.1 + 0.01);
            const auto pitchRelease = static_cast<float>(sweepImpact() * 0.9 + 0.1);

            _fmManager.processSchema<true>(realOut, realOutSize, 1.0f, readIndex, key, rootFrequency, {
               DSP::FM::Internal::Operator {
                    DSP::Generator::Waveform::Triangle,
                    1.0f,
                    0.0f,
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
               DSP::FM::Internal::Operator {
                    boost() ? DSP::Generator::Waveform::Triangle : DSP::Generator::Waveform::Sine,
                    1.0f,
                    0.0f,
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
                pitchDecay,
                0.0f,
                0.01f,
                pitchRelease
            });
            return std::make_pair(realOutSize, 0u);
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
