/**
 * @ Author: Pierre Veysseyre
 * @ Description: Interpolation.ipp
 */

inline std::size_t Audio::DSP::GetInterpolationSize(const std::size_t inputSize, const std::size_t interpolationRatio) noexcept
{
    std::bitset<64> rate(interpolationRatio);
    constexpr auto GetMaxPower = [](const std::bitset<64> &bitset) -> std::int8_t {
        for (auto i = 0u; i < 64; ++i) {
            if (bitset[64 - i - 1])
                return 64 - i - 1;
        }
        return 0;
    };
    if (rate.count() == 1)
        return GetInterpolationOctaveSize(inputSize, GetMaxPower(rate));
    // std::cout << "GetInterpolationSize: inputSize: " << inputSize << ", interpolationRatio: " << interpolationRatio << std::endl;
    return inputSize + (inputSize - 1) * (interpolationRatio - 1);
}

inline std::size_t Audio::DSP::GetInterpolationOctaveSize(const std::size_t inputSize, const std::uint8_t nOctave) noexcept
{
    return inputSize * (std::pow(2u, nOctave));
}

template<typename Type>
inline void Audio::DSP::Resampler::Internal::InterpolateOctave(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const std::uint8_t nOctave)
{
    // const std::size_t octaveRate = std::pow(2u, nOctave);
    // std::cout << "Interpolate octave: " << inputSize << " - " << static_cast<std::size_t>(nOctave) << ", " << octaveRate << std::endl;
    // return Interpolate(inputBuffer, outputBuffer, inputSize, octaveRate);
}


#include <iostream>

template<typename Type>
inline void Audio::DSP::Resampler::Interpolate(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const std::size_t interpolationRatio)
{
    std::cout << "Interpolate(" << interpolationRatio << ")::inputSize: " << inputSize << std::endl;

    coreAssert(interpolationRatio > 1,
        throw std::logic_error("DSP::Resampler::Interpolate: interpolationRatio must be greater than 1"));
    const auto interpolationSamples = interpolationRatio - 1;

}
