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
    class TaskSchedulerNoteRead;
    class TaskSchedulerNoteWrite;

    using NoteStack = std::unique_ptr<NoteEvents>;
};

class Audio::TaskNoteRead : public Audio::TaskBase
{
public:
    TaskNoteRead(Node *node) : TaskBase(node) {}

    void operator()(void) noexcept { std::cout << "Note read\n"; }

private:
    NoteStack _noteCache;
};

class Audio::TaskNoteWrite : public Audio::TaskBase
{
public:
    TaskNoteWrite(Node *node) : TaskBase(node) {}

    void operator()(void) noexcept { std::cout << "Note write\n"; }

private:
    NoteStack _noteCache;
};

class Audio::TaskSchedulerNoteRead : public Audio::TaskBaseScheduler
{
public:
    TaskSchedulerNoteRead(Node *node, const AScheduler *scheduler)
        : TaskBaseScheduler(node, scheduler) {}

    void operator()(void) noexcept { std::cout << "(Scheduler) Note read\n"; }

private:
    NoteStack _noteCache;
};

class Audio::TaskSchedulerNoteWrite : public Audio::TaskBaseScheduler
{
public:
    TaskSchedulerNoteWrite(Node *node, const AScheduler *scheduler)
        : TaskBaseScheduler(node, scheduler) {}

    void operator()(void) noexcept {
        std::cout << "(Scheduler) Note write\n";
        node().plugin()->receiveNotes(*_noteCache);
    }

private:
    NoteStack _noteCache;
};