/**
 * @ Author: Pierre Veysseyre
 * @ Description: NoteManager implementation
 */

#include <iostream>

template<Audio::DSP::EnvelopeType Envelope>
inline void Audio::NoteManager<Envelope>::feedNotes(const NoteEvents &notes) noexcept
{
    // std::cout << "NOTES> SEND NOTES " << notes.size() << std::endl;

    for (const auto &note : notes) {
        // std::cout << "##" << static_cast<std::size_t>(note.key) << ": " << static_cast<std::size_t>(note.type) << " - " << note.sampleOffset << std::endl;
        auto &target = _cache.modifiers[note.key];
        switch (note.type) {
        case NoteEvent::EventType::On:
        {
            const auto it = _cache.actives.find(note.key);
            if (it == _cache.actives.end())
                _cache.actives.push(note.key);
            _cache.readIndexes[note.key] = 0u;
            _cache.triggers[note.key] = true;
            enveloppe().resetKey(note.key);
            target.noteModifiers.velocity = note.velocity;
            target.noteModifiers.tuning = note.tuning;
            target.noteModifiers.sampleOffset = note.sampleOffset;
            break;
        }
        case NoteEvent::EventType::Off:
        {
            const auto it = _cache.actives.find(note.key);
            if (it != _cache.actives.end() && _cache.triggers[note.key]) {
                // Reset
                _cache.triggers[note.key] = false;
                enveloppe().setTriggerIndex(note.key, _cache.readIndexes[note.key] + note.sampleOffset);
                // enveloppe().setTriggerIndex(note.key, _cache.readIndexes[note.key]);
                target.noteModifiers.sampleOffset = note.sampleOffset;
            }
        } break;
        case NoteEvent::EventType::OnOff:
            _cache.activesBlock.push(note.key);
            _cache.triggers[note.key] = true;
            _cache.readIndexes[note.key] = 0u;
            enveloppe().resetTriggerIndex(note.key);
            enveloppe().resetInternalGain(note.key);
            target.noteModifiers.velocity = note.velocity;
            target.noteModifiers.tuning = note.tuning;
            target.noteModifiers.sampleOffset = note.sampleOffset;
            break;
        case NoteEvent::EventType::PolyPressure:
            target.polyPressureModifiers.velocity = note.velocity;
            target.polyPressureModifiers.tuning = note.tuning;
            target.noteModifiers.sampleOffset = note.sampleOffset;
            break;
        default:
            break;
        };
    }
}

template<Audio::DSP::EnvelopeType Envelope>
inline void Audio::NoteManager<Envelope>::resetCache(void) noexcept
{
    _cache.actives.clear();
    resetBlockCache();
}

template<Audio::DSP::EnvelopeType Envelope>
inline void Audio::NoteManager<Envelope>::resetBlockCache(void) noexcept
{
    for (const auto &note : _cache.activesBlock) {
        if (const auto it = _cache.actives.find(note); it != _cache.actives.end())
            _cache.activesBlock.erase(it);
    }
    _cache.activesBlock.clear();
}

template<Audio::DSP::EnvelopeType Envelope>
inline void Audio::NoteManager<Envelope>::resetReadIndexes(void) noexcept
{
    _cache.readIndexes.fill(0u);
}

template<Audio::DSP::EnvelopeType Envelope>
inline void Audio::NoteManager<Envelope>::resetNoteModifiers(void) noexcept
{
    for (auto &modifier : _cache.modifiers) {
        modifier.noteModifiers = NoteModifiers {
            0u, 0u
        };
    }
}

template<Audio::DSP::EnvelopeType Envelope>
inline void Audio::NoteManager<Envelope>::resetPolyModifiers(void) noexcept
{
    for (auto &modifier : _cache.modifiers) {
        modifier.polyPressureModifiers = NoteModifiers {
            0u, 0u
        };
    }
}

template<Audio::DSP::EnvelopeType Envelope>
inline void Audio::NoteManager<Envelope>::resetAllModifiers(void) noexcept
{
    _cache.modifiers.fill(NoteCache {
        NoteModifiers { 0u, 0u },
        NoteModifiers { 0u, 0u }
    });
}

template<Audio::DSP::EnvelopeType Envelope>
inline bool Audio::NoteManager<Envelope>::incrementReadIndex(const Key key, const std::uint32_t maxIndex, std::uint32_t amount) noexcept
{
    auto &trigger = _cache.triggers[key];
    auto &readIndex = _cache.readIndexes[key];

    readIndex += amount;
    if ((maxIndex && readIndex >= maxIndex) || !enveloppe().lastGain(key)) {
        readIndex = 0u;
        trigger = false;
        enveloppe().resetKey(key);
        return true;
    } else
        return false;
}

template<Audio::DSP::EnvelopeType Envelope>
inline void Audio::NoteManager<Envelope>::resetTriggers(void) noexcept
{
    _cache.triggers.fill(false);
}

template<Audio::DSP::EnvelopeType Envelope>
inline bool Audio::NoteManager<Envelope>::setTrigger(const Key key, const bool state) noexcept
{
    if (state == _cache.triggers[key])
        return false;
    _cache.triggers[key] = state;
    return true;
}

template<Audio::DSP::EnvelopeType Envelope>
template<typename Functor>
void Audio::NoteManager<Envelope>::processNotes(Functor &&functor) noexcept
{
    // Process the active notes
    auto itActive = std::remove_if(_cache.actives.begin(), _cache.actives.end(),
        [this, &functor](const auto key) {
            const auto keyTrigger = trigger(key);
            const auto idx = readIndex(key);
            auto &modifiers = _cache.modifiers[key].noteModifiers;
            const auto pair = functor(key, keyTrigger, idx, modifiers);
            modifiers.sampleOffset = 0u;
            return incrementReadIndex(key, pair.second, pair.first);
        }
    );
    // Actually delete the concerned notes
    if (itActive != _cache.actives.end())
        _cache.actives.erase(itActive, _cache.actives.end());

    // Process the active block notes
    auto itActiveBlock = std::remove_if(_cache.activesBlock.begin(), _cache.activesBlock.end(),
        [this, &functor](const auto key) {
            auto &keyTrigger = _cache.triggers[key];
            const auto idx = readIndex(key);
            auto &modifiers = _cache.modifiers[key].noteModifiers;
            const auto pair = functor(key, keyTrigger, idx, modifiers);
            modifiers.sampleOffset = 0u;
            const bool ended = incrementReadIndex(key, pair.second, pair.first);

            // Reset the note
            if (!ended) {
                keyTrigger = false;
                enveloppe().setTriggerIndex(key, _cache.readIndexes[key]);
            }
            return ended;
        }
    );
    // Actually delete the concerned notes
    if (itActiveBlock != _cache.activesBlock.end())
        _cache.activesBlock.erase(itActiveBlock, _cache.activesBlock.end());
}
