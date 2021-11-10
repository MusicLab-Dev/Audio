/**
 * @ Author: Pierre Veysseyre
 * @ Description: Benchmark of the Biquad (filter) class
 */

#include <iostream>

#include <benchmark/benchmark.h>

#include <Audio/Math.hpp>

#include "Audio/InterpolatePtr.hpp"

static void FullInterpolation(benchmark::State &state)
{
    auto From = 0;
    auto To = state.range(0);
    auto Step = 0.1;
    auto SampleCount = state.range(1);

// std::cout << "From " << From << " To " << To << " Step " << Step << " SampleCount " << SampleCount << std::endl;
// std::cout << "_stepValue " << ptr._stepValue << " _stepIndex " << ptr._stepIndex << std::endl;
    for (auto _ : state) {
        Audio::InterpolatePtr<float> ptr(From, To, Step, SampleCount);
        for (auto i = 0u; i < SampleCount; ++i) {
            benchmark::DoNotOptimize(ptr[i]);
            // std::cout << "Ptr[" << i << "] " << ptr[i] << std::endl;
        }
    }
}

BENCHMARK(FullInterpolation)
    ->Args({ 1000, 1000 })
    ->Args({ 1000, 2000 })
    ->Args({ 1000, 10000 })
    ->Args({ 2000, 1000 })
    ->Args({ 10000, 1000 })
;


static void FullInterpolation2(benchmark::State &state)
{
    auto From = 0;
    auto To = state.range(0);
    auto Step = 1;
    auto SampleCount = state.range(1);

// std::cout << "From " << From << " To " << To << " Step " << Step << " SampleCount " << SampleCount << std::endl;
// std::cout << "_stepValue " << ptr._stepValue << " _stepIndex " << ptr._stepIndex << std::endl;
    for (auto _ : state) {
        Audio::InterpolatePtr2<float> ptr(From, To, Step, SampleCount);
        for (auto i = 0u; i < SampleCount; ++ptr, ++i) {
            benchmark::DoNotOptimize(*ptr);
            // std::cout << "Ptr[" << i << "] " << ptr[i] << std::endl;
        }
    }
}

BENCHMARK(FullInterpolation2)
    ->Args({ 1000, 1000 })
    ->Args({ 1000, 2000 })
    ->Args({ 1000, 10000 })
    ->Args({ 2000, 1000 })
    ->Args({ 10000, 1000 })
;
