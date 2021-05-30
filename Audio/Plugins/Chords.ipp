/**
 * @file Chords.ipp
 * @brief
 *
 * @author Pierre V
 * @date 2021-05-21
 */

inline void Audio::Chords::onAudioGenerationStarted(const BeatRange &range)
{
    UNUSED(range);
    _cache.reset();
    // _cache.setBeatRange(range);
    // _cache.setNoteCount(static_cast<ChordsManager::NoteIndex>(noteCount()));
}

inline void Audio::Chords::sendNotes(const NoteEvents &notes, const BeatRange &range)
{
    UNUSED(range);
    UNUSED(notes);
    if (static_cast<bool>(byPass())) {
        _byPassCache = notes;
        return;
    }

    // _cache.setBeatRange(range);
    // _cache.setNoteCount(static_cast<ChordsManager::NoteIndex>(noteCount()));
    if (notes.size()) {
        // std::cout << range << std::endl;
        _cache.feedNotes(notes, static_cast<std::int8_t>(noteOffset()), static_cast<ChordsManager::ChordType>(chordType()));
    }
}

inline void Audio::Chords::receiveNotes(NoteEvents &notes)
{
    UNUSED(notes);
    if (static_cast<bool>(byPass())) {
        notes.swap(_byPassCache);
        return;
    }
    _cache.generateNotes(notes);
}
