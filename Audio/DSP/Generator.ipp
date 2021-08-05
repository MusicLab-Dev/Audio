/**
 * @ Author: Pierre Veysseyre
 * @ Description: Generator implementation
 */

template<bool Accumulate, typename Type>
inline void Audio::DSP::Generator::GenerateWaveform(Waveform waveform, Type *output, const Type *input, const std::size_t outputSize,
        const float frequencyNorm, const std::uint32_t phaseOffset, const DB gain) noexcept
{
    switch (waveform) {
    case Waveform::Sine:
        return GenerateSine<Accumulate>(output, input, outputSize, frequencyNorm, phaseOffset, gain);
    case Waveform::Cosine:
        return GenerateCosine<Accumulate>(output, input, outputSize, frequencyNorm, phaseOffset, gain);
    case Waveform::Square:
        return GenerateSquare<Accumulate>(output, input, outputSize, frequencyNorm, phaseOffset, gain);
    case Waveform::Triangle:
        return GenerateTriangle<Accumulate>(output, input, outputSize, frequencyNorm, phaseOffset, gain);
    case Waveform::Saw:
        return GenerateSaw<Accumulate>(output, input, outputSize, frequencyNorm, phaseOffset, gain);
    case Waveform::Noise:
        return GenerateNoise<Accumulate>(output, input, outputSize, frequencyNorm, phaseOffset, gain);
    default:
        return GenerateSine<Accumulate>(output, input, outputSize, frequencyNorm, phaseOffset, gain);
    }
}

template<bool Accumulate, typename Type>
inline void Audio::DSP::Generator::GenerateWaveform(Waveform waveform, Type *output, const std::size_t outputSize,
        const float frequencyNorm, const std::uint32_t phaseOffset, const DB gain) noexcept
{
    switch (waveform) {
    case Waveform::Sine:
        return GenerateSine<Accumulate>(output, outputSize, frequencyNorm, phaseOffset, gain);
    case Waveform::Cosine:
        return GenerateCosine<Accumulate>(output, outputSize, frequencyNorm, phaseOffset, gain);
    case Waveform::Square:
        return GenerateSquare<Accumulate>(output, outputSize, frequencyNorm, phaseOffset, gain);
    case Waveform::Triangle:
        return GenerateTriangle<Accumulate>(output, outputSize, frequencyNorm, phaseOffset, gain);
    case Waveform::Saw:
        return GenerateSaw<Accumulate>(output, outputSize, frequencyNorm, phaseOffset, gain);
    case Waveform::Noise:
        return GenerateNoise<Accumulate>(output, outputSize, frequencyNorm, phaseOffset, gain);
    default:
        return GenerateSine<Accumulate>(output, outputSize, frequencyNorm, phaseOffset, gain);
    }
}

template<bool Accumulate, typename Type>
inline void Audio::DSP::Generator::GenerateModulateWaveform(Waveform waveform, Type *output, const Type *input, const Type *modulation, const std::size_t outputSize,
        const float frequencyNorm, const std::uint32_t phaseOffset, const DB gain) noexcept
{
    switch (waveform) {
    case Waveform::Sine:
        return GenerateModulateSine<Accumulate>(output, input, modulation, outputSize, frequencyNorm, phaseOffset, gain);
    case Waveform::Cosine:
        return GenerateModulateCosine<Accumulate>(output, input, modulation, outputSize, frequencyNorm, phaseOffset, gain);
    case Waveform::Square:
        return GenerateModulateSquare<Accumulate>(output, input, modulation, outputSize, frequencyNorm, phaseOffset, gain);
    case Waveform::Triangle:
        return GenerateModulateTriangle<Accumulate>(output, input, modulation, outputSize, frequencyNorm, phaseOffset, gain);
    case Waveform::Saw:
        return GenerateModulateSaw<Accumulate>(output, input, modulation, outputSize, frequencyNorm, phaseOffset, gain);
    case Waveform::Noise:
        return GenerateModulateNoise<Accumulate>(output, input, modulation, outputSize, frequencyNorm, phaseOffset, gain);
    default:
        return GenerateModulateSine<Accumulate>(output, input, modulation, outputSize, frequencyNorm, phaseOffset, gain);
    }
}

