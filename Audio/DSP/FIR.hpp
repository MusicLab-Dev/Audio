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
        std::size_t windowSize;
        float sampleRate;
        float cutoffs[2];
    };

}

class Audio::DSP::FIR
{
public:
    template<typename Type>
    using VoidType = std::enable_if_t<std::is_floating_point_v<Type>, void>;
    template<typename Type>
    using ProcessType = std::enable_if_t<std::is_floating_point_v<Type>, Type>;

    /** @brief Perform filtering using convolution. ZeroPad is used to 'add' zero before the input during processing stage */
    template<typename Type>
    static VoidType<Type> Filter(const FilterSpecs specs, const Type *input, Type *output, const std::size_t inputSize, const bool zeroPad = true) noexcept;

    /** @brief Perform filtering using convolution. LastInput is used instead of zero padding */
    template<typename Type>
    static VoidType<Type> Filter(const FilterSpecs specs, const Type *input, Type *output, const std::size_t inputSize, const Type *lastInput, const std::size_t lastInputSize) noexcept;


    /** @brief Perform interpolation combine with filtering. It save cpu ! */
    template<unsigned ProcessRate, typename Type>
    static VoidType<Type> Resample(const Type *input, Type *output, const std::size_t inputSize, const std::size_t inputSampleRate, const std::size_t interpFactor, const std::size_t decimFactor) noexcept;

    static void DesignFilter(const FilterSpecs filterSpecs, float *windowCoefficients, const std::size_t windowSize, const bool centered = true) noexcept;
    static void DesignFilterLowPass(float *windowCoefficients, const std::size_t size, const double cutoffRate, const bool centered) noexcept;

private:
    template<typename Type>
    static ProcessType<Type> FilterImpl(const Type *input, const Type *window, const std::size_t size, const std::size_t zeroPadSize) noexcept;

};

#include "FIR.ipp"