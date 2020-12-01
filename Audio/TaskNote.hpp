/**
 * @ Author: Pierre Veysseyre
 * @ Description: TaskNote
 */

#pragma once

#include "TaskBase.hpp"
#include "Note.hpp"

namespace Audio
{
    class TaskNoteRead;
    class TaskNoteWrite;

    using NoteStack = std::unique_ptr<NoteEvents>;
};

class Audio::TaskNoteRead : public Audio::Internal::TaskBase
{
public:
    void operator()(void) noexcept {
        std::cout << "Note read\n";
    }

private:
    NoteStack _noteCache;
};

class Audio::TaskNoteWrite : public Audio::Internal::TaskBase
{
public:
    void operator()(void) noexcept {
        std::cout << "Note write\n";
    }

private:
    NoteStack _noteCache;
};
