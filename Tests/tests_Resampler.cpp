/**
 * @ Author: Pierre V
 * @ Description: Unit tests of Resampling
 */

#include <gtest/gtest.h>

#include <Audio/DSP/Resampler.hpp>

using namespace Audio;

using Type = float;
static constexpr auto Size = 16u;

using Rs = DSP::Resampler<Type>;

// static constexpr auto Size { 19'765u };
static const std::size_t SizesDown[11] {
    20940, 22185, 23504, 24901, 26381, 27949, 29610, 31370, 33235, 35211, 37304
};
static const std::size_t SizesUp[11] {
    18655, 17608, 16619, 15686, 14805, 13974, 13189, 12448, 11749, 11089, 10466
};

TEST(Resampler, SizesHelper)
{
    ASSERT_EQ(Rs::GetResampleOctaveBufferSize(Size, -1), Size * 2);
    ASSERT_EQ(Rs::GetResampleOctaveBufferSize(Size, -2), Size * 4);
    ASSERT_EQ(Rs::GetResampleOctaveBufferSize(Size, -3), Size * 8);

    ASSERT_EQ(Rs::GetResampleOctaveBufferSize(Size, 1), Size / 2);
    ASSERT_EQ(Rs::GetResampleOctaveBufferSize(Size, 2), Size / 4);
    ASSERT_EQ(Rs::GetResampleOctaveBufferSize(Size, 3), Size / 8);

    ASSERT_EQ(Rs::GetResampleSampleRateBufferSize(44100, 44100, 48000), 48000);
    ASSERT_EQ(Rs::GetResampleSampleRateBufferSize(48000, 48000, 44100), 44100);
    // Interpolation of 1 semitone (== resample 1 semitone down)
    ASSERT_EQ(Rs::GetResampleSampleRateBufferSize(44100, Rs::InterpolationSemitoneFactor, Rs::DecimationSemitoneFactor), 46722);
    // Decimation of 1 semiton (== resample 1 semitone up)
    ASSERT_EQ(Rs::GetResampleSampleRateBufferSize(44100, Rs::DecimationSemitoneFactor, Rs::InterpolationSemitoneFactor), 41625);

    ASSERT_EQ(Rs::GetResampleSemitoneBufferSize(44100, false), 46722);
    ASSERT_EQ(Rs::GetResampleSemitoneBufferSize(44100, true), 41625);
}


// TEST(Resampler, DefaultOctave)
// {
//     Buffer buf(Size * sizeof(T), 48000, ChannelArrangement::Mono);
//     const auto inputSize = buf.size<T>();
//     EXPECT_EQ(inputSize, Size);

//     BufferViews octaveBuffer;
//     DSP::Resampler<T>::GenerateDefaultOctave(buf, octaveBuffer);
//     EXPECT_EQ(octaveBuffer.size(), 11);

//     for (auto i = 0u; i < 11; ++i) {
//         if (i < DSP::Resampler<T>::RootKeyDefaultOctave)
//             EXPECT_EQ(octaveBuffer[i].size<T>(), SizesDown[i]);
//         else
//             EXPECT_EQ(octaveBuffer[i].size<T>(), SizesUp[i - DSP::Resampler<T>::RootKeyDefaultOctave]);
//     }
// }

// TEST(Resampler, SampleRate)
// {
//     Buffer buf(Size * sizeof(T), 48000, ChannelArrangement::Mono);
//     const auto inputSize = buf.size<T>();
//     EXPECT_EQ(inputSize, Size);

//     Buffer lowerRate = DSP::Resampler<T>::ResampleBySamplerate(buf, 44100);
//     EXPECT_EQ(lowerRate.size<T>(), 21512);

//     Buffer higherRate = DSP::Resampler<T>::ResampleBySamplerate(lowerRate, 48000);
//     EXPECT_EQ(higherRate.size<T>(), 19764);
// }
