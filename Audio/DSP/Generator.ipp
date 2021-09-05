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

template<bool Accumulate, Audio::ChannelArrangement Channels, auto ComputeFunc, typename Type>
inline float Audio::DSP::Generator::Internal::GenerateImpl(Type *output, const std::size_t outputSize,
        const float frequencyNorm, const float phaseOffset, const std::uint32_t indexOffset, const DB gain) noexcept
{
    constexpr auto Stride = Channels == ChannelArrangement::All ? static_cast<std::size_t>(ChannelArrangement::Stereo) : static_cast<std::size_t>(Channels);

    float phase = phaseOffset;
    auto k = indexOffset;

    for (auto i = 0ul; i < outputSize; ++i, ++k, output += Stride) {
        if constexpr (Accumulate) {
            *output += ComputeFunc(phase, frequencyNorm, k, gain);
            if constexpr (Channels == ChannelArrangement::All)
                *(output + 1u) += *output;
        } else {
            *output = ComputeFunc(phase, frequencyNorm, k, gain);
            if constexpr (Channels == ChannelArrangement::All)
                *(output + 1u) = *output;
        }
        phase = IncrementPhase(phase, frequencyNorm);
    }
    return phase;
}

template<bool Accumulate, Audio::ChannelArrangement Channels, auto ComputeFunc, typename Type>
inline float Audio::DSP::Generator::Internal::GenerateImpl(Type *output, const Type *input, const std::size_t outputSize,
        const float frequencyNorm, const float phaseOffset, const std::uint32_t indexOffset, const DB gain) noexcept
{
    constexpr auto Stride = Channels == ChannelArrangement::All ? static_cast<std::size_t>(ChannelArrangement::Stereo) : static_cast<std::size_t>(Channels);

    float phase = phaseOffset;
    auto k = indexOffset;

    for (auto i = 0ul; i < outputSize; ++i, ++k, output += Stride) {
        if constexpr (Accumulate) {
            *output += input[i] * ComputeFunc(phase, frequencyNorm, k, gain);
            if constexpr (Channels == ChannelArrangement::All)
                *(output + 1u) += *output;
        } else {
            *output = input[i] * ComputeFunc(phase, frequencyNorm, k, gain);
            if constexpr (Channels == ChannelArrangement::All)
                *(output + 1u) = *output;
        }
        phase = IncrementPhase(phase, frequencyNorm);
    }
    return phase;
}

template<bool Accumulate, Audio::ChannelArrangement Channels, auto ComputeFunc, typename Type>
inline float Audio::DSP::Generator::Internal::ModulateImpl(
        Type *output, const Type *modulation, const std::size_t outputSize,
        const float frequencyNorm, const float phaseOffset, const std::uint32_t indexOffset, const DB gain) noexcept
{
    constexpr auto Stride = Channels == ChannelArrangement::All ? static_cast<std::size_t>(ChannelArrangement::Stereo) : static_cast<std::size_t>(Channels);

    float phase = phaseOffset;
    auto k = indexOffset;

    for (auto i = 0ul; i < outputSize; ++i, ++k, output += Stride) {
        if constexpr (Accumulate) {
            *output += ComputeFunc(phase, frequencyNorm, k, modulation[i], gain);
            if constexpr (Channels == ChannelArrangement::All)
                *(output + 1u) += *output;
        } else {
            *output = ComputeFunc(phase, frequencyNorm, k, modulation[i], gain);
            if constexpr (Channels == ChannelArrangement::All)
                *(output + 1u) = *output;
        }
        phase = IncrementPhase(phase, frequencyNorm);
    }
    return phase;
}

template<bool Accumulate, Audio::ChannelArrangement Channels, auto ComputeFunc, typename Type>
inline float Audio::DSP::Generator::Internal::ModulateImpl(
        Type *output, const Type *input, const Type *modulation, const std::size_t outputSize,
        const float frequencyNorm, const float phaseOffset, const std::uint32_t indexOffset, const DB gain) noexcept
{
    constexpr auto Stride = Channels == ChannelArrangement::All ? static_cast<std::size_t>(ChannelArrangement::Stereo) : static_cast<std::size_t>(Channels);

    float phase = phaseOffset;
    auto k = indexOffset;

    for (auto i = 0ul; i < outputSize; ++i, ++k, output += Stride) {
        if constexpr (Accumulate) {
            *output += input[i] * ComputeFunc(phase, frequencyNorm, k, modulation[i], gain);
            if constexpr (Channels == ChannelArrangement::All)
                *(output + 1u) += *output;
        } else {
            *output = input[i] * ComputeFunc(phase, frequencyNorm, k, modulation[i], gain);
            if constexpr (Channels == ChannelArrangement::All)
                *(output + 1u) = *output;
        }
        phase = IncrementPhase(phase, frequencyNorm);
    }
    return phase;
}

