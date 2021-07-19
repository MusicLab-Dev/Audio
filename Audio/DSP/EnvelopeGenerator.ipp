/**
 * @ Author: Pierre Veysseyre
 * @ Description: EnvelopeGenerator implementation
 */

template<Audio::DSP::EnvelopeType Envelope, unsigned Count>
template<unsigned Index>
inline void Audio::DSP::EnvelopeBase<Envelope, Count>::resetKeys(void) noexcept
{
    static_assert(Index < Count,
        "Audio::DSP::EnvelopeBase::reset: Index must be less than Instance Count");

    for (auto &key : _cache)
        key[Index] = KeyCache {};
}

template<Audio::DSP::EnvelopeType Envelope, unsigned Count>
inline void Audio::DSP::EnvelopeBase<Envelope, Count>::resetKeys(void) noexcept
{
    for (auto &key : _cache)
        key.fill(KeyCache {});
}

template<Audio::DSP::EnvelopeType Envelope, unsigned Count>
template<unsigned Index>
inline void Audio::DSP::EnvelopeBase<Envelope, Count>::resetKey(const Key key) noexcept
{
    static_assert(Index < Count,
        "Audio::DSP::EnvelopeBase::reset: Index must be less than Instance Count");

    _cache[key][Index] = KeyCache {};
}

template<Audio::DSP::EnvelopeType Envelope, unsigned Count>
inline void Audio::DSP::EnvelopeBase<Envelope, Count>::resetKey(const Key key) noexcept
{
    _cache[key].fill(KeyCache {});
}

template<Audio::DSP::EnvelopeType Envelope, unsigned Count>
template<unsigned Index>
inline void Audio::DSP::EnvelopeBase<Envelope, Count>::resetTriggerIndexes(void) noexcept
{
    static_assert(Index < Count,
        "Audio::DSP::EnvelopeBase::resetTriggerIndexes: Index must be less than Instance Count");

    for (auto &key : _cache) {
        key[Index].triggerIndex = 0ul;
    }
}

template<Audio::DSP::EnvelopeType Envelope, unsigned Count>
inline void Audio::DSP::EnvelopeBase<Envelope, Count>::resetTriggerIndexes(void) noexcept
{
    for (auto &key : _cache) {
        for (auto &instance : key)
            instance.triggerIndex = 0u;
    }
}

template<Audio::DSP::EnvelopeType Envelope, unsigned Count>
template<unsigned Index>
inline void Audio::DSP::EnvelopeBase<Envelope, Count>::resetTriggerIndex(const Key key) noexcept
{
    static_assert(Index < Count,
        "Audio::DSP::EnvelopeBase::resetTriggerIndex: Index must be less than Instance Count");

    _cache[key][Index].triggerIndex = 0ul;
}

template<Audio::DSP::EnvelopeType Envelope, unsigned Count>
inline void Audio::DSP::EnvelopeBase<Envelope, Count>::resetTriggerIndex(const Key key) noexcept
{
    for (auto &instance : _cache[key])
        instance.triggerIndex = 0ul;
}

template<Audio::DSP::EnvelopeType Envelope, unsigned Count>
template<unsigned Index>
inline void Audio::DSP::EnvelopeBase<Envelope, Count>::setTriggerIndex(const Key key, const std::uint32_t triggerIndex) noexcept
{
    static_assert(Index < Count,
        "Audio::DSP::EnvelopeBase::setTriggerIndex: Index must be less than Instance Count");

    _cache[key][Index].triggerIndex = triggerIndex ? triggerIndex : 1u;
    // _cache[key][Index].triggerIndex = triggerIndex;
}

template<Audio::DSP::EnvelopeType Envelope, unsigned Count>
inline void Audio::DSP::EnvelopeBase<Envelope, Count>::setTriggerIndex(const Key key, const std::uint32_t triggerIndex) noexcept
{
    for (auto &instance : _cache[key])
        instance.triggerIndex = triggerIndex;
}

