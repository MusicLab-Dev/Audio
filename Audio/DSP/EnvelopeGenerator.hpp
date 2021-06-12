/**
 * @ Author: Pierre Veysseyre
 * @ Description: EnvelopeGenerator
 */

#pragma once

#include <Core/Utils.hpp>
#include <Core/Vector.hpp>

#include <Audio/Base.hpp>

#include <iostream>

namespace Audio::DSP
{
    enum class EnvelopeType : std::uint8_t {
        AR, AD, ADSR//, DADSR, DAHDSR
    };

    enum class InterpolationType : std::uint8_t {
        Linear, Exp, InverseExp
    };

    template<EnvelopeType Envelope>
    class EnvelopeBase;

    class AttackRelease;
    class AttackDecayReleaseSustain;
}


template<Audio::DSP::EnvelopeType Envelope>
class Audio::DSP::EnvelopeBase
{
public:
    struct alignas_half_cacheline KeyCache {
        std::uint32_t triggerIndex;
        float gain;
        float sustain;
    };
    static_assert_alignof_half_cacheline(KeyCache);

    using CacheList = std::array<KeyCache, KeyCount>;

    EnvelopeBase(void) { reset(); }

    /** @brief Reset all */
    void reset(void) noexcept { resetTriggerIndexes(); resetInternalGains(); resetInternalSustains(); }

    /** @brief Reset a key */
    void resetKey(const Key key) noexcept { resetTriggerIndex(key); resetInternalGain(key); resetInternalSustain(key); }


    /** @brief Reset all trigger indexes */
    void resetTriggerIndexes(void) noexcept
    {
        for (auto i = 0u; i < KeyCount; ++i)
            _cache[i].triggerIndex = 0ul;
    }

    /** @brief Reset a specific trigger index */
    void resetTriggerIndex(const Key key) noexcept { _cache[key].triggerIndex = 0ul; }

    /** @brief Set the internal trigger status */
    void setTriggerIndex(const Key key, const std::uint32_t triggerIndex) noexcept { _cache[key].triggerIndex = triggerIndex ? triggerIndex : 1u; }


    /** @brief Reset all internal gains */
    void resetInternalGains(void) noexcept
    {
        for (auto i = 0u; i < KeyCount; ++i)
            _cache[i].gain = 0.0f;
    }

    /** @brief Reset a specific gain */
    void resetInternalGain(const Key key) noexcept { _cache[key].gain = 0.0f; }

    /** @brief Reset all internal sustains */
    void resetInternalSustains(void) noexcept
    {
        for (auto i = 0u; i < KeyCount; ++i)
            _cache[i].sustain = 0.0f;
    }

    /** @brief Reset a specific sustain */
    void resetInternalSustain(const Key key) noexcept { _cache[key].sustain = 0.0f; }


    /** @brief Get the last gain */
    [[nodiscard]] float lastGain(const Key key) noexcept { return _cache[key].gain; }

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
        if constexpr (Envelope == EnvelopeType::AR)
            return attackRelease(key, index, isTrigger, attack, release, sampleRate);
        else if constexpr (Envelope == EnvelopeType::AD)
            return attackDecay(key, index, isTrigger, attack, decay, sampleRate);
        else if constexpr (Envelope == EnvelopeType::ADSR)
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
        auto &keyCache = _cache[key];

        if (isTrigger) {
            if (index < attackIdx) {
                outGain = static_cast<float>(index) / static_cast<float>(attackIdx);
            } else {
                outGain = 1.f;
            }
        } else {
            if (const auto releaseIndex = index - keyCache.triggerIndex; releaseIndex < releaseIdx) {
                outGain = 1.0f - static_cast<float>(releaseIndex) / static_cast<float>(releaseIdx);
            } else {
                outGain = 0.f;
            }
        }
        keyCache.gain = outGain;
        return outGain;
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
        auto &keyCache = _cache[key];

        UNUSED(isTrigger);
        if (!keyCache.triggerIndex || (index < keyCache.triggerIndex)) {
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
                // std::cout << keyCache.triggerIndex << std::endl;
                // Sustain
                outGain = sustain;
            }
        } else {
            // std::cout << index << " - " << keyCache.triggerIndex << std::endl;
            // Release
            // const std::uint32_t releaseIdx = ((keyCache.gain < sustain) ? keyCache.gain : release) * static_cast<float>(sampleRate);
            const std::uint32_t releaseIdx = static_cast<std::uint32_t>(release * static_cast<float>(sampleRate));
            if (const std::uint32_t realIndex = index - keyCache.triggerIndex;
                realIndex < releaseIdx) {
                // if (sustain != keyCache.gain) {
                //     std::cout << keyCache.gain << " != " << sustain << std::endl;
                // }
                if (!keyCache.sustain) {
                    keyCache.sustain = keyCache.gain;
                }
                // outGain = (1.f - static_cast<float>(realIndex) / static_cast<float>(releaseIdx)) * sustain;
                outGain = (1.f - static_cast<float>(realIndex) / static_cast<float>(releaseIdx)) * keyCache.sustain;
            }
            // End of the enveloppe
            else {
                outGain = 0.f;
            }
        }
        keyCache.gain = outGain;
        return outGain;
    }

    [[nodiscard]] float adsrOld(
            const Key key, const std::uint32_t index, const bool isTrigger,
            const float attack, const float decay, const float sustain, const float release, const SampleRate sampleRate) noexcept
    {
        const float OneMinusSustain = 1.0f - sustain;
        float outGain { 1.f };
        auto &keyCache = _cache[key];

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
            // const std::uint32_t releaseIdx = ((keyCache.gain < sustain) ? keyCache.gain : release) * static_cast<float>(sampleRate);
            const std::uint32_t releaseIdx = static_cast<std::uint32_t>(release * static_cast<float>(sampleRate));
            if (const std::uint32_t realIndex = index - keyCache.triggerIndex;
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
        // if (const float dt = std::abs(outGain - keyCache.gain); dt > SmoothLimit) {
        //     keyCache.gain = (outGain + keyCache.gain) / 2.f;
        // } else {
        //     keyCache.gain = outGain;
        // }
        // if (const float dt = outGain - keyCache.gain; dt > SmoothLimit) {
        //     keyCache.gain = outGain - SmoothLimit;
        //     // keyCache.gain = keyCache.gain + SmoothLimit;
        // } else if (dt < -SmoothLimit) {
        //     keyCache.gain = outGain + SmoothLimit;
        //     // keyCache.gain = keyCache.gain - SmoothLimit;
        // } else {
        //     keyCache.gain = outGain;
        // }

        keyCache.gain = outGain;
        return outGain;
    }

private:
    CacheList _cache;
    // IndexList _triggerIndex;
    // GainList _lastGain;
    // SustainList _sustains;
};

#include "EnvelopeGenerator.ipp"
