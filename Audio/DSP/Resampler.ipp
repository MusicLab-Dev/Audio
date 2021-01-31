/**
 * @ Author: Pierre V
 * @ Description: Resampler
 */

#include <cmath>

using namespace Audio;

inline std::size_t DSP::Resampler::GetResamplingSize(const std::size_t inputSize, const Semitone semitone) noexcept {
    const auto rate = std::pow(2.0, -semitone / 12.0);

    return std::ceil(inputSize * rate);
}

template<typename T>
inline Buffer DSP::Resampler::Interpolate(const BufferView &inputBuffer, const std::size_t interpolationSamples) noexcept
{
    const auto inputSize = inputBuffer.size<T>();
    const auto newSize = inputSize + (inputSize - 1) * interpolationSamples;
    std::cout << "interpolate: " << inputSize << " - " << newSize << std::endl;
}

template<typename T>
inline Buffer DSP::Resampler::Decimate(const BufferView &inputBuffer, const std::size_t decimationSamples) noexcept
{
    const auto inputSize = inputBuffer.size<T>();
    const auto newSize = std::ceil(static_cast<float>(inputSize) / decimationSamples);
    std::cout << "decimate: " << inputSize << " - " << newSize << std::endl;
}
