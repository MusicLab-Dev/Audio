/**
 * @ Author: Pierre V
 * @ Description: Unit tests of Resampling
 */

#include <gtest/gtest.h>

#include <Audio/DSP/Resampler.hpp>

using namespace Audio;

using Type = float;
static constexpr auto Size = 10u;

// static constexpr auto Size { 19'765u };
static const std::size_t SizesDown[11] {
    20940, 22185, 23504, 24901, 26381, 27949, 29610, 31370, 33235, 35211, 37304
};
static const std::size_t SizesUp[11] {
    18655, 17608, 16619, 15686, 14805, 13974, 13189, 12448, 11749, 11089, 10466
};

static std::size_t GetSemitoneRateSize(const Semitone semitone) {
    return std::pow(2.0, semitone / 12.0);
}

TEST(Resampler, InterpolationOneOctave)
{
    Type buffer[Size];
    Type outInter1[Size * 2];
    Type outInter2[Size * 2];
    for (auto i = 0; i < Size; ++i) {
        buffer[i] = 3;
        outInter1[i] = 1;
        outInter1[i + 1] = 1;
        outInter2[i] = 1;
        outInter2[i + 1] = 1;
    }
    DSP::Resampler<Type>::Interpolate(buffer, outInter1, Size, 2);
    DSP::Resampler<Type>::Internal::InterpolateOctave(buffer, outInter2, Size, 1);

    for (auto k = 0; k < Size; ++k) {
        EXPECT_EQ(outInter1[k * 2 + 1], 0);
        EXPECT_EQ(outInter1[k * 2], 3);
        EXPECT_EQ(outInter2[k * 2 + 1], 0);
        EXPECT_EQ(outInter2[k * 2], 3);
    }
}

TEST(Resampler, DecimationOneOctave)
{
    const std::size_t dRate = 4;
    Type buffer[Size];
    Type outDecim1[Size * 2];
    for (auto i = 0; i < Size; ++i) {
        buffer[i] = 3;
        outDecim1[i] = 1;
        outDecim1[i + 1] = 1;
    }

    DSP::Resampler<Type>::Decimate(buffer, outDecim1, Size, 4);
    for (auto k = 0; k < Size; ++k) {
        // std::cout << outDecim1[k] << std::endl;
        if (k < Size / 4) {
            EXPECT_EQ(outDecim1[k], 3);
        } else {
            EXPECT_EQ(outDecim1[k], 1);
        }
    }
}

TEST(Resampler, InterpolationSemitone)
{
    const std::size_t iRate = 5;
    Type buffer[Size];
    Type outBuffer[Size * iRate];
    for (auto i = 0; i < Size; ++i) {
        buffer[i] = 3;
        outBuffer[i] = 1;
        outBuffer[i + 1] = 1;
    }

    DSP::Resampler<Type>::Interpolate(buffer, outBuffer, Size, iRate);
    for (auto k = 0; k < Size; ++k) {
        EXPECT_EQ(outBuffer[k * iRate], 3);
        EXPECT_EQ(outBuffer[k * iRate + 1], 0);
    }
}

// TEST(Resampler, ResamplingSize)
// {
//     Buffer buf(Size * sizeof(T), 48000, ChannelArrangement::Mono);
//     const auto inputSize = buf.size<T>();
//     EXPECT_EQ(inputSize, Size);

//     for (auto i = 0u; i < 11; ++i) {
//         auto up = DSP::Resampler<T>::ResampleBySemitone(BufferView(buf), i + 1);
//         auto down = DSP::Resampler<T>::ResampleBySemitone(BufferView(buf), -(i + 1));
//         EXPECT_EQ(up.size<T>(), SizesUp[i]);
//         EXPECT_EQ(down.size<T>(), SizesDown[i]);
//     }

//     for (auto i = 0u; i < 3; ++i) {
//         auto octave = i + 1;
//         std::cout << octave <<std::endl;
//         auto up = DSP::Resampler<T>::DecimateByOctave(BufferView(buf), octave);
//         auto down = DSP::Resampler<T>::InterpolateByOctave(BufferView(buf), octave);
//         std::cout << "up: " << up.size<T>() << std::endl;
//         std::cout << "down: " << down.size<T>() << std::endl;
//         EXPECT_EQ(up.size<T>(), std::ceil(inputSize / std::pow(2.0, octave)));
//         EXPECT_EQ(down.size<T>(), inputSize * std::pow(2.0, octave));
//     }
// }

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
