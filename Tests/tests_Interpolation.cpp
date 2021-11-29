/**
 * @ Author: Pierre Veysseyre
 * @ Description: Unit tests of Biquad filters
 */

#include <gtest/gtest.h>

#include <Audio/Math.hpp>

#include <iostream>

using LogFreq = Audio::Utils::LogFrequency;

TEST(IntepolationMapping, Log2)
{
    // std::cout << LogFreq::FromF << std::endl;
    // std::cout << LogFreq::ToF << std::endl;

    EXPECT_FLOAT_EQ(LogFreq::GetLog(0.0f), 50.0f);
    // EXPECT_FLOAT_EQ((LogFreq::GetLog(0.25f)), 640.0f);
    // EXPECT_FLOAT_EQ((LogFreq::GetLog(0.5f)), 640.0f);
    // EXPECT_FLOAT_EQ((LogFreq::GetLog(0.75f)), 640.0f);
    EXPECT_FLOAT_EQ(LogFreq::GetLog(1.0f), 22'000.0f);
}
