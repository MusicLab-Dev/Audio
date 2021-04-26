/**
 * @ Author: Pierre V
 * @ Description: Resampler
 */

#include <iostream>

template<typename Type>
inline std::size_t Audio::DSP::Resampler<Type>::GetResampleSemitoneBufferSize(const std::size_t inputSize, const bool upScale) noexcept
{
    const auto interpolationFactor = upScale ? Resampler::InterpolationSemitoneFactor : Resampler::DecimationSemitoneFactor;
    const auto decimationFactor = upScale ? Resampler::DecimationSemitoneFactor : Resampler::InterpolationSemitoneFactor;

    return (inputSize * interpolationFactor) / decimationFactor;
}

template<typename Type>
inline std::size_t Audio::DSP::Resampler<Type>::GetResampleOctaveBufferSize(const std::size_t inputSize, const int nOctave) noexcept
{
    if (!nOctave)
        return inputSize;
    if (nOctave > 0)
        return inputSize / static_cast<std::size_t>(std::pow(2u, nOctave));
    return inputSize * static_cast<std::size_t>(std::pow(2u, -nOctave));
}

template<typename Type>
inline std::size_t Audio::DSP::Resampler<Type>::GetResampleSampleRateBufferSize(const std::size_t inputSize, const SampleRate inSampleRate, const SampleRate outSampleRate) noexcept
{
    const auto gcd = std::gcd(inSampleRate, outSampleRate);

    return (inputSize * (outSampleRate / gcd)) / (inSampleRate / gcd);
}

template<typename Type>
template<bool Accumulate, unsigned ProcessSize>
inline void Audio::DSP::Resampler<Type>::resampleSemitone(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const SampleRate sampleRate, const bool upScale, const std::size_t inputOffset) noexcept
{
    const auto iFactor = upScale ? Resampler::InterpolationSemitoneFactor : Resampler::DecimationSemitoneFactor;
    const auto dFactor = upScale ? Resampler::DecimationSemitoneFactor : Resampler::InterpolationSemitoneFactor;
    const auto factor = std::max(iFactor, dFactor);
    const auto factorScale = static_cast<float>(iFactor);
    const std::uint32_t filterSize = static_cast<uint32_t>(factor * ProcessSize);
    const Filter::FIRSpec filterSpecs {
        Filter::BasicType::LowPass,
        Filter::WindowType::Hanning,
        filterSize,
        static_cast<float>(sampleRate),
        { static_cast<float>(sampleRate) / 2.0f / static_cast<float>(factor), 0.0f },
        1.0f
    };

    // std::cout << "Resample: " << inputSize << ", " << upScale << std::endl;

    _filterCache.resize(filterSize);
    Filter::GenerateFilter(filterSpecs, _filterCache.data());

    auto trackIdx = 0ul;
    auto outIdx = 0ul;
    auto operationCount = 1ul;
    for (auto i = inputOffset; i < ProcessSize ; ++i) {
        for (auto j = 0ul; j < iFactor; ++j) {
            if (trackIdx % dFactor) {
                ++trackIdx;
                continue;
            }
            Type sample {};
            for (auto k = 0ul; k < operationCount; ++k) {
                sample += inputBuffer[i - k] * _filterCache[static_cast<std::uint32_t>(j + k * iFactor)];
            }
            if constexpr (Accumulate)
                outputBuffer[outIdx] += sample * factorScale;
            else
                outputBuffer[outIdx] = sample * factorScale;
            ++outIdx;
            ++trackIdx;
        }
        ++operationCount;
    }
    const auto end = inputOffset + inputSize;
    for (auto i = inputOffset > ProcessSize ? inputOffset : ProcessSize; i < end; ++i) {
        // const auto inputIdx = i - ProcessSize;
        for (auto j = 0ul; j < iFactor; ++j) {
            if (trackIdx % dFactor) {
                ++trackIdx;
                continue;
            }
            Type sample {};
            for (auto k = 0ul; k < ProcessSize; ++k)
                sample += inputBuffer[i - k] * _filterCache[static_cast<std::uint32_t>(j + k * iFactor)];
            if constexpr (Accumulate)
                outputBuffer[outIdx] += sample * factorScale;
            else
                outputBuffer[outIdx] = sample * factorScale;
            ++outIdx;
            ++trackIdx;
        }
    }
}

