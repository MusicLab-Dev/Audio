/**
 * @ Author: Pierre Veysseyre
 * @ Description: Oscillator implementation
 */

inline void Audio::Oscillator::onAudioGenerationStarted(const BeatRange &range)
{
    UNUSED(range);
    _noteManager.reset();
    _oscillator.reset();
}

inline void Audio::Oscillator::onAudioParametersChanged(void)
{
    _noteManager.reset();
    _noteManager.envelope().setSampleRate(audioSpecs().sampleRate);
}

inline void Audio::Oscillator::setExternalPaths(const ExternalPaths &paths)
{
    UNUSED(paths);
}

inline void Audio::Oscillator::sendNotes(const NoteEvents &notes, const BeatRange &range)
{
    UNUSED(range);
    if (notes.size()) {
        _noteManager.feedNotes(notes);
    }
}

inline void Audio::Oscillator::receiveAudio(BufferView output)
{
    const DB outGain = ConvertDecibelToRatio(static_cast<float>(outputVolume()) + DefaultVoiceGain);
    const auto outSize = static_cast<std::uint32_t>(output.size<float>());
    float *out = reinterpret_cast<float *>(output.byteData());

    _noteManager.envelope().setSpecs<0u>(DSP::EnvelopeSpecs {
        0.0f,
        static_cast<float>(envelopeAttack()),
        1.0f,
        0.0f,
        static_cast<float>(envelopeDecay()),
        static_cast<float>(envelopeSustain()),
        static_cast<float>(envelopeRelease()),
    });
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

            _noteManager.generateEnvelopeGains(key, readIndex, realOutSize);
            _oscillator.generate<true>(
                static_cast<DSP::Generator::Waveform>(waveform()),
                realOut,
                _noteManager.envelopeGain().data(),
                realOutSize,
                key,
                freqNorm,
                readIndex,
                outGain
            );

            return std::make_pair(realOutSize, 0u);
        }
    );

    // std::cout << std::endl;
    // std::cout << _noteManager.getActiveNoteSize() << std::endl;
    // std::cout << _noteManager.getActiveNoteBlockSize() << std::endl;

    // To benchmark, must be slower
    // Modifier<float>::ApplyIndexFunctor(out, outSize, 0u, [&, outGain](const std::size_t index) -> float {
    //     UNUSED(index);
    //     float sample {};
    //     const auto activeNote = _noteManager.getActiveNote();
    //     const auto activeNoteSize = activeNote.size();
    //     // std::cout << "Receive <audio>: " << activeNote.size() << std::endl;
    //     for (auto iKey = 0u; iKey < activeNoteSize; ++iKey) {
    //         const auto key = activeNote[iKey];
    //         const auto trigger = _noteManager.trigger(key);
    //         // Phase index
    //         const auto phaseIndex = _noteManager.readIndex(key);
    //         const float frequency = std::pow(2.f, static_cast<float>(static_cast<int>(key) - RootKey) / KeysPerOctave) * RootKeyFrequency;
    //         const float frequencyNorm = 2.f * static_cast<float>(M_PI) * frequency / static_cast<float>(audioSpecs().sampleRate);
    //         sample += std::sin(static_cast<float>(index) * frequencyNorm) *
    //         _noteManager.getEnvelopeGain(key, static_cast<std::uint32_t>(phaseIndex));
    //     }
    //     return sample * outGain;
    //         // std::sin(static_cast<float>(index) * frequencyNorm) *
    //         // _noteManager.getEnvelopeGain(key, static_cast<std::uint32_t>(index)) * gain;
    // });

}
