/**
 * @ Author: Pierre Veysseyre
 * @ Description: Benchmark of the Biquad (filter) class
 */

#include <iostream>

#include <benchmark/benchmark.h>

#include <Audio/DSP/Biquad.hpp>

using namespace Audio;

static void FilterBufferIIR(benchmark::State &state)
{
    DSP::Biquad::Filter<DSP::Biquad::Internal::Form::Transposed2> filter1;
    filter1.setup(DSP::Biquad::Internal::Specs {
        DSP::Filter::AdvancedType::LowPass,
        44'100.0f,
        { 0.22937f, 0.0f },
        1.0f,
        0.707f
    });
    filter1.reset();

    auto size = state.range(0);
    std::vector<float> buf(size);
    std::iota(buf.begin(), buf.end(), 0.0f);

    for (auto _ : state) {
        filter1.filterBlock(buf.data(), size, buf.data(), 0u, 1.0f);
    }
    benchmark::DoNotOptimize(filter1);
    benchmark::DoNotOptimize(buf);
}

BENCHMARK(FilterBufferIIR)
    ->Arg(512)
    ->Arg(1024)
    ->Arg(2048)
    ->Arg(4096)
    ->Arg(8192)
    ->Arg(8192 * 2)
;

static void FilterBufferIIRx2(benchmark::State &state)
{
    DSP::Biquad::Filter<DSP::Biquad::Internal::Form::Transposed2> filter1;
    filter1.setup(DSP::Biquad::Internal::Specs {
        DSP::Filter::AdvancedType::LowPass,
        44'100.0f,
        { 0.22937f, 0.0f },
        1.0f,
        0.707f
    });
    filter1.reset();
    DSP::Biquad::Filter<DSP::Biquad::Internal::Form::Transposed2> filter2;
    filter2.setup(DSP::Biquad::Internal::Specs {
        DSP::Filter::AdvancedType::LowPass,
        44'100.0f,
        { 0.22937f, 0.0f },
        1.0f,
        0.707f
    });
    filter2.reset();

    auto size = state.range(0);
    std::vector<float> buf(size);
    std::iota(buf.begin(), buf.end(), 0.0f);

    for (auto _ : state) {
        filter1.filterBlock(buf.data(), size, buf.data(), 0u, 1.0f);
        filter2.filterBlock(buf.data(), size, buf.data(), 0u, 1.0f);
    }
    benchmark::DoNotOptimize(filter1);
    benchmark::DoNotOptimize(filter2);
    benchmark::DoNotOptimize(buf);
}

BENCHMARK(FilterBufferIIRx2)
    ->Arg(512)
    ->Arg(1024)
    ->Arg(2048)
    ->Arg(4096)
    ->Arg(8192)
    ->Arg(8192 * 2)
;
