/**
 * @ Author: Pierre Veysseyre
 * @ Description: Decimation.ipp
 */

template<typename Type>
inline std::size_t Audio::DSP::Resampler<Type>::GetDecimationSize(const std::size_t inputSize, const std::size_t decimationRatio) noexcept
{
    std::bitset<64> rate(decimationRatio);
    constexpr auto GetMaxPower = [](const std::bitset<64> &bitset) -> std::int8_t {
        for (auto i = 0u; i < 64; ++i) {
            if (bitset[64 - i - 1])
                return 64 - i - 1;
        }
        return 0;
    };
    if (rate.count() == 1)
        return GetDecimationOctaveSize(inputSize, GetMaxPower(rate));
    return std::ceil(static_cast<float>(inputSize) / decimationRatio);
}

template<typename Type>
inline std::size_t Audio::DSP::Resampler<Type>::GetDecimationOctaveSize(const std::size_t inputSize, const std::uint8_t nOctave) noexcept
{
    return inputSize / (std::pow(2u, nOctave));
}

template<typename Type>
inline void Audio::DSP::Resampler<Type>::Internal::DecimateOctave(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const std::uint8_t nOctave)
{
    // std::cout << "Decimate octave: " << inputSize << " - " << nOctave << std::endl;
    const auto octaveRate = std::pow(2u, nOctave);
    return Decimate<false>(inputBuffer, outputBuffer, inputSize, octaveRate);
}

template<typename Type>
template<bool ProcessFiltering>
inline void Audio::DSP::Resampler<Type>::Decimate(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const std::size_t decimationRatio)
{
    std::cout << "Decimate(" << decimationRatio << ")::inputSize: " << inputSize << std::endl;

    coreAssert(decimationRatio > 1,
        throw std::logic_error("DSP::Resampler::Interpolate: decimationRatio must be greater than 1"));

    const std::size_t samples = inputSize / decimationRatio;
    const auto lastSample = std::ceil(inputSize / decimationRatio) != samples;
    for (auto i = 0u; i < samples; ++i) {
        outputBuffer[i] = inputBuffer[i * decimationRatio];
    }
    if (samples)
        outputBuffer[samples] = 0;

    // Filter output
    if constexpr (ProcessFiltering) {
        std::cout << "FILTERING DECIMATE" << std::endl;
        auto filter = BiquadMaker<BiquadParam::Optimization::Optimized>::MakeBiquad<Type>();
        filter.setupCoefficients(BiquadParam::GenerateCoefficients<
            BiquadParam::FilterType::LowPass
            >(44100, 44100 / 2, 0, 0.707, false));

        filter.processBlock(outputBuffer, GetDecimationSize(inputSize, decimationRatio));
        // filter.resetRegisters();
        // filter.processBlock(outputBuffer, GetDecimationSize(inputSize, decimationRatio));
    }
}
