/**
 * @ Author: Pierre V
 * @ Description: Resampler
 */

#include <numeric>
#include <bitset>

#include <Core/Assert.hpp>

template<typename Type>
inline std::size_t Audio::DSP::Resampler<Type>::GetOptimalResamplingSize(const std::size_t inputSize, const Semitone semitone) noexcept
{
    const auto rate = std::pow(2.0, -semitone / 12.0);
    return std::ceil(inputSize * rate);
}

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
    return inputSize + (inputSize - 1) * (interpolationRatio - 1);
}

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
inline std::size_t Audio::DSP::Resampler<Type>::GetInterpolationOctaveSize(const std::size_t inputSize, const std::uint8_t nOctave) noexcept
{
    return inputSize * (std::pow(2u, nOctave));
}

template<typename Type>
inline std::size_t Audio::DSP::Resampler<Type>::GetDecimationOctaveSize(const std::size_t inputSize, const std::uint8_t nOctave) noexcept
{
    return inputSize / (std::pow(2u, nOctave));
}

template<typename Type>
inline std::size_t Audio::DSP::Resampler<Type>::GetResamplingSizeSemitone(const std::size_t inputSize, const Semitone semitone) noexcept
{
    if (!semitone)
        return inputSize;
    const auto nOctave = semitone / 12;
    const auto nSemitone = std::abs(semitone) % 12;
    const auto iFactor = (semitone > 0) ? L_Factor : M_Factor;
    const auto dFactor = (semitone > 0) ? M_Factor : L_Factor;

    std::size_t newSize = GetResamplingSizeOctave(inputSize, nOctave);
    // std::cout << "iFactor: " << iFactor << std::endl;
    // std::cout << "dFactor: " << dFactor << std::endl;
    // std::cout << "nSemi: " << nSemitone << std::endl;
    // std::cout << "nOctave: " << nOctave << std::endl;
    // std::cout << "newSize: " << newSize << std::endl;
    for (auto i = 0u; i < nSemitone; ++i) {
        // Interpolation
        newSize = GetInterpolationSize(newSize, iFactor);
        // std::cout << "newSize: " << newSize << std::endl;
        // Decimation
        newSize = GetDecimationSize(newSize, dFactor);
        // std::cout << "newSize: " << newSize << std::endl;
    }
    return newSize;
}

#include <iostream>

template<typename Type>
inline std::size_t Audio::DSP::Resampler<Type>::GetResamplingSizeSampleRate(const std::size_t inputSize, const SampleRate inSampleRate, const SampleRate outSampleRate) noexcept
{
    const auto gcd = std::gcd(inSampleRate, outSampleRate);
    std::cout << "gcd: " << gcd << std::endl;
    std::cout << "in: " << inSampleRate / gcd << std::endl;
    std::cout << "out: " << outSampleRate / gcd << std::endl;
    std::cout << "size: " << inputSize << std::endl;



    return std::ceil(static_cast<float>(inputSize + (inputSize - 1) * (outSampleRate / gcd - 1)) / (inSampleRate / gcd));
}

template<typename Type>
inline std::size_t Audio::DSP::Resampler<Type>::GetResamplingSizeOctave(const std::size_t inputSize, const std::int8_t nOctave) noexcept
{
    if (!nOctave)
        return inputSize;
    if (nOctave > 0)
        return inputSize / std::pow(2u, nOctave);
    return inputSize * std::pow(2u, -nOctave);
}

template<typename Type>
inline void Audio::DSP::Resampler<Type>::Internal::InterpolateOctave(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const std::uint8_t nOctave) noexcept_ndebug
{
    const std::size_t octaveRate = std::pow(2u, nOctave);
    std::cout << "Interpolate octave: " << inputSize << " - " << static_cast<std::size_t>(nOctave) << ", " << octaveRate << std::endl;
    return Interpolate(inputBuffer, outputBuffer, inputSize, octaveRate);
}

template<typename Type>
inline void Audio::DSP::Resampler<Type>::Internal::DecimateOctave(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const std::uint8_t nOctave) noexcept_ndebug
{
    // std::cout << "Decimate octave: " << inputSize << " - " << nOctave << std::endl;
    const auto octaveRate = std::pow(2u, nOctave);
    return Decimate(inputBuffer, outputBuffer, inputSize, octaveRate);
}

template<typename Type>
inline void Audio::DSP::Resampler<Type>::Interpolate(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const std::size_t interpolationRatio) noexcept_ndebug
{
    // std::cout << "Interpolate: " << inputSize << " - " << interpolationRatio << std::endl;
    coreAssert(interpolationRatio > 1,
        throw std::logic_error("DSP::Resampler::Interpolate: interpolationRatio must be greater than 1"));
    // const auto interpolationSamples = interpolationRatio - 1;
    for (auto i = 0; i < inputSize; ++i) {
        outputBuffer[i * interpolationRatio] = inputBuffer[i];
        for (auto k = 1; k < interpolationRatio; ++k) {
            // This values depend of the Type -> this is the center value for a given type
            outputBuffer[i * interpolationRatio + k] = 0;
        }
    }
    // Filter output
}

template<typename Type>
inline void Audio::DSP::Resampler<Type>::Decimate(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const std::size_t decimationRatio) noexcept_ndebug
{
    // std::cout << "Decimate: " << inputSize << " - " << decimationRatio << std::endl;
    coreAssert(decimationRatio > 1,
        throw std::logic_error("DSP::Resampler::Interpolate: decimationRatio must be greater than 1"));

    for (auto i = 0u; i < inputSize / decimationRatio; ++i) {
        outputBuffer[i] = inputBuffer[i * decimationRatio];
    }
    // Filter output
}

