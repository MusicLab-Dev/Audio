/**
 * @ Author: Pierre Veysseyre
 * @ Description: Unit tests of Biquad filters
 */

#include <gtest/gtest.h>

#include <Audio/DSP/Reformater.hpp>

#include <iostream>

using namespace Audio::DSP;

using U8 = std::uint8_t;
using S8 = std::int8_t;
using F32 = float;
using F64 = double;

TEST(Reformater, FromFloatingBounds)
{
    F32 f32in { -1.0f };
    F64 f64out { 3.0 };
    U8 u8out { 3 };
    S8 s8out { 3 };

    Reformater::Reformat<F32, F64>(&f32in, &f64out, 1u);
    ASSERT_EQ(f64out, -1.0);
    Reformater::Reformat<F32, U8>(&f32in, &u8out, 1u);
    ASSERT_EQ(u8out, 0);
    Reformater::Reformat<F32, S8>(&f32in, &s8out, 1u);
    ASSERT_EQ(s8out, -127);


    f32in = { 1.0f };
    f64out = { 3.0 };
    u8out = { 3 };
    s8out = { 3 };

    Reformater::Reformat<F32, F64>(&f32in, &f64out, 1u);
    ASSERT_EQ(f64out, 1.0);
    Reformater::Reformat<F32, U8>(&f32in, &u8out, 1u);
    ASSERT_EQ(u8out, 255);
    Reformater::Reformat<F32, S8>(&f32in, &s8out, 1u);
    ASSERT_EQ(s8out, 127);


    f32in = { 0.0f };
    f64out = { 3.0 };
    u8out = { 3 };
    s8out = { 3 };

    Reformater::Reformat<F32, F64>(&f32in, &f64out, 1u);
    ASSERT_EQ(f64out, 0.0);
    Reformater::Reformat<F32, U8>(&f32in, &u8out, 1u);
    ASSERT_EQ(u8out, 127);
    Reformater::Reformat<F32, S8>(&f32in, &s8out, 1u);
    ASSERT_EQ(s8out, 0);



    F64 f64in { -1.0f };
    F32 f32out { 3.0f };
    u8out = { 3 };
    s8out = { 3 };

    Reformater::Reformat<F64, F32>(&f64in, &f32out, 1u);
    ASSERT_EQ(f32out, -1.0f);
    Reformater::Reformat<F64, U8>(&f64in, &u8out, 1u);
    ASSERT_EQ(u8out, 0);
    Reformater::Reformat<F64, S8>(&f64in, &s8out, 1u);
    ASSERT_EQ(s8out, -127);


    f64in = { 1.0 };
    f32out = { 3.0f };
    u8out = { 3 };
    s8out = { 3 };

    Reformater::Reformat<F64, F32>(&f64in, &f32out, 1u);
    ASSERT_EQ(f32out, 1.0f);
    Reformater::Reformat<F64, U8>(&f64in, &u8out, 1u);
    ASSERT_EQ(u8out, 255);
    Reformater::Reformat<F64, S8>(&f64in, &s8out, 1u);
    ASSERT_EQ(s8out, 127);


    f64in = { 0.0 };
    f32out = { 3.0f };
    u8out = { 3 };
    s8out = { 3 };

    Reformater::Reformat<F64, F32>(&f64in, &f32out, 1u);
    ASSERT_EQ(f32out, 0.0f);
    Reformater::Reformat<F64, U8>(&f64in, &u8out, 1u);
    ASSERT_EQ(u8out, 127);
    Reformater::Reformat<F64, S8>(&f64in, &s8out, 1u);
    ASSERT_EQ(s8out, 0);
}
