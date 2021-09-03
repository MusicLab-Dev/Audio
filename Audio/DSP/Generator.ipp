/**
 * @ Author: Pierre Veysseyre
 * @ Description: Generator implementation
 */


template<typename Type>
inline Type Audio::DSP::Generator::Internal::GenerateNoiseSample(const float indexNorm, const float freqNorm, const std::uint32_t index, const DB gain) noexcept
{
    UNUSED(indexNorm);
    UNUSED(freqNorm);
    if constexpr (std::is_signed_v<Type>) {
        if constexpr (std::is_floating_point_v<Type>) {
            return static_cast<Type>(
                gain * (static_cast<double>(Utils::RandomDataSet::FastRand(static_cast<std::size_t>(index))) / static_cast<double>(std::numeric_limits<std::size_t>::max()) - 0.5) * 2.0
            );
        } else {
            return static_cast<Type>(
                gain * static_cast<double>(Utils::RandomDataSet::FastRand(static_cast<std::size_t>(index))) / static_cast<double>(std::numeric_limits<std::size_t>::max()) * 2.0 * static_cast<double>(std::numeric_limits<Type>::max() - static_cast<double>(std::numeric_limits<Type>::max()))
            );
        }
    } else {
        return static_cast<Type>(
            gain * static_cast<double>(Utils::RandomDataSet::FastRand(static_cast<std::size_t>(index))) / static_cast<double>(std::numeric_limits<std::size_t>::max()) * static_cast<double>(std::numeric_limits<Type>::max())
        );
    }
}

template<bool Accumulate, auto ComputeFunc, typename Type>
inline float Audio::DSP::Generator::Internal::GenerateImpl(Type *output, const std::size_t outputSize,
        const float frequencyNorm, const float phaseOffset, const std::uint32_t indexOffset, const DB gain) noexcept
{
    float phase = phaseOffset;
    auto k = indexOffset;

    for (auto i = 0ul; i < outputSize; ++i, ++k) {
        if constexpr (Accumulate)
            output[i] += ComputeFunc(phase, frequencyNorm, k, gain);
        else
            output[i] = ComputeFunc(phase, frequencyNorm, k, gain);
        phase = IncrementPhase(phase, frequencyNorm);
    }
    return phase;
}

template<bool Accumulate, auto ComputeFunc, typename Type>
inline float Audio::DSP::Generator::Internal::GenerateImpl(Type *output, const Type *input, const std::size_t outputSize,
        const float frequencyNorm, const float phaseOffset, const std::uint32_t indexOffset, const DB gain) noexcept
{
    float phase = phaseOffset;
    auto k = indexOffset;

    for (auto i = 0ul; i < outputSize; ++i, ++k) {
        if constexpr (Accumulate)
            output[i] += input[i] * ComputeFunc(phase, frequencyNorm, k, gain);
        else
            output[i] = input[i] * ComputeFunc(phase, frequencyNorm, k, gain);
        phase = IncrementPhase(phase, frequencyNorm);
    }
    return phase;
}

template<bool Accumulate, auto ComputeFunc, typename Type>
inline float Audio::DSP::Generator::Internal::ModulateImpl(
        Type *output, const Type *modulation, const std::size_t outputSize,
        const float frequencyNorm, const float phaseOffset, const std::uint32_t indexOffset, const DB gain) noexcept
{
    float phase = phaseOffset;
    auto k = indexOffset;

    for (auto i = 0ul; i < outputSize; ++i, ++k) {
        if constexpr (Accumulate)
            output[i] += ComputeFunc(phase, frequencyNorm, k, modulation[i], gain);
        else
            output[i] = ComputeFunc(phase, frequencyNorm, k, modulation[i], gain);
        phase = IncrementPhase(phase, frequencyNorm);
    }
    return phase;
}

template<bool Accumulate, auto ComputeFunc, typename Type>
inline float Audio::DSP::Generator::Internal::ModulateImpl(
        Type *output, const Type *input, const Type *modulation, const std::size_t outputSize,
        const float frequencyNorm, const float phaseOffset, const std::uint32_t indexOffset, const DB gain) noexcept
{
    float phase = phaseOffset;
    auto k = indexOffset;

    for (auto i = 0ul; i < outputSize; ++i, ++k) {
        if constexpr (Accumulate)
            output[i] += input[i] * ComputeFunc(phase, frequencyNorm, k, modulation[i], gain);
        else
            output[i] = input[i] * ComputeFunc(phase, frequencyNorm, k, modulation[i], gain);
        phase = IncrementPhase(phase, frequencyNorm);
    }
    return phase;
}

template<bool Accumulate, auto ComputeFunc, typename Type>
inline float Audio::DSP::Generator::Internal::SemitoneShiftImpl(
        Type *output, const Type *semitone, const std::size_t outputSize,
        const float frequencyNorm, const float phaseOffset, const std::uint32_t indexOffset, const DB gain) noexcept
{
    float phase = phaseOffset;
    auto k = indexOffset;

    for (auto i = 0ul; i < outputSize; ++i, ++k) {
        if constexpr (Accumulate)
            output[i] += ComputeFunc(phase, frequencyNorm, k, gain);
        else
            output[i] = ComputeFunc(phase, frequencyNorm, k, gain);
        phase = IncrementPhase(phase, GetNoteFrequencyDelta(frequencyNorm, semitone[i]));
    }
    return phase;
}

