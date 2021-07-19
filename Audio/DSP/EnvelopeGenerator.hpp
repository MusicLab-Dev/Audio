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
        AR, AD, ADSR, DADSR, DAHDSR
    };

    enum class InterpolationType : std::uint8_t {
        Linear, Exp, InverseExp
    };

    template<EnvelopeType Envelope, unsigned Count = 1u>
    class EnvelopeBase;

}


template<Audio::DSP::EnvelopeType Envelope, unsigned Count>
class Audio::DSP::EnvelopeBase
{
public:
    struct alignas_half_cacheline KeyCache
    {
        std::uint32_t triggerIndex { 0u };
        float gain { 0.0f };
        float sustain { 0.0f };
    };
    static_assert_alignof_half_cacheline(KeyCache);

    using CacheList = std::array<std::array<KeyCache, Count>, KeyCount>;

    EnvelopeBase(void) { resetKeys(); }

    /** @brief Reset a specific instance for each keys */
    template<unsigned Index>
    void resetKeys(void) noexcept;

    /** @brief Reset all instances for each keys */
    void resetKeys(void) noexcept;


    /** @brief Reset a specific instance of a specific key */
    template<unsigned Index>
    void resetKey(const Key key) noexcept;

    /** @brief Reset all instances of a specific key */
    void resetKey(const Key key) noexcept;


    /** @brief Reset a specific instance's trigger index for each key */
    template<unsigned Index>
    void resetTriggerIndexes(void) noexcept;

    /** @brief Reset all instances' trigger indexes for each key */
    void resetTriggerIndexes(void) noexcept;

    /** @brief Reset a specific instance's trigger index for a specific key */
    template<unsigned Index>
    void resetTriggerIndex(const Key key) noexcept;

    /** @brief Reset all instances' trigger indexes for a specific key */
    void resetTriggerIndex(const Key key) noexcept;

    /** @brief Set a specific instance's trigger status of a specific key */
    template<unsigned Index>
    void setTriggerIndex(const Key key, const std::uint32_t triggerIndex) noexcept;

    /** @brief Set all instances' trigger status of a specific key */
    void setTriggerIndex(const Key key, const std::uint32_t triggerIndex) noexcept;


    /** @brief Reset a specific instance's gain for each key */
    template<unsigned Index>
    void resetInternalGains(void) noexcept;

    /** @brief Reset all instances' gains for each key */
    void resetInternalGains(void) noexcept;

    /** @brief Reset a specific instance's gain of a specific key */
    template<unsigned Index>
    void resetInternalGain(const Key key) noexcept;

    /** @brief Reset all instances' gains of a specific key */
    void resetInternalGain(const Key key) noexcept;


    /** @brief Check if the gain is null for a specific key */
    [[nodiscard]] bool isGainEnded(const Key key) const noexcept;

    /** @brief Get the last gain of a specific instance at a specific key */
    template<unsigned Index = 0u>
    [[nodiscard]] float lastGain(const Key key) noexcept { return _cache[key][Index].gain; }

    /** @brief Get the enveloppe gain */
    template<unsigned Index = 0u>
    [[nodiscard]] float getGain(
            const Key key, const std::uint32_t index,
            const float delay, const float attack,
            const float hold, const float decay,
            const float sustain, const float release,
            const SampleRate sampleRate) noexcept;

    /** @brief AR implementation */
    template<unsigned Index = 0u>
    [[nodiscard]] float attackRelease(
            const Key key, const std::uint32_t index,
            const float attack, const float release, const SampleRate sampleRate) noexcept;

    /** @brief AD implementation */
    template<unsigned Index = 0u>
    [[nodiscard]] float attackDecay(
            const Key key, const std::uint32_t index,
            const float attack, const float decay, const SampleRate sampleRate) noexcept;

    /** @brief ADSR implementation */
    template<unsigned Index = 0u>
    [[nodiscard]] float adsr(
            const Key key, const std::uint32_t index,
            const float attack, const float decay, const float sustain, const float release, const SampleRate sampleRate) noexcept;

    /** @brief DADSR implementation */
    template<unsigned Index = 0u>
    [[nodiscard]] float dadsr(
            const Key key, const std::uint32_t index,
            const float delay, const float attack, const float decay, const float sustain, const float release, const SampleRate sampleRate) noexcept;

private:
    CacheList _cache;

    [[nodiscard]] float smoothGain(KeyCache &keyCache, const float nextGain, const SampleRate sampleRate) noexcept;
};

#include "EnvelopeGenerator.ipp"
