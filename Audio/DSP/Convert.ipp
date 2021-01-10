/**
 * @ Author: Antoine la Mache
 * @ Description: Merge algorithm
 */

#include <limits>
#include <iostream>

template<typename InputUnit, typename OutputUnit, std::size_t BufferSize>
inline void Audio::DSP::Convert(InputUnit *input, OutputUnit *output) noexcept
{
    constexpr std::size_t BufferByteSize = BufferSize * sizeof(InputUnit);
    constexpr std::size_t CacheLineCount = BufferByteSize / Core::CacheLineSize;
    constexpr std::size_t UnitPerCacheLine = Core::CacheLineSize / sizeof(InputUnit);

    static_assert(!(BufferByteSize == 0ul) && (BufferByteSize >= Core::CacheLineSize) && !(BufferByteSize & (BufferByteSize - 1ul)),
        "BufferByteSize must be a power of two of at least one cacheline size");

    for (std::size_t i = 0ul; i < CacheLineCount; ++i) {
        UnrolledConvert<InputUnit, OutputUnit, UnitPerCacheLine>(output + (i * UnitPerCacheLine),
                input + (i * UnitPerCacheLine));
    }
}

template<typename InputUnit, typename OutputUnit, std::size_t Left>
inline void Audio::DSP::UnrolledConvert(OutputUnit * const output, const InputUnit * const input) noexcept
{
    if constexpr (std::is_floating_point_v<InputUnit>) {
        if constexpr (std::is_unsigned_v<OutputUnit>) {
            *output = (*input + static_cast<InputUnit>(1)) * (ConvertLimits<InputUnit, OutputUnit>::Max / static_cast<OutputUnit>(2));
        } else if constexpr (std::is_signed_v<OutputUnit>) {
            *output = *input * ConvertLimits<InputUnit, OutputUnit>::Max;
        } else if constexpr (std::is_floating_point_v<OutputUnit>) {
            *output = static_cast<OutputUnit>(*input);
        }
    } else if constexpr (std::is_unsigned_v<InputUnit>) {
        if constexpr (std::is_floating_point<OutputUnit>::value) {
            *output = static_cast<OutputUnit>(*input) / static_cast<OutputUnit>(std::numeric_limits<InputUnit>::max()) * static_cast<OutputUnit>(2) - static_cast<OutputUnit>(1);
        } else if constexpr (std::is_signed_v<OutputUnit>) {
            *output = (static_cast<OutputUnit>(*input)) / (std::numeric_limits<OutputUnit>::max() * 2);
        } else if constexpr (std::is_unsigned_v<OutputUnit>) {
            *output = (*input * std::numeric_limits<OutputUnit>::max() / std::numeric_limits<InputUnit>::max());
        }
    } else if constexpr (std::is_signed_v<InputUnit>) {
        if constexpr (std::is_floating_point<OutputUnit>::value) {
            *output = (static_cast<float>(*input) < 0) ?
                (static_cast<OutputUnit>(*input)) / (std::numeric_limits<InputUnit>::max() / 2) :
                (static_cast<OutputUnit>(*input)) / (std::numeric_limits<InputUnit>::max() / 2) - 1;
        } else if constexpr (std::is_unsigned_v<OutputUnit>) {
            *output = (*input / (std::numeric_limits<OutputUnit>::max()));
        } else if constexpr (std::is_signed_v<OutputUnit>) {
            *output = (*input * std::numeric_limits<OutputUnit>::max() / std::numeric_limits<InputUnit>::max());
        }
    }
    if constexpr (Left > 0)
        UnrolledConvert<InputUnit, OutputUnit, Left - 1>(output + 1, input + 1);
}
    // std::is_floating_point_v -> float / double
    // std::is_integral_v -> char /

    /*
    #include <limits>
std::numeric_limits<std::int8_t>::max();
 /**
 * F [ -1 : 1 ]
 * S [ -MAX / 2 : MAX / 2 - 1 ]
 * U [ 0 : MAX ]
 *
 * F -> S
 * x < 0 : x * (MAX / 2) : x * (MAX / 2 - 1)
 * F -> U
 * (x + 1) / 2 * MAX
 * U -> F
 * x / MAX * 2 - 1
 * S -> F
 * x < 0 : x / (MAX / 2) : x / (MAX / 2 - 1)
 */