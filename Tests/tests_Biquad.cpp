/**
 * @ Author: Pierre Veysseyre
 * @ Description: Unit tests of Biquad filters
 */

#include <gtest/gtest.h>

#include <Audio/DSP/Biquad.hpp>

#include <Audio/Math.hpp>

#include <iostream>

using namespace Audio::DSP;

using TypeD1 = std::uint8_t;
using TypeD2 = std::int8_t;
using TypeT1 = float;
using TypeT2 = double;

static constexpr auto Size = 1024u;

TEST(Biquad, Instantiation)
{
    auto filterD1 = Biquad::Filter<Biquad::Internal::Form::Direct1>();
    auto filterD2 = Biquad::Filter<Biquad::Internal::Form::Direct2>();
    auto filterT1 = Biquad::Filter<Biquad::Internal::Form::Transposed1>();
    auto filterT2 = Biquad::Filter<Biquad::Internal::Form::Transposed2>();

    UNUSED(filterD1);
    UNUSED(filterD2);
    UNUSED(filterT1);
    UNUSED(filterT2);


    EXPECT_FLOAT_EQ(Audio::Utils::Log2<20, 200>::GetLog(0.0f), 20.0f);
    EXPECT_FLOAT_EQ(Audio::Utils::Log2<20, 200>::GetLog(1.0f), 200.0f);

}
