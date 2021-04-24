/**
 * @file FIR.ipp
 * @brief FIR implementation
 *
 * @author Pierre V
 * @date 2021-04-22
 */

template<typename Type>
template<bool UseLastInput>
typename Audio::DSP::VoidType<Type> Audio::DSP::FIR<Type>::filter(const Type *input, const std::size_t inputSize, Type *output) noexcept
{
    const auto filterSize = _coefficients.size();
    const auto filterSizeMinusOne = filterSize - 1;
    // const auto filterSizePlusOne = filterSize + 1;

    // Begin
    for (auto i = 0ul; i < filterSizeMinusOne; ++i) {
        output[i] = filterImpl<UseLastInput>(input, filterSize, filterSizeMinusOne - i);
    }
    // Body
    const Type *inputShifted = input - filterSizeMinusOne;
    for (auto i = filterSizeMinusOne; i < inputSize; ++i) {
        output[i] = filterImpl<false>(inputShifted + i, filterSize);
    }
    // Save for last input
    std::memcpy(_lastInputCache.data(), input + inputSize - filterSizeMinusOne, filterSizeMinusOne * sizeof(Type));
}

template<typename Type>
template<bool UseLastInput>
typename Audio::DSP::ProcessType<Type> Audio::DSP::FIR<Type>::filterImpl(const Type *input, const std::size_t size, const std::size_t zeroPad) noexcept
{
    const std::size_t realSize = size - zeroPad;
    const std::size_t realSizeMinusOne = realSize - 1;
    Type sample { 0.0 };

    // WXYZabcde
    //  01234
    if constexpr (UseLastInput) {
        for (auto i = 0ul; i < zeroPad; ++i) {
            sample += _lastInputCache[realSizeMinusOne + i] * _coefficients[i];
        }
    }
    for (auto i = 0ul; i < realSize; ++i) {
        sample += input[i] * _coefficients[zeroPad + i];
    }
    return sample;
}

template<typename Type>
template<bool UseLastInput>
typename Audio::DSP::VoidType<Type> Audio::DSP::FIRFilter<Type>::filter(const Type *input, const std::size_t inputSize, Type *output) noexcept
{
    _instance.template filter<true>(input, inputSize, output);
}

template<typename Type>
inline bool Audio::DSP::FIRFilter<Type>::setSpecs(const Filter::FIRSpecs specs) noexcept
{
    if (_specs == specs)
        return false;
    _specs = specs;
    _instance.coefficients().resize(specs.size);
    _instance.lastInput().resize(specs.size - 1);
    _instance.lastInput().clear();
    Filter::GenerateFilter(specs, _instance.coefficients().data());

    return true;
}

template<typename Type>
inline bool Audio::DSP::FIRFilter<Type>::setCutoffs(const float cutoffFrom, const float cutoffTo) noexcept
{
    return setSpecs(
        Filter::FIRSpecs {
            _specs.filterType,
            _specs.windowType,
            _specs.size,
            _specs.sampleRate,
            { cutoffFrom, cutoffTo }
        }
    );
}

template<typename Type>
inline bool Audio::DSP::FIRFilter<Type>::setSampleRate(const float sampleRate) noexcept
{
    return setSpecs(
        Filter::FIRSpecs {
            _specs.filterType,
            _specs.windowType,
            _specs.size,
            sampleRate,
            { _specs.cutoffs[0], _specs.cutoffs[1] }
        }
    );
}

template<typename Type>
inline bool Audio::DSP::FIRFilter<Type>::setFilterType(const Filter::BasicType filterType) noexcept
{
    return setSpecs(
        Filter::FIRSpecs {
            filterType,
            _specs.windowType,
            _specs.size,
            _specs.sampleRate,
            { _specs.cutoffs[0], _specs.cutoffs[1] }
        }
    );
}