template<bool Accumulate, Audio::ChannelArrangement Channels, auto ComputeFunc, typename Type>
inline float Audio::DSP::Generator::Internal::SemitoneShiftImpl(
        Type *output, const Type *semitone, const std::size_t outputSize,
        const float frequencyNorm, const float phaseOffset, const std::uint32_t indexOffset, const DB gain) noexcept
{
    constexpr auto Stride = Channels == ChannelArrangement::All ? static_cast<std::size_t>(ChannelArrangement::Stereo) : static_cast<std::size_t>(Channels);

    float phase = phaseOffset;
    auto k = indexOffset;

    for (auto i = 0ul; i < outputSize; ++i, ++k, output += Stride) {
        if constexpr (Accumulate) {
            *output += ComputeFunc(phase, frequencyNorm, k, gain);
            if constexpr (Channels == ChannelArrangement::All)
                *(output + 1u) += *output;
        } else {
            *output = ComputeFunc(phase, frequencyNorm, k, gain);
            if constexpr (Channels == ChannelArrangement::All)
                *(output + 1u) = *output;
        }
        phase = IncrementPhase(phase, GetNoteFrequencyDelta(frequencyNorm, semitone[i]));
    }
    return phase;
}

template<bool Accumulate, Audio::ChannelArrangement Channels, auto ComputeFunc, typename Type>
inline float Audio::DSP::Generator::Internal::SemitoneShiftImpl(
        Type *output, const Type *input, const Type *semitone, const std::size_t outputSize,
        const float frequencyNorm, const float phaseOffset, const std::uint32_t indexOffset, const DB gain) noexcept
{
    constexpr auto Stride = Channels == ChannelArrangement::All ? static_cast<std::size_t>(ChannelArrangement::Stereo) : static_cast<std::size_t>(Channels);

    float phase = phaseOffset;
    auto k = indexOffset;

    for (auto i = 0ul; i < outputSize; ++i, ++k, output += Stride) {
        if constexpr (Accumulate) {
            *output += input[i] * ComputeFunc(phase, frequencyNorm, k, gain);
            if constexpr (Channels == ChannelArrangement::All)
                *(output + 1u) += *output;
        } else {
            *output = input[i] * ComputeFunc(phase, frequencyNorm, k, gain);
            if constexpr (Channels == ChannelArrangement::All)
                *(output + 1u) = *output;
        }
        phase = IncrementPhase(phase, GetNoteFrequencyDelta(frequencyNorm, semitone[i]));
    }
    return phase;
}


template<bool Accumulate, Audio::ChannelArrangement Channels, auto ComputeFunc, typename Type>
inline float Audio::DSP::Generator::Internal::ModulateSemitoneShiftImpl(
        Type *output, const Type *modulation, const Type *semitone, const std::size_t outputSize,
        const float frequencyNorm, const float phaseOffset, const std::uint32_t indexOffset, const DB gain) noexcept
{
    constexpr auto Stride = Channels == ChannelArrangement::All ? static_cast<std::size_t>(ChannelArrangement::Stereo) : static_cast<std::size_t>(Channels);

    float phase = phaseOffset;
    auto k = indexOffset;

    for (auto i = 0ul; i < outputSize; ++i, ++k, output += Stride) {
        if constexpr (Accumulate) {
            *output += ComputeFunc(phase, frequencyNorm, k, modulation[i], gain);
            if constexpr (Channels == ChannelArrangement::All)
                *(output + 1u) += *output;
        } else {
            *output = ComputeFunc(phase, frequencyNorm, k, modulation[i], gain);
            if constexpr (Channels == ChannelArrangement::All)
                *(output + 1u) = *output;
        }
        phase = IncrementPhase(phase, GetNoteFrequencyDelta(frequencyNorm, semitone[i]));
    }
    return phase;
}

template<bool Accumulate, Audio::ChannelArrangement Channels, auto ComputeFunc, typename Type>
inline float Audio::DSP::Generator::Internal::ModulateSemitoneShiftImpl(
        Type *output, const Type *input, const Type *modulation, const Type *semitone, const std::size_t outputSize,
        const float frequencyNorm, const float phaseOffset, const std::uint32_t indexOffset, const DB gain) noexcept
{
    constexpr auto Stride = Channels == ChannelArrangement::All ? static_cast<std::size_t>(ChannelArrangement::Stereo) : static_cast<std::size_t>(Channels);

    float phase = phaseOffset;
    auto k = indexOffset;

    for (auto i = 0ul; i < outputSize; ++i, ++k, output += Stride) {
        if constexpr (Accumulate) {
            *output += input[i] * ComputeFunc(phase, frequencyNorm, k, modulation[i], gain);
            if constexpr (Channels == ChannelArrangement::All)
                *(output + 1u) += *output;
        } else {
            *output = input[i] * ComputeFunc(phase, frequencyNorm, k, modulation[i], gain);
            if constexpr (Channels == ChannelArrangement::All)
                *(output + 1u) = *output;
        }
        phase = IncrementPhase(phase, GetNoteFrequencyDelta(frequencyNorm, semitone[i]));
    }
    return phase;
}



