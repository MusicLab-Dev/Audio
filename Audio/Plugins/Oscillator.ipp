/**
 * @ Author: Pierre Veysseyre
 * @ Description: Oscillator implementation
 */

#include <iomanip>

inline void Audio::Oscillator::onAudioGenerationStarted(const BeatRange &range)
{
    UNUSED(range);
    _noteManager.reset();
}

inline void Audio::Oscillator::onAudioParametersChanged(void)
{
}

inline void Audio::Oscillator::setExternalPaths(const ExternalPaths &paths)
{
    UNUSED(paths);
}

inline void Audio::Oscillator::sendNotes(const NoteEvents &notes)
{
    _noteManager.feedNotes(notes);
}

inline void Audio::Oscillator::receiveAudio(BufferView output)
{
    using Type = float;
    const DB voiceGain = ConvertDecibelToRatio(static_cast<float>(outputVolume()) + DefaultVoiceGain);
    const auto outSize = output.size<Type>();
    Type *out = reinterpret_cast<Type *>(output.byteData());

    // std::cout << "Oscillator::receiveAudio::sampleSize: " << sampleSize << std::endl;

    const auto activeNote = _noteManager.getActiveNote();
    // std::cout << "Receive <audio>: " << activeNote.size() << std::endl;
    for (auto iKey = 0u; iKey < activeNote.size(); ++iKey) {
        const auto key = activeNote[iKey];
        const auto trigger = _noteManager.trigger(key);
        // Phase index
        const auto phaseIndex = _noteManager.readIndex(key);
        const Type frequency = std::pow(2.f, static_cast<Type>(static_cast<int>(key) - RootKey) / KeysPerOctave) * RootKeyFrequency;
        // const int nextReadIndex = sampleSize - (baseIndex + audioSpecs().processBlockSize);
        // const auto readSize = nextReadIndex < 0 ? outSize + nextReadIndex : outSize;

        // std::cout << "freq: " << frequency << std::endl;
        generateWaveform<true>(_oscillator, out, outSize, frequency, audioSpecs().sampleRate, phaseIndex, key, trigger, voiceGain);//1.f / static_cast<Type>(activeNote.size()));
        // const auto g = getEnveloppeGain(key, phaseIndex, trigger);
        _noteManager.incrementReadIndex(key, 0ul, audioSpecs().processBlockSize);
        // std::cout << g << std::endl;
        // std::cout << "  " << phaseIndex << std::endl;
    }

    // To benchmark, must be slower
    // Modifier<Type>::ApplyIndexFunctor(out, outSize, 0u, [&, voiceGain](const std::size_t index) -> Type {
    //     UNUSED(index);
    //     Type sample {};
    //     const auto activeNote = _noteManager.getActiveNote();
    //     const auto activeNoteSize = activeNote.size();
    //     // std::cout << "Receive <audio>: " << activeNote.size() << std::endl;
    //     for (auto iKey = 0u; iKey < activeNoteSize; ++iKey) {
    //         const auto key = activeNote[iKey];
    //         const auto trigger = _noteManager.trigger(key);
    //         // Phase index
    //         const auto phaseIndex = _noteManager.readIndex(key);
    //         const Type frequency = std::pow(2.f, static_cast<Type>(static_cast<int>(key) - RootKey) / KeysPerOctave) * RootKeyFrequency;
    //         const float frequencyNorm = 2.f * static_cast<float>(M_PI) * frequency / static_cast<float>(audioSpecs().sampleRate);
    //         sample += std::sin(static_cast<float>(index) * frequencyNorm) *
    //         getEnveloppeGain(key, static_cast<std::uint32_t>(phaseIndex), trigger);
    //     }
    //     return sample * voiceGain;
    //         // std::sin(static_cast<Type>(index) * frequencyNorm) *
    //         // getEnveloppeGain(key, static_cast<std::uint32_t>(index), trigger) * gain;
    // });

}

template<bool Accumulate, typename Type>
inline void Audio::Oscillator::generateWaveform(
        const Osc &oscillator, Type *output, const std::size_t outputSize,
        const float frequency, const SampleRate sampleRate, const std::uint32_t phaseOffset,
        const Key key, const bool trigger, const DB gain) noexcept
{
    switch (oscillator.waveform) {
    case Osc::Waveform::Sine:
        return generateSine<Accumulate>(output, outputSize, frequency, sampleRate, phaseOffset, key, trigger, gain);
    case Osc::Waveform::Square:
        return generateSquare<Accumulate>(output, outputSize, frequency, sampleRate, phaseOffset, key, trigger, gain);
    case Osc::Waveform::Triangle:
        return generateTriangle<Accumulate>(output, outputSize, frequency, sampleRate, phaseOffset, key, trigger, gain);
    case Osc::Waveform::Saw:
        return generateSaw<Accumulate>(output, outputSize, frequency, sampleRate, phaseOffset, key, trigger, gain);
    default:
        return generateSine<Accumulate>(output, outputSize, frequency, sampleRate, phaseOffset, key, trigger, gain);
    }
}

