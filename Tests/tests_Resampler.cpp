/**
 * @ Author: Pierre V
 * @ Description: Unit tests of Resampling
 */

#include <gtest/gtest.h>

#include <Audio/DSP/Resampler.hpp>

using namespace Audio;

using T = float;

static constexpr auto Size { 19'765u };
static const std::size_t SizesDown[11] {
    20940, 22185, 23504, 24901, 26381, 27949, 29610, 31370, 33235, 35211, 37304
};
static const std::size_t SizesUp[11] {
    18655, 17608, 16619, 15686, 14805, 13974, 13189, 12448, 11749, 11089, 10466
};

static std::size_t GetSemitoneRateSize(const Semitone semitone) {
    return std::pow(2.0, semitone / 12.0);
}

TEST(Resampler, ResamplingSize)
{
    Buffer buf(Size * sizeof(T), 48000, ChannelArrangement::Mono);
    const auto inputSize = buf.size<T>();
    EXPECT_EQ(inputSize, Size);

    for (auto i = 0u; i < 11; ++i) {
        auto up = DSP::Resampler<T>::ResampleSemitone(BufferView(buf), i + 1);
        auto down = DSP::Resampler<T>::ResampleSemitone(BufferView(buf), -(i + 1));
        EXPECT_EQ(up.size<T>(), SizesUp[i]);
        EXPECT_EQ(down.size<T>(), SizesDown[i]);
    }

    for (auto i = 0u; i < 3; ++i) {
        auto octave = i + 1;
        std::cout << octave <<std::endl;
        auto up = DSP::Resampler<T>::DecimateOctave(BufferView(buf), octave);
        auto down = DSP::Resampler<T>::InterpolateOctave(BufferView(buf), octave);
        std::cout << "up: " << up.size<T>() << std::endl;
        std::cout << "down: " << down.size<T>() << std::endl;
        EXPECT_EQ(up.size<T>(), std::ceil(inputSize / std::pow(2.0, octave)));
        EXPECT_EQ(down.size<T>(), inputSize * std::pow(2.0, octave));
    }
}

TEST(Resampler, DefaultOctave)
{
    Buffer buf(Size * sizeof(T), 48000, ChannelArrangement::Mono);
    const auto inputSize = buf.size<T>();
    EXPECT_EQ(inputSize, Size);

    BufferViews octaveBuffer;
    DSP::Resampler<T>::GenerateDefaultOctave(buf, octaveBuffer);
    EXPECT_EQ(octaveBuffer.size(), 11);

    for (auto i = 0u; i < 11; ++i) {
        if (i < DSP::Resampler<T>::RootKeyDefaultOctave)
            EXPECT_EQ(octaveBuffer[i].size<T>(), SizesDown[i]);
        else
            EXPECT_EQ(octaveBuffer[i].size<T>(), SizesUp[i - DSP::Resampler<T>::RootKeyDefaultOctave]);
    }
}
