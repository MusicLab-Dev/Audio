/**
 * @ Author: Pierre Veysseyre
 * @ Description: Scheduler.cpp
 */

#include "Interpreter.hpp"

Core::SPSCQueue<std::uint8_t> Interpreter::_AudioCallbackBuffer { 16384 };

void Scheduler::onAudioBlockGenerated(void) {
    std::cout << "<onAudioBlockGenerated>" << std::endl;
    // std::cout << _currentBeatRange << std::endl;
}

bool Scheduler::setProcessBlockSize(const std::size_t processBlockSize) noexcept
{
    if (_processBlockSize == processBlockSize)
        return false;
    _processBlockSize = processBlockSize;
    return true;
}
