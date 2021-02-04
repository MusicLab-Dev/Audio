/**
 * @ Author: Pierre Veysseyre
 * @ Description: Unit tests of IPlugin class
 */

#include <gtest/gtest.h>

#include <Audio/Plugins/SimpleDelay.hpp>
#include <Audio/Plugins/Oscillator.hpp>

using namespace Audio;

static constexpr BlockSize Size = 1024u;
static constexpr SampleRate SR = 48000u;
static constexpr auto Arrangement = ChannelArrangement::Stereo;
static constexpr auto MaxInputs = 4;

using Type = char;

TEST(IPlugin, SimpleDelay)
{
    Buffer dummy(Size * sizeof(Type), SR, Arrangement);
    SimpleDelay delay (SR, Size * sizeof(Type), Arrangement, 10);

    EXPECT_EQ(delay.delay(), 1);
    EXPECT_EQ(delay.readIdx(), 0);
    EXPECT_EQ(delay.writeIdx(), 0);

    delay.receiveAudio(dummy);
    EXPECT_EQ(delay.readIdx(), 1);
    EXPECT_EQ(delay.writeIdx(), 0);

    BufferViews input;
    for (auto i = 0u; i < MaxInputs; ++i) {
        Buffer dum(Size * sizeof(Type), SR, Arrangement);
        auto b = input.push(dum);
        for (auto c = 0u; c < static_cast<std::uint32_t>(Arrangement); ++c) {
            for (auto j = 0u; j < Size; ++j) {
                dum.data<Type>(static_cast<Channel>(c))[j] = c;
            }
        }
    }

    delay.sendAudio(input);
    EXPECT_EQ(delay.readIdx(), 1);
    EXPECT_EQ(delay.writeIdx(), 1);

    // Buffer dummy(Size * sizeof(Type), SR, Arrangement);
    delay.receiveAudio(dummy);
    EXPECT_EQ(delay.readIdx(), 1);
    EXPECT_EQ(delay.writeIdx(), 1);

    // delay.onAudioBlockGenerated();
    // EXPECT_EQ(delay.readIdx(), 2);
    // EXPECT_EQ(delay.writeIdx(), 1);
}

TEST(IPlugin, Oscillator)
{
    // Oscillator osc;
    // auto env = osc.enveloppe();

    // env.triggerOff();

}