template<bool Accumulate, typename Type>
inline void Audio::DSP::Generator::GenerateModulateWaveform(Waveform waveform, Type *output, const Type *modulation, const std::size_t outputSize,
        const float frequencyNorm, const std::uint32_t phaseOffset, const DB gain) noexcept
{
    switch (waveform) {
    case Waveform::Sine:
        return GenerateModulateSine<Accumulate>(output, modulation, outputSize, frequencyNorm, phaseOffset, gain);
    case Waveform::Cosine:
        return GenerateModulateCosine<Accumulate>(output, modulation, outputSize, frequencyNorm, phaseOffset, gain);
    case Waveform::Square:
        return GenerateModulateSquare<Accumulate>(output, modulation, outputSize, frequencyNorm, phaseOffset, gain);
    case Waveform::Triangle:
        return GenerateModulateTriangle<Accumulate>(output, modulation, outputSize, frequencyNorm, phaseOffset, gain);
    case Waveform::Saw:
        return GenerateModulateSaw<Accumulate>(output, modulation, outputSize, frequencyNorm, phaseOffset, gain);
    case Waveform::Noise:
        return GenerateModulateNoise<Accumulate>(output, modulation, outputSize, frequencyNorm, phaseOffset, gain);
    default:
        return GenerateModulateSine<Accumulate>(output, modulation, outputSize, frequencyNorm, phaseOffset, gain);
    }
}

template<bool Accumulate, auto ComputeFunc, typename Type>
inline void Audio::DSP::Generator::Internal::GenerateWaveformImpl(
        Type *output, const Type *input, const std::size_t outputSize,
        const float frequencyNorm, const std::uint32_t phaseOffset, const DB gain) noexcept
{
    auto k = 0ul;
    for (auto i = phaseOffset; k < outputSize; ++i, ++k) {
        if constexpr (Accumulate)
            output[k] += input[k] * ComputeFunc(frequencyNorm, i, gain);
        else
            output[k] = input[k] * ComputeFunc(frequencyNorm, i, gain);
    }
}

template<bool Accumulate, auto ComputeFunc, typename Type>
inline void Audio::DSP::Generator::Internal::GenerateWaveformImpl(
        Type *output, const std::size_t outputSize,
        const float frequencyNorm, const std::uint32_t phaseOffset, const DB gain) noexcept
{
    auto k = 0ul;
    for (auto i = phaseOffset; k < outputSize; ++i, ++k) {
        if constexpr (Accumulate)
            output[k] += ComputeFunc(frequencyNorm, i, gain);
        else
            output[k] = ComputeFunc(frequencyNorm, i, gain);
    }
}

template<bool Accumulate, auto ComputeFunc, typename Type>
inline void Audio::DSP::Generator::Internal::GenerateModulateWaveformImpl(
        Type *output, const Type *input, const Type *modulation, const std::size_t outputSize,
        const float frequencyNorm, const std::uint32_t phaseOffset, const DB gain) noexcept
{
    auto k = 0ul;
    for (auto i = phaseOffset; k < outputSize; ++i, ++k) {
        const float mod = modulation[k];
        if constexpr (Accumulate)
            output[k] += input[k] * ComputeFunc(frequencyNorm, mod, i, gain);
        else
            output[k] = input[k] * ComputeFunc(frequencyNorm, modulation[k], i, gain);
    }
}

template<bool Accumulate, auto ComputeFunc, typename Type>
inline void Audio::DSP::Generator::Internal::GenerateModulateWaveformImpl(
        Type *output, const Type *modulation, const std::size_t outputSize,
        const float frequencyNorm, const std::uint32_t phaseOffset, const DB gain) noexcept
{
    auto k = 0ul;
    for (auto i = phaseOffset; k < outputSize; ++i, ++k) {
        if constexpr (Accumulate)
            output[k] += ComputeFunc(frequencyNorm, modulation[k], i, gain);
        else
            output[k] = ComputeFunc(frequencyNorm, modulation[k], i, gain);
    }
}
