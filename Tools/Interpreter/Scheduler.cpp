/**
 * @ Author: Pierre Veysseyre
 * @ Description: Scheduler.cpp
 */

#include "Interpreter.hpp"

// #include <Audio/BaseIndex.hpp>

void Scheduler::processBeatMiss(void)
{
    auto nextBeatRange = currentBeatRange();
    _beatMissCount += _beatMissOffset;
    if (_beatMissCount >= 1) {
        // std::cout << "<blop>" << std::endl;
        _beatMissCount -= 1;
        setBeatRange({ nextBeatRange.from, nextBeatRange.to + 1 });
    }
}
void Scheduler::processLooping(void)
{
    if (isLooping() && (currentBeatRange().to > loopBeatRange().to)) {
        setBeatRange(Audio::BeatRange({
            0u,
            processBeatSize()
        }));
        // this->_AudioQueue.clear();
    }
}

void Scheduler::onAudioBlockGenerated(void)
{
    // const Audio::BufferView buffer(project()->master()->cache());
    // const auto size = buffer.size<std::uint8_t>();
    // int count = 0;

    // for (auto i = 0; i < size; ++i) {
    //     if (buffer.byteData()[i] != 0)
    //         ++count;
    // }
    // if (count)
    //     std::cout << "Audio block non-null: " << count << std::endl;

    // std::cout << "beatrangeProcess: " << this->processBeatSize() << std::endl;
    // std::cout << "next beatrange: " << currentBeatRange() << std::endl;


    processLooping();
    // processBeatMiss();

    /** @todo Execute this on main thread */
    dispatchApplyEvents();
    dispatchNotifyEvents();
}

void Scheduler::onAudioQueueBusy(void)
{
    // std::cout << "AudioQueueBusy" << std::endl;
    // std::cout << "next beatrange (busy): " << currentBeatRange() << std::endl;

    // processBeatMiss();
    processLooping();

    /** @todo Execute this on main thread */
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
