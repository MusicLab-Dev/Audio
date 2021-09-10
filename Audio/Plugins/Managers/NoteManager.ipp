/**
 * @ Author: Pierre Veysseyre
 * @ Description: NoteManager implementation
 */

#include <iostream>

template<Audio::DSP::EnvelopeType EnvelopeType>
inline void Audio::NoteManager<EnvelopeType>::feedNotes(const NoteEvents &notes) noexcept
{
    // std::cout << "NOTES> SEND NOTES " << notes.size() << std::endl;

    for (const auto &note : notes) {
        auto &target = _cache.modifiers[note.key];
        target.push(NoteCache {
            note.type,
            NoteModifiers {
                note.velocity,
                note.tuning,
                note.sampleOffset
            }
        });
        switch (note.type) {
        case NoteEvent::EventType::On:
        {
            const auto it = _cache.actives.find(note.key);
            if (it == _cache.actives.end())
                _cache.actives.push(note.key);
            // _cache.readIndexes[note.key] = 0u;
            // envelope().resetKey(note.key);
            // target.noteModifiers.velocity = note.velocity;
            // target.noteModifiers.tuning = note.tuning;
            // target.noteModifiers.sampleOffset = note.sampleOffset;
            break;
        }
        case NoteEvent::EventType::Off:
        {
            // const auto it = _cache.actives.find(note.key);
            // if (it != _cache.actives.end()) {
            //     // Reset
            //     envelope().setTriggerIndex(note.key, _cache.readIndexes[note.key] + note.sampleOffset);
            //     // envelope().setTriggerIndex(note.key, _cache.readIndexes[note.key]);
            //     // target.noteModifiers.sampleOffset = note.sampleOffset;
            // }
        } break;
        case NoteEvent::EventType::OnOff:
            // _cache.activesBlock.push(note.key);
            // _cache.readIndexes[note.key] = 0u;
            // envelope().resetTriggerIndex(note.key);
            // envelope().resetInternalGain(note.key);
            // // target.noteModifiers.velocity = note.velocity;
            // // target.noteModifiers.tuning = note.tuning;
            // // target.noteModifiers.sampleOffset = note.sampleOffset;
            // break;
        case NoteEvent::EventType::PolyPressure:
            // target.polyPressureModifiers.velocity = note.velocity;
            // target.polyPressureModifiers.tuning = note.tuning;
            // target.noteModifiers.sampleOffset = note.sampleOffset;
            break;
        default:
            break;
        };
    }
}

template<Audio::DSP::EnvelopeType EnvelopeType>
inline void Audio::NoteManager<EnvelopeType>::resetCache(void) noexcept
{
    _cache.actives.clear();
    resetBlockCache();
}

template<Audio::DSP::EnvelopeType EnvelopeType>
inline void Audio::NoteManager<EnvelopeType>::clearEnvelopeCache(void) noexcept
{
    _envelopeGain.clear();
}

template<Audio::DSP::EnvelopeType EnvelopeType>
inline void Audio::NoteManager<EnvelopeType>::resetBlockCache(void) noexcept
{
    for (const auto &note : _cache.activesBlock) {
        if (const auto it = _cache.actives.find(note); it != _cache.actives.end())
            _cache.activesBlock.erase(it);
    }
    _cache.activesBlock.clear();
}

template<Audio::DSP::EnvelopeType EnvelopeType>
inline void Audio::NoteManager<EnvelopeType>::resetReadIndexes(void) noexcept
{
    _cache.readIndexes.fill(0u);
}

template<Audio::DSP::EnvelopeType EnvelopeType>
inline void Audio::NoteManager<EnvelopeType>::resetNoteModifiers(void) noexcept
{
    for (auto &modifier : _cache.modifiers) {
        modifier.noteModifiers = NoteModifiers {
            0u, 0u
        };
    }
}

template<Audio::DSP::EnvelopeType EnvelopeType>
inline void Audio::NoteManager<EnvelopeType>::resetAllModifiers(void) noexcept
{
    for (auto &modifier : _cache.modifiers) {
        modifier.clear();
    }
}

template<Audio::DSP::EnvelopeType EnvelopeType>
template<typename ResetFunctor>
inline bool Audio::NoteManager<EnvelopeType>::incrementReadIndex(const Key key, const std::uint32_t maxIndex, std::uint32_t amount, ResetFunctor &&resetFunctor) noexcept
{
    auto &readIndex = _cache.readIndexes[key];

    // std::cout << "increment: " << readIndex << " -> " << readIndex + amount << std::endl;
    readIndex += amount;
    if ((maxIndex && readIndex >= maxIndex) || !envelope().lastGain(key)) {
        // std::cout << "RESET KEY !!!" << std::endl;
        readIndex = 0u;
        envelope().resetKey(key);
        resetFunctor(key);
        return true;
    } else
        return false;
}


template<Audio::DSP::EnvelopeType EnvelopeType>
inline void Audio::NoteManager<EnvelopeType>::generateEnvelopeGains(const Key key, const std::uint32_t index, const std::size_t outputSize) noexcept
{
    _envelopeGain.clear();
    _envelope.template generateGains<false, 0u>(key, index, _envelopeGain.data(), outputSize);
}

