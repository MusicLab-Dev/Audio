/**
 * @ Author: Pierre Veysseyre
 * @ Description: Correlation.cpp
 */

#include <cmath>
#include <kissfft/kiss_fft.h>

#include "Vocoder.hpp"

Audio::Buffer DSP::Vocoder::PitchShift(const Audio::Buffer &input, const std::uint8_t seminote, bool pitchDirection) noexcept
{
    const std::uint32_t inputSize = getBuffersSize(input);
    const std::uint32_t scaledSize = (pitchDirection ?
        inputSize * std::pow(2, 1.f / 12.f) * seminote :
        inputSize / std::pow(2, 1.f / 12.f) * seminote
    );

    Audio::Buffer output(input.channelByteSize(), input.sampleRate(), input.channelArrangement());
    // std::vector<kiss_fft_cpx> inputFFT()



    return output;
}
