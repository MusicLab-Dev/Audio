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
            if (it == _cache.actives.end())
                _cache.actives.push(note.key);
            _cache.readIndexes[note.key] = 0u;
            _cache.triggers[note.key] = true;
            enveloppe().resetTriggerIndex(note.key);
            enveloppe().resetGain(note.key);
            target.noteModifiers.velocity = note.velocity;
            target.noteModifiers.tuning = note.tuning;
            target.noteModifiers.sampleOffset = note.sampleOffset;
            break;
        }
        case NoteEvent::EventType::Off:
        {
            const auto it = _cache.actives.find(note.key);
            if (it != _cache.actives.end()) {
                // Reset
                _cache.triggers[note.key] = false;
                enveloppe().setTriggerIndex(note.key, _cache.readIndexes[note.key]);
                target.noteModifiers.sampleOffset = note.sampleOffset;
            }
        } break;
        case NoteEvent::EventType::OnOff:
            _cache.activesBlock.push(note.key);
            _cache.triggers[note.key] = true;
            _cache.readIndexes[note.key] = 0u;
            enveloppe().resetTriggerIndex(note.key);
            enveloppe().resetGain(note.key);
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
inline bool Audio::NoteManager<Enveloppe>::incrementReadIndex(const Key key, const std::uint32_t maxIndex, std::uint32_t amount) noexcept
{
    auto &trigger = _cache.triggers[key];
    auto &readIndex = _cache.readIndexes[key];

    readIndex += amount;
    if (maxIndex && readIndex >= maxIndex) {
        readIndex = 0u;
        trigger = false;
        enveloppe().resetTriggerIndex(key);
        return true;
    } else
        return false;
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
void Audio::NoteManager<Enveloppe>::processNotes(Functor &&functor) noexcept
{
    // Process the active notes
    auto itActive = std::remove_if(_cache.actives.begin(), _cache.actives.end(),
        [this, &functor](const auto key) {
            const auto keyTrigger = trigger(key);
            const auto idx = readIndex(key);
            auto &modifiers = _cache.modifiers[key].noteModifiers;
            const auto pair = functor(key, keyTrigger, idx, modifiers);
            modifiers.sampleOffset = 0u;
            const bool ended = incrementReadIndex(key, pair.second, pair.first);

            // Erase the note if it's an oscillator (pair.second == 0u) & the enveloppe gain is null
            // std::cout << "ITER" << std::endl;
            return ended || (!pair.second && !enveloppe().lastGain(key));
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
            keyTrigger = false;
            enveloppe().setTriggerIndex(key, _cache.readIndexes[key]);

            // std::cout << "ITER" << std::endl;
            // Erase the note if it's an oscillator (pair.second == 0u) & the enveloppe gain is null
            return ended || (!pair.second && !enveloppe().lastGain(key));
        }
    );
    // Actually delete the concerned notes
    if (itActiveBlock != _cache.activesBlock.end())
        _cache.activesBlock.erase(itActiveBlock, _cache.activesBlock.end());
}
