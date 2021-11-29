/**
 * @ Author: Pierre Veysseyre
 * @ Description: Benchmark of the Biquad (filter) class
 */

#include <iostream>

#include <benchmark/benchmark.h>

#include <Audio/SampleFile/SampleManager.hpp>
#include <Audio/DSP/FIR.hpp>

static void FilterBufferFIR(benchmark::State &state)
{
    // Audio::SampleSpecs specs {
    //     44'100u,
    //     Audio::ChannelArrangement::Mono,
    //     Audio::Format::Floating32
    // };
    // Audio::SampleSpecs fileSpecs {};
    // Audio::Buffer input = Audio::SampleManager<float>::LoadSampleFile(("Samples/chord.wav"), specs, fileSpecs);
    // const auto inputSize = input.size<float>();
    // const auto outputSize = Audio::DSP::GetResamplingSizeOneSemitone(inputSize, false);
    // Audio::Buffer output(outputSize * sizeof(float), input.sampleRate(), input.channelArrangement(), input.format());

    Audio::DSP::FIR::BasicFilter<float> filter;
    filter.init(
        Audio::DSP::Filter::FIRSpecs(
            Audio::DSP::Filter::BasicType::LowPass,
            Audio::DSP::Filter::WindowType::Hanning,
            state.range(1),
            44'100.0f,
            0.22937f,
            0.0f,
            1.0f
        )
    );

    auto size = state.range(0);
    std::vector<float> buf(size);
    std::iota(buf.begin(), buf.end(), 0.0f);

    for (auto _ : state) {
        filter.filter(buf.data(), size, buf.data());
    }
    benchmark::DoNotOptimize(filter);
    benchmark::DoNotOptimize(buf);
}

#define BENCH_FIR(Size) \
    BENCHMARK(FilterBufferFIR) \
        ->Args({ Size, 15 }) \
        ->Args({ Size, 31 }) \
        ->Args({ Size, 63 }) \
        ->Args({ Size, 127 }) \
        ->Args({ Size, 255 }) \
    ;

BENCH_FIR(512)
BENCH_FIR(1024)
BENCH_FIR(2048)

// static void ProcessVector(benchmark::State &state)
// {
//     Audio::SampleSpecs specs;
//     Audio::Buffer input = Audio::SampleManager<float>::LoadSampleFile(("Samples/chord.wav"), specs);
//     const auto inputSize = input.size<float>();
//     const auto outputSize = Audio::DSP::GetResamplingSizeOneSemitone(inputSize, false);
//     Audio::Buffer output(outputSize * sizeof(float), input.sampleRate(), input.channelArrangement(), input.format());

//     benchmark::DoNotOptimize(input);
//     benchmark::DoNotOptimize(output);
//     for (auto _ : state) {
//         Audio::DSP::FIR::ResampleX<8u>(input.data<float>(), output.data<float>(), inputSize, input.sampleRate(), 196, 185);
//     }
// }

// BENCHMARK(ProcessVector)
//     ->Args({ 44100, 512 })
// ;

// static void ProcessStd(benchmark::State &state)
// {
//     Audio::SampleSpecs specs;
//     Audio::Buffer input = Audio::SampleManager<float>::LoadSampleFile(("Samples/chord.wav"), specs);
//     const auto inputSize = input.size<float>();
//     const auto outputSize = Audio::DSP::GetResamplingSizeOneSemitone(inputSize, false);
//     Audio::Buffer output(outputSize * sizeof(float), input.sampleRate(), input.channelArrangement(), input.format());

//     benchmark::DoNotOptimize(input);
//     benchmark::DoNotOptimize(output);
//     for (auto _ : state) {
//         Audio::DSP::FIR::ResampleSTD<8u>(input.data<float>(), output.data<float>(), inputSize, input.sampleRate(), 196, 185);
//     }
// }

// BENCHMARK(ProcessStd)
//     ->Args({ 44100, 512 })
// ;
