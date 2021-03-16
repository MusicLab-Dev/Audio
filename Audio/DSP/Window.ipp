/**
 * @ Author: Pierre Veysseyre
 * @ Description: Window.ipp
 */

#include <cmath>

inline void Audio::DSP::WindowMaker::GenerateFilterCoefficients(const WindowType type, const std::size_t size, float *windowCoefficients, const bool isSymetric) noexcept
{
    switch (type) {
    case WindowType::Hanning:
        return Hanning(size, windowCoefficients, isSymetric);
    case WindowType::Hamming:
        return Hamming(size, windowCoefficients, isSymetric);
    default:
        return Hanning(size, windowCoefficients, isSymetric);
    }
}

inline void Audio::DSP::WindowMaker::Hanning(const std::size_t size, float *outputWindow, const bool isSymetric) noexcept
{
    if (isSymetric) {
        for (auto i = 0u; i < size; i++) {
            outputWindow[i] = 0.5f - 0.5f * (std::cos(2.0 * M_PI * i / static_cast<float>(size - 1)));
        }
    } else {
    }
}

inline void Audio::DSP::WindowMaker::Hamming(const std::size_t size, float *outputWindow, const bool isSymetric) noexcept
{
    if (isSymetric) {
        for (auto i = 0u; i < size; i++) {
            outputWindow[i] = 0.54f - 0.46f * (std::cos(2.0 * M_PI * i / static_cast<float>(size - 1)));
        }
    } else {
    }
}
