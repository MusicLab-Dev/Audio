/**
 * @ Author: Pierre Veysseyre
 * @ Description: EnveloppeGenerator
 */

#pragma once

#include <Core/Utils.hpp>
#include <Core/Vector.hpp>

#include <Audio/BaseDevice.hpp>

#include <iostream>

namespace Audio::DSP
{
    enum class EnveloppeType : std::uint8_t {
        AR, AD, ADSR//, DADSR, DAHDSR
    };

    enum class InterpolationType : std::uint8_t {
        Linear, Exp, InverseExp
    };

    template<EnveloppeType Enveloppe>
    class EnveloppeBase;

    class AttackRelease;
    class AttackDecayReleaseSustain;
}


template<Audio::DSP::EnveloppeType Enveloppe>
class Audio::DSP::EnveloppeBase
{
public:
    using IndexList = std::array<std::size_t, KeyCount>;
    using GainList = std::array<float, KeyCount>;

    /** @brief Reset all trigger indexes */
    void resetTriggerIndexes(void) noexcept { _triggerIndex.fill(0ul); }

    /** @brief Reset a specific trigger index */
    void resetTriggerIndex(const Key key) noexcept { setTriggerIndex(key, 0ul); }

    /** @brief Set the internal trigger status */
    void setTriggerIndex(const Key key, const std::size_t triggerIndex) noexcept { _triggerIndex[key] = triggerIndex; }

    /** @brief Reset all internal gains */
    void resetInternalGains(void) noexcept { _lastGain.fill(0ul); }

    /** @brief Reset a specific gain */
    void resetGain(const Key key) noexcept { _lastGain[key] = 0ul; }

    template<bool Debug = false>
    [[nodiscard]] float getGain(
            const Key key, const std::size_t index, const bool isTrigger,
            const float delay, const float attack,
            const float hold, const float decay,
            const float sustain, const float release,
            const SampleRate sampleRate) noexcept
    {
        if constexpr (Enveloppe == EnveloppeType::AR)
            return attackRelease(key, index, isTrigger, attack, release, sampleRate);
        else if constexpr (Enveloppe == EnveloppeType::AD)
            return attackDecay(key, index, isTrigger, attack, decay, sampleRate);
        else if constexpr (Enveloppe == EnveloppeType::ADSR)
            return adsr<true>(key, index, isTrigger, attack, decay, release, sustain, sampleRate);
        return 1.f;
    }

    /** @brief AR oomplementation */
    [[nodiscard]] float attackRelease(
            const Key key, const std::size_t index, const bool isTrigger,
            const float attack, const float release, const SampleRate sampleRate) noexcept
    {
        const std::size_t attackIdx = attack * static_cast<float>(sampleRate);
        const std::size_t releaseIdx = release * static_cast<float>(sampleRate);
        if (isTrigger) {
            if (index < attackIdx) {
                return index / static_cast<float>(attackIdx);
            }
            return 1.f;
        } else {
            if (const auto releaseIndex = index - _triggerIndex[key]; releaseIndex < releaseIdx) {
                return 1 - releaseIndex / static_cast<float>(releaseIdx);
            }
            return 0.f;
        }
        return 1.f;
    }

    /** @brief AD implementation */
    [[nodiscard]] float attackDecay(
            const Key key, const std::size_t index, const bool isTrigger,
            const float attack, const float decay, const SampleRate sampleRate) noexcept
    {
        if (isTrigger) {
            const std::size_t attackIdx = attack * static_cast<float>(sampleRate);
            if (attackIdx >= index) {
                return index / static_cast<float>(attackIdx);
            }
            if (const std::size_t decayIdx = decay * static_cast<float>(sampleRate);
                index < decayIdx + attackIdx)
                return 1.f - (index - attackIdx) / static_cast<float>(decayIdx);
        }
        return 0.f;
    }

    /** @brief ADSR implementation */
    template<bool Debug = false>
    [[nodiscard]] float adsr(
            const Key key, const std::size_t index, const bool isTrigger,
            const float attack, const float decay, const float sustain, const float release, const SampleRate sampleRate) noexcept
    {
        const float OneMinusSustain = 1.0f - sustain;
        float outGain { 1.f };

        if constexpr (Debug) {
            const auto a = attack * static_cast<float>(sampleRate);
            const auto d = decay * static_cast<float>(sampleRate);
            // const auto a = attack * static_cast<float>(sampleRate);
            // const auto a = attack * static_cast<float>(sampleRate);
            std::cout << "index: " << index << "(" << isTrigger << "), atk: " << a << ", dc: " << d << ", a+d: " << a + d << std::endl;
        }

        if (isTrigger) {
            const std::size_t attackIdx = attack * static_cast<float>(sampleRate);
            // Attack
            if (index < attackIdx) {
                if constexpr (Debug)
                    std::cout << "AAAAAA: " << (index / static_cast<float>(attackIdx)) << std::endl;
                // Linear
                outGain = index / static_cast<float>(attackIdx);
            }
            // Decay
            else if (const std::size_t decayIdx = decay * static_cast<float>(sampleRate);
                index < (decayIdx + attackIdx)) {
                // Sustain max -> no decay
                if constexpr (Debug)
                    std::cout << "DDDDDD: " << (1.f - (index - attackIdx) / static_cast<float>(decayIdx)) * OneMinusSustain + OneMinusSustain << std::endl;
                if (sustain == 1.f)
                    outGain = 1.f;
                else
                    outGain = (1.f - (index - attackIdx) / static_cast<float>(decayIdx)) * OneMinusSustain + sustain;
            } else {
                // Sustain
                if constexpr (Debug)
                    std::cout << "SSSSSS: " << sustain << std::endl;
                outGain = sustain;
            }
        } else {
            // Release
            // const std::size_t releaseIdx = ((_lastGain[key] < sustain) ? _lastGain[key] : release) * static_cast<float>(sampleRate);
            const std::size_t releaseIdx = release * static_cast<float>(sampleRate);
            if (const std::size_t realIndex = index - _triggerIndex[key];
                realIndex < releaseIdx) {
                if constexpr (Debug)
                    std::cout << "RRRR: " << (1.f - realIndex / static_cast<float>(releaseIdx)) * sustain << std::endl;
                outGain = (1.f - realIndex / static_cast<float>(releaseIdx)) * sustain;
            }
            // End of the enveloppe
            else {
                // std::cout << "______" << std::endl;
                outGain = 0.f;
            }
        }

        // Smooth gain when sudden changes
        // static constexpr auto SmoothLimit = 0.5f;
        // if (const float dt = std::abs(outGain - _lastGain[key]); dt > SmoothLimit) {
        //     _lastGain[key] = (outGain + _lastGain[key]) / 2.f;
        // } else {
        //     _lastGain[key] = outGain;
        // }
        // if (const float dt = outGain - _lastGain[key]; dt > SmoothLimit) {
        //     _lastGain[key] = outGain - SmoothLimit;
        //     // _lastGain[key] = _lastGain[key] + SmoothLimit;
        // } else if (dt < -SmoothLimit) {
        //     _lastGain[key] = outGain + SmoothLimit;
        //     // _lastGain[key] = _lastGain[key] - SmoothLimit;
        // } else {
        //     _lastGain[key] = outGain;
        // }
        _lastGain[key] = outGain;
        return _lastGain[key];
    }

private:
    IndexList _triggerIndex;
    GainList _lastGain;
};

#include "EnveloppeGenerator.ipp"