template<Audio::DSP::EnvelopeType EnvelopeType>
template<typename ProcessFunctor, typename ResetFunctor, bool ProcessEnvelope>
void Audio::NoteManager<EnvelopeType>::processNotesImpl(const std::uint32_t outSize, ProcessFunctor &&processFunctor, ResetFunctor &&resetFunctor) noexcept
{
    // Process the active notes
    auto itActive = std::remove_if(_cache.actives.begin(), _cache.actives.end(),
        [this, outSize, &processFunctor, &resetFunctor](const auto key) {
            // if (!_cache.modifiers[key].empty())
                // std::cout << "EVENTS: " << static_cast<std::size_t>(_cache.modifiers[key].size()) << std::endl;

            // auto realOutSize = outSize;
            bool ended = false;

            const auto evtCount = _cache.modifiers[key].size();
            // const auto evtCountMinusOne = evtCount - 1u;

            if (!evtCount) {
                // Process the active key (no event)
                const auto idx = readIndex(key);
                // std::cout << "___NoEvent Process: idx: " << idx << ", size: " << outSize << std::endl;
                if constexpr (ProcessEnvelope) {
                    _envelopeGain.clear();
                    generateEnvelopeGains(key, idx, outSize);
                }
                const auto pair = processFunctor(key, idx, NoteModifiers(), outSize);
                ended = incrementReadIndex(key, pair.second, pair.first, resetFunctor);
                // std::cout << "end: " << ended << ", " << pair.second << "; " << pair.first << std::endl;
            } else {
                // Process the active key with different events
                auto idx = readIndex(key);
                auto realOutSize = outSize;
                bool process = idx;
                auto i = 0u;
                NoteModifiers processModifiers {};
                for (; i < evtCount; ++i) {
                    auto &evt = _cache.modifiers[key][static_cast<std::uint8_t>(i)];
                    // const bool hasNextEvt = i < evtCountMinusOne;

                    if (evt.type == NoteEvent::EventType::On) {
                        if (process) {
                            realOutSize -= (outSize - evt.noteModifiers.sampleOffset);
                            if constexpr (ProcessEnvelope) {
                                _envelopeGain.clear();
                                generateEnvelopeGains(key, idx, realOutSize);
                            }
                            // std::cout << "process(BEGIN): " << idx << ", offset: " << processModifiers.sampleOffset << ", size: " << realOutSize << std::endl;
                            const auto pair = processFunctor(key, idx, processModifiers, realOutSize);
                            // evt.noteModifiers.sampleOffset = 0u;
                            ended = incrementReadIndex(key, pair.second, pair.first, resetFunctor);
                            // std::cout << "end: " << ended << ", " << pair.second << "; " << pair.first << std::endl;
                        }

                        // std::cout << "___ON " << evt.noteModifiers.sampleOffset << std::endl;
                        processModifiers = evt.noteModifiers;
                        envelope().template setTriggerIndex<0u>(key, 0u);
                        setReadIndex(key, 0u);
                        idx = readIndex(key);
                        realOutSize = outSize - evt.noteModifiers.sampleOffset;
                        process = true;
                    } else if (evt.type == NoteEvent::EventType::Off) {
                        const auto realOffset = evt.noteModifiers.sampleOffset - processModifiers.sampleOffset;
                        envelope().setTriggerIndex(key, _cache.readIndexes[key] + realOffset);
                        // std::cout << "___OFF idx:" << _cache.readIndexes[key] << ", offset" << evt.noteModifiers.sampleOffset << std::endl;
                        // std::cout << "_____setTriggerIndex " << _cache.readIndexes[key] + evt.noteModifiers.sampleOffset << std::endl;
                        // process = envelope().lastGain(key);
                    }
                }
                //
                if (process) {
                    // std::cout << "process(END): " << idx << ", offset: " << processModifiers.sampleOffset << ", size: " << realOutSize << std::endl;
                    if constexpr (ProcessEnvelope) {
                        _envelopeGain.clear();
                        generateEnvelopeGains(key, idx, realOutSize);
                    }
                    const auto pair = processFunctor(key, idx, processModifiers, realOutSize);
                    // evt.noteModifiers.sampleOffset = 0u;
                    ended = incrementReadIndex(key, pair.second, pair.first, resetFunctor);
                    // std::cout << "end: " << ended << ", " << pair.second << "; " << pair.first << std::endl;
                }
            }
            // _cache.modifiers[key].erase(_cache.modifiers[key].begin(), evtCountMinusOne);
            _cache.modifiers[key].clear();

            // auto &modifiers = _cache.modifiers[key][0u].noteModifiers;
            // const auto realOutSize = (modifiers.sampleOffset && !idx) ? outSize - modifiers.sampleOffset : outSize;
            // Generate envelope for the given key
            // generateEnvelopeGains(key, idx, realOutSize);

            // Compute the processing functor
            // const auto pair = processFunctor(key, idx, modifiers, realOutSize);
            // modifiers.sampleOffset = 0u;

            // Increment internal read index (may remove the key of active list)

            return ended;
        }
    );
    // Actually delete the concerned notes
    if (itActive != _cache.actives.end())
        _cache.actives.erase(itActive, _cache.actives.end());

    // Process the active block notes
    auto itActiveBlock = std::remove_if(_cache.activesBlock.begin(), _cache.activesBlock.end(),
        [this, outSize, &processFunctor, &resetFunctor](const auto key) {
            const auto idx = readIndex(key);
            auto &modifiers = _cache.modifiers[key][0u].noteModifiers;
            const auto realOutSize = (modifiers.sampleOffset && !idx) ? outSize - modifiers.sampleOffset : outSize;
            generateEnvelopeGains(key, idx, realOutSize);
            const auto pair = processFunctor(key, idx, modifiers, realOutSize);
            modifiers.sampleOffset = 0u;
            const bool ended = incrementReadIndex(key, pair.second, pair.first, resetFunctor);

            // Reset the note
            if (!ended) {
                envelope().setTriggerIndex(key, _cache.readIndexes[key]);
            }
            return ended;
        }
    );
    // Actually delete the concerned notes
    if (itActiveBlock != _cache.activesBlock.end())
        _cache.activesBlock.erase(itActiveBlock, _cache.activesBlock.end());
}
