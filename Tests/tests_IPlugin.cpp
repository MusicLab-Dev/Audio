/**
 * @ Author: Pierre Veysseyre
 * @ Description: Unit tests of IPlugin class
 */

#include <gtest/gtest.h>

#include <Audio/IPlugin.hpp>

using namespace Audio;

static constexpr BlockSize Size = 1024u;
static constexpr SampleRate SR = 48000u;
static constexpr auto Arrangement = ChannelArrangement::Stereo;
static constexpr auto MaxInputs = 4;

using Type = char;

TEST(IPlugin, Basics)
{

}