template<Audio::DSP::EnvelopeType Envelope, unsigned Count>
template<unsigned Index>
inline void Audio::DSP::EnvelopeBase<Envelope, Count>::resetInternalGains(void) noexcept
{
    static_assert(Index < Count,
        "Audio::DSP::EnvelopeBase::resetInternalGains: Index must be less than Instance Count");

    for (auto &key : _cache) {
        key[Index].gain = 0.0f;
    }
}

template<Audio::DSP::EnvelopeType Envelope, unsigned Count>
inline void Audio::DSP::EnvelopeBase<Envelope, Count>::resetInternalGains(void) noexcept
{
    for (auto &key : _cache) {
        for (auto &instance : key)
            instance.gain = 0.0f;
    }
}

template<Audio::DSP::EnvelopeType Envelope, unsigned Count>
template<unsigned Index>
inline void Audio::DSP::EnvelopeBase<Envelope, Count>::resetInternalGain(const Key key) noexcept
{
    static_assert(Index < Count,
        "Audio::DSP::EnvelopeBase::resetInternalGain: Index must be less than Instance Count");

    _cache[key][Index].gain = 0.0f;
}

template<Audio::DSP::EnvelopeType Envelope, unsigned Count>
inline void Audio::DSP::EnvelopeBase<Envelope, Count>::resetInternalGain(const Key key) noexcept
{
    for (auto &instance : _cache[key])
        instance.gain = 0.0f;
}

template<Audio::DSP::EnvelopeType Envelope, unsigned Count>
inline bool Audio::DSP::EnvelopeBase<Envelope, Count>::isGainEnded(const Key key) const noexcept
{
    for (auto &instance : _cache[key]) {
        if (instance.gain)
            return false;
    }
    return true;
}

template<Audio::DSP::EnvelopeType Envelope, unsigned Count>
template<unsigned Index>
inline float Audio::DSP::EnvelopeBase<Envelope, Count>::getGain(
        const Key key, const std::uint32_t index,
        const float delay, const float attack,
        const float hold, const float decay,
        const float sustain, const float release,
        const SampleRate sampleRate) noexcept
{
    static_assert(Index < Count,
        "Audio::DSP::EnvelopeBase::getGain: Index must be less than Instance Count");

    UNUSED(delay);
    UNUSED(hold);
    if constexpr (Envelope == EnvelopeType::AR)
        return attackRelease<Index>(key, index, (attack >= EnvelopeMinTimeStepUp ? attack : EnvelopeMinTimeStepUp), (release >= EnvelopeMinTimeStepDown ? release : EnvelopeMinTimeStepDown), sampleRate);
    else if constexpr (Envelope == EnvelopeType::AD)
        return attackDecay<Index>(key, index, (attack >= EnvelopeMinTimeStepUp ? attack : EnvelopeMinTimeStepUp), (decay >= EnvelopeMinTimeStepDown ? decay : EnvelopeMinTimeStepDown), sampleRate);
    else if constexpr (Envelope == EnvelopeType::ADSR)
        return adsr<Index>(key, index, (attack >= EnvelopeMinTimeStepUp ? attack : EnvelopeMinTimeStepUp), (decay >= EnvelopeMinTimeStepDown ? decay : EnvelopeMinTimeStepDown), sustain, (release >= EnvelopeMinTimeStepDown ? release : EnvelopeMinTimeStepDown), sampleRate);
    return 0.0f;
}

