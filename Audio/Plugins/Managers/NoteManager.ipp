/**
 * @ Author: Pierre Veysseyre
 * @ Description: NoteManager implementation
 */

#include <iostream>

template<Audio::DSP::EnveloppeType Enveloppe>
inline void Audio::NoteManager<Enveloppe>::feedNotes(const NoteEvents &notes) noexcept
{
    for (const auto &note : notes) {
        auto &target = _cache.modifiers[note.key];
        switch (note.type) {
        case NoteEvent::EventType::On:
        {
            const auto it = _cache.actives.find(note.key);
            if (it != _cache.actives.end()) // Reset trigger
                _cache.readIndexes[note.key] = 0u;
            else
                _cache.actives.push(note.key);
            _cache.triggers[note.key] = true;
            enveloppe().setTriggerIndex(note.key, 0u);
            enveloppe().resetGain(note.key);
            target.noteModifiers.velocity = note.velocity;
            target.noteModifiers.tuning = note.tuning;
            break;
        }
        case NoteEvent::EventType::Off:
        {
            const auto it = _cache.actives.find(note.key);
            if (it != _cache.actives.end()) {
                // Reset
                _cache.triggers[note.key] = false;
                enveloppe().setTriggerIndex(note.key, _cache.readIndexes[note.key]);
            }
        } break;
        case NoteEvent::EventType::OnOff:
            _cache.activesBlock.push(note.key);
            _cache.triggers[note.key] = true;
            enveloppe().setTriggerIndex(note.key, 0u);
            enveloppe().resetGain(note.key);
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

    readIndex += amount;
    if (maxIndex && readIndex >= maxIndex) {
        if (const auto it = _cache.actives.find(key); it != _cache.actives.end())
            _cache.actives.erase(it);
        readIndex = 0u;
        trigger = false;
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

template<Audio::DSP::EnveloppeType Enveloppe>
template<typename Functor>
void Audio::NoteManager<Enveloppe>::processNotes(Functor &&functor, const std::uint32_t processBlockSize) noexcept
{
    for (const auto key : _cache.actives) {
        const auto keyTrigger = trigger(key);
        const auto idx = readIndex(key);
        const auto maxIdx = functor(key, keyTrigger, idx);
        incrementReadIndex(key, maxIdx, processBlockSize);
    }
    for (const auto key : _cache.activesBlock) {
        auto &keyTrigger = _cache.triggers[key];
        const auto idx = readIndex(key);
        const auto maxIdx = functor(key, keyTrigger, idx);
        incrementReadIndex(key, maxIdx, processBlockSize);
        // Reset the note
        keyTrigger = false;
        enveloppe().setTriggerIndex(key, _cache.readIndexes[key]);
    }
    _cache.activesBlock.clear();
}