/**
 * @ Author: Pierre Veysseyre
 * @ Description: Oscillator implementation
 */

#include <iomanip>

inline void Audio::Oscillator::onAudioParametersChanged(void)
{
}

inline void Audio::Oscillator::setExternalPaths(const ExternalPaths &paths)
{
}

inline void Audio::Oscillator::sendNotes(const NoteEvents &notes)
{
    _noteManager.feedNotes(notes);
}

inline void Audio::Oscillator::receiveAudio(BufferView output)
{
    const auto outSize = output.size<float>();
    float *out = reinterpret_cast<float *>(output.byteData());

    output.clear();

    // std::cout << "Oscillator::receiveAudio::sampleSize: " << sampleSize << std::endl;
    static auto Cpt = 0ull;

    const auto activeNote = _noteManager.getActiveNote();
    // std::cout << "Receive <audio>: " << activeNote.size() << std::endl;
    for (auto iKey = 0u; iKey < activeNote.size(); ++iKey) {
        const auto key = activeNote[iKey];
        const auto trigger = _noteManager.trigger(key);
        // Phase index
        const auto phaseIndex = _noteManager.readIndex(key);
        const float frequency = std::pow(2.f, static_cast<float>(static_cast<int>(key) - RootKey) / KeysPerOctave) * RootKeyFrequency;
        // const int nextReadIndex = sampleSize - (baseIndex + audioSpecs().processBlockSize);
        // const auto readSize = nextReadIndex < 0 ? outSize + nextReadIndex : outSize;

        // std::cout << "freq: " << frequency << std::endl;
        generateWaveform(_oscillator, out, outSize, frequency, audioSpecs().sampleRate, phaseIndex, key, trigger, 0.2);//1.f / static_cast<float>(activeNote.size()));
        // const auto g = getEnveloppeGain(key, phaseIndex, trigger);
        _noteManager.incrementReadIndex(key, 0ul, audioSpecs().processBlockSize);
        // std::cout << g << std::endl;
        std::cout << "  " << phaseIndex << std::endl;
    }
    // std::cout << "<END>" << std::endl;
    // auto zeros = std::all_of(out, out + outSize * sizeof(float), [](const auto x) { return x == 0.0f; });
    // std::cout << "__" << Cpt << ": " << zeros << std::endl;
    // auto min = 1000.f;
    // auto max = -1000.f;
    // for (auto i = 0ul; i < outSize; ++i) {
    //     min = std::min(out[i], min);
    //     max = std::max(out[i], max);
    // }
    // if (min < -1 || max > 1)
    //     std::cout << "<<<CLIPING>>>" << std::endl;
    // else
    //     std::cout << std::setprecision(4) << "min: " << min << ", max: " << max << std::endl;

    Cpt++;
}

inline void Audio::Oscillator::onAudioGenerationStarted(const BeatRange &range)
{
    _noteManager.reset();
}

template<bool Accumulate, typename Type>
inline void Audio::Oscillator::generateWaveform(
        const Osc &oscillator, Type *output, const std::size_t outputSize,
        const float frequency, const SampleRate sampleRate, const std::size_t phaseOffset,
        const Key key, const bool trigger, const float gain) noexcept
{
    switch (oscillator.waveform) {
    case Osc::Waveform::Sine:
        return generateSine<Accumulate, Type>(output, outputSize, frequency, sampleRate, phaseOffset, key, trigger, gain);
    case Osc::Waveform::Square:
        return generateSquare<Accumulate, Type>(output, outputSize, frequency, sampleRate, phaseOffset, key, trigger, gain);
    case Osc::Waveform::Triangle:
        return generateTriangle<Accumulate, Type>(output, outputSize, frequency, sampleRate, phaseOffset, key, trigger, gain);
    case Osc::Waveform::Saw:
        return generateSaw<Accumulate, Type>(output, outputSize, frequency, sampleRate, phaseOffset, key, trigger, gain);
    default:
        return generateSine<Accumulate, Type>(output, outputSize, frequency, sampleRate, phaseOffset, key, trigger, gain);
    }
}

template<bool Accumulate, typename Type>
inline void Audio::Oscillator::generateSine(
        Type *output, const std::size_t outputSize,
        const float frequency, const SampleRate sampleRate, const std::size_t phaseOffset,
        const Key key, const bool trigger, const float gain) noexcept
{
    const float frequencyNorm = 2.f * M_PI * frequency / sampleRate;
    const auto end = outputSize + phaseOffset;

    float outGain = 1.f;
    for (auto i = phaseOffset, k = 0ul; i < end; ++i, ++k) {
        outGain = getEnveloppeGain(key, i, trigger) * gain;
        if constexpr (Accumulate)
            output[k] += std::sin(i * frequencyNorm) * outGain;
        else
            output[k] = std::sin(i * frequencyNorm) * outGain;
    }

    // (void)_noteManager.enveloppe().adsr<true>(key, phaseOffset, trigger, enveloppeAttack(), enveloppeDecay(), enveloppeSustain(), enveloppeRelease(), audioSpecs().sampleRate);
}

template<bool Accumulate, typename Type>
inline void Audio::Oscillator::generateSquare(
        Type *output, const std::size_t outputSize,
        const float frequency, const SampleRate sampleRate, const std::size_t phaseOffset,
        const Key key, const bool trigger, const float gain) noexcept
{
    const float frequencyNorm = 2.f * M_PI * frequency / sampleRate;
    const auto end = outputSize + phaseOffset;

    for (auto i = phaseOffset, k = 0ul; i < end; ++i, ++k) {
        if constexpr (Accumulate)
            output[k] += std::sin(i * frequencyNorm) > 0.f ? 1.f : -1.f;
        else
            output[k] = std::sin(i * frequencyNorm) > 0.f ? 1.f : -1.f;
    }
}

template<bool Accumulate, typename Type>
inline void Audio::Oscillator::generateTriangle(
        Type *output, const std::size_t outputSize,
        const float frequency, const SampleRate sampleRate, const std::size_t phaseOffset,
        const Key key, const bool trigger, const float gain) noexcept
{
    const float frequencyNorm = 2.f * M_PI / (frequency / sampleRate);
    const auto end = outputSize + phaseOffset;

    for (auto i = phaseOffset, k = 0ul; i < end; ++i, ++k) {
        if constexpr (Accumulate)
            output[k] += std::asin(std::sin(i * frequencyNorm)) * M_2_PI;
        else
            output[k] = std::asin(std::sin(i * frequencyNorm)) * M_2_PI;
    }
}

template<bool Accumulate, typename Type>
inline void Audio::Oscillator::generateSaw(
        Type *output, const std::size_t outputSize,
        const float frequency, const SampleRate sampleRate, const std::size_t phaseOffset,
        const Key key, const bool trigger, const float gain) noexcept
{
    const float frequencyNorm = M_PI / (frequency / sampleRate);
    const auto end = outputSize + phaseOffset;

    for (auto i = phaseOffset, k = 0ul; i < end; ++i, ++k) {
        if constexpr (Accumulate)
            output[k] += std::atan(Utils::cot(i * frequencyNorm)) * -M_2_PI;
        else
            output[k] = std::atan(Utils::cot(i * frequencyNorm)) * -M_2_PI;
    }
}
