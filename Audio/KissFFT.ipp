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
    std::cout << "processForward" << std::endl;
    if (processInBlock)
        return forwardBlock(timeInput, freqOutput, inputSize);
    setProcessTimeSize(inputSize);
    kiss_fftr(_forward, timeInput, freqOutput);
}

inline void KissFFT::Engine::processInverse(const TypeCpx *freqInput, TypeScalar *timeOutput, const std::size_t inputSize, const bool processInBlock) noexcept
{
    std::cout << "processInverse" << std::endl;
    if (processInBlock)
        return inverseBlock(freqInput, timeOutput, inputSize);
    setProcessTimeSize(inputSize);
    kiss_fftri(_inverse, freqInput, timeOutput);
}

inline void KissFFT::Engine::FilterSection(TypeCpx *frequencies, const TypeCpx *filter, const std::size_t size) noexcept
{
    for (auto i = 0u; i < size; ++i) {
        TypeCpx filterSample {
            // a                c                   b               d
            frequencies[i].r * filter[i].r - frequencies[i].i * filter[i].i,
            frequencies[i].r * filter[i].i + frequencies[i].i * filter[i].r
        };
        frequencies[i] = filterSample;
    }
}

inline void KissFFT::Engine::Mult(TypeScalar *out, const TypeScalar *in, const std::size_t size) noexcept
{
    for (auto i = 0u; i < size; ++i) {
        out[i] *= in[i];
    }
}

inline void KissFFT::Engine::Filter(const FirFilterSpecs filterSpecs, const TypeScalar *timeInput, TypeScalar *timeOutput, const std::size_t inputSize) noexcept
{
    constexpr std::size_t processSize = 32768u / 32;
    const std::size_t overlapSize = processSize / 2;
    const std::size_t inputPadSize = inputSize % processSize;
    const std::size_t nBlock = std::ceil(inputSize / static_cast<float>(processSize));
    const std::size_t processSizeTotal = nBlock * processSize;
    // Ensure the window is odd size
    constexpr std::size_t windowSize = processSize / 2;
    // const std::size_t windowSize = processSize + ((processSize & 1) ? 0 : -1);

    std::cout << "Filter::" << std::endl;
    std::cout << inputSize << std::endl;
    std::cout << windowSize << std::endl;
    std::cout << processSize << std::endl;
    std::cout << nBlock << std::endl;

    KissFFT::Engine engine(processSize);
    // The frequencies buffers for the process
    std::vector<TypeCpx> freqs(processSizeTotal);
    std::vector<TypeCpx> freqsOverlap((nBlock + 1) * processSize);

    // The input buffers for the process
    std::vector<TypeScalar> input(processSizeTotal, 0.f);
    std::memcpy(input.data(), timeInput, inputSize * sizeof(TypeScalar));
    std::vector<TypeScalar> inputOverlap((nBlock + 1) * processSize, 0.f);
    std::memcpy(inputOverlap.data() + overlapSize, timeInput, inputSize * sizeof(TypeScalar));


    // Window the inputs
    {
        // Get the process window
        std::vector<TypeScalar> windowProcess(processSize);
        Audio::DSP::WindowMaker::Hanning(processSize, windowProcess.data());

        for (auto i = 0u; i < nBlock; ++i) {
            const auto idx = i * processSize;
            Mult(input.data() + idx, windowProcess.data(), processSize);
            Mult(inputOverlap.data() + idx, windowProcess.data(), processSize);
        }
        // The extra one
        Mult(inputOverlap.data() + processSizeTotal, windowProcess.data(), processSize);
    }


    // Compute frequencies (FFT)
    std::cout << "----Normal" << std::endl;
    engine.processForward(input.data(), freqs.data(), processSizeTotal, true);
    std::cout << "----Overlap" << std::endl;
    engine.processForward(inputOverlap.data(), freqsOverlap.data(), processSizeTotal + processSize, true);


    // Apply filter on every chunks
    {
        // Get the filter coefficients
        std::vector<TypeScalar> filterCoefs(processSize, 0.f);
        Audio::DSP::WindowMaker::GenerateWindow(filterSpecs.windowType, windowSize, filterCoefs.data());
        // DSP::FIR::DesignFilter(filterSpecs, filterCoefs.data(), windowSize);
        // Filter impulse response output
        std::vector<TypeCpx> filterFreqs(processSize);
            filterCoefs[windowSize - 1] = 0.f;
        // Get the impulse response of the filter
        engine.processForward(filterCoefs.data(), filterFreqs.data(), processSize, true);

        // Filter frequencies
        for (auto i = 0u; i < nBlock; ++i) {
            const auto idx = i * processSize;
            FilterSection(freqs.data() + idx, filterFreqs.data(), windowSize);
            FilterSection(freqsOverlap.data() + idx, filterFreqs.data(), windowSize);
        }
        // The extra one
        FilterSection(freqsOverlap.data() + processSizeTotal, filterFreqs.data(), processSize);
    }


    // Compute samples (IFFT)
    {
        std::vector<TypeScalar> time(processSizeTotal, 0.f);
        std::vector<TypeScalar> timeOverlap(processSizeTotal + processSize, 0.f);
        std::cout << "----Normal" << std::endl;
        engine.processInverse(freqs.data(), time.data(), processSizeTotal, true);
        std::cout << "----Overlap" << std::endl;
        engine.processInverse(freqsOverlap.data(), timeOverlap.data(), processSizeTotal + processSize, true);

        for (auto i = 0u; i < inputSize; ++i) {
            const auto idxOverlap = i + overlapSize;
            timeOutput[i] = time[i] + timeOverlap[idxOverlap];
            // timeOutput[i] = time[i];
            // Normalize output
            timeOutput[i] /= processSize;
        }
    }

    std::cout << "FINISH FILTERING !" << std::endl;
}


inline void KissFFT::Engine::forwardBlock(const TypeScalar *timeInput, TypeCpx *freqOutput, const std::size_t inputSize) noexcept
{
    std::cout << "--forwardBlock: " << inputSize << std::endl;
    if (inputSize == _processTimeSize)
        return kiss_fftr(_forward, timeInput, freqOutput);

    const std::size_t nBlock = std::ceil(inputSize / static_cast<float>(_processTimeSize));
    const std::size_t nLastBlock = inputSize % _processTimeSize;
    std::cout << "  nBlock: " << nBlock << std::endl;
    std::cout << "  nLastBlock: " << nLastBlock << std::endl;

    for (auto i = 0u; i < nBlock - (nLastBlock ? 1 : 0); ++i) {
        // std::cout << "  - " << i << std::endl;
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
    std::cout << "--inverseBlock: " << inputSize << std::endl;
    if (inputSize == _processTimeSize)
        kiss_fftri(_inverse, freqInput, timeOutput);

    const std::size_t nBlock = std::ceil(inputSize / static_cast<float>(_processTimeSize));
    std::cout << "  nBlock: " << nBlock << std::endl;
    for (auto i = 0u; i < nBlock - 1; ++i) {
        // std::cout << "  - " << i << std::endl;
        kiss_fftri(_inverse, freqInput + (i * _processTimeSize), timeOutput + (i * _processTimeSize));
    }

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
