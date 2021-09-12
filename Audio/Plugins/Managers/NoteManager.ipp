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
bool Audio::NoteManager<EnvelopeType>::processOneNoteImpl(
        ProcessFunctor &&processFunctor, ResetFunctor &&resetFunctor,
        const Key key, const std::uint32_t readIndex, const NoteModifiers &modifiers,
        float *realOutput, const std::uint32_t realOutSize, const std::size_t channelCount
) noexcept
{
    if constexpr (ProcessEnvelope) {
        _envelopeGain.clear();
        generateEnvelopeGains(key, readIndex, realOutSize);
    }
    if (modifiers.sampleOffset && !readIndex) {
        realOutput += modifiers.sampleOffset * channelCount;
    }
    const auto pair = processFunctor(key, readIndex, modifiers, realOutput, realOutSize, channelCount);
    return incrementReadIndex(key, pair.second, pair.first, resetFunctor);
}

template<Audio::DSP::EnvelopeType EnvelopeType>
template<typename ProcessFunctor, typename ResetFunctor, bool ProcessEnvelope>
void Audio::NoteManager<EnvelopeType>::processNotesImpl(BufferView outputFrame, ProcessFunctor &&processFunctor, ResetFunctor &&resetFunctor) noexcept
{
    const auto outSize = static_cast<std::uint32_t>(outputFrame.size<float>());
    const auto channelCount = static_cast<std::size_t>(outputFrame.channelArrangement());
    float *realOutput = outputFrame.data<float>();

    // Process the active notes
    auto itActive = std::remove_if(_cache.actives.begin(), _cache.actives.end(),
        [this, realOutput, outSize, channelCount, &processFunctor, &resetFunctor] (const auto key)
        {
            // std::cout << "EVENTS: " << static_cast<std::size_t>(_cache.modifiers[key].size()) << std::endl;
            const auto evtCount = _cache.modifiers[key].size();
            bool ended = false;

            if (!evtCount) {
                // Process the active key (no event)
                const auto idx = readIndex(key);
                // std::cout << "___NoEvent Process: idx: " << idx << ", size: " << outSize << std::endl;
                ended = processOneNoteImpl<ProcessFunctor, ResetFunctor, ProcessEnvelope>(std::move(processFunctor), std::move(resetFunctor), key, idx, NoteModifiers(), realOutput, outSize, channelCount);
                // if constexpr (ProcessEnvelope) {
                //     _envelopeGain.clear();
                //     generateEnvelopeGains(key, idx, outSize);
                // }
                // const auto pair = processFunctor(key, idx, NoteModifiers(), outSize);
                // ended = incrementReadIndex(key, pair.second, pair.first, resetFunctor);
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

                    if (evt.type == NoteEvent::EventType::On) {
                        if (process) {
                            realOutSize -= (outSize - evt.noteModifiers.sampleOffset);
                            // std::cout << "process(BEGIN): " << idx << ", offset: " << processModifiers.sampleOffset << ", size: " << realOutSize << std::endl;
                            ended = processOneNoteImpl<ProcessFunctor, ResetFunctor, ProcessEnvelope>(std::move(processFunctor), std::move(resetFunctor), key, idx, processModifiers, realOutput, realOutSize, channelCount);
                            // if constexpr (ProcessEnvelope) {
                            //     _envelopeGain.clear();
                            //     generateEnvelopeGains(key, idx, realOutSize);
                            // }
                            // const auto pair = processFunctor(key, idx, processModifiers, realOutSize);
                            // ended = incrementReadIndex(key, pair.second, pair.first, resetFunctor);
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
                    }
                }
                //
                if (process) {
                    // std::cout << "process(END): " << idx << ", offset: " << processModifiers.sampleOffset << ", size: " << realOutSize << std::endl;
                    ended = processOneNoteImpl<ProcessFunctor, ResetFunctor, ProcessEnvelope>(std::move(processFunctor), std::move(resetFunctor), key, idx, processModifiers, realOutput, realOutSize, channelCount);
                    // if constexpr (ProcessEnvelope) {
                    //     _envelopeGain.clear();
                    //     generateEnvelopeGains(key, idx, realOutSize);
                    // }
                    // const auto pair = processFunctor(key, idx, processModifiers, realOutSize);
                    // ended = incrementReadIndex(key, pair.second, pair.first, resetFunctor);
                    // // std::cout << "end: " << ended << ", " << pair.second << "; " << pair.first << std::endl;
                }
            }
            _cache.modifiers[key].clear();
            return ended;
        }
    );
    // Actually delete the concerned notes
    if (itActive != _cache.actives.end())
        _cache.actives.erase(itActive, _cache.actives.end());
}
