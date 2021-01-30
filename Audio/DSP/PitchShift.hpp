#pragma once

#include <Audio/KissFFT.hpp>

namespace Audio::DSP
{
    struct PitchShift;
};

struct Audio::DSP::PitchShift
{
    /** @brief Block size */
    static constexpr auto BlockSize = 1024u;

    /** @brief Overlap rate */
    static constexpr auto FrameHopRate = 4; // 75% == (1 - 1 / FrameHopRate) == (1 - 1 / 4) = 3/4
    static constexpr auto FrameHopSize = BlockSize / 4; // 75% == (1 - 1 / FrameHopRate) == (1 - 1 / 4) = 3/4

    using Semitone = char;

    template<typename T>
    static Audio::Buffer Shift(const Audio::BufferView &input, const Semitone semitone);

};

#include "PitchShift.ipp"
