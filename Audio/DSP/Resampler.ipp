/**
 * @ Author: Pierre V
 * @ Description: Resampler
 */

#include <cmath>
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
inline std::size_t DSP::Resampler<Type>::GetRealResamplingSize(const std::size_t inputSize, const Semitone semitone) noexcept
{
    if (!semitone)
        return inputSize;
    const auto iFactor = (semitone > 0) ? M_Factor : L_Factor;
    const auto dFactor = (semitone > 0) ? L_Factor : M_Factor;

    std::size_t newSize { 0u };
    for (auto i = 0u; i < std::abs(semitone); ++i) {
        const auto size = i ? newSize : inputSize;
        // Interpolation
        newSize = size + (size - 1) * (iFactor - 1);
        // Decimation
        newSize = std::ceil(static_cast<float>(newSize) / dFactor);
    }
    return newSize;
}

template<typename Type>
inline Buffer DSP::Resampler<Type>::InterpolateByOctave(const BufferView &inputBuffer, const std::uint8_t nOctave) noexcept
{
    const auto inputSize = inputBuffer.size<Type>();
    Buffer outBuffer(inputBuffer.channelByteSize() * std::pow(2.0, nOctave), inputBuffer.sampleRate(), inputBuffer.channelArrangement());

    for (auto iChannel = 0u; iChannel < static_cast<std::size_t>(inputBuffer.channelArrangement()); ++iChannel) {
        const auto channel = static_cast<Channel>(iChannel);
        for (auto i = 0u; i < inputSize; ++i) {
            outBuffer.data<Type>(channel)[2 * i] = inputBuffer.data<Type>(channel)[i];
            outBuffer.data<Type>(channel)[2 * i + 1] = 0;
        }
    }
    // Filter outBuffer, then return
    return outBuffer;
}

template<typename Type>
inline Buffer DSP::Resampler<Type>::Interpolate(const BufferView &inputBuffer, const std::size_t interpolationSamples) noexcept_ndebug
{
    coreAssert(interpolationSamples > 1,
        throw std::logic_error("DSP::Resampler::Interpolate: interpolationSamples must be greater than 1"));
    if (interpolationSamples == 2)
        return InterpolateByOctave(inputBuffer, 1);

    const auto inputSize = inputBuffer.size<Type>();
    const auto newSize = inputSize + (inputSize - 1) * (interpolationSamples);

    return Buffer(newSize * sizeof(Type), inputBuffer.sampleRate(), inputBuffer.channelArrangement());
}

template<typename Type>
inline Buffer DSP::Resampler<Type>::DecimateByOctave(const BufferView &inputBuffer, const std::uint8_t nOctave) noexcept
{
    const auto inputSize = inputBuffer.size<Type>();
    Buffer outBuffer(std::ceil(inputSize / std::pow(2.0, nOctave)) * sizeof(Type), inputBuffer.sampleRate(), inputBuffer.channelArrangement());
    const auto outSize = outBuffer.size<Type>();

    for (auto iChannel = 0u; iChannel < static_cast<std::size_t>(inputBuffer.channelArrangement()); ++iChannel) {
        const auto channel = static_cast<Channel>(iChannel);
        for (auto i = 0u; i < outSize; ++i) {
            outBuffer.data<Type>(channel)[i] = inputBuffer.data<Type>(channel)[i * 2];
        }
    }
    // Filter outBuffer, then return
    return outBuffer;
}

template<typename Type>
inline Buffer DSP::Resampler<Type>::Decimate(const BufferView &inputBuffer, const std::size_t decimationSamples) noexcept_ndebug
{
    coreAssert(decimationSamples > 1,
        throw std::logic_error("DSP::Resampler::Decimate: decimationSamples must be greater than 1"));
    if (decimationSamples == 2)
        return DecimateByOctave(inputBuffer, 1);

    const auto inputSize = inputBuffer.size<Type>();
    const auto newSize = std::ceil(static_cast<float>(inputSize) / decimationSamples);

    return Buffer(newSize * sizeof(Type), inputBuffer.sampleRate(), inputBuffer.channelArrangement());
}


template<typename Type>
inline Buffer DSP::Resampler<Type>::ResampleOneSemitone(const BufferView &inputBuffer, bool upScale) noexcept
{
    const auto inputSize = inputBuffer.size<Type>();
    const auto iFactor = upScale ? L_Factor : M_Factor;
    const auto dFactor = upScale ? M_Factor : L_Factor;
    const std::size_t interpolateBufferSize = inputSize + (inputSize - 1) * (iFactor - 1);
    const std::size_t outputSize = static_cast<float>(interpolateBufferSize) / dFactor;

    auto interpolatedBuffer = Interpolate(inputBuffer, iFactor - 1);
    // Filter interpolatedBuffer !
    auto outBuffer = Decimate(interpolatedBuffer, dFactor);

    return outBuffer;
}


template<typename Type>
inline Buffer DSP::Resampler<Type>::ResampleBySemitone(const BufferView &inputBuffer, const Semitone semitone) noexcept_ndebug
{
    coreAssert((semitone != 0) && (semitone < 12 && semitone > -12),
        throw std::logic_error("DSP::Resampler<Type>::ResampleBySemitone: semitone arguments must fit an octave [-11, 11]"));
    const bool upScale = semitone > 0;
    const auto nSemitone = std::abs(semitone);
    const auto inputSize = inputBuffer.size<Type>();

    if (nSemitone == 1) {
        return ResampleOneSemitone(inputBuffer, upScale);
    }

    Buffer first = ResampleOneSemitone(inputBuffer, upScale);
    BufferView view(first);
    Buffer *ptr = nullptr;

    for (auto i = 0u; i < nSemitone - 1; ++i) {
        Buffer buf = ResampleOneSemitone(view, upScale);
        view = buf;
        ptr = &buf;
    }

    // change
    return first;
}


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
    Buffer firstLow = ResampleOneSemitone(inputBuffer, false);
    Buffer firstHigh = ResampleOneSemitone(inputBuffer, true);
    BufferView view(firstLow);

    // Lower sample buffers
    outBuffers.push(view);
    for (auto i = 0u; i < RootKeyDefaultOctave - 1; ++i) {
        Buffer buf = ResampleOneSemitone(view, false);
        view = buf;
        outBuffers.push(view);
    }
    // Higher sample buffers
    view = firstHigh;
    outBuffers.push(view);
    for (auto i = 0u; i < 11 - RootKeyDefaultOctave - 1; ++i) {
        Buffer buf = ResampleOneSemitone(view, true);
        view = buf;
        outBuffers.push(view);
    }
}
