/**
 * @ Author: Pierre Veysseyre
 * @ Description: Kick implementation
 */

#include <iomanip>

inline void Audio::Kick::onAudioGenerationStarted(const BeatRange &range)
{
    UNUSED(range);
    _fmManager.reset();
}

inline void Audio::Kick::onAudioParametersChanged(void)
{
    _fmManager.reset();
    _fmManager.schema().setSampleRate(audioSpecs().sampleRate);
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
        _fmManager.feedNotes(notes);
    }
}

inline void Audio::Kick::receiveAudio(BufferView output)
{
    const DB outGain = ConvertDecibelToRatio(static_cast<float>(outputVolume()) + DefaultVoiceGain);
    const auto outSize = static_cast<std::uint32_t>(output.size<float>());
    float *out = reinterpret_cast<float *>(output.byteData());
    // const bool noRelease = !enveloppeRelease();
    const bool noRelease = false;



    _fmManager.processNotes(
        [this, outGain, outSize, out, noRelease](const Key key, const bool trigger, const std::uint32_t readIndex, const NoteModifiers &modifiers) -> std::pair<std::uint32_t, std::uint32_t> {
            const float rootFrequency = std::pow(2.f, static_cast<float>(static_cast<int>(key) - RootKey) / KeysPerOctave) * RootKeyFrequency;
            auto realOut = out;
            auto realOutSize = outSize;
            if (modifiers.sampleOffset) {
                if (trigger) {
                    realOut += modifiers.sampleOffset;
                    realOutSize -= modifiers.sampleOffset;
                } else if (noRelease) {
                    realOutSize = modifiers.sampleOffset;
                }
            }
            UNUSED(readIndex);
            UNUSED(rootFrequency);
            UNUSED(modifiers);
            _fmManager.processSchema<true>(realOut, realOutSize, 1.0f, readIndex, key, rootFrequency, {
               DSP::FM::Internal::Operator {
                    DSP::Generator::Waveform::Sine,
                    0.5f,
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
                    0.5f,
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
                static_cast<float>(sweepDuration() * 0.1 + 0.01),
                0.0f,
                0.01f,
                static_cast<float>(sweepImpact() * 0.9 + 0.1)
            });
            return std::make_pair(realOutSize, 0u);
        }
    );
    static constexpr float MaxDriveRatio = 5.0f;
    // const DB outGainInvert = 1.0f / outGain;
    const float drive = static_cast<float>(overdrive()) * MaxDriveRatio + 1.0f;
    for (auto i = 0u; i < outSize; ++i) {
        out[i] = std::tanh(drive * out[i]) * outGain;
    }
}
