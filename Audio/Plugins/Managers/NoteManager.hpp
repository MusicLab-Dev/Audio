/**
 * @ Author: Pierre Veysseyre
 * @ Description: NoteManager
 */

#pragma once

#include <array>
#include <memory>

#include "Audio/Notes.hpp"

namespace Audio
{
    class NoteManager;

    using NoteManagerPtr = std::unique_ptr<NoteManager>;
};

/** @brief Note manager store states of each note */
class alignas_double_cacheline Audio::NoteManager
{
public:
    using KeyList = Core::TinyVector<Key>;
    using IndexList = std::array<std::size_t, KeyCount>;
    using TriggerList = std::array<bool, KeyCount>;

    /** @brief Modifiers of a note */
    struct NoteModifiers
    {
        Velocity velocity { 0u };
        Tuning tuning { 0u };
    };

    /** @brief Store the cache of a note */
    struct alignas_eighth_cacheline NoteCache
    {
        NoteModifiers noteModifiers {};
        NoteModifiers polyPressureModifiers {};
    };

    static_assert_fit_eighth_cacheline(NoteCache);

    using ModifiersList = std::array<NoteCache, KeyCount>;

    /** @brief Describe the internal cache */
    struct alignas_double_cacheline Cache
    {
        alignas_quarter_cacheline KeyList actives {};
        alignas_quarter_cacheline KeyList activesBlock {};
        ModifiersList modifiers;
        IndexList readIndexes;
        TriggerList triggers;
    };

    static_assert_alignof_double_cacheline(Cache);


    std::size_t getAllActiveNoteSize(void) const noexcept { return getActiveNoteSize() + getActiveNoteBlockSize(); }
    std::size_t getActiveNoteSize(void) const noexcept { return _cache.actives.size(); }
    std::size_t getActiveNoteBlockSize(void) const noexcept { return _cache.activesBlock.size(); }

    KeyList &getActiveNote(void) noexcept { return _cache.actives; }
    KeyList &getActiveNoteBlock(void) noexcept { return _cache.activesBlock; }

    const KeyList &getActiveNote(void) const noexcept { return _cache.actives; }
    const KeyList &getActiveNoteBlock(void) const noexcept { return _cache.activesBlock; }


    /** @brief Process a list of notes and update the internal cache */
    void feedNotes(const NoteEvents &notes) noexcept;


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
    bool trigger(const Key key) const noexcept { return _cache.triggers[key]; }

    /** @brief Set a trigger state by Key. Return true if the trigger state changed */
    bool setTrigger(const Key key, const bool state) noexcept;

    /** @brief Reset all read indexes */
    void resetReadIndexes(void) noexcept;

    /** @brief Increment the read index of given key */
    void incrementReadIndex(const Key key, const std::size_t maxIndex, std::size_t amount = 1u) noexcept;

    [[nodiscard]] std::size_t readIndex(const Key key) const noexcept { return _cache.readIndexes[key]; }

private:
    Cache   _cache;
};

static_assert_alignof_double_cacheline(Audio::NoteManager);

#include "NoteManager.ipp"
