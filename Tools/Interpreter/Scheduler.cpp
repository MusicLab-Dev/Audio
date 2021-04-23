/**
 * @ Author: Pierre Veysseyre
 * @ Description: Scheduler.cpp
 */

#include "Interpreter.hpp"

// #include <Audio/BaseIndex.hpp>

bool Scheduler::onAudioBlockGenerated(void)
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



    /** @todo Execute this on main thread */
    dispatchApplyEvents();
    dispatchNotifyEvents();
    return false;
}

bool Scheduler::onAudioQueueBusy(void)
{
    // std::cout << "AudioQueueBusy" << std::endl;
    // std::cout << "next beatrange (busy): " << currentBeatRange() << std::endl;

    /** @todo Execute this on main thread */
    dispatchApplyEvents();
    dispatchNotifyEvents();
    return false;
}
