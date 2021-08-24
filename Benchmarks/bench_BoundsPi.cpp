/**
 * @ Author: Pierre Veysseyre
 * @ Description: Benchmark of the Biquad (filter) class
 */

#include <iostream>

#include <cmath>

#include <benchmark/benchmark.h>

static constexpr float Pi2 = static_cast<float>(2.0 * M_PI);
static constexpr float Freq = Pi2 / 5.12453f;

struct Phase
{
    float Loop(const float freq)
    {
        phase += freq;
        while (phase >= Pi2)
            phase -= Pi2;
        return phase;
    }

    float Division(const float freq)
    {
        phase += freq;
        if (const float dt = phase / Pi2; dt >= 1.0f)
            phase -= static_cast<std::size_t>(dt) * Pi2;
        return phase;
    }

    float Division2(const float freq)
    {
        phase += freq;
        phase = static_cast<std::size_t>(phase / Pi2) * Pi2;
        return phase;
    }

    float ModF(const float freq)
    {
        phase = std::fmod(phase + freq, Pi2);
        return phase;
    }

    float phase { 0.0f };
};

static void BoundsPiLoop(benchmark::State &state)
{
    std::vector<float> tmp(state.range());
    benchmark::DoNotOptimize(tmp);
    Phase phase;
    benchmark::DoNotOptimize(phase);

    for (auto _ : state) {
        for (auto i = 0u; i < state.range(); ++i) {
            tmp[i] = phase.Loop(Freq);
        }
    }
}
BENCHMARK(BoundsPiLoop)
    ->Arg(1 << 8)
    // ->Arg(1 << 9)
    // ->Arg(1 << 10)
    // ->Arg(1 << 11)
    // ->Arg(1 << 12)
    // ->Arg(1 << 13)
;

static void BoundsPiDivision(benchmark::State &state)
{
    std::vector<float> tmp(state.range());
    benchmark::DoNotOptimize(tmp);
    Phase phase;
    benchmark::DoNotOptimize(phase);

    for (auto _ : state) {
        for (auto i = 0u; i < state.range(); ++i) {
            tmp[i] = phase.Division(Freq);
        }
    }
}
BENCHMARK(BoundsPiDivision)
    ->Arg(1 << 8)
    // ->Arg(1 << 9)
    // ->Arg(1 << 10)
    // ->Arg(1 << 11)
    // ->Arg(1 << 12)
    // ->Arg(1 << 13)
;

static void BoundsPiDivision2(benchmark::State &state)
{
    std::vector<float> tmp(state.range());
    benchmark::DoNotOptimize(tmp);
    Phase phase;
    benchmark::DoNotOptimize(phase);

    for (auto _ : state) {
        for (auto i = 0u; i < state.range(); ++i) {
            tmp[i] = phase.Division2(Freq);
        }
    }
}
BENCHMARK(BoundsPiDivision2)
    ->Arg(1 << 8)
    // ->Arg(1 << 9)
    // ->Arg(1 << 10)
    // ->Arg(1 << 11)
    // ->Arg(1 << 12)
    // ->Arg(1 << 13)
;

static void BoundsPiModF(benchmark::State &state)
{
    std::vector<float> tmp(state.range());
    benchmark::DoNotOptimize(tmp);
    Phase phase;
    benchmark::DoNotOptimize(phase);

    for (auto _ : state) {
        // float x = 0.0f;
        for (auto i = 0u; i < state.range(); ++i) {
            tmp[i] = phase.ModF(Freq);
        }
    }
}
BENCHMARK(BoundsPiModF)
    ->Arg(1 << 8)
    // ->Arg(1 << 9)
    // ->Arg(1 << 10)
    // ->Arg(1 << 11)
    // ->Arg(1 << 12)
    // ->Arg(1 << 13)
;
