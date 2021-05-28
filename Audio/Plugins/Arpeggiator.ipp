/**
 * @file Arpeggiator.ipp
 * @brief
 *
 * @author Pierre V
 * @date 2021-05-21
 */

inline void Audio::Arpeggiator::onAudioGenerationStarted(const BeatRange &range)
{
    UNUSED(range);
    _cache.reset();
    _cache.setBeatRange(range);
    _cache.setNoteCount(static_cast<ArpeggiatorManager::NoteIndex>(noteCount()));
}

inline void Audio::Arpeggiator::sendNotes(const NoteEvents &notes, const BeatRange &range)
{
    UNUSED(notes);
    if (static_cast<bool>(byPass())) {
        _byPassCache = notes;
        return;
    }

    _cache.setBeatRange(range);
    _cache.setNoteCount(static_cast<ArpeggiatorManager::NoteIndex>(noteCount()));
    if (notes.size()) {
        // std::cout << range << std::endl;
        _cache.feedNotes(notes);
    }
}

inline void Audio::Arpeggiator::receiveNotes(NoteEvents &notes)
{
    UNUSED(notes);
    if (static_cast<bool>(byPass())) {
        notes.swap(_byPassCache);
        return;
    }
    const NoteType realNoteType = noteLength() ? static_cast<NoteType>(1u << static_cast<std::uint8_t>(noteLength())) : NoteType::WholeNote;
    _cache.generateNotes(notes, static_cast<ArpeggiatorManager::ArpMode>(arpeggiatorMode()), realNoteType);
}
