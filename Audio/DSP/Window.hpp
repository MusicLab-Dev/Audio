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
        Hamming
    };

    struct WindowMaker;
}

struct Audio::DSP::WindowMaker
{
public:
    static void GenerateCoefficients(const WindowType type, const std::size_t size, float *windowCoefficients, const bool isSymetric = true) noexcept;

private:
    static void Hanning(const std::size_t size, float *windowCoefficients, const bool isSymetric) noexcept;
    static void Hamming(const std::size_t size, float *windowCoefficients, const bool isSymetric) noexcept;
};

#include "Window.ipp"