/**
 * @ Author: Pierre Veysseyre
 * @ Description: TaskNote
 */

#pragma once

#include "TaskBase.hpp"
#include "Note.hpp"

namespace Audio
{
    template<bool HasNoteInput, bool HasNoteOutput>
    class SchedulerTask;
};

template<bool HasNoteInput, bool HasNoteOutput>
class Audio::SchedulerTask : public Audio::Internal::TaskBase
{
public:
    // SchedulerTask(...)

    void operator()(void) noexcept {
        *_noteStack = *_parentNoteStack;
        const auto beatRange = scheduler().currentBeatRange();
        collectNotes(beatRange, node().partitions(), *_noteStack);
        if constexpr (HasNoteInput) {
            node().plugin()->sendNotes(*_noteStack);
            _noteStack->clear();
        }
        if constexpr (HasNoteOutput) {
            node().plugin()->receiveNotes(*_noteStack);
        }
    }

private:
    std::unique_ptr<NoteEvents> _noteStack { std::make_unique<NoteEvents>() };
    const NoteEvents *_parentNoteStack { nullptr };
    BufferViews _bufferStack {};

    void collectNotes(const BeatRange &beatRange, const Partitions &partitions, NoteEvents &output) noexcept
    {
        //
    }
};

// class Audio::TaskNoteWrite : public Audio::Internal::TaskBase
// {
// public:
//     void operator()(void) noexcept {
//         for (auto &child : node().children()) {
//             child->plugin()->receiveNotes();
//         }
//         std::cout << "Note write\n";
//     }

// private:
//     NoteStack _noteCache;
// };
