/**
 * @ Author: Pierre V
 * @ Description: KissFFT
 */

#include <iostream>

#include "Math.hpp"

inline bool KissFFT::Engine::setProcessTimeSize(const std::size_t processTimeSize) noexcept
{
    if (_processTimeSize == processTimeSize)
        return false;
    std::free(_forward);
    std::free(_inverse);
    _forward = kiss_fftr_alloc(processTimeSize, 0, nullptr, nullptr);
    _inverse = kiss_fftr_alloc(processTimeSize, 1, nullptr, nullptr);
    return true;
}

inline void KissFFT::Engine::processForward(const TypeScalar *timeInput, TypeCpx *freqOutput, const std::size_t inputSize, const bool processInBlock) noexcept
{
    if (processInBlock)
        return forwardBlock(timeInput, freqOutput, inputSize);
    setProcessTimeSize(inputSize);
}

inline void KissFFT::Engine::processInverse(const TypeCpx *freqInput, TypeScalar *timeOutput, const std::size_t inputSize, const bool processInBlock) noexcept
{
    if (processInBlock)
        return inverseBlock(freqInput, timeOutput, inputSize);
    setProcessTimeSize(inputSize);
}

inline void KissFFT::Engine::Filter(const FirFilterSpecs filterSpecs, const TypeScalar *timeInput, TypeScalar *timeOutput, const std::size_t inputSize) noexcept
{
    // Ensure the window is odd size
    const std::size_t windowSize = inputSize + ((inputSize & 1) ? 0 : -1);
    const std::size_t fftSize = inputSize + ((inputSize & 1) ? 1 : 0);

    std::cout << inputSize << std::endl;
    std::cout << windowSize << std::endl;
    std::cout << fftSize << std::endl;

    KissFFT::Engine engine(fftSize);

    std::vector<TypeScalar> window(windowSize);
    std::vector<TypeCpx> windowFreq(fftSize);
    std::vector<TypeCpx> inputFreq(fftSize);
    Audio::DSP::WindowMaker::GenerateCoefficients(filterSpecs.windowType, windowSize, window.data());
    DesignFilter(filterSpecs, window.data(), windowSize);

    // Get the impulse response of the filter
    engine.processForward(window.data(), windowFreq.data(), fftSize);
    // FFT the input signal
    engine.processForward(timeInput, inputFreq.data(), fftSize);
    // Filter the freq signal
    for (auto i = 0u; i < inputSize; ++i) {
        TypeCpx filterSample {
            // a                c                   b               d
            inputFreq[i].r * windowFreq[i].r - inputFreq[i].i * windowFreq[i].i,
            inputFreq[i].r * windowFreq[i].i + inputFreq[i].i * windowFreq[i].r
        };
        inputFreq[i] = filterSample;
    }
    // IFFT the filtered freq signal
    engine.processInverse(inputFreq.data(), timeOutput, fftSize);
}

inline void KissFFT::Engine::DesignFilter(const FirFilterSpecs filterSpecs, float *windowCoefficients, const std::size_t windowSize) noexcept
{
    const float cutoffRate = filterSpecs.cutoffs[0] / static_cast<float>(filterSpecs.sampleRate);
    switch (filterSpecs.filterType) {
    case FirFilterType::LowPass:
        return DesignFilterLowPass(windowCoefficients, windowSize, cutoffRate);

    default:
        return DesignFilterLowPass(windowCoefficients, windowSize, cutoffRate);
    }
}

inline void KissFFT::Engine::DesignFilterLowPass(float *windowCoefficients, const std::size_t size, const double cutoffRate) noexcept
{
    const std::size_t first = size / 2;
    const double realRate = 2.0 * cutoffRate;
    for (auto i = 0u; i < size; ++i) {
        int idx = i - first;
        *windowCoefficients *= (realRate * Utils::sinc(idx * realRate));
        ++windowCoefficients;
    }
}

inline void KissFFT::Engine::forwardBlock(const TypeScalar *timeInput, TypeCpx *freqOutput, const std::size_t inputSize) noexcept
{
    if (inputSize == _processTimeSize)
        return kiss_fftr(_forward, timeInput, freqOutput);

    const std::size_t nBlock = std::ceil(inputSize / static_cast<float>(_processTimeSize));
    const std::size_t nLastBlock = inputSize % _processTimeSize;
    // std::cout << "nBlock: " << nBlock << std::endl;
    // std::cout << "nLastBlock: " << nLastBlock << std::endl;

    for (auto i = 0u; i < nBlock - (nLastBlock ? 1 : 0); ++i) {
        kiss_fftr(_forward, timeInput + (i * _processTimeSize), freqOutput + (i * _processTimeSize));
    }
    if (nLastBlock) {
        TypeScalar timeBuf[_processTimeSize];
        for (auto i = 0u; i < nLastBlock; ++i) {
            timeBuf[i] = timeInput[(nBlock - 1) * _processTimeSize + i];
        }
        for (auto i = nLastBlock; i < _processTimeSize; ++i) {
            timeBuf[i] = 0.0f;
        }
        TypeCpx freqBuf[_processTimeSize];
        kiss_fftr(_forward, timeBuf, freqBuf);
        for (auto i = 0u; i < nLastBlock; ++i) {
            freqOutput[(nBlock - 1) * _processTimeSize + i] = freqBuf[i];
        }
    }
}

inline void KissFFT::Engine::inverseBlock(const TypeCpx *freqInput, TypeScalar *timeOutput, const std::size_t inputSize) noexcept
{
    if (inputSize == _processTimeSize)
        kiss_fftri(_inverse, freqInput, timeOutput);
}


// void KissFFT::Engine::processForward(const TimeBuffer &timeArray, FreqBuffer &freqArray)
// {
    // const auto timeArraySize = timeArray.size();
    // if ((timeArraySize != freqArray.size()) ||
    //     (freqArray[0].size() != (timeArray[0].size() / 2 + 1)))
    //     throw std::runtime_error("KissFFT::processForward: timeArray & freqArray sizes must match !");

    // for (auto i = 0u; i < timeArraySize; ++i) {
    //     kiss_fftr(_forward, timeArray[i].data(), freqArray[i].data());
    // }
// }

// void KissFFT::Engine::processInverse(const FreqBuffer &freqArray, TimeBuffer &timeArray)
// {
    // const auto timeArraySize = timeArray.size();
    // if ((timeArraySize != freqArray.size()) ||
    //     (freqArray[0].size() != (timeArray[0].size() / 2 + 1)))
    //     throw std::runtime_error("KissFFT::processForward: timeArray & freqArray sizes must match !");

    // for (auto i = 0u; i < timeArraySize; ++i) {
    //     kiss_fftri(_inverse, freqArray[i].data(), timeArray[i].data());
    // }
// }
