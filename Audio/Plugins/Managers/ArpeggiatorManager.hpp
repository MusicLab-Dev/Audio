/**
 * @file ArpeggiatorManager.hpp
 * @brief
 *
 * @author Pierre V
 * @date 2021-05-21
 */

#pragma once

#include <Core/SortedSmallVector.hpp>
#include <Audio/Note.hpp>

namespace Audio
{
    class ArpeggiatorManager;
}

class Audio::ArpeggiatorManager
{
public:
    // using KeyListActives = Core::SortedTinySmallVector<Key, 8u>;
    // using KeyList = Core::SortedTinySmallVector<Key, 1u>;

    using NoteIndex = std::uint8_t;
    using KeyListActives = Core::SortedSmallVector<Key, 14u, NoteIndex>;
    using KeyListPending = Core::SortedSmallVector<Key, 30u, NoteIndex>;
    using KeyListOffs = Core::SortedSmallVector<Key, 30u, NoteIndex>;
    using NoteModifiersArray = std::array<NoteModifiers, KeyCount>;
    using NoteTriggerList = std::array<bool, KeyCount>;

    enum class ArpMode : std::uint8_t
    {
        Up, Down, UpDown, DownUp, Random
    };

    struct alignas_double_cacheline Cache
    {
        // void f () {
        //     sizeof(KeyListActives) +
        //     sizeof(KeyListPending) +
        //     sizeof(KeyListOffs);
        //     sizeof(Cache);
        // }

        // 104 bytes
        KeyListActives actives {};
        KeyListPending pending {};
        KeyListOffs offs {};

        // 2 bytes
        NoteIndex noteCount { 0u };    // 1
        NoteIndex noteIndex { 0u };    // 1

        // 8 bytes
        alignas_eighth_cacheline BeatRange range { 0u, 0u };    // 8
        // 8 bytes
        Beat nextBeat { 0u };          // 4
        bool hasStart { false };       // 1
        bool hasEnded { false };       // 1
        bool rampUp { true };          // 1
        ArpMode mode;                  // 1
    };

    static_assert_fit_double_cacheline(Cache);

    /** @brief Process a list of notes and update the internal cache */
    void feedNotes(const NoteEvents &notes) noexcept;

    /** @brief Process the internal cache to generate notes */
    void generateNotes(NoteEvents &notes, const ArpMode mode, const NoteType noteType) noexcept;

    /** @brief Reset all */
    void reset(void) noexcept
    {
        _cache.actives.clear();
        _cache.pending.clear();
        _cache.offs.clear();

        _cache.nextBeat = 0u;
        _cache.range = { 0u, 0u };
        _cache.noteCount = 0u;
        _cache.noteIndex = 0u;
        _cache.rampUp = true;
        _cache.hasStart = false;
        _cache.hasEnded = false;
    }

    // /** @brief Set the internal starting beat */
    // void setStartingBeat(const Beat startingBeat) noexcept { _startingBeat = startingBeat; }

    /** @brief Set the internal frame size */
    void setBeatRange(const BeatRange range) noexcept { _cache.range = range; }

    void setNoteCount(const NoteIndex noteCount) noexcept { _cache.noteCount = noteCount; }

    void resetKey(const Key key) noexcept;

private:
    Cache _cache;

    void setNoteIndex(const Key key) noexcept;
    void setMode(const ArpMode mode, const NoteIndex noteCount) noexcept;
    NoteIndex getNextNoteIndex(const NoteIndex maxIndex) noexcept;


    Key incrementNoteIndex(const NoteIndex maxIndex) noexcept;
    NoteIndex updateActivesNotes(const NoteIndex activesAvailableSlots) noexcept;
    bool deleteNote(const Key key) noexcept;
    bool updateOffNotes(NoteIndex &realNoteCount) noexcept;
};

#include "ArpeggiatorManager.ipp"
