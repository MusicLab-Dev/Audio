/**
 * @ Author: Pierre Veysseyre
 * @ Description: Unit tests of Auto Correlation
 */

#include <gtest/gtest.h>

#include <Audio/DSP/Vocoder.hpp>

using namespace DSP;
using namespace Audio;

TEST(Vocoder, Basics)
{
    Buffer input(12, 12, ChannelArrangement::Mono);
    Vocoder::PitchShift(input, 1, true);
}