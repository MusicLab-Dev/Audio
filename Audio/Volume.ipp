/**
 * @file Volume.ipp
 * @brief Volume implementation
 *
 * @author Pierre V
 * @date 2021-04-23
 */

template<typename Type>
void Audio::Volume<Type>::applyRatio(Type *input, const std::size_t inputSize, const float ratio) noexcept
{
    _peak = 0.0f;
    _rms = 0.0f;
    for (auto i = 0ul; i < inputSize; ++i) {
        input[i] *= ratio;
        _peak = std::max(std::abs(input[i]), _peak);
        _rms += input[i] * input[i];
    }
    _rms = std::sqrt(_rms / inputSize);
}

template<typename Type>
template<bool Accumulate>
void Audio::Volume<Type>::assignRangeFunctor(Type *input, const std::size_t inputSize, const std::size_t offset, RangeFunctor &&functor) noexcept
{
    _peak = 0.0f;
    _rms = 0.0f;
    const auto end = inputSize + offset;
    for (auto i = offset, k = 0ul; i < end; ++i, ++k) {
        if constexpr (Accumulate)
            input[k] += functor(i);
        else
            input[k] = functor(i);
        _peak = std::max(std::abs(input[k]), _peak);
        _rms += input[k] * input[k];
    }
    _rms = std::sqrt(_rms / inputSize);
}
