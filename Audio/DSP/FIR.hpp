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
        WindowType windowType { WindowType::Hanning };
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

    /** @brief Perform filtering using convolution */
    template<
        bool RemoveDelay = true,
        bool ProcessFirstChunk = true,
        typename Type>
    static VoidType<Type> Filter(const Audio::DSP::FilterSpecs specs, const Type *input, Type *output, const std::size_t inputSize) noexcept;

    template<
        bool RemoveDelay = true,
        bool ProcessFirstChunk = true,
        typename Type>
    static VoidType<Type> FilterLastInput(const Audio::DSP::FilterSpecs specs, const Type *input, Type *output, const std::size_t inputSize, const Type *lastInput, const std::size_t lastInputSize) noexcept;




    // template<typename Type>
    // static VoidType<Type> Filter(const FilterSpecs specs, const Type *input, Type *output, const std::size_t inputSize, const bool zeroPad = true) noexcept;
    // template<typename Type>
    // static VoidType<Type> Filter2(const FilterSpecs specs, const Type *input, Type *output, const std::size_t inputSize) noexcept;

    // /** @brief Perform filtering using convolution. LastInput is used instead of zero padding */
    // template<typename Type>
    // static VoidType<Type> Filter(const FilterSpecs specs, const Type *input, Type *output, const std::size_t inputSize, const Type *lastInput, const std::size_t lastInputSize) noexcept;


    /** @brief Perform interpolation combine with filtering. It save cpu ! */
    template<unsigned ProcessRate, bool Accumulate, typename Type>
    static VoidType<Type> Resample(
            const Type *input, Type *output,
            const std::size_t inputSize, const std::size_t inputSampleRate,
            const std::size_t interpFactor, const std::size_t decimFactor,
            const std::size_t offset = 0u) noexcept;

    template<unsigned ProcessRate, bool Accumulate, typename Type>
    static VoidType<Type> ResampleOctave(
        const Type *input, Type *output,
        const std::size_t inputSize, const std::size_t inputSampleRate,
        const std::int8_t nOctave,
        const std::size_t offset = 0u) noexcept;

    /** @brief Perform interpolation combine with filtering. It save cpu ! */
    template<unsigned ProcessRate, typename Type>
    static VoidType<Type> ResampleSTD(const Type *input, Type *output, const std::size_t inputSize, const std::size_t inputSampleRate, const std::size_t interpFactor, const std::size_t decimFactor) noexcept;

    /** @brief Perform interpolation combine with filtering. It save cpu ! */
    template<unsigned ProcessRate, typename Type>
    static VoidType<Type> ResampleX(const Type *input, Type *output, const std::size_t inputSize, const std::size_t inputSampleRate, const std::size_t interpFactor, const std::size_t decimFactor) noexcept;

    static std::size_t GetResampleSize(const std::size_t size, const std::size_t interpFactor, const std::size_t decimFactor) noexcept;

    static void DesignFilter(const FilterSpecs filterSpecs, float *windowCoefficients, const std::size_t windowSize, const bool centered = true) noexcept;
    static void DesignFilterLowPass(float *windowCoefficients, const std::size_t size, const double cutoffRate, const bool centered) noexcept;

private:
    /** @brief Implementation for filtering a signal with filter coefficients (window) */
    template<typename Type>
    static ProcessType<Type> FilterImpl(
            const Type *input, const Type *window,
            const std::size_t processSize, const std::size_t zeroPad) noexcept;

    /** @brief Implementation for filtering a signal with filter coefficients (window) and a last input chunk */
    template<typename Type>
    static ProcessType<Type> FilterLastInputImpl(
            const Type *input, const Type *lastInput,
            const Type *window, const std::size_t processSize,
            const std::size_t zeroPad, const std::size_t zeroPadLastInput) noexcept;



    /** @brief Implementation for resample a signal */
    template<unsigned ProcessRate, typename Type>
    static VoidType<Type> InterpolateOctaveImpl(
        const Type *input, Type *output,
        const std::size_t inputSize, const std::size_t inputSampleRate,
        const std::uint8_t nOctave,
        const std::size_t offset) noexcept;

    /** @brief Implementation for resample a signal */
    template<unsigned ProcessRate, typename Type>
    static VoidType<Type> DecimateOctaveImpl(
        const Type *input, Type *output,
        const std::size_t inputSize, const std::size_t inputSampleRate,
        const std::uint8_t nOctave,
        const std::size_t offset) noexcept;

    /** @brief Implementation for interpolation within a resampling process */
    template<unsigned ProcessRate, typename Type>
    static VoidType<Type> Interpolate(
        Type *producedData, const Type *input,
        const Type *filterCoefs, const std::size_t interpFactor,
        const std::size_t zeroPadBegin = 0u, const std::size_t zeroPadEnd = 0u);


};

#include "FIR.ipp"