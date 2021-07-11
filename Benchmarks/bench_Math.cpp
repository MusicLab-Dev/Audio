/**
 * @ Author: Pierre Veysseyre
 * @ Description: Benchmark of the Biquad (filter) class
 */

#include <iostream>

#include <benchmark/benchmark.h>

#include <Audio/Math.hpp>


static void ExponentialSTD(benchmark::State &state)
{
    std::vector<float> buffer(state.range());
    benchmark::DoNotOptimize(buffer);

    for (auto _ : state) {
        for (auto i = 0u; i < state.range(); ++i)
            buffer[i] = static_cast<float>(std::exp(i));
    }
}

BENCHMARK(ExponentialSTD)
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

static void ExponentialSTDPower(benchmark::State &state)
{
    std::vector<float> buffer(state.range());
    benchmark::DoNotOptimize(buffer);

    for (auto _ : state) {
        for (auto i = 0u; i < state.range(); ++i)
            buffer[i] = static_cast<float>(std::pow(M_E, i));
    }
}

BENCHMARK(ExponentialSTDPower)
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

static void ExponentialFast(benchmark::State &state)
{
    constexpr double Factor = 1.0 / std::log(2.0);

    std::vector<float> buffer(state.range());
    benchmark::DoNotOptimize(buffer);

    for (auto _ : state) {
        for (auto i = 0u; i < state.range(); ++i) {
            // const auto x = i * Factor;
            buffer[i] = static_cast<float>(std::pow(2.0, i * Factor));
        }
    }
}

BENCHMARK(ExponentialFast)
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
