/**
 * @ Author: Pierre Veysseyre
 * @ Description: Reformater implementation
 */

#include <Audio/Base.hpp>

template<typename InType, typename OutType>
inline void Audio::DSP::Reformater::Reformat(const InType *input, OutType *output, const std::size_t inputSize) noexcept
{
    // Do nothing
    if constexpr (std::is_same_v<InType, OutType>) {

        return;
    } else if constexpr (std::is_floating_point_v<InType>) {
        return Internal::FromFloating(input, output, inputSize);
    } else if constexpr (std::is_unsigned_v<InType>) {
        if constexpr (std::is_floating_point_v<OutType>) {
            // unsigned -> double/float
        } else if constexpr (std::is_unsigned_v<OutType>) {
            // unsigned -> unsigned
        } else /*if constexpr (std::is_integral_v<OutType>)*/ {
            // unsigned -> signed
        }
    } else /*if constexpr (std::is_integral_v<InType>)*/ {
        if constexpr (std::is_floating_point_v<OutType>) {
            // signed -> double/float
        } else if constexpr (std::is_unsigned_v<OutType>) {
            // signed -> unsigned
        } else /*if constexpr (std::is_integral_v<OutType>)*/ {
            // signed -> signed
        }

    }

    // f -> s       mult by iMax
    // f -> u       f+=1 then mult by iMax/2
    // s -> f
    // u -> f
    // s -> s
    // s -> u
    // u -> u
    // u -> s



    // float related
    // f -> s
    // f -> u
    // s -> f
    // u -> f

    // s -> u
    // u -> s

    // Same first value (0)
    // integers
    // i1 -> s2 (i1 < i2)
    // i1 -> s2 (i1 > i2)
    // unsigned
    // u1 -> u2 (u1 < u2)
    // u1 -> u2 (u1 > u2)


}


template<typename InType, typename OutType>
inline void Audio::DSP::Reformater::Internal::TrivialFormat(const InType *input, OutType *output, const std::size_t inputSize) noexcept
{
    for (auto i = 0u; i < inputSize; ++i) {
        output[i] = static_cast<OutType>(input[i]);
    }
}

template<typename InType, typename OutType>
inline void Audio::DSP::Reformater::Internal::FromFloating(const InType *input, OutType *output, const std::size_t inputSize) noexcept
{
    if constexpr (std::is_floating_point_v<OutType>) {
        // float/double -> double/float
        return TrivialFormat(input, output, inputSize);
    } else if constexpr (std::is_unsigned_v<OutType>) {
        // float/double -> unsigned
        return FromFloatingImpl<InType, OutType, true>(input, output, inputSize);
    } else /*if constexpr (std::is_integral_v<OutType>)*/ {
        // float/double -> signed
        return FromFloatingImpl<InType, OutType, false>(input, output, inputSize);
    }
}

template<typename InType, typename OutType, bool InputOffset>
inline void Audio::DSP::Reformater::Internal::FromFloatingImpl(const InType *input, OutType *output, const std::size_t inputSize) noexcept
{
    constexpr auto OutMaxValue = std::numeric_limits<OutType>::max();

    for (auto i = 0u; i < inputSize; ++i) {
        if constexpr (InputOffset) {
            output[i] = static_cast<OutType>(((input[i] + 1) / 2) * OutMaxValue);
        } else {
            output[i] = static_cast<OutType>(input[i] * OutMaxValue);
        }
    }
}