template<typename Type>
inline void Audio::DSP::Resampler<Type>::ResampleClosestSemitoneImpl(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, bool upScale) noexcept
{
    const auto iFactor = upScale ? L_Factor : M_Factor;
    const auto dFactor = upScale ? M_Factor : L_Factor;

    Interpolate(inputBuffer, outputBuffer, inputSize, iFactor);
    Decimate(inputBuffer, outputBuffer, inputSize, dFactor);
}

template<typename Type>
inline void Audio::DSP::Resampler<Type>::ResampleSemitoneOctaveImpl(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const Semitone semitone) noexcept
{
    const auto upScale = semitone > 0;
    const auto nSemitone = std::abs(semitone);

    for (auto i = 0; i < nSemitone; ++i) {
        ResampleClosestSemitoneImpl(inputBuffer, outputBuffer, inputSize, upScale);
    }
}

template<typename Type>
inline void Audio::DSP::Resampler<Type>::ResampleOctave(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const std::int8_t nOctave) noexcept_ndebug
{
    coreAssert(nOctave,
        throw std::logic_error("DSP::Resampler::ResampleOctave: nOctave must be different than zero."));
    if (nOctave > 0)
        Internal::DecimateOctave(inputBuffer, outputBuffer, inputSize, nOctave);
    else
        Internal::InterpolateOctave(inputBuffer, outputBuffer, inputSize, -nOctave);
}

template<typename Type>
inline void Audio::DSP::Resampler<Type>::ResampleSemitone(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const Semitone semitone) noexcept_ndebug
{
    const bool upScale = semitone > 0;
    const auto nOctave = semitone / 12;
    const auto nSemitone = std::abs(semitone) % 12;
    const auto iFactor = upScale ? L_Factor : M_Factor;
    const auto dFactor = upScale ? M_Factor : L_Factor;

    // std::cout << semitone << " -> " << nOctave << ", " << nSemitone << std::endl;
    ResampleOctave(inputBuffer, outputBuffer, inputSize, nOctave);
    ResampleSemitoneOctaveImpl(inputBuffer, outputBuffer, inputSize, nSemitone);
}


// template<typename Type>
// inline Buffer Audio::DSP::Resampler<Type>::ResampleBySemitone(const BufferView &inputBuffer, const Semitone semitone) noexcept_ndebug
// {
//     coreAssert((semitone != 0) && (semitone < 12 && semitone > -12),
//         throw std::logic_error("Audio::DSP::Resampler<Type>::ResampleBySemitone: semitone arguments must fit an octave [-11, 11]"));
//     const bool upScale = semitone > 0;
//     const auto nSemitone = std::abs(semitone);
//     const auto inputSize = inputBuffer.size<Type>();

//     if (nSemitone == 1) {
//         return ResampleOneSemitone(inputBuffer, upScale);
//     }

//     Buffer first = ResampleOneSemitone(inputBuffer, upScale);
//     BufferView view(first);
//     Buffer *ptr = nullptr;

//     for (auto i = 0u; i < nSemitone - 1; ++i) {
//         Buffer buf = ResampleOneSemitone(view, upScale);
//         view = buf;
//         ptr = &buf;
//     }

//     // change
//     return first;
// }


// template<typename Type>
// inline Buffer Audio::DSP::Resampler<Type>::ResampleBySamplerate(const BufferView &inputBuffer, SampleRate newSampleRate) noexcept_ndebug
// {
//     return ResampleSpecificSampleRate(inputBuffer, inputBuffer.sampleRate(), newSampleRate);
// }

// template<typename Type>
// inline Buffer Audio::DSP::Resampler<Type>::ResampleSpecificSampleRate(const BufferView &inputBuffer, SampleRate inSampleRate, SampleRate outSampleRate) noexcept_ndebug
// {
//     coreAssert(inSampleRate && outSampleRate && (inSampleRate != outSampleRate),
//         throw std::logic_error("DSP::Resampler::ResampleSpecificSampleRate: invalid sampleRate range."));
//     const auto div = std::gcd(inSampleRate, outSampleRate);

//     inSampleRate /= div;
//     outSampleRate /= div;

//     auto interpolatedBuffer = Interpolate(inputBuffer, inSampleRate - 1);
//     // Filter interpolatedBuffer !
//     auto outBuffer = Decimate(interpolatedBuffer, outSampleRate);

//     // outBuffer.setSampleRate(outSampleRate * div);
//     return outBuffer;
// }

// template<typename Type>
// inline void Audio::DSP::Resampler<Type>::GenerateDefaultOctave(const BufferView &inputBuffer, BufferViews &outBuffers) noexcept
// {
    // Buffer firstLow = ResampleOneSemitone(inputBuffer, false);
    // Buffer firstHigh = ResampleOneSemitone(inputBuffer, true);
    // BufferView view(firstLow);

    // // Lower sample buffers
    // outBuffers.push(view);
    // for (auto i = 0u; i < RootKeyDefaultOctave - 1; ++i) {
    //     Buffer buf = ResampleOneSemitone(view, false);
    //     view = buf;
    //     outBuffers.push(view);
    // }
    // // Higher sample buffers
    // view = firstHigh;
    // outBuffers.push(view);
    // for (auto i = 0u; i < 11 - RootKeyDefaultOctave - 1; ++i) {
    //     Buffer buf = ResampleOneSemitone(view, true);
    //     view = buf;
    //     outBuffers.push(view);
    // }
// }
