/**
 * @ Author: Pierre Veysseyre
 * @ Description: FMManager
 */

#pragma once

#include <array>
#include <memory>
#include <utility>

#include <Audio/Note.hpp>
#include <Audio/DSP/EnvelopeGenerator.hpp>
#include <Audio/DSP/FM.hpp>

namespace Audio
{
    template<DSP::EnvelopeType Envelope, unsigned OperatorCount, DSP::FM::AlgorithmType Algo, bool PitchEnv>
    class FMManager;

    template<DSP::EnvelopeType Envelope, unsigned OperatorCount, DSP::FM::AlgorithmType Algo, bool PitchEnv>
    using FMManagerPtr = std::unique_ptr<FMManager<Envelope, OperatorCount, Algo, PitchEnv>>;
}

/** @brief Note manager store states of each note */
template<Audio::DSP::EnvelopeType Envelope, unsigned OperatorCount, Audio::DSP::FM::AlgorithmType Algo = Audio::DSP::FM::AlgorithmType::Default, bool PitchEnv = false>
class alignas_double_cacheline Audio::FMManager
{
public:
    using KeyList = Core::TinySmallVector<Key, Core::CacheLineQuarterSize>;
    using IndexList = std::array<std::uint32_t, KeyCount>;
    using TriggerList = std::array<bool, KeyCount>;

    /** @brief Store the cache of a note */
    struct alignas_quarter_cacheline NoteCache
    {
        NoteModifiers noteModifiers {};
        NoteModifiers polyPressureModifiers {};
    };

    static_assert_fit_quarter_cacheline(NoteCache);

    using ModifiersList = std::array<NoteCache, KeyCount>;

    /** @brief Describe the internal cache */
    struct alignas_double_cacheline Cache
    {
        KeyList actives {};
        KeyList activesBlock {};
        ModifiersList modifiers;
        IndexList readIndexes;
        TriggerList triggers;
    };

    static_assert_alignof_double_cacheline(Cache);


    [[nodiscard]] std::uint32_t getAllActiveNoteSize(void) const noexcept { return getActiveNoteSize() + getActiveNoteBlockSize(); }
    [[nodiscard]] std::uint32_t getActiveNoteSize(void) const noexcept { return _cache.actives.size(); }
    [[nodiscard]] std::uint32_t getActiveNoteBlockSize(void) const noexcept { return _cache.activesBlock.size(); }

    [[nodiscard]] KeyList &getActiveNote(void) noexcept { return _cache.actives; }
    [[nodiscard]] KeyList &getActiveNoteBlock(void) noexcept { return _cache.activesBlock; }

    [[nodiscard]] const KeyList &getActiveNote(void) const noexcept { return _cache.actives; }
    [[nodiscard]] const KeyList &getActiveNoteBlock(void) const noexcept { return _cache.activesBlock; }

    template<typename Functor>
    void processNotes(Functor &&functor) noexcept;


    /** @brief Process a list of notes and update the internal cache */
    void feedNotes(const NoteEvents &notes) noexcept;

    /** @brief Reset all */
    void reset(void)
    {
        resetCache();
        resetAllModifiers();
        resetTriggers();
        resetReadIndexes();
        _enveloppe.resetTriggerIndexes();
        _enveloppe.resetInternalGains();

        _schema.envelopes().resetKeys();
    }

    /** @brief Reset the internal cache. All notes are turned off */
    void resetCache(void) noexcept;

    /** @brief Reset the internal cache for this block. On&Off notes are turned off */
    void resetBlockCache(void) noexcept;


    /** @brief Reset the internal note modifiers */
    void resetNoteModifiers(void) noexcept;

    /** @brief Reset the internal note poly pressure modifiers */
    void resetPolyModifiers(void) noexcept;

    /** @brief Reset the internal modifiers. Both note and poly pressure modifiers are reset */
    void resetAllModifiers(void) noexcept;


    /** @brief Reset the internal triggers */
    void resetTriggers(void) noexcept;

    /** @brief Get a trigger state by Key */
    [[nodiscard]] bool trigger(const Key key) const noexcept { return _cache.triggers[key]; }

    /** @brief Set a trigger state by Key. Return true if the trigger state changed */
    bool setTrigger(const Key key, const bool state) noexcept;

    /** @brief Reset all read indexes */
    void resetReadIndexes(void) noexcept;

    /** @brief Increment the read index of given key */
    [[nodiscard]] bool incrementReadIndex(const Key key, const std::uint32_t maxIndex, std::uint32_t amount = 1u) noexcept;


    /** @brief Get the read index of given key */
    [[nodiscard]] std::uint32_t readIndex(const Key key) const noexcept { return _cache.readIndexes[key]; }

    /** @brief Set the read index of given key */
    void setReadIndex(const Key key, const std::uint32_t index) noexcept { _cache.readIndexes[key] = key; }

    /** @brief Reset the read index of given key */
    void resetReadIndex(const Key key) noexcept { _cache.readIndexes[key] = 0u; }


    /** @brief Get the enveloppe gain of given key */
    [[nodiscard]] inline float getEnvelopeGain(
            const Key key, const std::uint32_t index,
            const float delay, const float attack,
            const float hold, const float decay,
            const float sustain, const float release,
            const SampleRate sampleRate) noexcept
    { return _enveloppe.getGain(key, index, delay, attack, hold, decay, sustain, release, sampleRate); }

    [[nodiscard]] const DSP::EnvelopeBase<Envelope> &enveloppe(void) const noexcept { return _enveloppe; }
    [[nodiscard]] DSP::EnvelopeBase<Envelope> &enveloppe(void) noexcept { return _enveloppe; }

    [[nodiscard]] const DSP::FM::Schema<OperatorCount, Algo, PitchEnv> &schema(void) const noexcept { return _schema; }
    [[nodiscard]] DSP::FM::Schema<OperatorCount, Algo, PitchEnv> &schema(void) noexcept { return _schema; }

    template<bool Accumulate>
    void processSchema(
            float *output, const std::uint32_t processSize, const float outGain,
            const std::uint32_t phaseIndex, const Key key, const float rootFrequency,
            const DSP::FM::Internal::OperatorArray<OperatorCount> &operators
    ) noexcept
    {
        updateLongestEnvOperatorIndex(operators);
        _schema.template process<Accumulate>(output, processSize, outGain, phaseIndex, key, rootFrequency, operators);
    }

private:
    Cache   _cache;
    DSP::EnvelopeBase<Envelope> _enveloppe;

    DSP::FM::Schema<OperatorCount, Algo, PitchEnv> _schema;
    DSP::FM::Internal::OperatorCountType _longestEnvOpIndex { 0u };

    void updateLongestEnvOperatorIndex(const DSP::FM::Internal::OperatorArray<OperatorCount> &operators) noexcept
    {
        float maxEnvSize { 0.f };
        for (DSP::FM::Internal::OperatorCountType i = 0; i < OperatorCount; ++i) {
            if (const float envSize = operators[i].attack + operators[i].decay + operators[i].sustain + operators[i].release; envSize > maxEnvSize) {
                maxEnvSize = envSize;
                _longestEnvOpIndex = i;
            }
        }
    }
};

// static_assert_alignof_double_cacheline(Audio::FMManager);

#include "FMManager.ipp"
