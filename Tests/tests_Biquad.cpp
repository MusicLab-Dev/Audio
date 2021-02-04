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

// TEST(Biquad, Instantiation)
// {
//     auto filterD1 = BiquadMaker<BiquadParam::Optimization::Classic>::MakeBiquad<TypeD1>(48000, 0.2);
//     auto filterD2 = BiquadMaker<BiquadParam::Optimization::Optimized>::MakeBiquad<TypeD2>(48000, 0.2);
//     // auto filterTP1 = BiquadMaker<BiquadParam::Optimization::Classic>::MakeBiquad<TypeD3>(48000, 0.2);
//     // auto filterTP2 = BiquadMaker<BiquadParam::Optimization::Optimized>::MakeBiquad<TypeD4>(48000, 0.2);

//     TypeD1 bufD1[Size] {};
//     TypeD2 bufD2[Size] {};
//     TypeT1 bufT1[Size] {};
//     TypeT2 bufT2[Size] {};

//     filterD1.processBlock(bufD1, Size);
//     filterD1.processBlock(bufD2, Size);

// }