template<bool Accumulate, Audio::ChannelArrangement Channels, typename ...Args>
inline float Audio::DSP::Generator::Internal::WaveformGenerateHelper(const Waveform waveform, Args &&...args)
{
    switch (waveform) {
    case Waveform::Sine:
        return GenerateSine<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::Cosine:
        return GenerateCosine<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::Square:
        return GenerateSquare<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::Triangle:
        return GenerateTriangle<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::Saw:
        return GenerateSaw<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::Noise:
        return GenerateNoise<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::PulseThird:
        return GeneratePulseThird<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::PulseQuarter:
        return GeneratePulseQuarter<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::SquareAnalog:
        return GenerateSquareAnalog<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::SawAnalog:
        return GenerateSawAnalog<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::PulseThirdAnalog:
        return GeneratePulseThirdAnalog<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::PulseQuarterAnalog:
        return GeneratePulseQuarterAnalog<Accumulate, Channels>(std::forward<Args>(args)...);
    default:
        return 0.0f;
    }
}

template<bool Accumulate, Audio::ChannelArrangement Channels, typename ...Args>
inline float Audio::DSP::Generator::Internal::WaveformModulateHelper(const Waveform waveform, Args &&...args)
{
    switch (waveform) {
    case Waveform::Sine:
        return ModulateSine<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::Cosine:
        return ModulateCosine<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::Square:
        return ModulateSquare<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::Triangle:
        return ModulateTriangle<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::Saw:
        return ModulateSaw<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::Noise:
        return ModulateNoise<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::PulseThird:
        return ModulatePulseThird<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::PulseQuarter:
        return ModulatePulseQuarter<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::SquareAnalog:
        return ModulateSquareAnalog<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::SawAnalog:
        return ModulateSawAnalog<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::PulseThirdAnalog:
        return ModulatePulseThirdAnalog<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::PulseQuarterAnalog:
        return ModulatePulseQuarterAnalog<Accumulate, Channels>(std::forward<Args>(args)...);
    default:
        return 0.0f;
    }
}

template<bool Accumulate, Audio::ChannelArrangement Channels, typename ...Args>
inline float Audio::DSP::Generator::Internal::WaveformSemitoneShiftHelper(const Waveform waveform, Args &&...args)
{
    switch (waveform) {
    case Waveform::Sine:
        return SemitoneShiftSine<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::Cosine:
        return SemitoneShiftCosine<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::Square:
        return SemitoneShiftSquare<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::Triangle:
        return SemitoneShiftTriangle<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::Saw:
        return SemitoneShiftSaw<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::Noise:
        return SemitoneShiftNoise<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::PulseThird:
        return SemitoneShiftPulseThird<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::PulseQuarter:
        return SemitoneShiftPulseQuarter<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::SquareAnalog:
        return SemitoneShiftSquareAnalog<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::SawAnalog:
        return SemitoneShiftSawAnalog<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::PulseThirdAnalog:
        return SemitoneShiftPulseThirdAnalog<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::PulseQuarterAnalog:
        return SemitoneShiftPulseQuarterAnalog<Accumulate, Channels>(std::forward<Args>(args)...);
    default:
        return 0.0f;
    }
}

template<bool Accumulate, Audio::ChannelArrangement Channels, typename ...Args>
inline float Audio::DSP::Generator::Internal::WaveformModulateSemitoneShiftHelper(const Waveform waveform, Args &&...args)
{
    switch (waveform) {
    case Waveform::Sine:
        return ModulateSemitoneShiftSine<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::Cosine:
        return ModulateSemitoneShiftCosine<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::Square:
        return ModulateSemitoneShiftSquare<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::Triangle:
        return ModulateSemitoneShiftTriangle<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::Saw:
        return ModulateSemitoneShiftSaw<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::Noise:
        return ModulateSemitoneShiftNoise<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::PulseThird:
        return ModulateSemitoneShiftPulseThird<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::PulseQuarter:
        return ModulateSemitoneShiftPulseQuarter<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::SquareAnalog:
        return ModulateSemitoneShiftSquareAnalog<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::SawAnalog:
        return ModulateSemitoneShiftSawAnalog<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::PulseThirdAnalog:
        return ModulateSemitoneShiftPulseThirdAnalog<Accumulate, Channels>(std::forward<Args>(args)...);
    case Waveform::PulseQuarterAnalog:
        return ModulateSemitoneShiftPulseQuarterAnalog<Accumulate, Channels>(std::forward<Args>(args)...);
    default:
        return 0.0f;
    }
}
