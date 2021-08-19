/**
 * @ Author: Pierre Veysseyre
 * @ Description: Math.hpp
 */

#include <utility>

#include "Math.hpp"

using namespace Audio;

[[nodiscard]] static constexpr std::size_t GenerateRandomData(std::size_t &out)
{
    // std::size_t z = (out += UINT64_C(0x9E3779B97F4A7C15));
    // z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
    // z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
    // return z ^ (z >> 31);
    out += UINT64_C(0x9E3779B97F4A7C15);
    out ^= (out >> 30) * UINT64_C(0xBF58476D1CE4E5B9);
    out ^= (out >> 27) * UINT64_C(0x94D049BB133111EB);
    return out ^ (out >> 31);

    // 32bits version
    // out ^= out << (13u);
    // out ^= out >> (17u);
    // out ^= out << (5u);
    // return out;
}

template <auto ...Indexes>
[[nodiscard]] static constexpr std::array<std::size_t, Utils::RandomDataSet::Size> GenerateRandomDataSet(std::integer_sequence<std::size_t, Indexes...>) noexcept
{
    std::size_t out { 1234567890ul };

    return std::array<std::size_t, Utils::RandomDataSet::Size> {
        (static_cast<void>(Indexes), GenerateRandomData(out))...
    };
}

/** @brief Random data set used by FastRand */
std::array<std::size_t, Utils::RandomDataSet::Size> Utils::RandomDataSet::Array {
    GenerateRandomDataSet(std::make_integer_sequence<std::size_t, Utils::RandomDataSet::Size> {})
};
