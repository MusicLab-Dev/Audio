/**
 * @ Author: Pierre Veysseyre
 * @ Description: Interpolation.ipp
 */

template<typename Type>
inline std::size_t Audio::DSP::Resampler<Type>::GetInterpolationSize(const std::size_t inputSize, const std::size_t interpolationRatio) noexcept
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

template<typename Type>
inline std::size_t Audio::DSP::Resampler<Type>::GetInterpolationOctaveSize(const std::size_t inputSize, const std::uint8_t nOctave) noexcept
{
    return inputSize * (std::pow(2u, nOctave));
}

template<typename Type>
inline void Audio::DSP::Resampler<Type>::Internal::InterpolateOctave(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const std::uint8_t nOctave) noexcept_ndebug
{
    const std::size_t octaveRate = std::pow(2u, nOctave);
    std::cout << "Interpolate octave: " << inputSize << " - " << static_cast<std::size_t>(nOctave) << ", " << octaveRate << std::endl;
    return Interpolate<true>(inputBuffer, outputBuffer, inputSize, octaveRate);
}


#include <iostream>

template<typename Type>
template<bool ProcessFiltering>
inline void Audio::DSP::Resampler<Type>::Interpolate(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const std::size_t interpolationRatio) noexcept_ndebug
{
    std::cout << "Interpolate(" << interpolationRatio << ")::inputSize: " << inputSize << std::endl;

    coreAssert(interpolationRatio > 1,
        throw std::logic_error("DSP::Resampler::Interpolate: interpolationRatio must be greater than 1"));
    const auto interpolationSamples = interpolationRatio - 1;

    auto cpt = 0u;
    for (auto i = 0u; i < inputSize; ++i) {
        outputBuffer[i * interpolationRatio] = inputBuffer[i];
        cpt++;
        // std::cout << "ii:: " << i << std::endl;
        if (i == inputSize - 1)
            continue;
        for (auto k = 0u; k < interpolationSamples; ++k) {
            // std::cout << "k:: " << k << std::endl;
            // This values depend of the Type -> this is the center value for a given type
            outputBuffer[i * interpolationRatio + k + 1] = 0;
            cpt++;
        }
    }

    std::cout << "INTERPOLATE: " << cpt << std::endl;

    // Filter output
    if constexpr (ProcessFiltering) {
        std::cout << "FILTERING INTERPOLATE" << std::endl;
        auto filter = BiquadMaker<BiquadParam::Optimization::Optimized>::MakeBiquad<Type>();
        filter.setupCoefficients(BiquadParam::GenerateCoefficients<
            BiquadParam::FilterType::LowPass
            >(44100, 44100 / 2 / interpolationRatio, 0, 0.707, false));

        std::cout << GetInterpolationSize(inputSize, interpolationRatio) << std::endl;
        filter.processBlock(outputBuffer, GetInterpolationSize(inputSize, interpolationRatio));
        // filter.processBlock(outputBuffer, GetInterpolationSize(inputSize, interpolationRatio));
        // filter.processBlock(outputBuffer, GetInterpolationSize(inputSize, interpolationRatio));
    }
}
