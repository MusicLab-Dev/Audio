/**
 * @ Author: Pierre Veysseyre
 * @ Description: BufferOctave.hpp
 */

#pragma once

#include "Buffer.hpp"

namespace Audio
{
    static constexpr std::uint8_t KeysPerOctave = 12u;
    static constexpr std::uint8_t OctaveRootKey = 5u;
    static constexpr std::uint8_t RootOctave = 5u;

    static_assert(OctaveRootKey < KeysPerOctave,
        "BufferOctave: OctaveRootKey must be less than KeysPerOctave");

    using BufferOctave = std::array<Buffer, KeysPerOctave>;

    template<typename Type>
    static void GenerateOctave(const BufferView input, BufferOctave &octave) noexcept;
}

#include "BufferOctave.ipp"