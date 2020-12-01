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


class Audio::TaskAudioRead : public Audio::Internal::TaskBase
{
public:
    void operator()(void) noexcept {
        std::cout << "Audio read\n";
        for (auto &child : node().children()) {
            _audioCache->push(child->cache());
        }
        node().plugin()->sendAudio(*_audioCache);
    }

private:
    AudioStack     _audioCache;
};

class Audio::TaskAudioWrite : public Audio::Internal::TaskBase
{
public:
    void operator()(void) noexcept {
        std::cout << "Audio write\n";
        node().plugin()->receiveAudio(node().cache());
    }
};


class Audio::TaskAudioHub : public Audio::Internal::TaskBase
{
    void operator()(void) noexcept {
        std::cout << "Audio write\n";
        node().plugin()->receiveAudio(node().cache());
    }
};