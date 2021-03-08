/**
 * @ Author: Pierre Veysseyre
 * @ Description: Scheduler.cpp
 */

#include "Interpreter.hpp"

Core::SPSCQueue<std::uint8_t> Interpreter::_AudioCallbackBuffer { 16384 };

void Scheduler::onAudioBlockGenerated(void) {
    std::cout << "<onAudioBlockGenerated> " << _currentBeatRange << std::endl;

    auto &masterNode = project()->master();
    auto *masterCache = masterNode->cache().data<std::uint8_t>(Audio::Channel::Mono);
    auto total = Interpreter::AudioCallbackBuffer().pushRange(
        masterCache,
        masterCache + (masterNode->cache().channelByteSize() * static_cast<std::size_t>(masterNode->cache().channelArrangement())));
}

bool Scheduler::setProcessBlockSize(const std::size_t processBlockSize) noexcept
{
    if (_processBlockSize == processBlockSize)
        return false;
    _processBlockSize = processBlockSize;
    return true;
}