template<bool Accumulate, auto ComputeFunc, typename Type>
inline float Audio::DSP::Generator::Internal::SemitoneShiftImpl(
        Type *output, const Type *input, const Type *semitone, const std::size_t outputSize,
        const float frequencyNorm, const float phaseOffset, const std::uint32_t indexOffset, const DB gain) noexcept
{
    float phase = phaseOffset;
    auto k = indexOffset;

    for (auto i = 0ul; i < outputSize; ++i, ++k) {
        if constexpr (Accumulate)
            output[i] += input[i] * ComputeFunc(phase, frequencyNorm, k, gain);
        else
            output[i] = input[i] * ComputeFunc(phase, frequencyNorm, k, gain);
        phase = IncrementPhase(phase, GetNoteFrequencyDelta(frequencyNorm, semitone[i]));
    }
    return phase;
}


template<bool Accumulate, auto ComputeFunc, typename Type>
inline float Audio::DSP::Generator::Internal::ModulateSemitoneShiftImpl(
        Type *output, const Type *modulation, const Type *semitone, const std::size_t outputSize,
        const float frequencyNorm, const float phaseOffset, const std::uint32_t indexOffset, const DB gain) noexcept
{
    float phase = phaseOffset;
    auto k = indexOffset;

    for (auto i = 0ul; i < outputSize; ++i, ++k) {
        if constexpr (Accumulate)
            output[i] += ComputeFunc(phase, frequencyNorm, k, modulation[i], gain);
        else
            output[i] = ComputeFunc(phase, frequencyNorm, k, modulation[i], gain);
        phase = IncrementPhase(phase, GetNoteFrequencyDelta(frequencyNorm, semitone[i]));
    }
    return phase;
}

template<bool Accumulate, auto ComputeFunc, typename Type>
inline float Audio::DSP::Generator::Internal::ModulateSemitoneShiftImpl(
        Type *output, const Type *input, const Type *modulation, const Type *semitone, const std::size_t outputSize,
        const float frequencyNorm, const float phaseOffset, const std::uint32_t indexOffset, const DB gain) noexcept
{
    float phase = phaseOffset;
    auto k = indexOffset;

    for (auto i = 0ul; i < outputSize; ++i, ++k) {
        if constexpr (Accumulate)
            output[i] += input[i] * ComputeFunc(phase, frequencyNorm, k, modulation[i], gain);
        else
            output[i] = input[i] * ComputeFunc(phase, frequencyNorm, k, modulation[i], gain);
        phase = IncrementPhase(phase, GetNoteFrequencyDelta(frequencyNorm, semitone[i]));
    }
    return phase;
}



template<bool Accumulate, typename ...Args>
inline float Audio::DSP::Generator::Internal::WaveformGenerateHelper(const Waveform waveform, Args &&...args)
{
    switch (waveform) {
    case Waveform::Sine:
        return GenerateSine<Accumulate>(std::forward<Args>(args)...);
    case Waveform::Cosine:
        return GenerateCosine<Accumulate>(std::forward<Args>(args)...);
    case Waveform::Square:
        return GenerateSquare<Accumulate>(std::forward<Args>(args)...);
    case Waveform::Triangle:
        return GenerateTriangle<Accumulate>(std::forward<Args>(args)...);
    case Waveform::Saw:
        return GenerateSaw<Accumulate>(std::forward<Args>(args)...);
    case Waveform::Noise:
        return GenerateNoise<Accumulate>(std::forward<Args>(args)...);
    case Waveform::PulseThird:
        return GeneratePulseThird<Accumulate>(std::forward<Args>(args)...);
    case Waveform::PulseQuarter:
        return GeneratePulseQuarter<Accumulate>(std::forward<Args>(args)...);
    case Waveform::SquareAnalog:
        return GenerateSquareAnalog<Accumulate>(std::forward<Args>(args)...);
    case Waveform::SawAnalog:
        return GenerateSawAnalog<Accumulate>(std::forward<Args>(args)...);
    case Waveform::PulseThirdAnalog:
        return GeneratePulseThirdAnalog<Accumulate>(std::forward<Args>(args)...);
    case Waveform::PulseQuarterAnalog:
        return GeneratePulseQuarterAnalog<Accumulate>(std::forward<Args>(args)...);
    default:
        return 0.0f;
    }
}

