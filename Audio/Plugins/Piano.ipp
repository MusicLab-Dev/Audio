/**
 * @ Author: Pierre Veysseyre
 * @ Description: Piano implementation
 */

#include <iomanip>

inline void Audio::Piano::onAudioGenerationStarted(const BeatRange &range)
{
    UNUSED(range);
    _fmManager.reset();
    _filter.init(
        DSP::Filter::FIRSpecs(
            DSP::Filter::BasicType::LowPass,
            DSP::Filter::WindowType::Hanning,
            33ul,
            static_cast<float>(audioSpecs().sampleRate),
            8000.0f,
            0.0f,
            1.0f
        )
    );
    _cache.resize(GetFormatByteLength(audioSpecs().format) * audioSpecs().processBlockSize, audioSpecs().sampleRate, audioSpecs().channelArrangement, audioSpecs().format);
    _cache.clear();
}

inline void Audio::Piano::onAudioParametersChanged(void)
{
    _fmManager.reset();
    _fmManager.schema().setSampleRate(audioSpecs().sampleRate);
}

inline void Audio::Piano::setExternalPaths(const ExternalPaths &paths)
{
    UNUSED(paths);
}

inline void Audio::Piano::sendNotes(const NoteEvents &notes, const BeatRange &range)
{
    UNUSED(range);
    if (notes.size()) {
        // std::cout << range << std::endl;
        _fmManager.feedNotes(notes);
    }
}

inline void Audio::Piano::receiveAudio(BufferView output)
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

            // CONTROL_MAP(brightness, opBvolume, )
            // opAvolume(brightness());
            _fmManager.processSchema<true>(realOut, realOutSize, outGain, readIndex, key, rootFrequency, {
               DSP::FM::Internal::Operator {
                    DSP::Generator::Waveform::Sine,
                    static_cast<float>(opAratio()),
                    static_cast<float>(opAattack()),
                    static_cast<float>(opAdecay()),
                    static_cast<float>(opAsustain()),
                    static_cast<float>(opArelease()),
                    ConvertDecibelToRatio(static_cast<float>(opAvolume())),
                    static_cast<float>(opAdetune()),
                    static_cast<std::uint32_t>(opAfeedback()),
                    static_cast<Key>(69u),
                    static_cast<float>(opAkeyAmountLeft()) / 100.0f,
                    static_cast<float>(opAkeyAmountRight()) / 100.0f
               },
               DSP::FM::Internal::Operator {
                    DSP::Generator::Waveform::Sine,
                    static_cast<float>(opBratio()),
                    static_cast<float>(opBattack()),
                    static_cast<float>(opBdecay()),
                    static_cast<float>(opBsustain()),
                    static_cast<float>(opBrelease()),
                    ConvertDecibelToRatio(static_cast<float>(opBvolume())),
                    static_cast<float>(opBdetune()),
                    static_cast<std::uint32_t>(opBfeedback()),
                    static_cast<Key>(69u),
                    static_cast<float>(opBkeyAmountLeft()) / 100.0f,
                    static_cast<float>(opBkeyAmountRight()) / 100.0f
               },
               DSP::FM::Internal::Operator {
                    DSP::Generator::Waveform::Saw,
                    static_cast<float>(opCratio()),
                    static_cast<float>(opCattack()),
                    static_cast<float>(opCdecay()),
                    static_cast<float>(opCsustain()),
                    static_cast<float>(opCrelease()),
                    ConvertDecibelToRatio(static_cast<float>(opCvolume())),
                    static_cast<float>(opCdetune()),
                    static_cast<std::uint32_t>(opCfeedback()),
                    static_cast<Key>(69u),
                    static_cast<float>(opCkeyAmountLeft()) / 100.0f,
                    static_cast<float>(opCkeyAmountRight()) / 100.0f
               },
               DSP::FM::Internal::Operator {
                    DSP::Generator::Waveform::Sine,
                    static_cast<float>(opDratio()),
                    static_cast<float>(opDattack()),
                    static_cast<float>(opDdecay()),
                    static_cast<float>(opDsustain()),
                    static_cast<float>(opDrelease()),
                    ConvertDecibelToRatio(static_cast<float>(opDvolume())),
                    static_cast<float>(opDdetune()),
                    static_cast<std::uint32_t>(opDfeedback()),
                    static_cast<Key>(69u),
                    static_cast<float>(opDkeyAmountLeft()) / 100.0f,
                    static_cast<float>(opDkeyAmountRight()) / 100.0f
               }
            },
            DSP::FM::Internal::PitchOperator {
                static_cast<float>(pitchAttack()),
                static_cast<float>(pitchPeak()),
                static_cast<float>(pitchDecay()),
                static_cast<float>(pitchSustain()),
                static_cast<float>(pitchRelease()),
                static_cast<float>(pitchVolume())
            });
            return std::make_pair(realOutSize, 0u);
        }
    );

    _filter.setSpecs(
        DSP::Filter::FIRSpecs(
            DSP::Filter::BasicType::LowPass,
            DSP::Filter::WindowType::Default,
            33ul,
            static_cast<float>(audioSpecs().sampleRate),
            8000.0f,
            0.0f,
            1.0f
        )
    );
    // _filter.filter<true>(out, audioSpecs().processBlockSize, out, outGain);

}
