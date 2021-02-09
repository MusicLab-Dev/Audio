/**
 * @ Author: Pierre V
 * @ Description: KissFFT
 */

#pragma once

#include <Core/FlatVector.hpp>
#include "Buffer.hpp"

// #include <kissfft/kiss_fft.h>
#include <kissfft/tools/kiss_fftr.h>


namespace KissFFT
{
    using TypeScalar = kiss_fft_scalar;
    using TypeCpx = kiss_fft_cpx;
    using TimeArray = Core::FlatVector<TypeScalar>;
    using FreqArray = Core::FlatVector<TypeCpx>;

    struct TimeBuffer {
        Core::FlatVector<TimeArray> data;
        std::size_t size;
    };
    using TimeBuffers = Core::FlatVector<TimeBuffer>;

    struct FreqBuffer {
        Core::FlatVector<FreqArray> data;
        std::size_t size;
    };
    using FreqBuffers = Core::FlatVector<FreqBuffer>;

    template<typename T>
    static TimeBuffer ConvertBuffer(const Audio::BufferView &inputBuffer);

    struct Engine;
};


struct KissFFT::Engine
{
public:
    Engine(const std::size_t size) :
        _forward(kiss_fftr_alloc(size, 0, nullptr, nullptr)),
        _inverse(kiss_fftr_alloc(size, 1, nullptr, nullptr))
    {}

    ~Engine(void) {
        free(_forward);
        free(_inverse);
    }

    void processForward(const TimeBuffer &timeArray, FreqBuffer &freqArray);
    void processInverse(const FreqBuffer &freqArray, TimeBuffer &timeArray);

private:
    kiss_fftr_cfg _forward { nullptr };
    kiss_fftr_cfg _inverse { nullptr };
};

#include "KissFFT.ipp"
