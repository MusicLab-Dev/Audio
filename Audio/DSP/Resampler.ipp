/**
 * @ Author: Pierre V
 * @ Description: Resampler
 */

#include <numeric>

#include <Core/Assert.hpp>

using namespace Audio;

template<typename Type>
inline std::size_t DSP::Resampler<Type>::GetOptimalResamplingSize(const std::size_t inputSize, const Semitone semitone) noexcept
{
    const auto rate = std::pow(2.0, -semitone / 12.0);
    return std::ceil(inputSize * rate);
}

template<typename Type>
inline std::size_t DSP::Resampler<Type>::GetInterpolationSize(const std::size_t inputSize, const std::size_t interpolationRatio) noexcept
{
    return inputSize + (inputSize - 1) * (interpolationRatio - 1);
}

template<typename Type>
inline std::size_t DSP::Resampler<Type>::GetDecimationSize(const std::size_t inputSize, const std::size_t decimationRatio) noexcept
{
    return std::ceil(static_cast<float>(inputSize) / decimationRatio);
}

template<typename Type>
inline std::size_t DSP::Resampler<Type>::GetInterpolationOctaveSize(const std::size_t inputSize, const std::uint8_t nOctave) noexcept
{
    return inputSize * (nOctave + 1);
}

template<typename Type>
inline std::size_t DSP::Resampler<Type>::GetDecimationOctaveSize(const std::size_t inputSize, const std::uint8_t nOctave) noexcept
{
    return inputSize * (nOctave + 1);
}

template<typename Type>
inline std::size_t DSP::Resampler<Type>::GetResamplingSizeSemitone(const std::size_t inputSize, const Semitone semitone) noexcept
{
    if (!semitone)
        return inputSize;
    const auto iFactor = (semitone > 0) ? M_Factor : L_Factor;
    const auto dFactor = (semitone > 0) ? L_Factor : M_Factor;

    std::size_t newSize { 0u };
    for (auto i = 0u; i < std::abs(semitone); ++i) {
        const auto size = i ? newSize : inputSize;
        // Interpolation
        newSize = GetInterpolationSize(size, iFactor);
        // Decimation
        newSize = GetDecimationSize(newSize, dFactor);
    }
    return newSize;
}

template<typename Type>
inline std::size_t DSP::Resampler<Type>::GetResamplingSize(const std::size_t inputSize, const SampleRate inSampleRate, const SampleRate outSampleRate) noexcept
{
    return 0;
}

template<typename Type>
inline void DSP::Resampler<Type>::Internal::InterpolateOctave(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const std::uint8_t nOctave) noexcept_ndebug
{
    const std::size_t octaveRate = std::pow(2u, nOctave);

    for (auto i = 0u; i < inputSize; ++i) {
        outputBuffer[i * octaveRate] = inputBuffer[i];
        outputBuffer[i * octaveRate + 1] = 0;
    }
    // Filter outBuffer
}

template<typename Type>
inline void DSP::Resampler<Type>::Interpolate(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const std::size_t interpolationRatio) noexcept_ndebug
{
    coreAssert(interpolationRatio > 1,
        throw std::logic_error("DSP::Resampler::Interpolate: interpolationRatio must be greater than 1"));
    if (interpolationRatio == 2)
        return Resampler<Type>::Internal::InterpolateOctave(inputBuffer, outputBuffer, inputSize, 1);

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
inline void DSP::Resampler<Type>::Decimate(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const std::size_t decimationRatio) noexcept_ndebug
{
    coreAssert(decimationRatio > 1,
        throw std::logic_error("DSP::Resampler::Interpolate: decimationRatio must be greater than 1"));

    for (auto i = 0u; i < inputSize / decimationRatio; ++i) {
        outputBuffer[i] = inputBuffer[i * decimationRatio];
    }
    // Filter output
}

template<typename Type>
inline void DSP::Resampler<Type>::ResampleClosestSemitoneImpl(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, bool upScale) noexcept
{
    const auto iFactor = upScale ? L_Factor : M_Factor;
    const auto dFactor = upScale ? M_Factor : L_Factor;

    Interpolate(inputBuffer, outputBuffer, inputSize, iFactor);
    // Filter outputBuffer
    Decimate(inputBuffer, outputBuffer, inputSize, dFactor);
    // Filter outputBuffer
}

template<typename Type>
inline void DSP::Resampler<Type>::ResampleSemitoneOctaveImpl(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const Semitone semitone) noexcept
{
    const auto upScale = semitone > 0;
    const auto nSemitone = std::abs(semitone);

    for (auto i = 0; i < nSemitone; ++i) {
        ResampleClosestSemitoneImpl(inputBuffer, outputBuffer, inputSize, upScale);
    }
}


template<typename Type>
inline void DSP::Resampler<Type>::ResampleSemitone(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const Semitone semitone) noexcept_ndebug
{
    const bool upScale = semitone > 0;
    const auto nOctave = std::abs(semitone) / 12;
    const auto nSemitone = std::abs(semitone) % 12;
    const auto iFactor = upScale ? L_Factor : M_Factor;
    const auto dFactor = upScale ? M_Factor : L_Factor;

    std::cout << semitone << " -> " << nOctave << ", " << nSemitone << std::endl;
}



// template<typename Type>
// inline Buffer DSP::Resampler<Type>::ResampleBySemitone(const BufferView &inputBuffer, const Semitone semitone) noexcept_ndebug
// {
//     coreAssert((semitone != 0) && (semitone < 12 && semitone > -12),
//         throw std::logic_error("DSP::Resampler<Type>::ResampleBySemitone: semitone arguments must fit an octave [-11, 11]"));
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


template<typename Type>
inline Buffer DSP::Resampler<Type>::ResampleBySamplerate(const BufferView &inputBuffer, SampleRate newSampleRate) noexcept_ndebug
{
    return ResampleSpecificSampleRate(inputBuffer, inputBuffer.sampleRate(), newSampleRate);
}

template<typename Type>
inline Buffer DSP::Resampler<Type>::ResampleSpecificSampleRate(const BufferView &inputBuffer, SampleRate inSampleRate, SampleRate outSampleRate) noexcept_ndebug
{
    coreAssert(inSampleRate && outSampleRate && (inSampleRate != outSampleRate),
        throw std::logic_error("DSP::Resampler::ResampleSpecificSampleRate: invalid sampleRate range."));
    const auto div = std::gcd(inSampleRate, outSampleRate);

    inSampleRate /= div;
    outSampleRate /= div;

    auto interpolatedBuffer = Interpolate(inputBuffer, inSampleRate - 1);
    // Filter interpolatedBuffer !
    auto outBuffer = Decimate(interpolatedBuffer, outSampleRate);

    // outBuffer.setSampleRate(outSampleRate * div);
    return outBuffer;
}

template<typename Type>
inline void DSP::Resampler<Type>::GenerateDefaultOctave(const BufferView &inputBuffer, BufferViews &outBuffers) noexcept
{
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
}
