/**
 * @file ChordsManager.ipp
 * @brief ChordsManager implementation
 *
 * @author Pierre V
 * @date 2021-05-21
 */

inline void Audio::ChordsManager::feedNotes(const NoteEvents &notes, const std::int8_t semitoneOffset, const ChordType type) noexcept
{
    // std::cout << "CHORDS> SEND NOTES " << notes.size() << std::endl;
    for (const auto &note : notes) {
        const auto key = static_cast<Key>(static_cast<std::int8_t>(note.key) + semitoneOffset);
        UNUSED(key);
        switch (note.type) {
        case NoteEvent::EventType::On:
        {
            makeChord(key, type);
            break;
        }
        case NoteEvent::EventType::Off:
        {
            releaseChord(key);
            break;
        }
        default:
            break;
        }
    }
}

inline void Audio::ChordsManager::generateNotes(NoteEvents &notes) noexcept
{
    UNUSED(notes);
}

inline void Audio::ChordsManager::makeChord(const Key key, const ChordType type) noexcept
{
    UNUSED(key);
    UNUSED(type);
    const auto evt = _cache.actives.push(ChordSignature { key, type });

    UNUSED(evt);
}

inline void Audio::ChordsManager::releaseChord(const Key key) noexcept
{
    UNUSED(key);

}
