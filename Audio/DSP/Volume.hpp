/**
 * @file Volume.hpp
 * @brief Volume and gain handler
 *
 * @author Pierre V
 * @date 2021-04-22
 */

#pragma once

#include <Audio/Base.hpp>

namespace Audio
{
    /** @brief Analog decibel max (dBu) */
    static constexpr float DBUMax = 12.0f;

    template<typename Type>
    struct Volume;
}

template<typename Type>
struct Audio::Volume
{
public:
    /**
     * @brief Apply a ratio to a input sequence
     */
    void applyRatio(Type *input, const std::size_t inputSize, const float ratio) noexcept
    {
        _peak = 0.0f;
        _rms = 0.0f;
        for (auto i = 0ul; i < inputSize; ++i) {
            _peak = std::max(std::abs(input[i]));
            _rms += input[i] * input[i];
            input[i] *= ratio;
        }
        _rms = std::sqrt(_rms / inputSize);
    }

private:
    float _peak { 0.0f };
    float _rms { 0.0f };
};