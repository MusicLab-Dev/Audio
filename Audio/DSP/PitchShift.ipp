/**
 * @ Author: Pierre V
 * @ Description: PitchShift
 */

#include <iostream>
#include <Audio/KissFFT.hpp>

using namespace Audio;
using namespace DSP;
using namespace KissFFT;


template<typename T>
inline Buffer PitchShift::Shift(const BufferView &inputBuffer, const Semitone semitone)
{
    const auto inputSize = inputBuffer.size<T>();
    const auto ZeroPadRate = 3u;
    const auto ZeroPadSize = FrameHopSize * ZeroPadRate; // must be less than BlockSize && multiple of FrameHopSize
    const auto ZeroPadSizeEnd = FrameHopSize - ((inputSize + ZeroPadSize) % FrameHopSize);
    const auto framesMax = std::round(((inputSize + ZeroPadSize + ZeroPadSizeEnd) / FrameHopSize) - FrameHopRate);


    std::cout << static_cast<int>(inputBuffer.channelArrangement()) << " " << static_cast<int>(inputBuffer.sampleRate()) << " " << static_cast<int>(inputBuffer.channelByteSize()) << std::endl;
    std::cout << "input size: " << inputSize << std::endl;
    std::cout << "max frames: " << framesMax << std::endl;
    std::cout << "ZeroPadSize: " << ZeroPadSize << std::endl;
    std::cout << "ZeroPadSizeEnd: " << ZeroPadSizeEnd << std::endl;
    std::cout << "rate: " << inputSize % FrameHopSize << std::endl;

    Buffer outputBuffer(inputBuffer.channelByteSize(), inputBuffer.sampleRate(), inputBuffer.channelArrangement());
    BufferViews frames;
    TimeBuffers timeArray(framesMax);


    // Cut input buffer into overlaping frames (buffers)
    //      Zero pad the input beggining --> OK !
    auto inputIdx = 0u;
    for (auto iFrame = 0u; iFrame < ZeroPadRate; ++iFrame) {
        Buffer frame(sizeof(T) * BlockSize, inputBuffer.sampleRate(), inputBuffer.channelArrangement());

        for (auto iChannel = 0u; iChannel < static_cast<std::size_t>(inputBuffer.channelArrangement()); ++iChannel) {
            inputIdx = 0;

            for (auto i = 0u; i < BlockSize; ++i) {
                if (i < ((ZeroPadRate - iFrame) * FrameHopSize)) {
                    // std::cout << "i: " << i << std::endl;
                    frame.data<T>(static_cast<Channel>(iChannel))[i] = 0;
                } else {
                    frame.data<T>(static_cast<Channel>(iChannel))[i] = inputBuffer.data<T>(static_cast<Channel>(iChannel))[inputIdx++];
                }
            }
        }
        timeArray[iFrame] = KissFFT::ConvertBuffer<T>(frame);
    }

    // Not OK !
    inputIdx = 0;
    for (auto iFrame = ZeroPadRate; iFrame < framesMax; ++iFrame) {
        for (auto iChannel = 0u; iChannel < static_cast<std::size_t>(inputBuffer.channelArrangement()); ++iChannel) {
            Buffer frame(sizeof(T) * BlockSize, inputBuffer.sampleRate(), inputBuffer.channelArrangement());
            std::cout << iFrame << "-" << iChannel << "-" << inputIdx << std::endl;
            for (auto i = 0u; i < BlockSize; ++i) {
                // std::cout << "\t" << inputIdx << " " << i << std::endl;
                frame.data<T>(static_cast<Channel>(iChannel))[i] = inputBuffer.data<T>(static_cast<Channel>(iChannel))[inputIdx++];
            }
            std::cout << "inputIdx: " << inputIdx << std::endl;
            inputIdx -= ZeroPadSize;
            std::cout << "-- inputIdx: " << inputIdx << std::endl;
            // freqArray[iChannel] = KissFFT::ConvertBuffer(frame);
        }
    }
    std::cout << "-- inputIdx: " << inputIdx << std::endl;


    // FFT the frames, previously windowed
    // Mess with the phase components of each frames ~<!>~
    // Overlap & add frames into a final buffer
    // Interpolate/decimate (depends semitone sign) the final buffer

    return outputBuffer;
}