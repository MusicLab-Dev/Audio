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
    // std::cout << "RANGE: " << range << " == " << range.to - range.from << std::endl;
    if (notes.size()) {
        _noteManager.feedNotes(notes);
    }
}

inline void Audio::Oscillator::receiveAudio(BufferView output)
{
    // static auto Cpt = 1u;
    // std::cout << " ::" << Cpt << " - " << (Cpt - 1u) * audioSpecs().processBlockSize << std::endl;
    // ++Cpt;

    const DB outGain = ConvertDecibelToRatio(static_cast<float>(outputVolume()) + DefaultVoiceGain);
    const auto outSize = static_cast<std::uint32_t>(output.channelSampleCount());
    float *out = reinterpret_cast<float *>(output.byteData());
    const auto channels = static_cast<std::size_t>(output.channelArrangement());

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
        [this, outGain, outSize, out, channels](const Key key, const std::uint32_t readIndex, const NoteModifiers &modifiers) -> std::pair<std::uint32_t, std::uint32_t>
        {
            const auto channelsMinusOne = channels - 1u;
            const auto outGainNorm = channelsMinusOne ? outGain / 2.0f : outGain / 4.0f;

            auto realOutLeft = out;
            auto realOutRight = out + channelsMinusOne;
            auto realOutSize = outSize;
            if (modifiers.sampleOffset && !readIndex) {
                const auto dt = modifiers.sampleOffset * channels;
                realOutLeft += dt;
                realOutRight += dt;
                realOutSize -= modifiers.sampleOffset;
            }
            const auto freq = GetNoteFrequency(key);
            const auto freqNorm = GetFrequencyNorm(freq, audioSpecs().sampleRate);

            constexpr auto MaxDetuneSemitone = 0.5f;
            const float det = static_cast<float>(detune());
            const DB pan = channelsMinusOne ? static_cast<float>(panning()) : 0.0f;
            const DB gainL = pan <= 0.0f ? outGainNorm : (1.0f - pan) * outGainNorm;
            const DB gainR = pan >= 0.0f ? outGainNorm : (1.0f + pan) * outGainNorm;

            // Reset phase index -> move this shit
            if (!readIndex)
                _oscillator.resetKey<0u>(key);
            // Sync voices when no detune (unison)
            if (det == 0.0f) {
                const auto rootPhase = _oscillator.phase<0u>(key);
                _oscillator.setPhase<1u>(key, rootPhase);
                _oscillator.setPhase<2u>(key, rootPhase);
            }
            _noteManager.generateEnvelopeGains(key, readIndex, realOutSize);
            _oscillator.generate<true, 0u, ChannelArrangement::Mono, false>(
                static_cast<DSP::Generator::Waveform>(waveform()),
                realOutLeft,
                _noteManager.envelopeGain().data(),
                realOutSize,
                key,
                freqNorm,
                readIndex,
                gainL
            );
            _oscillator.generate<true, 0u, ChannelArrangement::Mono>(
                static_cast<DSP::Generator::Waveform>(waveform()),
                realOutRight,
                _noteManager.envelopeGain().data(),
                realOutSize,
                key,
                freqNorm,
                readIndex,
                gainR
            );
            _oscillator.generate<true, 1u, ChannelArrangement::Mono>(
                static_cast<DSP::Generator::Waveform>(waveform()),
                realOutLeft,
                _noteManager.envelopeGain().data(),
                realOutSize,
                key,
                GetNoteFrequencyDelta(freqNorm, -det * MaxDetuneSemitone),
                readIndex,
                gainL
            );
            _oscillator.generate<true, 2u, ChannelArrangement::Mono>(
                static_cast<DSP::Generator::Waveform>(waveform()),
                realOutRight,
                _noteManager.envelopeGain().data(),
                realOutSize,
                key,
                GetNoteFrequencyDelta(freqNorm, det * MaxDetuneSemitone),
                readIndex,
                gainR
            );

            return std::make_pair(realOutSize, 0u);
        }
    );

    // const auto channelsMinusOne = channels - 1u;
    // if (channelsMinusOne) {

    //     // ABCD___ -> ABCDABCD
    //     for (auto ch = 1u; ch < channels; ++ch)
    //         std::memcpy(out + (ch * outSize), out, outSize * sizeof(float));

    //     // ABCDABCD -> AABBCCDD
    //     for (auto i = 0u; i < outSize; ++i) {
    //         for (auto ch = 0u; ch < channels; ++ch) {
    //             out[i * channels + ch] = out[i + channelsMinusOne * outSize];
    //         }
    //     }
    // }

    // std::cout << std::endl;
    // std::cout << _noteManager.getActiveNoteSize() << std::endl;
    // std::cout << _noteManager.getActiveNoteBlockSize() << std::endl;

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
