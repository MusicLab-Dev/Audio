/**
 * @ Author: Pierre Veysseyre
 * @ Description: Unit tests of Instance class
 */

#include <gtest/gtest.h>

#include <Audio/Device.hpp>
#include <iostream>

using namespace Audio;

static const Device::LogicalDescriptor TestDescriptor {
    /*.name = */ "device-test",
    /*.blockSize = */ 2048u,
    /*.sampleRate = */ 44100u,
    /*.isInput = */ false,
    /*.format = */ Format::Floating32,
    /*.midiChannels = */ 2u,
    /*.channelsArragement = */ ChannelArrangement::Stereo
};

static float Phase { 0.0f };

static void Callback(std::uint8_t *stream, int len)
{
    // std::memset(stream, 0, len);

    const auto channels = static_cast<std::size_t>(TestDescriptor.channelArrangement);
    float *out = reinterpret_cast<float *>(stream);
    for (auto i = 0u; i < TestDescriptor.blockSize; ++i) {
        for (auto j = 0u; j < channels; ++j) {
            out[channels * i + j] = std::sin(Phase);
        }
        // out[i] = std::sin(Phase);
        Phase += 2.0f * M_PI * 0.01f;
    }
    while (Phase >= 2.0f * M_PI)
        Phase -= 2.0f * M_PI;
    std::cout << "AUDIO CALLBACK TICK !\n";
}

TEST(Instanciation, NoInitSDL)
{
    try {
        Device device(TestDescriptor, [](std::uint8_t *data, const std::size_t size) { Callback(data, size); });
        FAIL() << "Expected throw !";
    } catch (const std::runtime_error &err) {
        EXPECT_STREQ(err.what(), "Couldn't open any audio device: Audio subsystem is not initialized");
    } catch (...) {
        FAIL() << "Expected std::runtime_error";
    }
}

TEST(Instantiation, InitSDL)
{
    Device::InitDriver();

    try {
        Device device(TestDescriptor, &Callback);
        SUCCEED() << "Device correctly initialized.";
    } catch (...) {
        FAIL() << "No throw was expected !";
    }

    Device::ReleaseDriver();
}

TEST(Instantiation, BasicsStartStop)
{
    Device::InitDriver();

    Device device(TestDescriptor, &Callback);
    EXPECT_EQ(device.running(), false);

    device.start();
    EXPECT_EQ(device.running(), true);

    sleep(1);

    device.stop();
    EXPECT_EQ(device.running(), false);

    Device::ReleaseDriver();
}
