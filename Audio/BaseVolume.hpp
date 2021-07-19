/**
 * @file BaseVolume.hpp
 * @brief Base for volume management
 *
 * @author Pierre V
 * @date 2021-04-23
 */

#pragma once

namespace Audio
{
    /** @brief Gain value */
    using Gain = float;

    /** @brief DB value */
    using DB = float;

    /** @brief Analog decibel max (dBu) */
    static constexpr DB DBUMax = 12.0f;

    /** @brief Default gain for each voice */
    static constexpr DB DefaultVoiceGain = -DBUMax;

    static constexpr float EnvelopeMinTimeStepUp { 0.00005f }; // 0.001
    static constexpr float EnvelopeMinTimeStepDown { EnvelopeMinTimeStepUp * 2.0f };

    /** @brief Default plugin output volume */
    static constexpr DB DefaultPluginOutputVolume = -6.0;

    /** @brief Convert a sample value to the decibel unit */
    template<typename Type>
    [[nodiscard]] inline DB ConvertSampleToDecibel(const Type value) noexcept { return 20.0f * std::log10(std::abs(value) / static_cast<double>(std::numeric_limits<Type>::max())); }

    /** @brief Convert a decibel value to sample unit */
    template<typename Type>
    [[nodiscard]] inline Type ConvertDecibelToSample(const DB value) noexcept
    {
        if (value == 0.0f)
            return 1.0f;
        return static_cast<Type>(std::pow(10.0f, value / 20.0f) * std::numeric_limits<Type>::max());
    }

    /** @brief Convert a decibel value to sample unit */
    [[nodiscard]] inline DB ConvertDecibelToRatio(const DB value) noexcept
    {
        if (value == 0.0f)
            return 1.0f;
        return std::pow(10.0f, value / 20.0f);
    }

}