template<typename Type>
template<bool Accumulate, unsigned ProcessSize>
inline void Audio::DSP::Resampler<Type>::resampleOctave(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const SampleRate sampleRate, const int nOctave, const std::size_t inputOffset) noexcept
{
    const std::size_t factor = static_cast<std::size_t>(std::pow(2, std::abs(nOctave)));
    const auto factorScale = static_cast<float>(nOctave > 0 ? 1 : factor);
    const std::uint32_t filterSize = static_cast<std::uint32_t>(factor * ProcessSize);
    const Filter::FIRSpec filterSpecs {
        Filter::BasicType::LowPass,
        Filter::WindowType::Hanning,
        filterSize,
        static_cast<float>(sampleRate),
        { static_cast<float>(sampleRate) / 2.0f / static_cast<float>(factor), 0.0f },
        1.0f
    };

    _filterCache.resize(static_cast<std::uint32_t>(filterSize));
    Filter::GenerateFilter(filterSpecs, _filterCache.data());

    if (nOctave == 0) {
        std::memcpy(outputBuffer, inputBuffer + inputOffset, inputSize * sizeof(Type));
    } else if (nOctave < 0) {
        // Only filter + interpolation
        auto outIdx = 0ul;
        auto operationCount = 1ul;
        for (auto i = inputOffset; i < ProcessSize; ++i) {
            const auto inputIdx = i - (operationCount - 1);
            for (auto j = 0ul; j < factor; ++j) {
                Type sample {};
                for (auto k = 0ul; k < operationCount; ++k)
                    sample += inputBuffer[inputIdx + k] * _filterCache[static_cast<std::uint32_t>(j + k * factor)];
                if constexpr (Accumulate)
                    outputBuffer[outIdx] += sample * factorScale;
                else
                    outputBuffer[outIdx] = sample * factorScale;
                ++outIdx;
            }
            ++operationCount;
        }
        const auto end = inputOffset + inputSize;
        for (auto i = inputOffset ? inputOffset : ProcessSize; i < end; ++i) {
            const auto inputIdx = i - ProcessSize;
            for (auto j = 0ul; j < factor; ++j) {
                Type sample {};
                for (auto k = 0ul; k < ProcessSize; ++k)
                    sample += inputBuffer[inputIdx + k] * _filterCache[static_cast<std::uint32_t>(j + k * factor)];
                if constexpr (Accumulate)
                    outputBuffer[outIdx] += sample * factorScale;
                else
                    outputBuffer[outIdx] = sample * factorScale;
                ++outIdx;
            }
        }
    } else {
        // Only filter + decimation
        const auto filterMinusOne = filterSize - 1;
        std::uint32_t zeroPads = filterMinusOne;
        auto outIdx = 0ul;
        for (auto i = inputOffset; i < filterSize; i += factor) {
            const auto count = filterSize - zeroPads;
            Type sample {};
            std::uint32_t k = zeroPads;
            for (auto j = 0u; j < count; ++j, ++k)
                sample += inputBuffer[j] * _filterCache[k];
            if constexpr (Accumulate)
                outputBuffer[outIdx] += sample * factorScale;
            else
                outputBuffer[outIdx] = sample * factorScale;
            zeroPads -= static_cast<std::uint32_t>(factor);
            ++outIdx;
        }
        const auto end = inputOffset + inputSize;
        for (auto i = inputOffset ? inputOffset : filterSize; i < end; i += factor) {
            const auto iMinusFilterMinusOne = i - filterMinusOne;
            Type sample {};
            for (auto j = 0u; j < filterSize; ++j)
                sample += inputBuffer[iMinusFilterMinusOne + j] * _filterCache[j];
            if constexpr (Accumulate)
                outputBuffer[outIdx] += sample * factorScale;
            else
                outputBuffer[outIdx] = sample * factorScale;
            ++outIdx;
        }
    }
}


template<typename Type>
template<unsigned ProcessSize>
inline void Audio::DSP::Resampler<Type>::resampleSampleRate(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const SampleRate inSampleRate, const SampleRate outSampleRate) noexcept
{
    UNUSED(inputBuffer);
    UNUSED(outputBuffer);
    UNUSED(inputSize);
    UNUSED(inputBuffer);
    UNUSED(inSampleRate);
    UNUSED(outSampleRate);
    // const auto gcd = std::gcd(inSampleRate, outSampleRate);
    // // const auto realInput

    // return (inputSize * (outSampleRate / gcd)) / (inSampleRate / gcd);

}
