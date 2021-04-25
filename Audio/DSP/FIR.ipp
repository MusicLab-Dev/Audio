/**
 * @file FIR.ipp
 * @brief FIR implementation
 *
 * @author Pierre V
 * @date 2021-04-22
 */

template<typename Type>
typename Audio::DSP::FIR::VoidType<Type> Audio::DSP::FIR::Internal::Instance<Type>::filter(const Type *input, const std::size_t inputSize, Type *output, const Type outGain) noexcept
{
    const auto filterSize = _coefficients.size();
    const auto filterSizeMinusOne = filterSize - 1;

    // Begin
    for (auto i = 0ul; i < filterSizeMinusOne; ++i) {
        output[i] = filterImpl(input, filterSize, filterSizeMinusOne - i) * outGain;
    }
    // Body
    const Type *inputShifted = input - filterSizeMinusOne;
    for (auto i = filterSizeMinusOne; i < inputSize; ++i) {
        output[i] = filterImpl(inputShifted + i, filterSize) * outGain;
    }
    // Save for last input
    std::memcpy(_lastInputCache.data(), input + inputSize - filterSizeMinusOne, filterSizeMinusOne * sizeof(Type));
}

template<typename Type>
typename Audio::DSP::FIR::ProcessType<Type> Audio::DSP::FIR::Internal::Instance<Type>::filterImpl(const Type *input, const std::size_t size, const std::size_t zeroPad) noexcept
{
    const std::size_t realSize = size - zeroPad;
    const std::size_t realSizeMinusOne = realSize - 1;
    Type sample { 0.0 };

    for (auto i = 0ul; i < zeroPad; ++i) {
        sample += _lastInputCache[realSizeMinusOne + i] * _coefficients[i];
    }
    for (auto i = 0ul; i < realSize; ++i) {
        sample += input[i] * _coefficients[zeroPad + i];
    }
    return sample;
}

template<unsigned InstanceCount, typename Type>
typename Audio::DSP::FIR::VoidType<Type> Audio::DSP::FIR::Internal::MultiInstance<InstanceCount, Type>::filter(const Type *input, const std::size_t inputSize, Type *output, const Internal::GainArray<InstanceCount> &gains) noexcept
{
    const auto filterSize = _coefficients.size();
    const auto filterSizeMinusOne = filterSize - 1;

    // Begin
    for (auto i = 0ul; i < filterSizeMinusOne; ++i) {
        auto k = 0ul;
        for (const auto gain : gains) {
            std::cout << gain << std::endl;
            output[i] = filterImpl(input, filterSize, _coefficients[k++].data(), filterSizeMinusOne - i) * gain;
        }
    }
    // Body
    const Type *inputShifted = input - filterSizeMinusOne;
    for (auto i = filterSizeMinusOne; i < inputSize; ++i) {
        // Apply all instance
        auto k = 0ul;
        for (const auto gain : gains) {
            // std::cout << gain << std::endl;
            output[i] = filterImpl(inputShifted + i, filterSize, _coefficients[k++].data()) * gain;
        }
    }
    // Save for last input
    std::memcpy(_lastInputCache.data(), input + inputSize - filterSizeMinusOne, filterSizeMinusOne * sizeof(Type));
    std::cout << std::endl;
}

template<unsigned InstanceCount, typename Type>
typename Audio::DSP::FIR::ProcessType<Type> Audio::DSP::FIR::Internal::MultiInstance<InstanceCount, Type>::filterImpl(const Type *input, const std::size_t size, const Type *coefs, const std::size_t zeroPad) noexcept
{
    const std::size_t realSize = size - zeroPad;
    const std::size_t realSizeMinusOne = realSize - 1;
    Type sample { 0.0 };

    for (auto i = 0ul; i < zeroPad; ++i) {
        sample += _lastInputCache[realSizeMinusOne + i] * _coefficients[i];
    }
    for (auto i = 0ul; i < realSize; ++i) {
        sample += input[i] * coefs[zeroPad + i];
    }
    return sample;
}
