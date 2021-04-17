/**
 * @ Author: Pierre V
 * @ Description: Resampler
 */

#include <Core/Assert.hpp>

#include <Audio/DSP/FIR.hpp>

#include <iostream>

inline std::size_t Audio::DSP::GetOptimalResamplingSize(const std::size_t inputSize, const Semitone semitone) noexcept
{
    const auto rate = std::pow(2.0, -semitone / 12.0);
    return std::ceil(inputSize * rate);
}

inline std::size_t Audio::DSP::GetResamplingSizeOctave(const std::size_t inputSize, const std::int8_t nOctave) noexcept
{
    if (!nOctave)
        return inputSize;
    if (nOctave > 0)
        return inputSize / std::pow(2u, nOctave);
    return inputSize * std::pow(2u, -nOctave);
}

inline std::size_t Audio::DSP::GetResamplingSizeOneSemitone(const std::size_t inputSize, const bool upScale) noexcept
{
    const auto iFactor = upScale ? Resampler::L_Factor : Resampler::M_Factor;
    const auto dFactor = upScale ? Resampler::M_Factor : Resampler::L_Factor;

    return (inputSize * iFactor) / dFactor;
    // return std::ceil((inputSize + (inputSize - 1) * iFactor) / (static_cast<float>(dFactor)));
}

inline std::size_t Audio::DSP::GetResamplingSizeSemitone(const std::size_t inputSize, const Semitone semitone) noexcept
{
    if (!semitone)
        return inputSize;
    const auto nOctave = semitone / 12;
    const auto nSemitone = std::abs(semitone) % 12;
    const auto upScale = semitone > 0;
    std::size_t newSize = 0u;

    newSize = GetResamplingSizeOctave(inputSize, nOctave);
    for (auto i = 0; i < nSemitone; ++i) {
        newSize = GetResamplingSizeOneSemitone(newSize, upScale);
    }
    return newSize;
}

inline std::size_t Audio::DSP::GetResamplingSizeSampleRate(const std::size_t inputSize, const SampleRate inSampleRate, const SampleRate outSampleRate) noexcept
{
    const auto gcd = std::gcd(inSampleRate, outSampleRate);

    return (inputSize * (outSampleRate / gcd)) / (inSampleRate / gcd);
    // return std::ceil(static_cast<float>(inputSize + (inputSize - 1) * (outSampleRate / gcd - 1)) / (inSampleRate / gcd));
}

template<bool Accumulate, typename Type>
inline void Audio::DSP::Resampler::ResampleOctave(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const SampleRate sampleRate, const std::int8_t nOctave, const std::size_t offset)
{    
    coreAssert(nOctave,
        throw std::logic_error("DSP::Resampler::ResampleOctave: nOctave must be different than zero."));

    if (nOctave > 0)
        FIR::Resample<8u, Accumulate>(inputBuffer, outputBuffer, inputSize, sampleRate, 1, std::pow(2, std::abs(nOctave)), offset);
        // Internal::DecimateOctave(inputBuffer, outputBuffer, inputSize, nOctave);
    else
        FIR::Resample<8u, Accumulate>(inputBuffer, outputBuffer, inputSize, sampleRate, std::pow(2, std::abs(nOctave)), 1, offset);
        // Internal::InterpolateOctave(inputBuffer, outputBuffer, inputSize, -nOctave);
}

template<bool Accumulate, typename Type>
inline void Audio::DSP::Resampler::ResampleSemitone(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const SampleRate inSampleRate, const bool upScale)
{
    const auto iFactor = upScale ? Resampler::L_Factor : Resampler::M_Factor;
    const auto dFactor = upScale ? Resampler::M_Factor : Resampler::L_Factor;

    std::cout << "ResampleSemitone: input size: " << inputSize << std::endl;
    FIR::Resample<8u, Accumulate>(inputBuffer, outputBuffer, inputSize, inSampleRate, iFactor, dFactor);
}
