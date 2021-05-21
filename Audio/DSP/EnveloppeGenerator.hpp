/**
 * @ Author: Pierre Veysseyre
 * @ Description: EnveloppeGenerator
 */

#pragma once

#include <Core/Utils.hpp>
#include <Core/Vector.hpp>

#include <Audio/Base.hpp>

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
    using IndexList = std::array<std::uint32_t, KeyCount>;
    using GainList = std::array<float, KeyCount>;
    using SustainList = std::array<float, KeyCount>;

    /** @brief Reset all */
    void reset(void) noexcept { resetTriggerIndexes(); resetInternalGains(); resetInternalSustains(); }

    /** @brief Reset a key */
    void resetKey(const Key key) noexcept { resetTriggerIndex(key); resetInternalGain(key); resetInternalSustain(key); }


    /** @brief Reset all trigger indexes */
    void resetTriggerIndexes(void) noexcept { _triggerIndex.fill(0ul); }

    /** @brief Reset a specific trigger index */
    void resetTriggerIndex(const Key key) noexcept { setTriggerIndex(key, 0ul); }

    /** @brief Set the internal trigger status */
    void setTriggerIndex(const Key key, const std::uint32_t triggerIndex) noexcept { _triggerIndex[key] = triggerIndex; }


    /** @brief Reset all internal gains */
    void resetInternalGains(void) noexcept { _lastGain.fill(0.0f); }

    /** @brief Reset a specific gain */
    void resetInternalGain(const Key key) noexcept { _lastGain[key] = 0.0f; }

    /** @brief Reset all internal sustains */
    void resetInternalSustains(void) noexcept { _sustains.fill(0.0f); }

    /** @brief Reset a specific sustain */
    void resetInternalSustain(const Key key) noexcept { _sustains[key] = 0.0f; }


    /** @brief Get the last gain */
    [[nodiscard]] float lastGain(const Key key) noexcept { return _lastGain[key]; }

    [[nodiscard]] float getGain(
            const Key key, const std::uint32_t index, const bool isTrigger,
            const float delay, const float attack,
            const float hold, const float decay,
            const float sustain, const float release,
            const SampleRate sampleRate) noexcept
    {
        UNUSED(delay);
        UNUSED(attack);
        UNUSED(hold);
        UNUSED(decay);
        UNUSED(sustain);
        UNUSED(release);
        if constexpr (Enveloppe == EnveloppeType::AR)
            return attackRelease(key, index, isTrigger, attack, release, sampleRate);
        else if constexpr (Enveloppe == EnveloppeType::AD)
            return attackDecay(key, index, isTrigger, attack, decay, sampleRate);
        else if constexpr (Enveloppe == EnveloppeType::ADSR)
            return adsr(key, index, isTrigger, attack, decay, sustain, release, sampleRate);
        return 1.f;
    }

    /** @brief AR implementation */
    [[nodiscard]] float attackRelease(
            const Key key, const std::uint32_t index, const bool isTrigger,
            const float attack, const float release, const SampleRate sampleRate) noexcept
    {
        const std::uint32_t attackIdx = static_cast<std::uint32_t>(attack * static_cast<float>(sampleRate));
        const std::uint32_t releaseIdx = static_cast<std::uint32_t>(release * static_cast<float>(sampleRate));
        float outGain { 1.f };

        if (isTrigger) {
            if (index < attackIdx) {
                outGain = static_cast<float>(index) / static_cast<float>(attackIdx);
            } else {
                outGain = 1.f;
            }
        } else {
            if (const auto releaseIndex = index - _triggerIndex[key]; releaseIndex < releaseIdx) {
                outGain = 1.0f - static_cast<float>(releaseIndex) / static_cast<float>(releaseIdx);
            } else {
                outGain = 0.f;
            }
        }
        _lastGain[key] = outGain;
        return _lastGain[key];
    }

    /** @brief AD implementation */
    [[nodiscard]] float attackDecay(
            const Key key, const std::uint32_t index, const bool isTrigger,
            const float attack, const float decay, const SampleRate sampleRate) noexcept
    {
        if (isTrigger) {
            const std::uint32_t attackIdx = attack * static_cast<float>(sampleRate);
            if (attackIdx >= index) {
                return static_cast<float>(index) / static_cast<float>(attackIdx);
            }
            if (const std::uint32_t decayIdx = decay * static_cast<float>(sampleRate); index < decayIdx + attackIdx)
                return 1.f - static_cast<float>(index - attackIdx) / static_cast<float>(decayIdx);
        }
        return 0.f;
    }

    /** @brief ADSR implementation */
    [[nodiscard]] float adsr(
            const Key key, const std::uint32_t index, const bool isTrigger,
            const float attack, const float decay, const float sustain, const float release, const SampleRate sampleRate) noexcept
    {
        const float OneMinusSustain = 1.0f - sustain;
        float outGain { 1.f };
        const auto triggerIndex = _triggerIndex[key];

        UNUSED(isTrigger);
        if (!triggerIndex || (index < triggerIndex)) {
            const std::uint32_t attackIdx = static_cast<std::uint32_t>(attack * static_cast<float>(sampleRate));
            // Attack
            if (index < attackIdx) {
                // Linear
                outGain = static_cast<float>(index) / static_cast<float>(attackIdx);
            }
            // Decay
            else if (const std::uint32_t decayIdx = static_cast<std::uint32_t>(decay * static_cast<float>(sampleRate)); index < (decayIdx + attackIdx)) {
                // Sustain max -> no decay
                if (sustain == 1.f)
                    outGain = 1.f;
                else
                    outGain = (1.f - static_cast<float>(index - attackIdx) / static_cast<float>(decayIdx)) * OneMinusSustain + sustain;
            } else {
                std::cout << triggerIndex << std::endl;
                // Sustain
                outGain = sustain;
            }
        } else {
            // std::cout << index << " - " << triggerIndex << std::endl;
            // Release
            // const std::uint32_t releaseIdx = ((_lastGain[key] < sustain) ? _lastGain[key] : release) * static_cast<float>(sampleRate);
            const std::uint32_t releaseIdx = static_cast<std::uint32_t>(release * static_cast<float>(sampleRate));
            if (const std::uint32_t realIndex = index - _triggerIndex[key];
                realIndex < releaseIdx) {
                // if (sustain != _lastGain[key]) {
                //     std::cout << _lastGain[key] << " != " << sustain << std::endl;
                // }
                if (!_sustains[key]) {
                    _sustains[key] = _lastGain[key];
                }
                // outGain = (1.f - static_cast<float>(realIndex) / static_cast<float>(releaseIdx)) * sustain;
                outGain = (1.f - static_cast<float>(realIndex) / static_cast<float>(releaseIdx)) * _sustains[key];
            }
            // End of the enveloppe
            else {
                outGain = 0.f;
            }
        }
        _lastGain[key] = outGain;
        return _lastGain[key];
    }

    [[nodiscard]] float adsrOld(
            const Key key, const std::uint32_t index, const bool isTrigger,
            const float attack, const float decay, const float sustain, const float release, const SampleRate sampleRate) noexcept
    {
        const float OneMinusSustain = 1.0f - sustain;
        float outGain { 1.f };

        if (isTrigger) {
            const std::uint32_t attackIdx = static_cast<std::uint32_t>(attack * static_cast<float>(sampleRate));
            // Attack
            if (index < attackIdx) {
                // Linear
                outGain = static_cast<float>(index) / static_cast<float>(attackIdx);
            }
            // Decay
            else if (const std::uint32_t decayIdx = static_cast<std::uint32_t>(decay * static_cast<float>(sampleRate)); index < (decayIdx + attackIdx)) {
                // Sustain max -> no decay
                if (sustain == 1.f)
                    outGain = 1.f;
                else
                    outGain = (1.f - static_cast<float>(index - attackIdx) / static_cast<float>(decayIdx)) * OneMinusSustain + sustain;
            } else {
                // Sustain
                outGain = sustain;
            }
        } else {
            // Release
            // const std::uint32_t releaseIdx = ((_lastGain[key] < sustain) ? _lastGain[key] : release) * static_cast<float>(sampleRate);
            const std::uint32_t releaseIdx = static_cast<std::uint32_t>(release * static_cast<float>(sampleRate));
            if (const std::uint32_t realIndex = index - _triggerIndex[key];
                realIndex < releaseIdx) {
                outGain = (1.f - static_cast<float>(realIndex) / static_cast<float>(releaseIdx)) * sustain;
            }
            // End of the enveloppe
            else {
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
    SustainList _sustains;
};

#include "EnveloppeGenerator.ipp"
