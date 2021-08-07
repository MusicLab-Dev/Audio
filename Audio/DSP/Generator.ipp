/**
 * @ Author: Pierre Veysseyre
 * @ Description: Generator implementation
 */

template<bool Accumulate, auto ComputeFunc, typename Type>
inline void Audio::DSP::Generator::Internal::GenerateImpl(Type *output, const std::size_t outputSize,
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
inline void Audio::DSP::Generator::Internal::GenerateImpl(Type *output, const Type *input, const std::size_t outputSize,
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
inline void Audio::DSP::Generator::Internal::ModulateImpl(
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
inline void Audio::DSP::Generator::Internal::ModulateImpl(
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


template<bool Accumulate, typename ...Args>
inline void Audio::DSP::Generator::Internal::WaveformGenerateHelper(const Waveform waveform, Args &&...args)
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
    default:
        return;
    }
}

template<bool Accumulate, typename ...Args>
inline void Audio::DSP::Generator::Internal::WaveformModulateHelper(const Waveform waveform, Args &&...args)
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
    default:
        return;
    }
}
