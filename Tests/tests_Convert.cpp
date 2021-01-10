/**
 * @ Author: Antoine La Mache
 * @ Description: Unit tests of merge function
 */

#include <vector>
#include <iostream>

#include <gtest/gtest.h>

#include <Audio/DSP/Convert.hpp>

using namespace Audio;

template<typename Type, std::size_t Range>
std::vector<Type> GenerateIncreasingRange(void)
{
    std::vector<Type> vec;
    Type value {};

    vec.resize(Range);
    for (auto &it : vec)
        it = ++value;
    return vec;
}

template<typename Type, std::size_t Range>
std::vector<Type> GenerateDecreasingRange(void)
{
    std::vector<Type> vec;
    Type value {};

    vec.resize(Range);
    for (auto &it : vec)
        it = --value;
    return vec;
}


template<typename Input, typename Output, std::size_t BufferSize = 16>
static void TestUniqueConversion(const Input input, const Output expected)
{
    std::vector<Input> inputs(BufferSize, input);
    std::vector<Output> outputs(BufferSize, Output {});

    DSP::Convert<Input, Output, BufferSize>(inputs.data(), outputs.data());
    for (auto &elem : outputs) {
        ASSERT_EQ(elem, expected);
    }
}

TEST(Convert, FloatToInt)
{
    TestUniqueConversion<float, int>(0.0f, 0);
    TestUniqueConversion<float, int>(1.0f, DSP::ConvertLimits<float, int>::Max + 1);
    TestUniqueConversion<float, int>(-1.0f, -1 * (DSP::ConvertLimits<float, int>::Max + 1));
}

// TEST(test_Convert, float)
// {
//     constexpr std::size_t BufferSize = 64;

//      std::cout << std::fixed;
//     float ecart = 0;
//     std::vector<float> output(BufferSize, 0.0f);
//     std::vector<int> first    = GenerateIncreasingRange<int, 64>();
//     std::vector<unsigned int> second   = GenerateDecreasingRange<unsigned int, 64>();
//     std::vector<char> third    = GenerateDecreasingRange<char, 64>();
//     std::vector<int> output2(BufferSize, 0);

//     DSP::Convert<char, int,BufferSize>(
//         third.data(), output2.data()
//     );
//     for (auto i = 3; i < BufferSize; ++i) {
//         std::cout <<"I= "<< i <<
//             " I[]= " << static_cast<int>(third.at(i)) <<
//             " O[]= " << output2.at(i) << std::endl;
//         ASSERT_EQ(
//             output2.at(i) - output2.at(i -1),
//             output2.at(i -2) - output2.at(i-3)
//         );
//     }
//     DSP::Convert<int, float,BufferSize>(
//         first.data(), output.data()
//     );
//     for (auto i = 4; i < BufferSize; ++i) {
//        std::cout
//        << std::setprecision(10)
//         <<"I= "<< i <<
//             " I[]= " << first.at(i) <<
//             " O[]= " << output.at(i) << std::endl;
//         ASSERT_FLOAT_EQ(
//             output.at(i) - output.at(i -1),
//             output.at(i -2) - output.at(i-3)
//         );
//     }
// }