template<Audio::DSP::EnvelopeType Envelope, unsigned Count>
template<unsigned Index>
inline float Audio::DSP::EnvelopeBase<Envelope, Count>::attackRelease(
        const Key key, const std::uint32_t index,
        const float attack, const float release, const SampleRate sampleRate) noexcept
{
    static_assert(Index < Count,
        "Audio::DSP::EnvelopeBase::attackRelease: Index must be less than Instance Count");

    const std::uint32_t attackIdx = static_cast<std::uint32_t>(attack * static_cast<float>(sampleRate));
    const std::uint32_t releaseIdx = static_cast<std::uint32_t>(release * static_cast<float>(sampleRate));
    float outGain { 1.0f };
    auto &keyCache = _cache[key][Index];

    if (!keyCache.triggerIndex || (index < keyCache.triggerIndex)) {
        if (index < attackIdx) {
            outGain = static_cast<float>(index) / static_cast<float>(attackIdx);
        } else {
            outGain = 1.0f;
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

template<Audio::DSP::EnvelopeType Envelope, unsigned Count>
template<unsigned Index>
inline float Audio::DSP::EnvelopeBase<Envelope, Count>::attackDecay(
        const Key key, const std::uint32_t index,
        const float attack, const float decay, const SampleRate sampleRate) noexcept
{
    static_assert(Index < Count,
        "Audio::DSP::EnvelopeBase::attackDecay: Index must be less than Instance Count");
    float outGain { 1.0f };
    auto &keyCache = _cache[key][Index];

    if (!keyCache.triggerIndex || (index < keyCache.triggerIndex)) {
        const std::uint32_t attackIdx = attack * static_cast<float>(sampleRate);
        if (attackIdx >= index) {
            outGain = static_cast<float>(index) / static_cast<float>(attackIdx);
        }
        if (const std::uint32_t decayIdx = decay * static_cast<float>(sampleRate); index < decayIdx + attackIdx)
            outGain = 1.0f - static_cast<float>(index - attackIdx) / static_cast<float>(decayIdx);
    } else {
        outGain = 0.0f;
    }

    keyCache.gain = outGain;
    return outGain;
}

template<Audio::DSP::EnvelopeType Envelope, unsigned Count>
template<unsigned Index>
inline float Audio::DSP::EnvelopeBase<Envelope, Count>::adsr(
        const Key key, const std::uint32_t index,
        const float attack, const float decay, const float sustain, const float release, const SampleRate sampleRate) noexcept
{
    static_assert(Index < Count,
        "Audio::DSP::EnvelopeBase::adsr: Index must be less than Instance Count");

    const float OneMinusSustain = 1.0f - sustain;
    float outGain { 1.0f };
    auto &keyCache = _cache[key][Index];

    if (!keyCache.triggerIndex || (index < keyCache.triggerIndex)) {
        const std::uint32_t attackIdx = static_cast<std::uint32_t>(attack * static_cast<float>(sampleRate));
        // Attack
        if (index < attackIdx) {
            // if (!keyCache.sustain && keyCache.gain) {
            //     // std::cout << "sustain reset: " << (1.0f - keyCache.gain) / static_cast<float>(attackIdx) << std::en
            //     keyCache.sustain = keyCache.gain;
            // }
            // outGain = keyCache.sustain + static_cast<float>(index) * (1.0f - keyCache.sustain) / static_cast<float>(attackIdx);

            outGain = keyCache.sustain + static_cast<float>(index) / static_cast<float>(attackIdx);
        }
        // Decay
        else if (const std::uint32_t decayIdx = static_cast<std::uint32_t>(decay * static_cast<float>(sampleRate)); index < (decayIdx + attackIdx)) {
            // Sustain max -> no decay
            if (sustain == 1.0f)
                outGain = 1.0f;
            else
                outGain = (1.0f - static_cast<float>(index - attackIdx) / static_cast<float>(decayIdx)) * OneMinusSustain + sustain;
        } else {
            // std::cout << keyCache.triggerIndex << std::endl;
            // Sustain
            keyCache.sustain = 0.0f;
            outGain = sustain;
        }
    } else {
        // std::cout << index << " - " << keyCache.triggerIndex << std::endl;
        // Release
        // const std::uint32_t releaseIdx = ((keyCache.gain < sustain) ? keyCache.gain : release) * static_cast<float>(sampleRate);
        const std::uint32_t releaseIdx = static_cast<std::uint32_t>(release * static_cast<float>(sampleRate));
        if (const std::uint32_t realIndex = index - keyCache.triggerIndex; realIndex < releaseIdx) {
            if (!keyCache.sustain) {
                keyCache.sustain = keyCache.gain;
            }
            outGain = (1.0f - static_cast<float>(realIndex) / static_cast<float>(releaseIdx)) * keyCache.sustain;
        }
        // End of the enveloppe
        else {
            keyCache.sustain = 0.0f;
            outGain = 0.0f;
        }
    }
    // if (outGain && outGain != 1.0f)
    //     std::cout << outGain << std::endl;
    return smoothGain(keyCache, outGain, sampleRate);
}

template<Audio::DSP::EnvelopeType Envelope, unsigned Count>
template<unsigned Index>
inline float Audio::DSP::EnvelopeBase<Envelope, Count>::dadsr(
        const Key key, const std::uint32_t index,
        const float delay, const float attack, const float decay, const float sustain, const float release, const SampleRate sampleRate) noexcept
{
    static_assert(Index < Count,
        "Audio::DSP::EnvelopeBase::adsr: Index must be less than Instance Count");

    const float OneMinusSustain = 1.0f - sustain;
    float outGain { 1.0f };
    auto &keyCache = _cache[key][Index];

    if (const std::uint32_t delayIdx = static_cast<std::uint32_t>(delay * static_cast<float>(sampleRate)); index < delayIdx) {
        outGain = 0.0f;
    }
    else if (!keyCache.triggerIndex || (index < keyCache.triggerIndex)) {
        const std::uint32_t attackIdx = static_cast<std::uint32_t>(attack * static_cast<float>(sampleRate));
        // Attack
        if (index < attackIdx) {
            // if (!keyCache.sustain && keyCache.gain) {
            //     // std::cout << "sustain reset: " << (1.0f - keyCache.gain) / static_cast<float>(attackIdx) << std::en
            //     keyCache.sustain = keyCache.gain;
            // }
            // outGain = keyCache.sustain + static_cast<float>(index) * (1.0f - keyCache.sustain) / static_cast<float>(attackIdx);

            outGain = keyCache.sustain + static_cast<float>(index) / static_cast<float>(attackIdx);
        }
        // Decay
        else if (const std::uint32_t decayIdx = static_cast<std::uint32_t>(decay * static_cast<float>(sampleRate)); index < (decayIdx + attackIdx)) {
            // Sustain max -> no decay
            if (sustain == 1.0f)
                outGain = 1.0f;
            else
                outGain = (1.0f - static_cast<float>(index - attackIdx) / static_cast<float>(decayIdx)) * OneMinusSustain + sustain;
        } else {
            // std::cout << keyCache.triggerIndex << std::endl;
            // Sustain
            keyCache.sustain = 0.0f;
            outGain = sustain;
        }
    } else {
        // std::cout << index << " - " << keyCache.triggerIndex << std::endl;
        // Release
        // const std::uint32_t releaseIdx = ((keyCache.gain < sustain) ? keyCache.gain : release) * static_cast<float>(sampleRate);
        const std::uint32_t releaseIdx = static_cast<std::uint32_t>(release * static_cast<float>(sampleRate));
        if (const std::uint32_t realIndex = index - keyCache.triggerIndex; realIndex < releaseIdx) {
            if (!keyCache.sustain) {
                keyCache.sustain = keyCache.gain;
            }
            outGain = (1.0f - static_cast<float>(realIndex) / static_cast<float>(releaseIdx)) * keyCache.sustain;
        }
        // End of the enveloppe
        else {
            keyCache.sustain = 0.0f;
            outGain = 0.0f;
        }
    }
    // if (outGain && outGain != 1.0f)
    //     std::cout << outGain << std::endl;
    return smoothGain(keyCache, outGain, sampleRate);
}

template<Audio::DSP::EnvelopeType Envelope, unsigned Count>
inline float Audio::DSP::EnvelopeBase<Envelope, Count>::smoothGain(KeyCache &keyCache, const float nextGain, const SampleRate sampleRate) noexcept
{
    // time * sampleRate = samples
    // gainStep = 1/samples <=> samples = 1/gainStep
    //
    // const auto dt = (nextGain - keyCache.gain);

    // if (!dt) {
    //     keyCache.gain = nextGain;
    // } else if (auto step = 1.0f / dt * static_cast<float>(sampleRate); step < -EnvelopeMinTimeStepDown) {
    //     // Go down (1 -> 0)
    //     keyCache.gain = nextGain + (dt / 2.0f);
    // } else if (step > EnvelopeMinTimeStepUp) {
    //     // Go up (0 -> 1)
    //     keyCache.gain = nextGain;
    // }
    UNUSED(sampleRate);
    keyCache.gain = nextGain;
    return keyCache.gain;
}
