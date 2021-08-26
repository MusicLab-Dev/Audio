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
    constexpr auto DefaultExponentialInterpolationRate = 4u;
    enum class InterpolationType : std::uint8_t
    {
        Linear = 0,
        Exp = 1,
        ExpInverse = 2
    };

    enum class EnvelopeType : std::uint8_t
    {
        AR, AD, ADSR, DADSR, DAHDSR
    };

    struct alignas_half_cacheline EnvelopeSpecs
    {
        float delay { 0.0f };
        float attack { 0.0f };
        float peak { 0.0f };
        float hold { 0.0f };
        float decay { 0.0f };
        float sustain { 0.0f };
        float release { 0.0f };
    };

    // enum class InterpolationType : std::uint8_t {
    //     Linear, Exp, InverseExp
    // };

    template<EnvelopeType Envelope, InterpolationType AttackInterp, InterpolationType DecayInterp, InterpolationType ReleaseInterp, bool Smooth, bool Clip, unsigned Count = 1u>
    class EnvelopeBase;

    template<EnvelopeType Envelope, unsigned Count>
    using EnvelopeDefaultLinear = EnvelopeBase<Envelope, InterpolationType::Linear, InterpolationType::Linear, InterpolationType::Linear, false, false, Count>;
    template<EnvelopeType Envelope, unsigned Count>
    using EnvelopeClipLinear = EnvelopeBase<Envelope, InterpolationType::Linear, InterpolationType::Linear, InterpolationType::Linear, false, true, Count>;

    template<EnvelopeType Envelope, unsigned Count>
    using EnvelopeDefaultExp = EnvelopeBase<Envelope, InterpolationType::Exp, InterpolationType::Exp, InterpolationType::Exp, false, false, Count>;
    template<EnvelopeType Envelope, unsigned Count>
    using EnvelopeDefaultExpInverse = EnvelopeBase<Envelope, InterpolationType::Exp, InterpolationType::ExpInverse, InterpolationType::ExpInverse, false, false, Count>;
    template<EnvelopeType Envelope, unsigned Count>
    using EnvelopeClipExp = EnvelopeBase<Envelope, InterpolationType::Exp, InterpolationType::Exp, InterpolationType::Exp, false, true, Count>;
    template<EnvelopeType Envelope, unsigned Count>
    using EnvelopeClipExpInverse = EnvelopeBase<Envelope, InterpolationType::Exp, InterpolationType::ExpInverse, InterpolationType::ExpInverse, false, true, Count>;
}


template<Audio::DSP::EnvelopeType Envelope, Audio::DSP::InterpolationType AttackInterp, Audio::DSP::InterpolationType DecayInterp, Audio::DSP::InterpolationType ReleaseInterp, bool Smooth, bool Clip, unsigned Count>
class Audio::DSP::EnvelopeBase
{
public:
    struct KeyCache
    {
        std::uint32_t triggerIndex { 0u };
        float gain { 0.0f };
        float sustain { 0.0f };
    };

    using CacheList = std::array<std::array<KeyCache, Count>, KeyCount>;
    using EnvelopeSpecsCache = std::array<EnvelopeSpecs, Count>;

    EnvelopeBase(void) { resetKeys(); }

    template<bool Accumulate, unsigned EnvelopeIndex = 0u>
    void generateGains(
            const Key key, const std::uint32_t index,
            float *output, const std::size_t outputSize) noexcept;


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

    /** @brief Set the internal envelope specs */
    template<unsigned Index>
    void setSpecs(const EnvelopeSpecs &specs) noexcept;

    /** @brief Set the internal envelope sample rate */
    void setSampleRate(const SampleRate sampleRate) noexcept { _sampleRate = sampleRate; }


    /** @brief Check if the gain is null for a specific key */
    [[nodiscard]] bool isGainEnded(const Key key) const noexcept;

    /** @brief Get the last gain of a specific instance at a specific key */
    template<unsigned Index = 0u>
    [[nodiscard]] float lastGain(const Key key) noexcept { return _cache[key][Index].gain; }

    /** @brief Get the enveloppe gain */
    template<unsigned Index = 0u>
    [[nodiscard]] float getGain(
            const Key key, const std::uint32_t index) noexcept;

    /** @brief AR implementation */
    template<unsigned Index = 0u>
    [[nodiscard]] float attackRelease(
            const Key key, const std::uint32_t index) noexcept;

    /** @brief AD implementation */
    template<unsigned Index = 0u>
    [[nodiscard]] float attackDecay(
            const Key key, const std::uint32_t index) noexcept;

    /** @brief ADSR implementation */
    template<unsigned Index = 0u>
    [[nodiscard]] float adsr(
            const Key key, const std::uint32_t index) noexcept;

    /** @brief DADSR implementation */
    template<unsigned Index = 0u>
    [[nodiscard]] float dadsr(
            const Key key, const std::uint32_t index) noexcept;

private:
    EnvelopeSpecsCache _specs;
    Audio::SampleRate _sampleRate;
    CacheList _cache;

    [[nodiscard]] float smoothGain(KeyCache &keyCache, const float nextGain) noexcept;

    template<unsigned Power = DefaultExponentialInterpolationRate>
    float unrollExponential(const float gain) const noexcept
    {
        if constexpr (Power != 0u)
            return gain * unrollExponential<Power - 1u>(gain);
        return 1.0f;
    }

    template<unsigned Power = DefaultExponentialInterpolationRate>
    float unrollExponentialInverse(const float gain) const noexcept
    {
        if constexpr (Power)
            return gain / unrollExponential<Power - 1u>(gain);
        return 1.0f;
    }

    void processAttack(float &outGain, const std::uint32_t index, const std::uint32_t duration) noexcept
    {
        outGain = static_cast<float>(index) / static_cast<float>(duration);

        if constexpr (AttackInterp == InterpolationType::Linear)
            return;
        else if constexpr (AttackInterp == InterpolationType::Exp) {
            outGain = unrollExponential(outGain);
        } else if constexpr (AttackInterp == InterpolationType::ExpInverse) {
        }
    }

    void processDecay(float &outGain, const std::uint32_t index, const std::uint32_t duration, const float sustain) noexcept
    {
        outGain = static_cast<float>(index) / static_cast<float>(duration);

        if constexpr (AttackInterp == InterpolationType::Linear)
            outGain = (1.0f - outGain) * (1.0f - sustain) + sustain;
        else if constexpr (DecayInterp == InterpolationType::Exp) {
            outGain = unrollExponential(1.0f - outGain);
            outGain = outGain * (1.0f - sustain) + sustain;
        } else if constexpr (DecayInterp == InterpolationType::ExpInverse) {
            outGain = unrollExponential(outGain);
            outGain = (1.0f - outGain) * (1.0f - sustain) + sustain;
        }
    }

    void processRelease(float &outGain, const std::uint32_t index, const std::uint32_t duration, const float sustain) noexcept
    {
        outGain = static_cast<float>(index) / static_cast<float>(duration);

        if constexpr (AttackInterp == InterpolationType::Linear)
            outGain = (1.0f - outGain) * sustain;
        else if constexpr (DecayInterp == InterpolationType::Exp) {
            outGain = unrollExponential(1.0f - outGain);
            outGain = outGain * sustain;
        } else if constexpr (DecayInterp == InterpolationType::ExpInverse) {
            outGain = unrollExponential(outGain);
            outGain = (1.0f - outGain) * sustain;
        }
    }
};

#include "EnvelopeGenerator.ipp"
