/**
 * @ Author: Pierre V
 * @ Description: KissFFT
 */

#pragma once

#include <Core/FlatVector.hpp>
#include "Buffer.hpp"

// #include <kissfft/kiss_fft.h>
#include <kissfft/tools/kiss_fftr.h>


#include "DSP/Window.hpp"

namespace KissFFT
{
    using TypeScalar = kiss_fft_scalar;
    using TypeCpx = kiss_fft_cpx;

    enum class FirFilterType : std::uint8_t
    {
        LowPass = 0,
        BandPass,
        BandStop,
        HighPass
    };

    struct FirFilterSpecs
    {
        FirFilterType filterType;
        Audio::DSP::WindowType windowType;
        Audio::SampleRate sampleRate;
        Audio::SampleRate cutoffs[2];
    };

    class Engine;
};


class KissFFT::Engine
{
public:

    Engine(const std::size_t processTimeSize) :
        _processTimeSize(processTimeSize),
        _forward(kiss_fftr_alloc(processTimeSize, 0, nullptr, nullptr)),
        _inverse(kiss_fftr_alloc(processTimeSize, 1, nullptr, nullptr))
    {}

    ~Engine(void) {
        std::free(_forward);
        std::free(_inverse);
    }

    const std::size_t processTimeSize(void) const noexcept { return _processTimeSize; }
    bool setProcessTimeSize(const std::size_t processTimeSize) noexcept;

    void processForward(const TypeScalar *timeInput, TypeCpx *freqOutput, const std::size_t inputSize, const bool processInBlock = false) noexcept;
    void processInverse(const TypeCpx *freqInput, TypeScalar *timeOutput, const std::size_t inputSize, const bool processInBlock = false) noexcept;

    static void Filter(const FirFilterSpecs filterSpecs, const TypeScalar *timeInput, TypeScalar *timeOutput, const std::size_t inputSize) noexcept;

private:
    std::size_t _processTimeSize { 0u };
    kiss_fftr_cfg _forward { nullptr };
    kiss_fftr_cfg _inverse { nullptr };

    void forwardBlock(const TypeScalar *timeInput, TypeCpx *freqOutput, const std::size_t inputSize) noexcept;
    void inverseBlock(const TypeCpx *freqInput, TypeScalar *timeOutput, const std::size_t inputSize) noexcept;

    static void DesignFilter(const FirFilterSpecs filterSpecs, float *windowCoefficients, const std::size_t windowSize) noexcept;
    static void DesignFilterLowPass(float *windowCoefficients, const std::size_t size, const double cutoffRate) noexcept;
};

#include "KissFFT.ipp"
