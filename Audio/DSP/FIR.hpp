/**
 * @ Author: Pierre Veysseyre
 * @ Description: FIR.hpp
 */

#pragma once

#include "Window.hpp"

namespace Audio::DSP
{
    class FIR;

    enum class FilterType : std::uint8_t
    {
        LowPass = 0,
        BandPass,
        BandStop,
        HighPass
    };

    struct FilterSpecs
    {
        FilterType filterType;
        WindowType windowType;
        std::size_t windowLenght;
        float sampleRate;
        float cutoffs[2];
    };

}

class Audio::DSP::FIR
{
public:
    template<typename Type>
    using CallType = std::enable_if_t<std::is_floating_point_v<Type>, void>;
    template<typename Type>
    using ProcessType = std::enable_if_t<std::is_floating_point_v<Type>, Type>;

    template<typename Type>
    static CallType<Type> Filter(const FilterSpecs specs, const Type *input, const std::size_t size, Type *output) noexcept;

    static void DesignFilter(const FilterSpecs filterSpecs, float *windowCoefficients, const std::size_t windowSize, bool centered = true) noexcept;
    static void DesignFilterLowPass(float *windowCoefficients, const std::size_t size, const double cutoffRate, bool centered) noexcept;


private:
    template<typename Type>
    static ProcessType<Type> FilterImpl(const Type *input, const Type *window, const std::size_t size) noexcept;

};

#include "FIR.ipp"