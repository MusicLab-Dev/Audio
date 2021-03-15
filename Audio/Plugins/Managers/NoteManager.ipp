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
            const auto it = std::find(_cache.actives.begin(), _cache.actives.end(), note.key);
            if (it == _cache.actives.end()) {
                _cache.actives.push(note.key);
                _cache.triggers[note.key] = true;
            }
            std::cout << "test: " << _cache.actives.size() << std::endl;
            target.noteModifiers.velocity = note.velocity;
            target.noteModifiers.tuning = note.tuning;
            break;
        }
        case NoteEvent::EventType::Off:
        {
            const auto it = std::find(_cache.actives.begin(), _cache.actives.end(), note.key);
            if (it != _cache.actives.end()) {
                _cache.actives.erase(it);
                _cache.triggers[note.key] = false;
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
    // std::cout << "::" << _cache.readIndexes[key] << std::endl;
    if (_cache.readIndexes[key] + amount >= maxIndex) {
        _cache.readIndexes[key] = 0u;
        // trigger = false;
    } else {
        // if (trigger)
            _cache.readIndexes[key] += amount;
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