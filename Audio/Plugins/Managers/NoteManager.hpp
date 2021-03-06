/**
 * @ Author: Pierre Veysseyre
 * @ Description: NoteManager
 */

#pragma once

#include <array>
#include <memory>

#include "Audio/Note.hpp"

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

    /** @brief Describe the internal cache */
    struct alignas_double_cacheline Cache
    {
        alignas_quarter_cacheline KeyList actives {};
        alignas_quarter_cacheline KeyList activesBlock {};
        std::array<NoteCache, KeyCount> notes;
    };

    static_assert_alignof_double_cacheline(Cache);


    std::size_t getTotalActiveNoteNumber(void) const noexcept { return getActiveNoteNumber() + getActiveNoteNumberBlock(); }
    std::size_t getActiveNoteNumber(void) const noexcept { return _cache.actives.size(); }
    std::size_t getActiveNoteNumberBlock(void) const noexcept { return _cache.activesBlock.size(); }

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

private:
    Cache   _cache;
};

static_assert_alignof_double_cacheline(Audio::NoteManager);

#include "NoteManager.ipp"
