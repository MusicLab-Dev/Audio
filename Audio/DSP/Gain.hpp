/**
 * @file Gain.hpp
 * @brief Gain/volume helper
 *
 * @author Pierre V
 * @date 2021-11-24
 */

#pragma once

#include <Audio/BaseDevice.hpp>

namespace Audio::DSP
{
    struct Gain;
}

struct Audio::DSP::Gain
{
    template<std::floating_point Type>
    static void Apply(Type *data, const std::size_t size, const Type gain) noexcept
    {
        for (auto i = 0u; i < size; ++i) {
            data[i] *= gain;
        }
    }

    template<std::signed_integral Type>
    static void Apply(Type *data, const std::size_t size, const float gain) noexcept
    {
        for (auto i = 0u; i < size; ++i) {
            data[i] = static_cast<Type>(static_cast<float>(data[i]) * gain);
        }
    }


    template<std::floating_point Type>
    static void Apply(Type *data, const std::size_t size, const Type gainFrom, const Type gainTo) noexcept
    {
        const auto gainRamp = (gainTo - gainFrom) / static_cast<float>(size + 1u);

        Type iF = 0.0;
        for (auto i = 0u; i < size; ++i, ++iF) {
            data[i] *= (iF * gainRamp + gainFrom);
        }
    }

    template<std::floating_point Type>
    static void ApplyStereo(Type *data, const std::size_t channelSize, const std::size_t channelCount, const Type gainFrom, const Type gainTo) noexcept
    {
        const auto fullSize = channelSize * channelCount;
        const auto gainRamp = (gainTo - gainFrom) / static_cast<float>(channelSize + 1u);

        Type iF = 0.0;
        auto i = 0u;
        for (; i < channelSize; ++i, ++iF) {
            data[i] *= (iF * gainRamp + gainFrom);
        }
        for (iF = 0.0f; i < fullSize; ++i, ++iF) {
            data[i] *= (iF * gainRamp + gainFrom);
        }
    }

    template<typename Type>
    static void Normalize(Type *data, const std::size_t size) noexcept
    {
        Type max { FormatHandler<Type>::MinValue };
        for (auto i = 0u; i < size; ++i) {
            max = std::max(max, std::abs(data[i]));
        }
        // Get gain correction
        const float gainCorrection = 1.0f / static_cast<float>(max);
        DSP::Gain::Apply<Type>(data, size, gainCorrection);
    }
};
