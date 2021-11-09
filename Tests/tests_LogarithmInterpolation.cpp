/**
 * @ Author: Pierre Veysseyre
 * @ Description: Unit tests of Biquad filters
 */

#include <gtest/gtest.h>

#include <Audio/Base.hpp>

#include <iostream>

TEST(LogInterp, Basics)
{
    // EXPECT_EQ(Audio::GetNoteFrequency(0u), 0.0f);
    // EXPECT_EQ(Audio::GetNoteFrequency(128u), 0.0f);
    // EXPECT_EQ(Audio::GetNoteFrequency(132u), 0.0f);

    EXPECT_FLOAT_EQ(Audio::GetFrequencyRatio(0.0f), 120.0f);
    EXPECT_FLOAT_EQ(Audio::GetFrequencyRatio(0.25f), 120.0f);
    EXPECT_FLOAT_EQ(Audio::GetFrequencyRatio(0.5f), 1640.0f);
    EXPECT_FLOAT_EQ(Audio::GetFrequencyRatio(0.75f), 1640.0f);
    EXPECT_FLOAT_EQ(Audio::GetFrequencyRatio(1.0f), 120'000.0f);
}
