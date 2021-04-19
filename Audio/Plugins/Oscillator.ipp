/**
 * @ Author: Pierre Veysseyre
 * @ Description: Oscillator implementation
 */


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


    // std::cout << "Oscillator::receiveAudio::sampleSize: " << sampleSize << std::endl;

    const auto activeNote = _noteManager.getActiveNote();
    for (auto iKey = 0u; iKey < activeNote.size(); ++iKey) {
        const auto key = activeNote[iKey];
        const auto trigger = _noteManager.trigger(key);
        // Phase index
        const auto phaseIndex = _noteManager.readIndex(key);
        const float frequency = std::pow(2.f, static_cast<float>(static_cast<int>(key) - RootKey) / KeysPerOctave) * RootKeyFrequency;
        // const int nextReadIndex = sampleSize - (baseIndex + audioSpecs().processBlockSize);
        // const auto readSize = nextReadIndex < 0 ? outSize + nextReadIndex : outSize;

        generateWaveform(_oscillator, out, outSize, frequency, audioSpecs().sampleRate, phaseIndex, key, trigger);
        _noteManager.incrementReadIndex(key, 0ul, audioSpecs().processBlockSize);
        // std::cout << trigger << std::endl;
}
    // std::cout << "<END>" << std::endl;

}

inline void Audio::Oscillator::onAudioGenerationStarted(const BeatRange &range)
{
    _noteManager.reset();
}

template<typename Type>
inline void Audio::Oscillator::generateWaveform(
        const Osc &oscillator, Type *output, const std::size_t outputSize,
        const float frequency, const SampleRate sampleRate, const std::size_t phaseOffset,
        const Key key, const bool trigger) noexcept
{
    switch (oscillator.waveform) {
    case Osc::Waveform::Sine:
        return generateSine<Type>(output, outputSize, frequency, sampleRate, phaseOffset, key, trigger);
    case Osc::Waveform::Square:
        return generateSquare<Type>(output, outputSize, frequency, sampleRate, phaseOffset, key, trigger);
    case Osc::Waveform::Triangle:
        return generateTriangle<Type>(output, outputSize, frequency, sampleRate, phaseOffset, key, trigger);
    case Osc::Waveform::Saw:
        return generateSaw<Type>(output, outputSize, frequency, sampleRate, phaseOffset, key, trigger);
    default:
        return generateSine<Type>(output, outputSize, frequency, sampleRate, phaseOffset, key, trigger);
    }
}

template<typename Type>
inline void Audio::Oscillator::generateSine(
        Type *output, const std::size_t outputSize,
        const float frequency, const SampleRate sampleRate, const std::size_t phaseOffset,
        const Key key, const bool trigger) noexcept
{
    const float frequencyNorm = 2.f * M_PI * frequency / sampleRate;
    const auto end = outputSize + phaseOffset;

    float gain = 1.f;
    for (auto i = phaseOffset, k = 0ul; i < end; ++i, ++k) {
        gain = getEnveloppeGain(key, i, trigger);
        output[k] = std::sin(i * frequencyNorm) * gain;
    }
}

template<typename Type>
inline void Audio::Oscillator::generateSquare(
        Type *output, const std::size_t outputSize,
        const float frequency, const SampleRate sampleRate, const std::size_t phaseOffset,
        const Key key, const bool trigger) noexcept
{
    const float frequencyNorm = 2.f * M_PI * frequency / sampleRate;
    const auto end = outputSize + phaseOffset;

    for (auto i = phaseOffset, k = 0ul; i < end; ++i, ++k) {
        output[k] = std::sin(i * frequencyNorm) > 0.f ? 1.f : -1.f;
    }
}

template<typename Type>
inline void Audio::Oscillator::generateTriangle(
        Type *output, const std::size_t outputSize,
        const float frequency, const SampleRate sampleRate, const std::size_t phaseOffset,
        const Key key, const bool trigger) noexcept
{
    const float frequencyNorm = 2.f * M_PI / (frequency / sampleRate);
    const auto end = outputSize + phaseOffset;

    for (auto i = phaseOffset, k = 0ul; i < end; ++i, ++k) {
        output[k] = std::asin(std::sin(i * frequencyNorm)) * M_2_PI;
    }
}

template<typename Type>
inline void Audio::Oscillator::generateSaw(
        Type *output, const std::size_t outputSize,
        const float frequency, const SampleRate sampleRate, const std::size_t phaseOffset,
        const Key key, const bool trigger) noexcept
{
    const float frequencyNorm = M_PI / (frequency / sampleRate);
    const auto end = outputSize + phaseOffset;

    for (auto i = phaseOffset, k = 0ul; i < end; ++i, ++k) {
        output[k] = std::atan(Utils::cot(i * frequencyNorm)) * -M_2_PI;
    }
}
