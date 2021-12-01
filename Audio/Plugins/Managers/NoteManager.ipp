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
        if (note.type == NoteEvent::EventType::On) {
            if (const auto it = _cache.actives.find(note.key); it == _cache.actives.end())
                _cache.actives.push(note.key);
        }
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
template<typename TestFunctor, typename ResetFunctor>
inline bool Audio::NoteManager<EnvelopeType>::incrementReadIndex(const Key key, const std::uint32_t maxIndex, std::uint32_t amount, TestFunctor &&testFunctor, ResetFunctor &&resetFunctor) noexcept
{
    auto &readIndex = _cache.readIndexes[key];

    // std::cout << "increment: " << readIndex << " -> " << readIndex + amount << std::endl;
    readIndex += amount;
    if ((maxIndex && readIndex >= maxIndex) || testFunctor(key)) {
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
template<bool ProcessEnvelope, bool Retrigger, typename ProcessFunctor, typename TestFunctor, typename ResetFunctor>
bool Audio::NoteManager<EnvelopeType>::processOneNoteImpl(
        ProcessFunctor &&processFunctor, TestFunctor &&testFunctor, ResetFunctor &&resetFunctor,
        const Key key, const std::uint32_t readIndex, const NoteModifiers &modifiers,
        float *realOutput, const std::uint32_t realOutSize, const std::size_t channelCount
) noexcept
{
    // std::cout << "\t+1" << std::endl;
    if constexpr (ProcessEnvelope) {
        _envelopeGain.clear();
        generateEnvelopeGains(key, readIndex, realOutSize);
    }
    const auto isStart = !readIndex;
    realOutput += modifiers.sampleOffset * isStart;

    // std::cout << "process:" << std::endl;
    // std::cout << "\t readIndex: " << readIndex << std::endl;
    // std::cout << "\t modifiers.sampleOffset: " << modifiers.sampleOffset << std::endl;
    // std::cout << "\t realOutSize: " << realOutSize << std::endl;
    // std::cout << "\t realOutput: " << realOutput << std::endl;
    // std::cout << std::endl << std::endl;

    if constexpr (Retrigger) {
        if (isStart)
            resetFunctor(key);
    }

    const auto pair = processFunctor(key, readIndex, modifiers, realOutput, realOutSize, channelCount);
    return incrementReadIndex(key, pair.second, pair.first, testFunctor, resetFunctor);
}

template<Audio::DSP::EnvelopeType EnvelopeType>
template<bool ProcessEnvelope, bool Retrigger, typename ProcessFunctor, typename TestFunctor, typename ResetFunctor>
void Audio::NoteManager<EnvelopeType>::processNotesImpl(BufferView outputFrame, ProcessFunctor &&processFunctor, TestFunctor &&testFunctor, ResetFunctor &&resetFunctor) noexcept
{
    const auto outSize = static_cast<std::uint32_t>(outputFrame.channelSampleCount());
    const auto channelCount = static_cast<std::size_t>(outputFrame.channelArrangement());
    float *realOutput = outputFrame.data<float>();

    // Process the active notes
    auto itActive = std::remove_if(_cache.actives.begin(), _cache.actives.end(),
        [this, realOutput, outSize, channelCount, &processFunctor, &testFunctor, &resetFunctor] (const auto key)
        {
            const auto evtCount = _cache.modifiers[key].size();
            bool ended = false;

            if (!evtCount) {
                // Process the active key (no event)
                // std::cout << "No event" << std::endl;
                const auto idx = readIndex(key);
                ended = processOneNoteImpl<ProcessEnvelope, Retrigger>(processFunctor, testFunctor, resetFunctor, key, idx, NoteModifiers(), realOutput, outSize, channelCount);
            } else {
                // Process the active key with different events
                auto idx = readIndex(key);
                auto realOutSize = outSize;
                bool process = idx;
                auto i = 0u;
                NoteModifiers processModifiers {};

                for (; i < evtCount; ++i) {
                    auto &evt = _cache.modifiers[key][static_cast<std::uint8_t>(i)];

                    // std::cout << "1 event - " << static_cast<std::size_t>(evt.type) << std::endl;

                    if (evt.type == NoteEvent::EventType::On) {
                        // std::cout << "<ON>" << std::endl;
                        if (process) {
                            realOutSize -= (outSize - evt.noteModifiers.sampleOffset);
                            // std::cout << "<ON-active>" << std::endl;
                            ended = processOneNoteImpl<ProcessEnvelope, Retrigger>(processFunctor, testFunctor, resetFunctor, key, idx, processModifiers, realOutput, realOutSize, channelCount);
                        }
                        processModifiers = evt.noteModifiers;
                        envelope().template setTriggerIndex<0u>(key, 0u);
                        setReadIndex(key, 0u);
                        idx = readIndex(key);
                        realOutSize = outSize - evt.noteModifiers.sampleOffset;
                        process = true;
                    } else if (evt.type == NoteEvent::EventType::Off) {
                        // std::cout << "<OFF>" << std::endl;
                        const auto realOffset = evt.noteModifiers.sampleOffset - processModifiers.sampleOffset;
                        envelope().setTriggerIndex(key, _cache.readIndexes[key] + realOffset);
                    }
                }
                if (process) {
                    ended = processOneNoteImpl<ProcessEnvelope, Retrigger>(processFunctor, testFunctor, resetFunctor, key, idx, processModifiers, realOutput, realOutSize, channelCount);
                }
            }
            _cache.modifiers[key].clear();
            return ended;
        }
    );
    // std::cout << "##############" << std::endl;
    // Actually delete the concerned notes
    if (itActive != _cache.actives.end())
        _cache.actives.erase(itActive, _cache.actives.end());
}
