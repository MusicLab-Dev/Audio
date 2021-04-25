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
        return inputSize / std::pow(2u, nOctave);
    return inputSize * std::pow(2u, -nOctave);
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
    const std::size_t filterSize = factor * ProcessSize;
    const Filter::FIRSpec filterSpecs {
        Filter::BasicType::LowPass,
        Filter::WindowType::Hanning,
        filterSize,
        static_cast<double>(sampleRate),
        { sampleRate / 2.0 / static_cast<double>(factor), 0.0 },
        1.0
    };

    _filterCache.resize(filterSize);
    Filter::GenerateWindow(filterSpecs.windowType, filterSize, _filterCache.data());
    Filter::DesignFilter(filterSpecs, _filterCache.data(), filterSize, true);

    std::cout << "Resample: " << inputSize << ", " << upScale << std::endl;

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
                sample += inputBuffer[i - k] * _filterCache[j + k * iFactor];
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
    auto count = 0ul;
    std::cout << "outIdx: " << outIdx << std::endl;
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
                sample += inputBuffer[i - k] * _filterCache[j + k * iFactor];
            if constexpr (Accumulate)
                outputBuffer[outIdx] += sample * factorScale;
            else
                outputBuffer[outIdx] = sample * factorScale;
            if (!sample)
                ++count;
            ++outIdx;
            ++trackIdx;
        }
    }
    std::cout << "_outIdx: " << outIdx << std::endl;
    std::cout << "__count: " << count << std::endl;
}

template<typename Type>
template<bool Accumulate, unsigned ProcessSize>
inline void Audio::DSP::Resampler<Type>::resampleOctave(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const SampleRate sampleRate, const int nOctave, const std::size_t inputOffset) noexcept
{
    const std::size_t factor = std::pow(2, std::abs(nOctave));
    const auto factorScale = static_cast<float>(nOctave > 0 ? 1 : factor);
    const std::size_t filterSize = factor * ProcessSize;
    const Filter::FIRSpec filterSpecs {
        Filter::BasicType::LowPass,
        Filter::WindowType::Hanning,
        filterSize,
        static_cast<double>(sampleRate),
        { sampleRate / 2.0 / static_cast<double>(factor), 0.0 },
        1.0
    };
    // std::cout << "cutoff:: " << (sampleRate / 2 / static_cast<float>(factor)) << ", size: " << filterSize << std::endl;

    _filterCache.resize(filterSize);
    Filter::GenerateWindow(filterSpecs.windowType, filterSize, _filterCache.data());
    Filter::DesignFilter(filterSpecs, _filterCache.data(), filterSize, true);

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
                    sample += inputBuffer[inputIdx + k] * _filterCache[j + k * factor];
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
                    sample += inputBuffer[inputIdx + k] * _filterCache[j + k * factor];
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
        auto zeroPads = filterMinusOne;
        auto outIdx = 0ul;
        for (auto i = inputOffset; i < filterSize; i += factor) {
            const auto count = filterSize - zeroPads;
            Type sample {};
            auto k = zeroPads;
            for (auto j = 0ul; j < count; ++j, ++k)
                sample += inputBuffer[j] * _filterCache[k];
            if constexpr (Accumulate)
                outputBuffer[outIdx] += sample * factorScale;
            else
                outputBuffer[outIdx] = sample * factorScale;
            zeroPads -= factor;
            ++outIdx;
        }
        const auto end = inputOffset + inputSize;
        for (auto i = inputOffset ? inputOffset : filterSize; i < end; i += factor) {
            const auto iMinusFilterMinusOne = i - filterMinusOne;
            Type sample {};
            for (auto j = 0ul; j < filterSize; ++j)
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
    // const auto gcd = std::gcd(inSampleRate, outSampleRate);
    // // const auto realInput

    // return (inputSize * (outSampleRate / gcd)) / (inSampleRate / gcd);

}
