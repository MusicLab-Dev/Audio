/**
 * @ Author: Pierre Veysseyre
 * @ Description: bench_Parallel.cpp
 */

#include <benchmark/benchmark.h>

#include <Audio/DSP/Biquad.hpp>

using namespace DSP;

static void VecSeq(benchmark::State &state)
{
    for (auto _ : state) {
        const float input[4] { 1, 2, 3, 4 };
        float output[4] { 0, 0, 0, 0 };
        benchmark::DoNotOptimize(input);
        benchmark::DoNotOptimize(output);
        for (auto ii = 0; ii < 80; ii++) {
            for (auto i = 0; i < 4; ++i)
                output[i] += input[i];
        }
    }
}
BENCHMARK(VecSeq);

static void VecArray(benchmark::State &state)
{
    for (auto _ : state) {
        const Vec4 input { 1, 2, 3, 4 };
        Vec4 output { 0, 0, 0, 0 };
        benchmark::DoNotOptimize(input);
        benchmark::DoNotOptimize(output);
        for (auto i = 0; i < 80; i++) {
            output += input;
        }
        // output.print();
    }
}
BENCHMARK(VecArray);

static void MatSeq(benchmark::State &state)
{
    for (auto _ : state) {
        const float input[4] { 1, 2, 3, 4 };
        float output[4];
        benchmark::DoNotOptimize(input);
        benchmark::DoNotOptimize(output);
        for (auto i = 0; i < 4; ++i)
            output[i] *= input[i];
    }
}
BENCHMARK(MatSeq);

static void MatArray(benchmark::State &state)
{
    for (auto _ : state) {
        // Mat4 input {
        //     { 1, 2, 3, 4 },
        //     { 5, 6, 7, 8 },
        //     { 9, 10, 11, 12 },
        //     { 13, 14, 15, 16 }
        // };
        // const Vec4 vec { 1, 2, 2, 1 };
        // Mat4 output;
        // benchmark::DoNotOptimize(input);
        // benchmark::DoNotOptimize(output);
        // input.multVect(vec);
        // input.print();
    }
}
BENCHMARK(MatArray);
