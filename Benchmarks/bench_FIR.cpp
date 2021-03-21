/**
 * @ Author: Pierre Veysseyre
 * @ Description: Benchmark of the Biquad (filter) class
 */

#include <iostream>

#include <benchmark/benchmark.h>

#include <Audio/SampleFile/SampleManager.hpp>
#include <Audio/DSP/FIR.hpp>

static void ProcessSPSCQueue(benchmark::State &state)
{
    Audio::SampleSpecs specs;
    Audio::Buffer input = Audio::SampleManager<float>::LoadSampleFile(("Samples/chord.wav"), specs);
    const auto inputSize = input.size<float>();
    const auto outputSize = Audio::DSP::GetResamplingSizeOneSemitone(inputSize, false);
    Audio::Buffer output(outputSize * sizeof(float), input.sampleRate(), input.channelArrangement(), input.format());

    benchmark::DoNotOptimize(input);
    benchmark::DoNotOptimize(output);
    for (auto _ : state) {
        Audio::DSP::FIR::Resample<8u>(input.data<float>(), output.data<float>(), inputSize, input.sampleRate(), 196, 185);
    }
}

BENCHMARK(ProcessSPSCQueue)
    ->Args({ 44100, 512 })
;

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
