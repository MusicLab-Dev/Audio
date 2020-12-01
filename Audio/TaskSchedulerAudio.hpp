/**
 * @ Author: Pierre Veysseyre
 * @ Description: TaskAudio
 */

#pragma once

#include "TaskAudio.hpp"

namespace Audio
{
    class TaskSchedulerAudioRead;
    class TaskSchedulerAudioWrite;
};


class Audio::TaskSchedulerAudioRead : public Audio::TaskBaseScheduler
{
public:
    TaskSchedulerAudioRead(Node *node, const AScheduler *scheduler)
        : TaskBaseScheduler(node, scheduler), _audioCache(std::make_unique<BufferViews>(1)) {}

    void operator()(void) noexcept { std::cout << "(Scheduler) Audio read\n"; }

private:
    AudioStack     _audioCache;
};

class Audio::TaskSchedulerAudioWrite : public Audio::TaskBaseScheduler
{
public:
    TaskSchedulerAudioWrite(Node *node, const AScheduler *scheduler)
        : TaskBaseScheduler(node, scheduler), _audioCache(std::make_unique<BufferViews>(1)) {}

    void operator()(void) noexcept { std::cout << "(Scheduler) Audio write\n"; }

private:
    AudioStack     _audioCache;
};
