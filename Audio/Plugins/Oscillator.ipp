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

inline void Audio::Oscillator::sendNotes(const NoteEvents &notes, const BeatRange &range)
{
    UNUSED(range);
    if (notes.size()) {
        // std::cout << range << std::endl;
        _noteManager.feedNotes(notes);
    }
}

inline void Audio::Oscillator::receiveAudio(BufferView output)
{
    const DB outGain = ConvertDecibelToRatio(static_cast<float>(outputVolume()) + DefaultVoiceGain);
    const auto outSize = static_cast<std::uint32_t>(output.size<float>());
    float *out = reinterpret_cast<float *>(output.byteData());
    // const bool noRelease = (enveloppeRelease() <= EnvelopeMinTimeStep);
    // const bool noRelease = !enveloppeRelease();

    _noteManager.processNotes(
        [this, outGain, outSize, out](const Key key, const std::uint32_t readIndex, const NoteModifiers &modifiers) -> std::pair<std::uint32_t, std::uint32_t> {
            const float frequency = std::pow(2.f, static_cast<float>(static_cast<int>(key) - RootKey) / KeysPerOctave) * RootKeyFrequency;
            auto realOut = out;
            auto realOutSize = outSize;
            if (modifiers.sampleOffset && !readIndex) {
                realOut += modifiers.sampleOffset;
                realOutSize -= modifiers.sampleOffset;
            }
            UNUSED(modifiers);
            generateWaveform<true>(static_cast<Osc::Waveform>(waveform()), realOut, realOutSize, frequency, audioSpecs().sampleRate, readIndex, key, outGain);
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
    //         getEnvelopeGain(key, static_cast<std::uint32_t>(phaseIndex));
    //     }
    //     return sample * outGain;
    //         // std::sin(static_cast<float>(index) * frequencyNorm) *
    //         // getEnvelopeGain(key, static_cast<std::uint32_t>(index)) * gain;
    // });

}

template<bool Accumulate, typename Type>
inline void Audio::Oscillator::generateWaveform(
        const Osc::Waveform waveform, Type *output, const std::size_t outputSize,
        const float frequency, const SampleRate sampleRate, const std::uint32_t phaseOffset,
        const Key key, const DB gain) noexcept
{
    switch (waveform) {
    case Osc::Waveform::Sine:
        return generateSine<Accumulate>(output, outputSize, frequency, sampleRate, phaseOffset, key, gain);
    case Osc::Waveform::Cosine:
        return generateCosine<Accumulate>(output, outputSize, frequency, sampleRate, phaseOffset, key, gain);
    case Osc::Waveform::Square:
        return generateSquare<Accumulate>(output, outputSize, frequency, sampleRate, phaseOffset, key, gain);
    case Osc::Waveform::Triangle:
        return generateTriangle<Accumulate>(output, outputSize, frequency, sampleRate, phaseOffset, key, gain);
    case Osc::Waveform::Saw:
        return generateSaw<Accumulate>(output, outputSize, frequency, sampleRate, phaseOffset, key, gain);
    case Osc::Waveform::Noise:
        return generateNoise<Accumulate>(output, outputSize, frequency, sampleRate, phaseOffset, key, gain);
    case Osc::Waveform::Error:
        return generateError<Accumulate>(output, outputSize, frequency, sampleRate, phaseOffset, key, gain);
    default:
        return generateSine<Accumulate>(output, outputSize, frequency, sampleRate, phaseOffset, key, gain);
    }
}

template<bool Accumulate, typename Type>
inline void Audio::Oscillator::generateSine(
        Type *output, const std::size_t outputSize,
        const float frequency, const SampleRate sampleRate, const std::uint32_t phaseOffset,
        const Key key, const DB gain) noexcept
{
    const float frequencyNorm = 2.f * static_cast<float>(M_PI) * frequency / static_cast<float>(sampleRate);

    float outGain = 1.f;
    auto k = 0ul;
    for (auto i = phaseOffset; k < outputSize; ++i, ++k) {
        outGain = getEnvelopeGain(key, i) * gain;
        if constexpr (Accumulate)
            output[k] += static_cast<Type>(std::sin(static_cast<float>(i) * frequencyNorm) * outGain);
        else
            output[k] = static_cast<Type>(std::sin(static_cast<float>(i) * frequencyNorm) * outGain);
    }


    // Functor equivalent
    // Modifier<Type>::ApplyIndexFunctor(output, outputSize, phaseOffset, [&, key, frequencyNorm, gain](const std::size_t index) -> Type {
    //     return static_cast<Type>(
    //         std::sin(static_cast<float>(index) * frequencyNorm) *
    //         getEnvelopeGain(key, static_cast<std::uint32_t>(index)) * gain
    //     );
    // });

    // (void)_noteManager.enveloppe().adsr<true>(key, phaseOffset, enveloppeAttack(), enveloppeDecay(), enveloppeSustain(), enveloppeRelease(), audioSpecs().sampleRate);
}

template<bool Accumulate, typename Type>
inline void Audio::Oscillator::generateCosine(
        Type *output, const std::size_t outputSize,
        const float frequency, const SampleRate sampleRate, const std::uint32_t phaseOffset,
        const Key key, const DB gain) noexcept
{
    const float frequencyNorm = 2.f * static_cast<float>(M_PI) * frequency / static_cast<float>(sampleRate);

    float outGain = 1.f;
    auto k = 0ul;
    for (auto i = phaseOffset; k < outputSize; ++i, ++k) {
        outGain = getEnvelopeGain(key, i) * gain;
        if constexpr (Accumulate)
            output[k] += static_cast<Type>(std::cos(static_cast<float>(i) * frequencyNorm) * outGain);
        else
            output[k] = static_cast<Type>(std::cos(static_cast<float>(i) * frequencyNorm) * outGain);
    }

}

template<bool Accumulate, typename Type>
inline void Audio::Oscillator::generateSquare(
        Type *output, const std::size_t outputSize,
        const float frequency, const SampleRate sampleRate, const std::uint32_t phaseOffset,
        const Key key, const DB gain) noexcept
{
    const float frequencyNorm = 2.f * static_cast<float>(M_PI) * frequency / static_cast<float>(sampleRate);

    float outGain = 1.f;
    auto k = 0ul;
    for (auto i = phaseOffset; k < outputSize; ++i, ++k) {
        outGain = getEnvelopeGain(key, i) * gain;
        if constexpr (Accumulate)
            output[k] += std::sin(static_cast<float>(i) * frequencyNorm) > 0.f ? outGain : -outGain;
        else
            output[k] = std::sin(static_cast<float>(i) * frequencyNorm) > 0.f ? outGain : -outGain;
    }
}

template<bool Accumulate, typename Type>
inline void Audio::Oscillator::generateTriangle(
        Type *output, const std::size_t outputSize,
        const float frequency, const SampleRate sampleRate, const std::uint32_t phaseOffset,
        const Key key, const DB gain) noexcept
{
    const float frequencyNorm = static_cast<float>(M_PI) * frequency / static_cast<float>(sampleRate);

    float outGain = 1.f;
    auto k = 0ul;
    for (auto i = phaseOffset; k < outputSize; ++i, ++k) {
        outGain = getEnvelopeGain(key, i) * gain;
        if constexpr (Accumulate)
            output[k] += (1.0f - std::acos(std::cos(2.0f * static_cast<float>(i) * frequencyNorm)) * static_cast<float>(M_2_PI)) * outGain;
        else
            output[k] = (1.0f - std::acos(std::cos(2.0f * static_cast<float>(i) * frequencyNorm)) * static_cast<float>(M_2_PI)) * outGain;
    }
}

template<bool Accumulate, typename Type>
inline void Audio::Oscillator::generateSaw(
        Type *output, const std::size_t outputSize,
        const float frequency, const SampleRate sampleRate, const std::uint32_t phaseOffset,
        const Key key, const DB gain) noexcept
{
    const float frequencyNorm = static_cast<float>(M_PI) * frequency / static_cast<float>(sampleRate);

    float outGain = 1.f;
    auto k = 0ul;
    for (auto i = phaseOffset; k < outputSize; ++i, ++k) {
        outGain = getEnvelopeGain(key, i) * gain;
        if constexpr (Accumulate)
            output[k] += -std::atan(Utils::cot(static_cast<float>(i) * frequencyNorm)) * static_cast<float>(M_2_PI) * outGain;
        else
            output[k] = -std::atan(Utils::cot(static_cast<float>(i) * frequencyNorm)) * static_cast<float>(M_2_PI) * outGain;
    }
}

template<bool Accumulate, typename Type>
inline void Audio::Oscillator::generateError(
        Type *output, const std::size_t outputSize,
        const float frequency, const SampleRate sampleRate, const std::uint32_t phaseOffset,
        const Key key, const DB gain) noexcept
{
    UNUSED(sampleRate);
    const float frequencyNorm = static_cast<float>(M_PI) * frequency;// / static_cast<float>(sampleRate);

    float outGain = 1.f;
    auto k = 0ul;
    for (auto i = phaseOffset; k < outputSize; ++i, ++k) {
        outGain = getEnvelopeGain(key, i) * gain;
        if constexpr (Accumulate)
            output[k] += -std::atan(Utils::cot(static_cast<float>(i) * frequencyNorm)) * static_cast<float>(M_2_PI) * outGain;
        else
            output[k] = -std::atan(Utils::cot(static_cast<float>(i) * frequencyNorm)) * static_cast<float>(M_2_PI) * outGain;
    }
}

template<bool Accumulate, typename Type>
inline void Audio::Oscillator::generateNoise(
        Type *output, const std::size_t outputSize,
        const float frequency, const SampleRate sampleRate, const std::uint32_t phaseOffset,
        const Key key, const DB gain) noexcept
{
    UNUSED(frequency);
    UNUSED(sampleRate);

    float outGain = 1.f;
    auto k = 0ul;
    for (auto i = phaseOffset; k < outputSize; ++i, ++k) {
        outGain = getEnvelopeGain(key, i) * gain;
        if constexpr (Accumulate)
            output[k] += static_cast<Type>(static_cast<int>(Utils::fastRand()) - std::numeric_limits<int>::max()) / static_cast<Type>(std::numeric_limits<int>::max()) * outGain;
        else
            output[k] = static_cast<Type>(static_cast<int>(Utils::fastRand()) - std::numeric_limits<int>::max()) / static_cast<Type>(std::numeric_limits<int>::max()) * outGain;
    }
    // auto rnd = Utils::fastRand();
    // auto rnd1 = static_cast<int>(Utils::fastRand());
    // auto rnd2 = static_cast<Type>(static_cast<int>(Utils::fastRand()) - std::numeric_limits<int>::max());
    // auto rnd3 = static_cast<Type>(static_cast<int>(Utils::fastRand()) - std::numeric_limits<int>::max()) / static_cast<Type>(std::numeric_limits<int>::max());
    // UNUSED(rnd);
    // UNUSED(rnd1);
    // UNUSED(rnd2);
    // UNUSED(rnd3);
}
