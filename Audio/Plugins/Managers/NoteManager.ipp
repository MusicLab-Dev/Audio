/**
 * @ Author: Pierre Veysseyre
 * @ Description: NoteManager implementation
 */

#include <iostream>

inline void Audio::NoteManager::feedNotes(const NoteEvents &notes) noexcept
{
    for (const auto &note : notes) {
        auto &target = _cache.modifiers[note.key];
        switch (note.type) {
        case NoteEvent::EventType::On:
        {
            // std::cout << "ON: " << _cache.actives.size() << std::endl;
            const auto it = std::find(_cache.actives.begin(), _cache.actives.end(), note.key);
            if (it == _cache.actives.end()) {
                // std::cout << "___\n";
                _cache.actives.push(note.key);
                _cache.triggers[note.key] = true;
            }
            target.noteModifiers.velocity = note.velocity;
            target.noteModifiers.tuning = note.tuning;
            break;
        }
        case NoteEvent::EventType::Off:
        {
            // std::cout << "OFF: " << _cache.actives.size() << std::endl;
            const auto it = std::find(_cache.actives.begin(), _cache.actives.end(), note.key);
            if (it != _cache.actives.end()) {
                // std::cout << "___\n";
                _cache.actives.erase(it);
                _cache.triggers[note.key] = false;
                // Reset
                _cache.readIndexes[note.key] = 0u;
            }
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

inline void Audio::NoteManager::resetReadIndexes(void) noexcept
{
    _cache.readIndexes.fill(0u);
}

inline void Audio::NoteManager::resetNoteModifiers(void) noexcept
{
    for (auto &modifier : _cache.modifiers) {
        modifier.noteModifiers = NoteModifiers {
            0u, 0u
        };
    }
}

inline void Audio::NoteManager::resetPolyModifiers(void) noexcept
{
    for (auto &modifier : _cache.modifiers) {
        modifier.polyPressureModifiers = NoteModifiers {
            0u, 0u
        };
    }
}

inline void Audio::NoteManager::resetAllModifiers(void) noexcept
{
    _cache.modifiers.fill(NoteCache {
        NoteModifiers { 0u, 0u },
        NoteModifiers { 0u, 0u }
    });
}

inline void Audio::NoteManager::incrementReadIndex(const Key key, const std::size_t maxIndex, std::size_t amount) noexcept
{
    auto &trigger = _cache.triggers[key];
    auto &readIndex = _cache.readIndexes[key];

    if (!trigger)
        return;

    readIndex += amount;
    // std::cout << "::" << _cache.readIndexes[key] << std::endl;
    if (readIndex >= maxIndex) {
        readIndex = 0u;
        trigger = false;
    }
}

inline void Audio::NoteManager::resetTriggers(void) noexcept
{
    _cache.triggers.fill(false);
}

inline bool Audio::NoteManager::setTrigger(const Key key, const bool state) noexcept
{
    if (state == _cache.triggers[key])
        return false;
    _cache.triggers[key] = state;
    return true;
}