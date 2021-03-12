/**
 * @ Author: Pierre Veysseyre
 * @ Description: Scheduler.cpp
 */

#include "Interpreter.hpp"

void Scheduler::onAudioBlockGenerated(void)
{
    auto nextBeatRange = currentBeatRange();


    const Audio::BufferView buffer(project()->master()->cache());
    const auto size = buffer.size<std::uint8_t>();
    int count = 0;
    // std::cout << "X: " << count << " " << size << " " << reinterpret_cast<const int *>(buffer.byteData()) << std::endl;
    for (auto i = 0; i < size; ++i) {
        if (buffer.byteData()[i] != 0)
            ++count;
    }
    // if (count)
    //     std::cout << "Audio block non-null: " << count << std::endl;

    if (isLooping() && (nextBeatRange.to > loopBeatRange().to)) {
        setBeatRange(Audio::BeatRange({
            0u,
            processBeatSize()
        }));
    }
    // if (currentBeatRange().from > 20000)
    //     setState(State::Pause);

    // Todo: execute this on main thread
    dispatchApplyEvents();
    dispatchNotifyEvents();
}

void Scheduler::onAudioQueueBusy(void)
{
    // std::cout << "AudioQUeueBusy" << std::endl;
    // Todo: execute this on main thread
    dispatchApplyEvents();
    dispatchNotifyEvents();
}

bool Scheduler::setProcessBlockSize(const std::size_t processBlockSize) noexcept
{
    if (_processBlockSize == processBlockSize)
        return false;
    _processBlockSize = processBlockSize;
    return true;
}

bool Scheduler::setLoopBeatRange(const Audio::BeatRange loopBeatRange) noexcept
{
    if (_loopBeatRange == loopBeatRange)
        return false;
    _loopBeatRange = loopBeatRange;
    return true;
}

bool Scheduler::setIsLooping(const bool isLooping) noexcept
{
    if (_isLooping == isLooping)
        return false;
    _isLooping = isLooping;
    return true;
}
