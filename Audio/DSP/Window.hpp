/**
 * @ Author: Pierre Veysseyre
 * @ Description: Window.ipp
 */

#pragma once

namespace Audio::DSP
{
    enum class WindowType : std::uint8_t
    {
        Hanning = 0,
        Hamming,
        Custom
    };

    struct WindowMaker;
}

struct Audio::DSP::WindowMaker
{
public:
    static void GenerateFilterCoefficients(const WindowType type, const std::size_t size, float *windowCoefficients, const bool isSymetric = true) noexcept;

    static void Hanning(const std::size_t size, float *windowCoefficients, const bool isSymetric) noexcept;
    static void Hamming(const std::size_t size, float *windowCoefficients, const bool isSymetric) noexcept;
    static void Custom(const std::size_t size, float *windowCoefficients, const bool isSymetric) noexcept;
};

#include "Window.ipp"