template<bool Accumulate, typename ...Args>
inline float Audio::DSP::Generator::Internal::WaveformModulateHelper(const Waveform waveform, Args &&...args)
{
    switch (waveform) {
    case Waveform::Sine:
        return ModulateSine<Accumulate>(std::forward<Args>(args)...);
    case Waveform::Cosine:
        return ModulateCosine<Accumulate>(std::forward<Args>(args)...);
    case Waveform::Square:
        return ModulateSquare<Accumulate>(std::forward<Args>(args)...);
    case Waveform::Triangle:
        return ModulateTriangle<Accumulate>(std::forward<Args>(args)...);
    case Waveform::Saw:
        return ModulateSaw<Accumulate>(std::forward<Args>(args)...);
    case Waveform::Noise:
        return ModulateNoise<Accumulate>(std::forward<Args>(args)...);
    case Waveform::PulseThird:
        return ModulatePulseThird<Accumulate>(std::forward<Args>(args)...);
    case Waveform::PulseQuarter:
        return ModulatePulseQuarter<Accumulate>(std::forward<Args>(args)...);
    case Waveform::SquareAnalog:
        return ModulateSquareAnalog<Accumulate>(std::forward<Args>(args)...);
    case Waveform::SawAnalog:
        return ModulateSawAnalog<Accumulate>(std::forward<Args>(args)...);
    case Waveform::PulseThirdAnalog:
        return ModulatePulseThirdAnalog<Accumulate>(std::forward<Args>(args)...);
    case Waveform::PulseQuarterAnalog:
        return ModulatePulseQuarterAnalog<Accumulate>(std::forward<Args>(args)...);
    default:
        return 0.0f;
    }
}

template<bool Accumulate, typename ...Args>
inline float Audio::DSP::Generator::Internal::WaveformSemitoneShiftHelper(const Waveform waveform, Args &&...args)
{
    switch (waveform) {
    case Waveform::Sine:
        return SemitoneShiftSine<Accumulate>(std::forward<Args>(args)...);
    case Waveform::Cosine:
        return SemitoneShiftCosine<Accumulate>(std::forward<Args>(args)...);
    case Waveform::Square:
        return SemitoneShiftSquare<Accumulate>(std::forward<Args>(args)...);
    case Waveform::Triangle:
        return SemitoneShiftTriangle<Accumulate>(std::forward<Args>(args)...);
    case Waveform::Saw:
        return SemitoneShiftSaw<Accumulate>(std::forward<Args>(args)...);
    case Waveform::Noise:
        return SemitoneShiftNoise<Accumulate>(std::forward<Args>(args)...);
    case Waveform::PulseThird:
        return SemitoneShiftPulseThird<Accumulate>(std::forward<Args>(args)...);
    case Waveform::PulseQuarter:
        return SemitoneShiftPulseQuarter<Accumulate>(std::forward<Args>(args)...);
    case Waveform::SquareAnalog:
        return SemitoneShiftSquareAnalog<Accumulate>(std::forward<Args>(args)...);
    case Waveform::SawAnalog:
        return SemitoneShiftSawAnalog<Accumulate>(std::forward<Args>(args)...);
    case Waveform::PulseThirdAnalog:
        return SemitoneShiftPulseThirdAnalog<Accumulate>(std::forward<Args>(args)...);
    case Waveform::PulseQuarterAnalog:
        return SemitoneShiftPulseQuarterAnalog<Accumulate>(std::forward<Args>(args)...);
    default:
        return 0.0f;
    }
}

template<bool Accumulate, typename ...Args>
inline float Audio::DSP::Generator::Internal::WaveformModulateSemitoneShiftHelper(const Waveform waveform, Args &&...args)
{
    switch (waveform) {
    case Waveform::Sine:
        return ModulateSemitoneShiftSine<Accumulate>(std::forward<Args>(args)...);
    case Waveform::Cosine:
        return ModulateSemitoneShiftCosine<Accumulate>(std::forward<Args>(args)...);
    case Waveform::Square:
        return ModulateSemitoneShiftSquare<Accumulate>(std::forward<Args>(args)...);
    case Waveform::Triangle:
        return ModulateSemitoneShiftTriangle<Accumulate>(std::forward<Args>(args)...);
    case Waveform::Saw:
        return ModulateSemitoneShiftSaw<Accumulate>(std::forward<Args>(args)...);
    case Waveform::Noise:
        return ModulateSemitoneShiftNoise<Accumulate>(std::forward<Args>(args)...);
    case Waveform::PulseThird:
        return ModulateSemitoneShiftPulseThird<Accumulate>(std::forward<Args>(args)...);
    case Waveform::PulseQuarter:
        return ModulateSemitoneShiftPulseQuarter<Accumulate>(std::forward<Args>(args)...);
    case Waveform::SquareAnalog:
        return ModulateSemitoneShiftSquareAnalog<Accumulate>(std::forward<Args>(args)...);
    case Waveform::SawAnalog:
        return ModulateSemitoneShiftSawAnalog<Accumulate>(std::forward<Args>(args)...);
    case Waveform::PulseThirdAnalog:
        return ModulateSemitoneShiftPulseThirdAnalog<Accumulate>(std::forward<Args>(args)...);
    case Waveform::PulseQuarterAnalog:
        return ModulateSemitoneShiftPulseQuarterAnalog<Accumulate>(std::forward<Args>(args)...);
    default:
        return 0.0f;
    }
}
