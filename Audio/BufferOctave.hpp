/**
 * @ Author: Pierre Veysseyre
 * @ Description: BufferOctave.hpp
 */

#pragma once

#include "Buffer.hpp"

namespace Audio
{
    static constexpr std::size_t KeysPerOctave = 12u;

    using BufferOctave = std::array<Buffer, KeysPerOctave>;
}