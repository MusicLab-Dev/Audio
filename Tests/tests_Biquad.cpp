/**
 * @ Author: Pierre Veysseyre
 * @ Description: Unit tests of Biquad filters
 */

#include <gtest/gtest.h>

#include <Audio/DSP/Biquad.hpp>

#include <iostream>

using namespace DSP;

using TypeD1 = std::uint8_t;
using TypeD2 = std::int8_t;
using TypeT1 = float;
using TypeT2 = double;

static constexpr auto Size = 1024u;

TEST(Biquad, Instantiation)
{
    auto filterD1 = BiquadMaker<BiquadParam::Optimization::Classic>::MakeBiquad<TypeD1>();
    auto filterD2 = BiquadMaker<BiquadParam::Optimization::Optimized>::MakeBiquad<TypeD2>();
    auto filterT1 = BiquadMaker<BiquadParam::Optimization::Classic>::MakeBiquad<TypeT1>();
    auto filterT2 = BiquadMaker<BiquadParam::Optimization::Optimized>::MakeBiquad<TypeT2>();

    ASSERT_EQ(decltype(filterD1)::InternalType, BiquadParam::InternalForm::Direct1);
    ASSERT_EQ(decltype(filterD2)::InternalType, BiquadParam::InternalForm::Direct2);
    ASSERT_EQ(decltype(filterT1)::InternalType, BiquadParam::InternalForm::Transposed1);
    ASSERT_EQ(decltype(filterT2)::InternalType, BiquadParam::InternalForm::Transposed2);

    TypeD1 bufD1[Size] {};
    TypeD2 bufD2[Size] {};
    TypeT1 bufT1[Size] {};
    TypeT2 bufT2[Size] {};



    // filterD1.processBlock(bufD1, Size);
    // filterD1.processBlock(bufD2, Size);

}
