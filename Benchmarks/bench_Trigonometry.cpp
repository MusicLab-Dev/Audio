/**
 * @ Author: Pierre Veysseyre
 * @ Description: Benchmark of the Biquad (filter) class
 */

#include <iostream>

#include <benchmark/benchmark.h>

#include <Audio/Math.hpp>


static void SineSTD(benchmark::State &state)
{
    std::vector<float> buffer(state.range());
    benchmark::DoNotOptimize(buffer);

    for (auto _ : state) {
        for (auto i = 0u; i < state.range(); ++i)
            buffer[i] = static_cast<float>(std::sin(i));
    }
}

BENCHMARK(SineSTD)
    ->Arg(1 << 0)
    // ->Arg(1 << 1)
    // ->Arg(1 << 2)
    // ->Arg(1 << 3)
    // ->Arg(1 << 4)
    // ->Arg(1 << 5)
    // ->Arg(1 << 6)
    // ->Arg(1 << 7)
    ->Arg(1 << 8)
;

static void Sine(benchmark::State &state)
{
    std::vector<float> buffer(state.range());
    benchmark::DoNotOptimize(buffer);

    for (auto _ : state) {
        for (auto i = 0u; i < state.range(); ++i)
            buffer[i] = static_cast<float>(std::sin(i));
    }
}

BENCHMARK(Sine)
    ->Arg(1 << 0)
    // ->Arg(1 << 1)
    // ->Arg(1 << 2)
    // ->Arg(1 << 3)
    // ->Arg(1 << 4)
    // ->Arg(1 << 5)
    // ->Arg(1 << 6)
    // ->Arg(1 << 7)
    ->Arg(1 << 8)
;
