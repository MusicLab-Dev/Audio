/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Merge algorithm
 */


template<typename Type>
void Audio::DSP::Merge(const BufferViews inputs, BufferView output, const DB ratio, const bool normalize) noexcept
{
    UNUSED(normalize);
    const auto inputSize = inputs.size();
    const std::size_t outputSize = output.size<Type>();

    if (!inputSize || !outputSize)
        return;
    Type *to = output.data<Type>();
    // Copy first input
    const Type *from = inputs[0].data<Type>();
    for (auto k = 0u; k < outputSize; ++k) {
        to[k] = from[k];
    }
    // Merge other inputs
    for (auto i = 1u; i < inputSize; ++i) {
        const Type *from = inputs[i].data<Type>();
        for (auto k = 0u; k < outputSize; ++k) {
            to[k] += from[k];
        }
    }
    for (auto k = 0u; k < outputSize; ++k) {
        to[k] *= ratio;
    }
}

template<typename Type>
void Audio::DSP::Merge(const BufferView input, BufferView output, const DB ratio, const bool normalize) noexcept
{
    UNUSED(normalize);
    const std::size_t inputSize = static_cast<std::size_t>(input.size<Type>());
    const std::size_t outputSize = output.size<Type>();

    if (!inputSize || !outputSize)
        return;
    const auto realSize = std::min(inputSize, outputSize);

    const Type *in = input.data<Type>();
    Type *out = output.data<Type>();
    for (auto i = 0u; i < realSize; ++i) {
        out[i] = in[i] * ratio;
    }
}

template<typename Unit, std::size_t BufferSize, typename ...Args>
void Audio::DSP::Merge(Unit * const output, Args &&...args) noexcept
{
    constexpr std::size_t InputCount = [] {
        return (... + static_cast<std::size_t>(std::is_convertible_v<Args, Unit * const>));
    }();

    Internal::MergeUnroll<Unit, BufferSize, 0ul, InputCount>(
        output,
        std::make_tuple<Args...>(std::forward<Args>(args)...)
    );
}

template<typename Unit, std::size_t Count, std::size_t InputCount>
void Audio::DSP::Internal::UnrolledCopy(Unit * const output, const Unit * const input, const Unit ratio) noexcept
{
    *output += *input * ratio / InputCount;
    if constexpr (Count > 1)
        UnrolledCopy<Unit, Count - 1, InputCount>(output + 1, input + 1, ratio);
}

template<typename Unit, std::size_t BufferSize, std::size_t Index, std::size_t InputCount, typename Tuple>
std::enable_if_t<std::is_convertible_v<std::tuple_element_t<Index, Tuple>, Unit * const>, void>
    Audio::DSP::Internal::MergeUnroll(Unit * const output, const Tuple &tuple) noexcept
{
    constexpr std::size_t BufferByteSize = BufferSize * sizeof(Unit);
    constexpr std::size_t CacheLineCount = BufferByteSize / Core::CacheLineSize;
    constexpr std::size_t UnitPerCacheLine = Core::CacheLineSize / sizeof(Unit);

    static_assert(!(BufferByteSize == 0ul) && !(BufferByteSize & (BufferByteSize - 1ul)),
        "BufferByteSize must be a power of two of at least one cacheline size");

    constexpr auto Process = [](Unit * const output, const Unit * const input, const Unit ratio) {
        for (std::size_t i = 0ul; i < CacheLineCount; ++i) {
            UnrolledCopy<Unit, UnitPerCacheLine, InputCount>(output + i * UnitPerCacheLine, input + i * UnitPerCacheLine, ratio);
        }
    };

    if constexpr (Index + 1 < std::tuple_size_v<Tuple>) {
        if constexpr (std::is_convertible_v<std::tuple_element_t<Index + 1, Tuple>, const Unit>) {
            Process(output, std::get<Index>(tuple), std::get<Index + 1>(tuple));
            if constexpr (Index + 2 < std::tuple_size_v<Tuple>)
                MergeUnroll<Unit, BufferSize, Index + 2, InputCount, Tuple>(output, tuple);
        } else {
            Process(output, std::get<Index>(tuple), static_cast<Unit>(1));
            if constexpr (Index + 1 < std::tuple_size_v<Tuple>)
                MergeUnroll<Unit, BufferSize, Index + 1, InputCount, Tuple>(output, tuple);
        }
    } else {
        Process(output, std::get<Index>(tuple), static_cast<Unit>(1));
        if constexpr (Index + 1 < std::tuple_size_v<Tuple>)
            MergeUnroll<Unit, BufferSize, Index + 1, InputCount, Tuple>(output, tuple);
    }
}
