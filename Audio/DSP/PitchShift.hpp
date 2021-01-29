#pragma once

#include <Audio/Buffer.hpp>

namespace DSP
{
    struct PitchShift;
};

struct DSP::PitchShift
{
    using Semitone = char;

    static Audio::Buffer Shift(const Audio::BufferView &input, const Audio::SampleRate sampleRate, const Semitone semitone);
};

#include "PitchShift.ipp"
