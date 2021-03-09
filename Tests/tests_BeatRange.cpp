/**
 * @ Author: Pierre Veysseyre
 * @ Description: Unit tests of BeatRange struct
 */

#include <gtest/gtest.h>

#include <Audio/BaseIndex.hpp>

using namespace Audio;


TEST(Note, EqualOperator)
{
    // reference
    BeatRange range { 10, 20 };

    // inferior
    BeatRange inf { 0, 5 };

    // bounds
    BeatRange boundInf { 5, 10 };
    BeatRange boundSup { 20, 25 };

    // overlap
    BeatRange rangeOverlapInf { 5, 15 };
    BeatRange rangeOverlapInside { 15, 18 };
    BeatRange rangeOverlapSup { 18, 25 };


    EXPECT_TRUE(inf < range);
    EXPECT_TRUE(inf <= range);
    EXPECT_TRUE(boundInf <= range);
    // EXPECT_TRUE(boundSup >= range);
    // EXPECT_TRUE(boundInf > range);
    // EXPECT_TRUE(boundSup < range);
}
