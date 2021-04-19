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
        AR,
        ADSR,
        // DADSR,
        // DAHDSR,
        AD
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

    /** @brief Reset all trigers */
    void resetTriggerIndex(void) noexcept { _triggerIndex.fill(0u); }

    /** @brief Set the internal trigger status */
    void setTriggerIndex(const Key key, const std::size_t triggerIndex) noexcept { _triggerIndex[key] = triggerIndex; }

    [[nodiscard]] float getGain(const Key key, const std::size_t index, const bool isTrigger,
            const float delay, const float attack,
            const float hold, const float decay,
            const float sustain, const float release,
            const SampleRate sampleRate) const noexcept
    {
        if constexpr (Enveloppe == EnveloppeType::AR)
            return attackRelease(key, index, isTrigger, attack, release, sampleRate);
        else if constexpr (Enveloppe == EnveloppeType::ADSR)
            return adsr(key, index, isTrigger, attack, decay, release, sustain, sampleRate);
        return 1.f;
    }

    /** @brief Compute the output gain for a given key at an index */
    [[nodiscard]] float attackRelease(const Key key, const std::size_t index, const bool isTrigger,
            const float attack, const float release, const SampleRate sampleRate) const noexcept
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

    /** @brief Compute the output gain for a given key at an index */
    [[nodiscard]] float adsr(const Key key, const std::size_t index, const bool isTrigger,
            const float attack, const float decay, const float sustain, const float release, const SampleRate sampleRate) const noexcept
    {
        // const std::size_t attackIdx = attack * static_cast<float>(sampleRate);
        // const std::size_t releaseIdx = release * static_cast<float>(sampleRate);
        // if (isTrigger) {
        //     if (index < attackIdx) {
        //         return index / static_cast<float>(attackIdx);
        //     }
        //     return 1.f;
        // } else {
        //     if (const auto releaseIndex = index - _triggerIndex[key]; releaseIndex < releaseIdx) {
        //         return 1 - releaseIndex / static_cast<float>(releaseIdx);
        //     }
        //     return 0.f;
        // }
        return 0.f;
    }

private:
    IndexList _triggerIndex;
};

#include "EnveloppeGenerator.ipp"