/**
 * @ Author: Pierre Veysseyre
 * @ Description: NoteManager implementation
 */

inline void Audio::NoteManager::feedNotes(const NoteEvents &notes) noexcept
{
    for (const auto &note : notes) {
        auto &target = _cache.notes[note.key];
        switch (note.type) {
        case NoteEvent::EventType::On:
            _cache.actives.push(note.key);
            target.noteModifiers.velocity = note.velocity;
            target.noteModifiers.tuning = note.tuning;
            break;
        case NoteEvent::EventType::Off: {
            const auto it = std::find(_cache.actives.begin(), _cache.actives.end(), note.key);
            if (it != _cache.actives.end())
                _cache.actives.erase(it);
        } break;
        case NoteEvent::EventType::OnOff:
            _cache.activesBlock.push(note.key);
            target.noteModifiers.velocity = note.velocity;
            target.noteModifiers.tuning = note.tuning;
            break;
        case NoteEvent::EventType::PolyPressure:
            target.polyPressureModifiers.velocity = note.velocity;
            target.polyPressureModifiers.tuning = note.tuning;
            break;
        default:
            break;
        };
    }
}

inline void Audio::NoteManager::resetCache(void) noexcept
{
    _cache.actives.clear();
    resetBlockCache();
}

inline void Audio::NoteManager::resetBlockCache(void) noexcept
{
    _cache.activesBlock.clear();
}
