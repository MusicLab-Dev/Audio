/**
 * @file ChordsManager.hpp
 * @brief
 *
 * @author Pierre V
 * @date 2021-05-21
 */

#pragma once

#include <Core/FlatVector.hpp>
#include <Audio/Note.hpp>

namespace Audio
{
    class ChordsManager;
}

class Audio::ChordsManager
{
public:
    using NoteIndex = std::uint8_t;

    enum class ChordType : std::uint8_t
    {
        Major, Minor, Diminished, Augmented
    };

    struct ChordSignature
    {
        Key rootKey;
        ChordType type;
    };

    using KeyList = Core::FlatVector<ChordSignature, NoteIndex>;

    struct Cache
    {
        KeyList actives;
        NoteEvents events;
    };


    /** @brief Process a list of notes and update the internal cache */
    void feedNotes(const NoteEvents &notes, const std::int8_t semitoneOffset, const ChordType type) noexcept;

    /** @brief Process the internal cache to generate notes */
    void generateNotes(NoteEvents &notes) noexcept;

    /** @brief Reset all */
    void reset(void) noexcept
    {
        _cache.actives.clear();
        // _cache.pending.clear();
        // _cache.offs.clear();

        // _cache.nextBeat = 0u;
        // _cache.range = { 0u, 0u };
        // _cache.noteCount = 0u;
        // _cache.noteIndex = 0u;
        // _cache.rampUp = true;
        // _cache.hasStart = false;
        // _cache.hasEnded = false;
    }

private:
    Cache _cache;

    void makeChord(const Key key, const ChordType type) noexcept;
    void releaseChord(const Key key) noexcept;
};

#include "ChordsManager.ipp"
