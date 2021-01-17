/**
 * @ Author: Pierre Veysseyre
 * @ Description: Correlation.hpp
 */

#pragma once

#include <Audio/Buffer.hpp>

namespace DSP
{
    class Vocoder;
};

class DSP::Vocoder
{
public:
    static constexpr auto AnalysisSize = 256;
    static constexpr auto AnalysisHopSize = AnalysisSize * 3 / 4;

    static Audio::Buffer PitchShift(const Audio::Buffer &input, const std::uint8_t seminote, bool pitchDirection) noexcept;

private:
    static std::uint32_t getBuffersSize(const Audio::Buffer &buffer) noexcept { return buffer.size<float>(); }
};