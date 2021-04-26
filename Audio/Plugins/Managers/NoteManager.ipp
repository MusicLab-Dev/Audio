/**
 * @ Author: Pierre Veysseyre
 * @ Description: NoteManager implementation
 */

#include <iostream>

template<Audio::DSP::EnveloppeType Enveloppe>
inline void Audio::NoteManager<Enveloppe>::feedNotes(const NoteEvents &notes) noexcept
{
    static constexpr auto Debug = false;
    if constexpr (Debug) {
        if (const auto notesSize = notes.size(); notesSize)
            std::cout << "<<Process " << std::to_string(notesSize) << " notes>>" << std::endl;
    }
    for (const auto &note : notes) {
        auto &target = _cache.modifiers[note.key];
        if constexpr (Debug)
            std::cout << ":: " << note << std::endl;
        switch (note.type) {
        case NoteEvent::EventType::On:
        {
            if constexpr (Debug)
                std::cout << "---ON: " << _cache.actives.size() << std::endl;
            const auto it = _cache.actives.find(note.key);
            if (it == _cache.actives.end()) {
                if constexpr (Debug)
                    std::cout << "  \\__push" << std::endl;
                _cache.actives.push(note.key);
                _cache.triggers[note.key] = true;
                enveloppe().setTriggerIndex(note.key, 0u);
                enveloppe().resetGain(note.key);
            } else {
                if constexpr (Debug)
                    std::cout << "  \\__reset" << std::endl;
                // Reset trigger
                _cache.triggers[note.key] = true;
                _cache.readIndexes[note.key] = 0u;
                enveloppe().setTriggerIndex(note.key, 0u);
                enveloppe().resetGain(note.key);
            }
            target.noteModifiers.velocity = note.velocity;
            target.noteModifiers.tuning = note.tuning;
            break;
        }
        case NoteEvent::EventType::Off:
        {
            if constexpr (Debug)
                std::cout << "---OFF: " << _cache.actives.size() << std::endl;
            const auto it = _cache.actives.find(note.key);
            if (it != _cache.actives.end()) {
                if constexpr (Debug)
                    std::cout << "  \\__reset trigger off" << std::endl;
                // Reset
                _cache.triggers[note.key] = false;
                // _cache.readIndexes[note.key] = 0u;
                enveloppe().setTriggerIndex(note.key, _cache.readIndexes[note.key]);
            }
        } break;
        case NoteEvent::EventType::OnOff:
            if constexpr (Debug)
                std::cout << "---ON & OFF: " << _cache.actives.size() << std::endl;
            // _cache.actives.push(note.key);
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
    if constexpr (Debug)
        std::cout << "__actives: " << _cache.actives.size() << std::endl;
}

template<Audio::DSP::EnveloppeType Enveloppe>
inline void Audio::NoteManager<Enveloppe>::resetCache(void) noexcept
{
    _cache.actives.clear();
    resetBlockCache();
}

template<Audio::DSP::EnveloppeType Enveloppe>
inline void Audio::NoteManager<Enveloppe>::resetBlockCache(void) noexcept
{
    for (const auto &note : _cache.activesBlock) {
        if (const auto it = _cache.actives.find(note); it != _cache.actives.end())
            _cache.activesBlock.erase(it);
    }
    _cache.activesBlock.clear();
}

template<Audio::DSP::EnveloppeType Enveloppe>
inline void Audio::NoteManager<Enveloppe>::resetReadIndexes(void) noexcept
{
    _cache.readIndexes.fill(0u);
}

template<Audio::DSP::EnveloppeType Enveloppe>
inline void Audio::NoteManager<Enveloppe>::resetNoteModifiers(void) noexcept
{
    for (auto &modifier : _cache.modifiers) {
        modifier.noteModifiers = NoteModifiers {
            0u, 0u
        };
    }
}

template<Audio::DSP::EnveloppeType Enveloppe>
inline void Audio::NoteManager<Enveloppe>::resetPolyModifiers(void) noexcept
{
    for (auto &modifier : _cache.modifiers) {
        modifier.polyPressureModifiers = NoteModifiers {
            0u, 0u
        };
    }
}

template<Audio::DSP::EnveloppeType Enveloppe>
inline void Audio::NoteManager<Enveloppe>::resetAllModifiers(void) noexcept
{
    _cache.modifiers.fill(NoteCache {
        NoteModifiers { 0u, 0u },
        NoteModifiers { 0u, 0u }
    });
}

template<Audio::DSP::EnveloppeType Enveloppe>
inline void Audio::NoteManager<Enveloppe>::incrementReadIndex(const Key key, const std::uint32_t maxIndex, std::uint32_t amount) noexcept
{
    auto &trigger = _cache.triggers[key];
    auto &readIndex = _cache.readIndexes[key];

    // if (!trigger)
    //     return;

    readIndex += amount;
    // std::cout << "  ::" << _cache.readIndexes[key] << std::endl;
    if (maxIndex && readIndex >= maxIndex) {
        // std::cout << "  :: RESET" << std::endl;
        if (const auto it = _cache.actives.find(key); it != _cache.actives.end())
            _cache.actives.erase(it);
        readIndex = 0u;
        trigger = false;
        // std::cout << "  :: RESET" << std::endl;
     }
}

template<Audio::DSP::EnveloppeType Enveloppe>
inline void Audio::NoteManager<Enveloppe>::resetTriggers(void) noexcept
{
    _cache.triggers.fill(false);
}

template<Audio::DSP::EnveloppeType Enveloppe>
inline bool Audio::NoteManager<Enveloppe>::setTrigger(const Key key, const bool state) noexcept
{
    if (state == _cache.triggers[key])
        return false;
    _cache.triggers[key] = state;
    return true;
}
