/**
 * @ Author: Pierre Veysseyre
 * @ Description: Benchmark of the Biquad (filter) class
 */

#include <iostream>

#include <benchmark/benchmark.h>

#include <Audio/DSP/Biquad.hpp>

using namespace DSP;

static void BlockProcessBasic(benchmark::State &state)
{
    // std::cout << (static_cast<float>(state.range(1)) / state.range(0) * 1'000'000.f) << " ns" << std::endl;
    // auto filter = BiquadFactory<>::CreateFilter<BiquadParam::BasicType::LowPass, BiquadParam::FormType::Inversed>(static_cast<double>(state.range()), 0.25, 0.0, 0.707);

    // auto filter = Biquad<BiquadParam::BiquadForm::DirectForm1, BiquadParam::BasicType::LowPass>();
    // auto filter = BiquadMaker<BiquadParam::Optimization::Optimized, float>::MakeBiquad(state.range(0), 0.2, 0);
    auto filter = BiquadMaker<BiquadParam::Optimization::Optimized, float>::MakeBiquad(state.range(0), 0.2, 0);
    // filter.

    benchmark::DoNotOptimize(filter);
    for (auto _ : state) {
        float buf[state.range(1)];
        // benchmark::DoNotOptimize(*buf);
        filter.processBlock(buf, state.range(1));
    }
}

BENCHMARK(BlockProcessBasic)
    ->Args({ 44100, 512 })
    ->Args({ 44100, 1024 })
    ->Args({ 44100, 2048 })
    ->Args({ 44100, 4096 })

    // ->Args({ 48000, 512 })
    // ->Args({ 48000, 1024 })
    // ->Args({ 48000, 2048 })
    // ->Args({ 48000, 4096 })

    // ->Args({ 92000, 512 })
    // ->Args({ 92000, 1024 })
    // ->Args({ 92000, 2048 })
    // ->Args({ 92000, 4096 })
;

static void BlockProcessBasicFMA(benchmark::State &state)
{
    auto filter = BiquadMaker<BiquadParam::Optimization::Optimized, float>::MakeBiquad(state.range(0), 0.2, 0);
    benchmark::DoNotOptimize(filter);
    for (auto _ : state) {
        float buf[state.range(1)];
        filter.processBlock1(buf, state.range(1));
    }
}

BENCHMARK(BlockProcessBasicFMA)
    ->Args({ 44100, 512 })
    ->Args({ 44100, 1024 })
    ->Args({ 44100, 2048 })
    ->Args({ 44100, 4096 })

    // ->Args({ 48000, 512 })
    // ->Args({ 48000, 1024 })
    // ->Args({ 48000, 2048 })
    // ->Args({ 48000, 4096 })

    // ->Args({ 92000, 512 })
    // ->Args({ 92000, 1024 })
    // ->Args({ 92000, 2048 })
    // ->Args({ 92000, 4096 })
;