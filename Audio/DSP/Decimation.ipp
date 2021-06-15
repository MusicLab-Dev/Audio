/**
 * @ Author: Pierre Veysseyre
 * @ Description: Decimation.ipp
 */

inline std::size_t Audio::DSP::GetDecimationSize(const std::size_t inputSize, const std::size_t decimationRatio) noexcept
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

inline std::size_t Audio::DSP::GetDecimationOctaveSize(const std::size_t inputSize, const std::uint8_t nOctave) noexcept
{
    return inputSize / (std::pow(2u, nOctave));
}

template<typename Type>
inline void Audio::DSP::Resampler::Internal::DecimateOctave(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const std::uint8_t nOctave)
{
    // // std::cout << "Decimate octave: " << inputSize << " - " << nOctave << std::endl;
    // const auto octaveRate = std::pow(2u, nOctave);
    // return Decimate(inputBuffer, outputBuffer, inputSize, octaveRate);
}

template<typename Type>
inline void Audio::DSP::Resampler::Decimate(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const std::size_t decimationRatio)
{
    // std::cout << "Decimate(" << decimationRatio << ")::inputSize: " << inputSize << std::endl;

    coreAssert(decimationRatio > 1,
        throw std::logic_error("DSP::Resampler::Interpolate: decimationRatio must be greater than 1"));

}
