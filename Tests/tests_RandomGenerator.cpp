/**
 * @ Author: Pierre Veysseyre
 * @ Description: Unit tests of Envelope Generator class
 */

#include <gtest/gtest.h>

#include <Audio/DSP/FM.hpp>

using namespace Audio;


TEST(RandomGenerator, Seed)
{
    Utils::RandomGenerator genDefault;
    Utils::RandomGenerator genFloat;

    for (auto i = 0u; i < 100; ++i) {
        const auto rnd = genDefault.rand();
        const auto rndFloat = genFloat.randAs<float>();
        std::cout << i << ": " << rnd << " - " << rndFloat << std::endl;
    }
}
