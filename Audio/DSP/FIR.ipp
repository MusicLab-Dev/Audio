/**
 * @file FIR.ipp
 * @brief FIR implementation
 *
 * @author Pierre V
 * @date 2021-04-22
 */

template<typename Type>
typename Audio::DSP::FIR<Type>::VoidType Audio::DSP::FIR<Type>::filter(const Type *input, const std::size_t inputSize, Type *output, const bool useLastInput) noexcept
{
    const auto filterSize = _coefficients.size();
    const auto filterSizeMinusOne = filterSize - 1;
    const auto inputSizeMinusFilter = inputSize - filterSize;

    // Zero padd
    for (auto i = 0ul; i < filterSizeMinusOne; ++i) {
        std::cout << "First: " << i << std::endl;
    }
    // Body
    for (auto i = filterSizeMinusOne; i < inputSizeMinusFilter; ++i) {
        std::cout << "Body: " << i << std::endl;
    }
    // End
    for (auto i = inputSizeMinusFilter; i < inputSize; ++i) {
        std::cout << "End: " << i << std::endl;
    }
}

template<typename Type>
typename Audio::DSP::FIR<Type>::VoidType Audio::DSP::FIR<Type>::filter(
        const Type *input, const std::size_t inputSize,
        const Type *filterCoefficients, const std::size_t filterSize,
        Type *output,
        const Type *lastInput, const std::size_t lastInputSize) noexcept
{

}

template<typename Type>
bool Audio::DSP::FIRFilter<Type>::setSpecs(const Filter::FIRSpecs specs) noexcept
{
    if (_specs == specs)
        return false;
    _instance.coefficients().resize(specs.size);
    _instance.lastInput().resize(specs.size);
    Filter::GenerateFilter(specs, _instance.coefficients().data());

    return true;
}
