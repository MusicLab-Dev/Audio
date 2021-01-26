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
        alignas_quarter_cacheline Core::TinyVector<Key> actives {};
        alignas_quarter_cacheline Core::TinyVector<Key> activesBlock {};
        std::array<NoteCache, KeyCount> notes;
    };

    static_assert_alignof_double_cacheline(Cache);


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
