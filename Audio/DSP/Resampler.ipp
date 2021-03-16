/**
 * @ Author: Pierre V
 * @ Description: Resampler
 */

#include <Core/Assert.hpp>

#include <Audio/DSP/FIR.hpp>

template<typename Type>
inline std::size_t Audio::DSP::Resampler<Type>::GetOptimalResamplingSize(const std::size_t inputSize, const Semitone semitone) noexcept
{
    const auto rate = std::pow(2.0, -semitone / 12.0);
    return std::ceil(inputSize * rate);
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
    std::size_t newSize = 0u;

    newSize = GetResamplingSizeOctave(inputSize, nOctave);

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
inline void Audio::DSP::Resampler<Type>::ResampleClosestSemitoneImpl(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, bool upScale) noexcept
{
    const auto iFactor = upScale ? L_Factor : M_Factor;
    const auto dFactor = upScale ? M_Factor : L_Factor;
    static auto LowPassFilter = FilterSpecs {
        FilterType::LowPass,
        WindowType::Hanning,
        113,
        44100,
        { 44100.f / static_cast<float>(iFactor), 0 }
    };

    std::vector<Type> tmp(GetInterpolationSize(inputSize, iFactor));
    std::cout << "ResampleClosestSemitoneImpl: interpolation size: " << tmp.size() << ", upscale; " << upScale << std::endl;
    Interpolate<false>(inputBuffer, tmp.data(), inputSize, iFactor);
    FIR::Filter(LowPassFilter, tmp.data(), tmp.size(), tmp.data());
    Decimate<false>(tmp.data(), outputBuffer, tmp.size(), dFactor);
    for (auto i = 0u; i < inputSize; ++i)
        outputBuffer[i] *= iFactor;
    auto min = std::min_element(outputBuffer, (outputBuffer + inputSize));
    auto max = std::max_element(outputBuffer, (outputBuffer + inputSize));
    std::cout << *min << ", " << *max << std::endl;
    // FIR::Filter<float>(LowPassFilter, outputBuffer, inputSize, outputBuffer);
    // std::memcpy(outputBuffer, tmp.data(), 36213 * sizeof(Type));
}

template<typename Type>
inline void Audio::DSP::Resampler<Type>::ResampleSemitoneOctaveImpl(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const Semitone semitone) noexcept
{
    const auto upScale = semitone > 0;
    const auto nSemitone = std::abs(semitone);

    std::cout << "ResampleSemitoneOctaveImpl: input size: " << inputSize << ", semitone: " << semitone << std::endl;
    auto *in = inputBuffer;
    auto *out = outputBuffer;
    for (auto i = 0; i < nSemitone; ++i) {
        ResampleClosestSemitoneImpl(in, out, inputSize, upScale);
        in = out;
    }
    outputBuffer = out;
}

template<typename Type>
inline void Audio::DSP::Resampler<Type>::ResampleOctave(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const std::int8_t nOctave)
{
    coreAssert(nOctave,
        throw std::logic_error("DSP::Resampler::ResampleOctave: nOctave must be different than zero."));
    if (nOctave > 0)
        Internal::DecimateOctave(inputBuffer, outputBuffer, inputSize, nOctave);
    else
        Internal::InterpolateOctave(inputBuffer, outputBuffer, inputSize, -nOctave);
}

template<typename Type>
inline void Audio::DSP::Resampler<Type>::ResampleSemitone(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const Semitone semitone)
{
    const bool upScale = semitone > 0;
    const auto nOctave = semitone / 12;
    const auto nSemitone = std::abs(semitone) % 12;
    const auto iFactor = upScale ? L_Factor : M_Factor;
    const auto dFactor = upScale ? M_Factor : L_Factor;

    auto finalSize = GetResamplingSizeSemitone(inputSize, semitone);
    auto maxSize = GetInterpolationOctaveSize(inputSize, nOctave);
    for (auto i = 0u; i < nSemitone; ++i) {
        maxSize = GetInterpolationSize(maxSize, iFactor);
        if (i != nSemitone - 1)
            maxSize = GetDecimationSize(maxSize, dFactor);
    }

    std::cout << "ResampleSemitone: input size: " << inputSize << std::endl;
    std::cout << "ResampleSemitone: final size: " << finalSize << std::endl;
    std::cout << "ResampleSemitone: max size: " << maxSize << std::endl;
    std::cout << static_cast<int>(semitone) << " -> " << nOctave << ", " << nSemitone << std::endl;

    Semitone processSemitone = upScale ? nSemitone : -nSemitone;
    // std::vector<Type> tmp(maxSize);
    if (nOctave) {
        // ResampleOctave(inputBuffer, tmp, inputSize, nOctave);
        // ResampleSemitoneOctaveImpl(tmp, outputBuffer, inputSize, processSemitone);
        // return;
    }
    ResampleSemitoneOctaveImpl(inputBuffer, outputBuffer, inputSize, processSemitone);
}


// template<typename Type>
// inline Buffer Audio::DSP::Resampler<Type>::ResampleBySemitone(const BufferView &inputBuffer, const Semitone semitone)
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
// inline Buffer Audio::DSP::Resampler<Type>::ResampleBySamplerate(const BufferView &inputBuffer, SampleRate newSampleRate)
// {
//     return ResampleSpecificSampleRate(inputBuffer, inputBuffer.sampleRate(), newSampleRate);
// }

// template<typename Type>
// inline Buffer Audio::DSP::Resampler<Type>::ResampleSpecificSampleRate(const BufferView &inputBuffer, SampleRate inSampleRate, SampleRate outSampleRate)
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
