/**
 * @file Volume.hpp
 * @brief Volume and gain handler
 *
 * @author Pierre V
 * @date 2021-04-22
 */

#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

#include <Core/Functor.hpp>

#include <Audio/Base.hpp>

namespace Audio
{
    template<typename Type>
    struct Volume;
}

template<typename Type>
struct Audio::Volume
{
/** @brief Global methods */
public:
    static constexpr auto PrintGain = [](const float outGain) {
        const std::size_t gainNorm = outGain * 100.f;
        std::cout << "<";
        for (auto i = 0ul; i < 100ul; ++i) {
            std::cout << (i < gainNorm ? "=" : " ");
        }
        std::cout << ">" << std::endl;
    };
    static constexpr auto ApplyRatio = [](BufferView buffer, const float ratio) {
        const auto size = buffer.size<float>();
        float *data = buffer.data<float>();
        std::for_each(data, data + size, [ratio](float &x) { x *= ratio; });
    };

    /** @brief Get the peak sample value of an input sequence */
    [[nodiscard]] static Type GetPeakVolume(const Type *input, const std::size_t inputSize) noexcept
    {
        Type peak = std::numeric_limits<Type>::min();

        for (auto i = 0ul; i < inputSize; ++i) {
            peak = std::max(std::abs(input[i]), peak);
        }
        return peak;
    }

    /** @brief Get the rms (average) sample value of an input sequence */
    [[nodiscard]] static Type GetRmsVolume(const Type *input, const std::size_t inputSize) noexcept
    {
        Type rms {};

        for (auto i = 0ul; i < inputSize; ++i) {
            rms += input[i] * input[i];
        }
        rms = std::sqrt(rms / inputSize);
        return rms;
    }


/** @brief Internal methods */
public:
    using RangeFunctor = Core::Functor<Type(const std::size_t index)>;
    // using ModifierFunctor = Core::Functor<void(Type &x)>;

    /** @brief Apply a ratio to a input sequence */
    void applyVolume(Type *input, const std::size_t inputSize, const DB volume) noexcept;


    /** @brief Apply a functor to a input sequence. Functor is call like this: functor(i + offset) */
    template<bool Accumulate = false>
    void applyRangeFunctor(Type *input, const std::size_t inputSize, const std::size_t offset, RangeFunctor &&functor) noexcept;


/** @brief Internal getters */
public:
    /** @brief Get the internal peak value */
    float peak(void) const noexcept { return _peak; }
    /** @brief Get the internal rms value */
    float rms(void) const noexcept { return _rms; }

private:
    float _peak { 0.0f };
    float _rms { 0.0f };
};

#include "Volume.ipp"
