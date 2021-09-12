/**
 * @ Author: Pierre Veysseyre
 * @ Description: Oscillator implementation
 */

static auto Frames = 1u;

inline void Audio::Oscillator::onAudioGenerationStarted(const BeatRange &range)
{
    Frames = 1u;
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
        // std::cout << "RANGE: " << range << std::endl;
        // std::cout << " ::" << Frames << " - " << (Frames - 1u) * audioSpecs().processBlockSize << std::endl;
        _noteManager.feedNotes(notes);
    }
}

inline void Audio::Oscillator::receiveAudio(BufferView output)
{
    ++Frames;

    const DB outGain = ConvertDecibelToRatio(static_cast<float>(outputVolume()) + DefaultVoiceGain);
    const auto outSize = static_cast<std::uint32_t>(output.channelSampleCount());
    const auto channels = output.channelArrangement();

    _noteManager.setEnvelopeSpecs(DSP::EnvelopeSpecs {
        0.0f,
        static_cast<float>(envelopeAttack()),
        1.0f,
        0.0f,
        static_cast<float>(envelopeDecay()),
        static_cast<float>(envelopeSustain()),
        static_cast<float>(envelopeRelease()),
    });
    _noteManager.envelopeGain().resize(outSize);
    _noteManager.processNotesEnvelope(
        output,
        [this, outGain, channels](const Key key, const std::uint32_t readIndex, const NoteModifiers &modifiers, float *realOutput, const std::uint32_t realOutSize, const std::size_t channelCount) -> std::pair<std::uint32_t, std::uint32_t>
        {
            UNUSED(modifiers);
            const auto channelsMinusOne = channelCount - 1u;
            const auto outGainNorm = channelsMinusOne ? outGain / 2.0f : outGain / 4.0f;

            auto realOutLeft = realOutput;
            auto realOutRight = realOutput + channelsMinusOne;

            const auto freq = GetNoteFrequency(key);
            const auto freqNorm = GetFrequencyNorm(freq, audioSpecs().sampleRate);

            constexpr auto MaxDetuneSemitone = 0.5f;
            const float det = static_cast<float>(detune());
            const DB pan = channelsMinusOne ? static_cast<float>(panning()) : 0.0f;
            const DB gainL = pan <= 0.0f ? outGainNorm : (1.0f - pan) * outGainNorm;
            const DB gainR = pan >= 0.0f ? outGainNorm : (1.0f + pan) * outGainNorm;

            // Reset phase index -> move this shit
            // if (!readIndex)
            //     _oscillator.resetKey<0u>(key);
            // Sync voices when no detune (unison)
            if (det == 0.0f) {
                const auto rootPhase = _oscillator.phase<0u>(key);
                _oscillator.setPhase<1u>(key, rootPhase);
                _oscillator.setPhase<2u>(key, rootPhase);
            }
            _oscillator.generate<true, 0u, false>(
                static_cast<DSP::Generator::Waveform>(waveform()),
                channels,
                realOutLeft,
                _noteManager.envelopeGain().data(),
                realOutSize,
                key,
                freqNorm,
                readIndex,
                gainL
            );
            _oscillator.generate<true, 0u>(
                static_cast<DSP::Generator::Waveform>(waveform()),
                channels,
                realOutRight,
                _noteManager.envelopeGain().data(),
                realOutSize,
                key,
                freqNorm,
                readIndex,
                gainR
            );
            _oscillator.generate<true, 1u>(
                static_cast<DSP::Generator::Waveform>(waveform()),
                channels,
                realOutLeft,
                _noteManager.envelopeGain().data(),
                realOutSize,
                key,
                GetNoteFrequencyDelta(freqNorm, -det * MaxDetuneSemitone),
                readIndex,
                gainL
            );
            _oscillator.generate<true, 2u>(
                static_cast<DSP::Generator::Waveform>(waveform()),
                channels,
                realOutRight,
                _noteManager.envelopeGain().data(),
                realOutSize,
                key,
                GetNoteFrequencyDelta(freqNorm, det * MaxDetuneSemitone),
                readIndex,
                gainR
            );

            return std::make_pair(realOutSize, 0u);
        },
        [this] (const Key key) -> bool
        {
            return !_noteManager.envelope().lastGain(key);
        },
        [this] (const Key key) -> void
        {
            _oscillator.resetKey(key);
        }
    );

    // const auto channelsMinusOne = channelCount - 1u;
    // if (channelsMinusOne) {

    //     // ABCD___ -> ABCDABCD
    //     for (auto ch = 1u; ch < channelCount; ++ch)
    //         std::memcpy(realOutput + (ch * outSize), realOutput, outSize * sizeof(float));

    //     // ABCDABCD -> AABBCCDD
    //     for (auto i = 0u; i < outSize; ++i) {
    //         for (auto ch = 0u; ch < channelCount; ++ch) {
    //             realOutput[i * channelCount + ch] = realOutput[i + channelsMinusOne * outSize];
    //         }
    //     }
    // }

    // To benchmark, must be slower
    // Modifier<float>::ApplyIndexFunctor(outLeft, outSize, 0u, [&, outGain](const std::size_t index) -> float {
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
