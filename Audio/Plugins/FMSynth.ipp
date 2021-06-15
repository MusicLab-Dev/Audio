/**
 * @ Author: Pierre Veysseyre
 * @ Description: FMX implementation
 */

#include <iomanip>

inline void Audio::FMX::onAudioGenerationStarted(const BeatRange &range)
{
    UNUSED(range);
    _fmManager.reset();
}

inline void Audio::FMX::onAudioParametersChanged(void)
{
    _fmManager.reset();
    _fmManager.schema().setSampleRate(audioSpecs().sampleRate);
}

inline void Audio::FMX::setExternalPaths(const ExternalPaths &paths)
{
    UNUSED(paths);
}

inline void Audio::FMX::sendNotes(const NoteEvents &notes, const BeatRange &range)
{
    UNUSED(range);
    if (notes.size()) {
        // std::cout << range << std::endl;
        _fmManager.feedNotes(notes);
    }
}

inline void Audio::FMX::receiveAudio(BufferView output)
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

            _fmManager.processSchema<true>(realOut, realOutSize, outGain, readIndex, key, rootFrequency, {
               DSP::FM::Internal::Operator {
                   static_cast<float>(opAratio()),
                   static_cast<float>(opAattack()),
                   static_cast<float>(opAdecay()),
                   static_cast<float>(opAsustain()),
                   static_cast<float>(opArelease()),
                   static_cast<float>(opAvolume() * opAvolumeRatio()),
                   static_cast<std::int32_t>(opAdetune()),
                   static_cast<std::uint32_t>(opAfeedback())
               },
               DSP::FM::Internal::Operator {
                   static_cast<float>(opBratio()),
                   static_cast<float>(opBattack()),
                   static_cast<float>(opBdecay()),
                   static_cast<float>(opBsustain()),
                   static_cast<float>(opBrelease()),
                   static_cast<float>(opBvolume() * opBvolumeRatio()),
                   static_cast<std::int32_t>(opBdetune()),
                   static_cast<std::uint32_t>(opBfeedback())
               },
               DSP::FM::Internal::Operator {
                   static_cast<float>(opCratio()),
                   static_cast<float>(opCattack()),
                   static_cast<float>(opCdecay()),
                   static_cast<float>(opCsustain()),
                   static_cast<float>(opCrelease()),
                   static_cast<float>(opCvolume() * opCvolumeRatio()),
                   static_cast<std::int32_t>(opCdetune()),
                   static_cast<std::uint32_t>(opCfeedback())
               },
               DSP::FM::Internal::Operator {
                   static_cast<float>(opDratio()),
                   static_cast<float>(opDattack()),
                   static_cast<float>(opDdecay()),
                   static_cast<float>(opDsustain()),
                   static_cast<float>(opDrelease()),
                   static_cast<float>(opDvolume() * opDvolumeRatio()),
                   static_cast<std::int32_t>(opDdetune()),
                   static_cast<std::uint32_t>(opDfeedback())
               },
               DSP::FM::Internal::Operator {
                   static_cast<float>(opEratio()),
                   static_cast<float>(opEattack()),
                   static_cast<float>(opEdecay()),
                   static_cast<float>(opEsustain()),
                   static_cast<float>(opErelease()),
                   static_cast<float>(opEvolume() * opEvolumeRatio()),
                   static_cast<std::int32_t>(opEdetune()),
                   static_cast<std::uint32_t>(opEfeedback())
               },
               DSP::FM::Internal::Operator {
                   static_cast<float>(opFratio()),
                   static_cast<float>(opFattack()),
                   static_cast<float>(opFdecay()),
                   static_cast<float>(opFsustain()),
                   static_cast<float>(opFrelease()),
                   static_cast<float>(opFvolume() * opFvolumeRatio()),
                   static_cast<std::int32_t>(opFdetune()),
                   static_cast<std::uint32_t>(opFfeedback())
               }
            });
            return std::make_pair(realOutSize, 0u);
        }
    );

    // std::cout << std::endl;
    // std::cout << _fmManager.getActiveNoteSize() << std::endl;
    // std::cout << _fmManager.getActiveNoteBlockSize() << std::endl;

    // To benchmark, must be slower
    // Modifier<float>::ApplyIndexFunctor(out, outSize, 0u, [&, outGain](const std::size_t index) -> float {
    //     UNUSED(index);
    //     float sample {};
    //     const auto activeNote = _fmManager.getActiveNote();
    //     const auto activeNoteSize = activeNote.size();
    //     // std::cout << "Receive <audio>: " << activeNote.size() << std::endl;
    //     for (auto iKey = 0u; iKey < activeNoteSize; ++iKey) {
    //         const auto key = activeNote[iKey];
    //         const auto trigger = _fmManager.trigger(key);
    //         // Phase index
    //         const auto phaseIndex = _fmManager.readIndex(key);
    //         const float frequency = std::pow(2.f, static_cast<float>(static_cast<int>(key) - RootKey) / KeysPerOctave) * RootKeyFrequency;
    //         const float frequencyNorm = 2.f * static_cast<float>(M_PI) * frequency / static_cast<float>(audioSpecs().sampleRate);
    //         sample += std::sin(static_cast<float>(index) * frequencyNorm) *
    //         getEnvelopeGain(key, static_cast<std::uint32_t>(phaseIndex), trigger);
    //     }
    //     return sample * outGain;
    //         // std::sin(static_cast<float>(index) * frequencyNorm) *
    //         // getEnvelopeGain(key, static_cast<std::uint32_t>(index), trigger) * gain;
    // });

}
