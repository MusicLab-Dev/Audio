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

static std::size_t GetSemitoneRateSize(const Semitone semitone) {
    return std::pow(2.0, semitone / 12.0);
}

static void ResetBuffer(Type *buffer, const std::size_t len, const Type value)
{
    for (auto i = 0u; i < len; ++i)
        buffer[i] = value;
}


TEST(Resampler, SizesHelper)
{
    ASSERT_EQ(Rs::GetInterpolationOctaveSize(Size, 1), Size * 2);
    ASSERT_EQ(Rs::GetInterpolationOctaveSize(Size, 2), Size * 4);
    ASSERT_EQ(Rs::GetInterpolationOctaveSize(Size, 3), Size * 8);

    ASSERT_EQ(Rs::GetInterpolationSize(Size, 2), Size * 2);
    ASSERT_EQ(Rs::GetInterpolationSize(Size, 3), Size + (Size - 1) * 2);
    ASSERT_EQ(Rs::GetInterpolationSize(Size, 4), Size * 4);
    ASSERT_EQ(Rs::GetInterpolationSize(Size, 5), Size + (Size - 1) * 4);


    ASSERT_EQ(Rs::GetDecimationOctaveSize(Size, 1), Size / 2);
    ASSERT_EQ(Rs::GetDecimationOctaveSize(Size, 2), Size / 4);
    ASSERT_EQ(Rs::GetDecimationOctaveSize(Size, 3), Size / 8);

    ASSERT_EQ(Rs::GetDecimationSize(Size, 2), Size / 2);
    ASSERT_EQ(Rs::GetDecimationSize(Size, 3), std::ceil(static_cast<float>(Size) / 3));
    ASSERT_EQ(Rs::GetDecimationSize(Size, 4), Size / 4);
    ASSERT_EQ(Rs::GetDecimationSize(Size, 5), std::ceil(static_cast<float>(Size) / 5));


    ASSERT_EQ(Rs::GetResamplingSizeOctave(Size, 1), Size / 2);
    ASSERT_EQ(Rs::GetResamplingSizeOctave(Size, -1), Size * 2);
    ASSERT_EQ(Rs::GetResamplingSizeOctave(Size, 2), Size / 4);
    ASSERT_EQ(Rs::GetResamplingSizeOctave(Size, -2), Size * 4);
    ASSERT_EQ(Rs::GetResamplingSizeOctave(Size, 3), Size / 8);
    ASSERT_EQ(Rs::GetResamplingSizeOctave(Size, -3), Size * 8);

    ASSERT_EQ(Rs::GetResamplingSizeSampleRate(44100, 44100, 48000), 47999);
    ASSERT_EQ(Rs::GetResamplingSizeSampleRate(48000, 48000, 44100), 44100);
    // Interpolation of 1 semitone (== resample 1 semitone down)
    ASSERT_EQ(Rs::GetResamplingSizeSampleRate(44100, Rs::L_Factor, Rs::M_Factor), 46722);
    // Decimation of 1 semiton (== resample 1 semitone up)
    ASSERT_EQ(Rs::GetResamplingSizeSampleRate(44100, Rs::M_Factor, Rs::L_Factor), 41625);

    ASSERT_EQ(Rs::GetResamplingSizeSemitone(44100, -1), 46722);
    ASSERT_EQ(Rs::GetResamplingSizeSemitone(44100, 1), 41625);
    ASSERT_EQ(Rs::GetResamplingSizeSemitone(44100, -12), 44100 * 2);
    ASSERT_EQ(Rs::GetResamplingSizeSemitone(44100, 12), 44100 / 2);
    ASSERT_EQ(Rs::GetResamplingSizeSemitone(44100, -13), 93444);
    ASSERT_EQ(Rs::GetResamplingSizeSemitone(44100, 13), 20812);
}

TEST(Resampler, InterpolationOneOctave)
{
    const std::size_t outSize = Rs::GetInterpolationOctaveSize(Size, 1);
    ASSERT_EQ(outSize, Size * 2);

    Type buffer[Size];
    Type outInter1[outSize];
    Type outInter2[outSize];
    ResetBuffer(buffer, Size, 3);
    ResetBuffer(outInter1, outSize, 1);
    ResetBuffer(outInter1, outSize, 1);

    Rs::Interpolate(buffer, outInter1, Size, 2);
    Rs::Internal::InterpolateOctave(buffer, outInter2, Size, 1);

    for (auto k = 0; k < Size; ++k) {
        EXPECT_EQ(outInter1[k * 2 + 1], 0);
        EXPECT_EQ(outInter1[k * 2], 3);
        EXPECT_EQ(outInter2[k * 2 + 1], 0);
        EXPECT_EQ(outInter2[k * 2], 3);
    }
}

TEST(Resampler, DecimationOneOctave)
{
    const std::size_t outSize = Rs::GetDecimationOctaveSize(Size, 1);
    ASSERT_EQ(outSize, Size / 2);

    Type buffer[Size];
    Type outDecim1[outSize];
    ResetBuffer(buffer, Size, 3);
    ResetBuffer(outDecim1, outSize, 1);

    Rs::Decimate(buffer, outDecim1, Size, 2);
    for (auto k = 0; k < outSize; ++k) {
        EXPECT_EQ(outDecim1[k], 3);
    }
}

TEST(Resampler, InterpolationSemitone)
{
    const std::size_t iRate = 5;
    const std::size_t outSize = Size * iRate;
    Type buffer[Size];
    Type outBuffer[outSize];
    ResetBuffer(buffer, Size, 3);
    ResetBuffer(outBuffer, outSize, 1);

    for (auto i = 0; i < Size; ++i) {
        buffer[i] = 3;
        outBuffer[i] = 1;
        outBuffer[i + 1] = 1;
    }

    Rs::Interpolate(buffer, outBuffer, Size, iRate);
    for (auto k = 0; k < Size; ++k) {
        EXPECT_EQ(outBuffer[k * iRate], 3);
        EXPECT_EQ(outBuffer[k * iRate + 1], 0);
    }
}

TEST(Resampler, ResampleOctave)
{
    const std::size_t nOctave = 1;
    const std::size_t outSize = Rs::GetResamplingSizeOctave(Size, nOctave);
    Type buffer[Size];
    Type outBuffer[outSize];
    ResetBuffer(buffer, Size, 3);
    ResetBuffer(outBuffer, outSize, 1);

    Rs::ResampleOctave(buffer, outBuffer, Size, 1);
    for (auto k = 0; k < outSize; ++k) {
        EXPECT_EQ(outBuffer[k], 3);
    }
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
