/**
 * @ Author: Pierre Veysseyre
 * @ Description: TaskAudio
 */

#pragma once

#include "TaskBase.hpp"

namespace Audio
{
    class TaskAudioRead;
    class TaskAudioWrite;

    class TaskAudioHub;

    using AudioStack = std::unique_ptr<BufferViews>;

};


class Audio::TaskAudioRead : public Audio::TaskBase
{
public:
    TaskAudioRead(Node *node) : TaskBase(node), _audioCache(std::make_unique<BufferViews>()) {}

    void operator()(void) noexcept { std::cout << "Audio read\n"; }

private:
    AudioStack     _audioCache;
};

class Audio::TaskAudioWrite : public Audio::TaskBase
{
public:
    // TaskAudioWrite(Node *node) : TaskBase(node), _audioCache(std::make_unique<BufferViews>(1)) {}

    void operator()(void) noexcept {
        std::cout << "Audio write\n";
        node().plugin()->receiveAudio((*_audioCache)[1]);
    }

private:
    AudioStack     _audioCache;
};


// class Audio::TaskAudioHub : public Audio::TaskAudioRead, Audio::TaskAudioWrite
// {
//     // TaskAudioHub(Node *node)
// };