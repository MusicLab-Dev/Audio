/**
 * @ Author: Pierre V
 * @ Description: KissFFT
 */

#include <iostream>

template<typename T>
inline KissFFT::TimeBuffer KissFFT::ConvertBuffer(const Audio::BufferView &inputBuffer)
{
    const auto inputSize = inputBuffer.size<T>();
    TimeBuffer outBuffer;
    outBuffer.size = inputSize;

    for (auto iChannel = 0u; iChannel < static_cast<std::size_t>(inputBuffer.channelArrangement()); ++iChannel) {
        auto buffer = outBuffer.data.push(TimeArray(inputSize));
        for (auto i = 0u; i < inputSize; ++i) {
            buffer[iChannel * inputSize + i] = inputBuffer.data<T>(static_cast<Audio::Channel>(iChannel))[i];
        }
    }
    return outBuffer;
}

void KissFFT::Engine::processForward(const TimeBuffer &timeArray, FreqBuffer &freqArray)
{
    // const auto timeArraySize = timeArray.size();
    // if ((timeArraySize != freqArray.size()) ||
    //     (freqArray[0].size() != (timeArray[0].size() / 2 + 1)))
    //     throw std::runtime_error("KissFFT::processForward: timeArray & freqArray sizes must match !");

    // for (auto i = 0u; i < timeArraySize; ++i) {
    //     kiss_fftr(_forward, timeArray[i].data(), freqArray[i].data());
    // }
}

void KissFFT::Engine::processInverse(const FreqBuffer &freqArray, TimeBuffer &timeArray)
{
    // const auto timeArraySize = timeArray.size();
    // if ((timeArraySize != freqArray.size()) ||
    //     (freqArray[0].size() != (timeArray[0].size() / 2 + 1)))
    //     throw std::runtime_error("KissFFT::processForward: timeArray & freqArray sizes must match !");

    // for (auto i = 0u; i < timeArraySize; ++i) {
    //     kiss_fftri(_inverse, freqArray[i].data(), timeArray[i].data());
    // }
}
