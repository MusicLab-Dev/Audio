/**
 * @file BaseNote.hpp
 * @brief Base for note management
 *
 * @author Pierre V
 * @date 2021-04-23
 */

#pragma once

#include <cmath>

namespace Audio
{
    /** @brief Key of a note */
    using Key = std::uint8_t;

    /** @brief Middle key */
    static constexpr Key RootKey = 69u;

    /** @brief Middle key (A4) */
    static constexpr float RootKeyFrequency = 440.f;

    /** @brief Semitone delta */
    static constexpr double SemitoneUpDelta = 1.05946309436;
    static constexpr double SemitoneCentUpDelta = (SemitoneUpDelta - 1.0) / 12.0 + 1.0;
    static constexpr double SemitoneDownDelta = 0.943874312682;
    static constexpr double SemitoneCentDownDelta = (SemitoneDownDelta - 1.0) / 12.0 + 1.0;

    static constexpr std::uint8_t KeysPerOctave = 12u;
    static constexpr std::uint8_t OctaveRootKey = 5u;
    static constexpr std::uint8_t RootOctave = 5u;

    /** @brief Semitone */
    using Semitone = std::int8_t;

    /** @brief Number of different handled keys */
    constexpr Key KeyCount = 128u;

    /** @brief Velocity of a note */
    using Velocity = std::uint16_t;

    /** @brief Note pitch tuning */
    using Tuning = std::uint16_t;

    static constexpr float MinFrequency = 20.0f;
    static constexpr float MaxFrequency = 20'000.0f;

    static constexpr float MinFrequencyLog = std::log2(MinFrequency);
    static constexpr float MaxFrequencyLog = std::log2(MaxFrequency);

    [[nodiscard]] inline float Lerp(const float x, const float a, const float b) noexcept
    {
        return a + x * (b - a);
    }

    [[nodiscard]] inline float GetFrequencyRatio(const float x) noexcept
    {
        return std::pow(2.0f, Lerp(x, MinFrequencyLog, MaxFrequencyLog));
    }

    /** @brief Get the frequency between freqMin & freqMax for a given rate in [0:1] */
    // [[nodiscard]] inline float GetFrequencyRatio(const float x, const float freqMin, const float freqMax) noexcept
    // {
    //     return std::pow(2.0f, static_cast<float>(x * freqMax)) * freqMin;
    // }

    /** @brief Get the frequency of a key */
    [[nodiscard]] inline float GetNoteFrequency(const Key key) noexcept
    {
        return std::pow(2.0f, static_cast<float>(static_cast<int>(key) - RootKey) / KeysPerOctave) * RootKeyFrequency;
    }

    /** @brief Get the frequency of a key */
    [[nodiscard]] inline float GetNoteFrequencyDelta(const float rootFreq, const float semitoneDelta) noexcept
    {
        if (semitoneDelta == 0.0f)
            return rootFreq;
        return std::pow(2.0f, static_cast<float>(semitoneDelta / KeysPerOctave)) * rootFreq;
    }
}
