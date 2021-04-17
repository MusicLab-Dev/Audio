/**
 * @ Author: Pierre V
 * @ Description: Resampler
 */

template<typename Type>
inline std::size_t Audio::DSP::Resampler<Type>::GetResampleSemitoneBufferSize(const std::size_t inputSize, const SampleRate inSampleRate, const bool upScale) noexcept
{
    const auto interpolationFactor = upScale ? Resampler::InterpolationSemitoneFactor : Resampler::DecimationSemitoneFactor;
    const auto decimationFactor = upScale ? Resampler::DecimationSemitoneFactor : Resampler::InterpolationSemitoneFactor;

    return (inputSize * interpolationFactor) / decimationFactor;
}

template<typename Type>
inline std::size_t Audio::DSP::Resampler<Type>::GetResampleSampleRateBufferSize(const std::size_t inputSize, const SampleRate inSampleRate, const SampleRate outSampleRate) noexcept
{
    const auto gcd = std::gcd(inSampleRate, outSampleRate);

    return (inputSize * (outSampleRate / gcd)) / (inSampleRate / gcd);
}

template<typename Type>
inline std::size_t Audio::DSP::Resampler<Type>::GetResampleOctaveBufferSize(const std::size_t inputSize, const SampleRate sampleRate, const int nOctave) noexcept
{
    if (!nOctave)
        return inputSize;
    if (nOctave > 0)
        return inputSize / std::pow(2u, nOctave);
    return inputSize * std::pow(2u, -nOctave);
}

template<typename Type>
template<bool Accumulate, unsigned ProcessSize>
inline void Audio::DSP::Resampler<Type>::resampleSemitone(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const SampleRate inSampleRate, const bool upScale)
{
    // const auto iFactor = upScale ? Resampler::L_Factor : Resampler::M_Factor;
    // const auto dFactor = upScale ? Resampler::M_Factor : Resampler::L_Factor;

}

template<typename Type>
template<bool Accumulate, unsigned ProcessSize>
inline void Audio::DSP::Resampler<Type>::resampleOctave(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const SampleRate sampleRate, const int nOctave, const std::size_t inputOffset)
{
    const std::size_t factor = std::pow(2, std::abs(nOctave));
    const auto factorScale = static_cast<float>(factor) * 0.707f / 2.0f;
    const std::size_t filterSize = factor * ProcessSize;
    const Filter::FilterSpecs filterSpecs {
        Filter::FilterType::LowPass,
        Filter::WindowType::Hanning,
        filterSize,
        static_cast<float>(sampleRate),
        { sampleRate / 2 / static_cast<float>(factor), 0.0 }
    };

    _filterCache.resize(filterSize);
    Filter::GenerateFilterCoefficients(filterSpecs.windowType, filterSize, _filterCache.data());
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
                    sample += inputBuffer[inputIdx + k] * _filterCache[j + k * factor] * factorScale;
                outputBuffer[outIdx] = sample;
                ++outIdx;
            }
            ++operationCount;
        }
        const auto end = inputOffset + inputSize;
        for (auto i = inputOffset ? inputOffset : ProcessSize; i < end; ++i) {
            const auto inputIdx = i - ProcessSize;
            for (auto j = 0ul; j < factor; ++j) {
                Type sample {};
                for (auto k = 0; k < ProcessSize; ++k)
                    sample += inputBuffer[inputIdx + k] * _filterCache[j + k * factor] * factorScale;
                outputBuffer[outIdx] = sample;
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
            for (auto j = 0ul, k = zeroPads; j < count; ++j, ++k)
                sample += inputBuffer[j] * _filterCache[k];
            outputBuffer[outIdx] = sample;
            zeroPads -= factor;
            ++outIdx;
        }
        const auto end = inputOffset + inputSize;
        for (auto i = inputOffset ? inputOffset : filterSize; i < end; i += factor) {
            const auto iMinusFilterMinusOne = i - filterMinusOne;
            Type sample {};
            for (auto j = 0ul; j < filterSize; ++j)
                sample += inputBuffer[iMinusFilterMinusOne + j] * _filterCache[j];
            outputBuffer[outIdx] = sample;
            ++outIdx;
        }
    }
}


template<typename Type>
template<unsigned ProcessSize>
inline void Audio::DSP::Resampler<Type>::resampleSampleRate(const Type *inputBuffer, Type *outputBuffer, const std::size_t inputSize, const SampleRate inSampleRate, const SampleRate outSampleRate)
{
    // const auto gcd = std::gcd(inSampleRate, outSampleRate);
    // // const auto realInput

    // return (inputSize * (outSampleRate / gcd)) / (inSampleRate / gcd);

}