template<bool Accumulate, typename Type>
inline void Audio::Oscillator::generateSine(
        Type *output, const std::size_t outputSize,
        const float frequency, const SampleRate sampleRate, const std::uint32_t phaseOffset,
        const Key key, const bool trigger, const DB gain) noexcept
{
    const float frequencyNorm = 2.f * static_cast<float>(M_PI) * frequency / static_cast<float>(sampleRate);
    const auto end = outputSize + phaseOffset;

    float outGain = 1.f;
    auto k = 0ul;
    for (auto i = phaseOffset; i < end; ++i, ++k) {
        outGain = getEnveloppeGain(key, i, trigger) * gain;
        if constexpr (Accumulate)
            output[k] += static_cast<Type>(std::sin(static_cast<float>(i) * frequencyNorm) * outGain);
        else
            output[k] = static_cast<Type>(std::sin(static_cast<float>(i) * frequencyNorm) * outGain);
    }


    // Functor equivalent
    // Modifier<Type>::ApplyIndexFunctor(output, outputSize, phaseOffset, [&, key, trigger, frequencyNorm, gain](const std::size_t index) -> Type {
    //     return static_cast<Type>(
    //         std::sin(static_cast<float>(index) * frequencyNorm) *
    //         getEnveloppeGain(key, static_cast<std::uint32_t>(index), trigger) * gain
    //     );
    // });

    // (void)_noteManager.enveloppe().adsr<true>(key, phaseOffset, trigger, enveloppeAttack(), enveloppeDecay(), enveloppeSustain(), enveloppeRelease(), audioSpecs().sampleRate);
}

template<bool Accumulate, typename Type>
inline void Audio::Oscillator::generateSquare(
        Type *output, const std::size_t outputSize,
        const float frequency, const SampleRate sampleRate, const std::uint32_t phaseOffset,
        const Key key, const bool trigger, const DB gain) noexcept
{
    UNUSED(key);
    UNUSED(trigger);
    UNUSED(gain);
    const float frequencyNorm = 2.f * static_cast<float>(M_PI) * frequency / static_cast<float>(sampleRate);
    const auto end = outputSize + phaseOffset;

    auto k = 0ul;
    for (auto i = phaseOffset; i < end; ++i, ++k) {
        if constexpr (Accumulate)
            output[k] += std::sin(static_cast<float>(i) * frequencyNorm) > 0.f ? 1.f : -1.f;
        else
            output[k] = std::sin(static_cast<float>(i) * frequencyNorm) > 0.f ? 1.f : -1.f;
    }
}

template<bool Accumulate, typename Type>
inline void Audio::Oscillator::generateTriangle(
        Type *output, const std::size_t outputSize,
        const float frequency, const SampleRate sampleRate, const std::uint32_t phaseOffset,
        const Key key, const bool trigger, const DB gain) noexcept
{
    UNUSED(key);
    UNUSED(trigger);
    UNUSED(gain);
    const float frequencyNorm = 2.f * static_cast<float>(M_PI) / frequency / static_cast<float>(sampleRate);
    const auto end = outputSize + phaseOffset;

    auto k = 0ul;
    for (auto i = phaseOffset; i < end; ++i, ++k) {
        if constexpr (Accumulate)
            output[k] += static_cast<Type>(std::asin(std::sin(static_cast<float>(i) * frequencyNorm)) * M_2_PI);
        else
            output[k] = static_cast<Type>(std::asin(std::sin(static_cast<float>(i) * frequencyNorm)) * M_2_PI);
    }
}

template<bool Accumulate, typename Type>
inline void Audio::Oscillator::generateSaw(
        Type *output, const std::size_t outputSize,
        const float frequency, const SampleRate sampleRate, const std::uint32_t phaseOffset,
        const Key key, const bool trigger, const DB gain) noexcept
{
    UNUSED(key);
    UNUSED(trigger);
    UNUSED(gain);
    const float frequencyNorm = static_cast<float>(M_PI) / frequency / static_cast<float>(sampleRate);
    const auto end = outputSize + phaseOffset;

    auto k = 0ul;
    for (auto i = phaseOffset; i < end; ++i, ++k) {
        if constexpr (Accumulate)
            output[k] += std::atan(Utils::cot(static_cast<float>(i) * frequencyNorm)) * static_cast<float>(-M_2_PI);
        else
            output[k] = std::atan(Utils::cot(static_cast<float>(i) * frequencyNorm)) * static_cast<float>(-M_2_PI);
    }
}
