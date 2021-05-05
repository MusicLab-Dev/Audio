/**
 * @ Author: Pierre Veysseyre
 * @ Description: AScheduler
 */

#include <iostream>

#include <Core/StringUtils.hpp>

#include "AScheduler.hpp"

using namespace Audio;


AScheduler::AScheduler(void)
{
    _dirtyFlags.fill(true);
    for (auto &cache : _graphs) {
        cache.graph.setRepeatCallback([this](void) -> bool {
            bool exited = false;
            if (_overflowCache) {
                exited = onAudioQueueBusy();
            } else {
                getCurrentBeatRange().increment(_processBeatSize);
                processBeatMiss();
                if (isLooping())
                    processLooping();
                if (produceAudioData(_project->master()->cache())) {
                    exited = onAudioBlockGenerated();
                } else {
                    exited = onAudioQueueBusy();
                }
            }
            if (exited) {
                std::cout << "Shutting down process graph, clearing cache" << std::endl;
                clearAudioQueue();
                clearOverflowCache();
                return false;
            } else
                return true;
        });
    }
}

bool AScheduler::setState(const State state) noexcept
{
    switch (state) {
    case State::Pause:
        for (State expected = State::Play; !_state.compare_exchange_strong(expected, State::Pause);) {
            if (expected == State::Pause)
                return false;
        }
        break;
    case State::Play:
        for (State expected = State::Pause; !_state.compare_exchange_strong(expected, State::Play);) {
            if (expected == State::Play)
                return false;
        }
        _beatMissCount = 0.0;
        _audioBlockBeatMissCount = 0.0;
        _audioElapsedBeat = 0u;
        if (!getCurrentGraph().running()) {
            if (_dirtyFlags[static_cast<std::size_t>(playbackMode())])
                invalidateCurrentGraph<false>();
            scheduleCurrentGraph();
        }
        break;
    }

    return true;
}

void AScheduler::processBeatMiss(void) noexcept
{
    auto &range = getCurrentBeatRange();

    _beatMissCount += _beatMissOffset;
    if (_beatMissCount <= -1.0) {
        _beatMissCount += 1.0;
        range = {
            range.from,
            range.to - 1
        };
    } else if (_beatMissCount >= 1.0) {
        _beatMissCount -= 1.0;
        range = {
            range.from,
            range.to + 1
        };
    }
}
void AScheduler::processLooping(void) noexcept
{
    auto &range = getCurrentBeatRange();

    if (range.to > _loopBeatRange.to || range.from < _loopBeatRange.from) {
        range = {
            _loopBeatRange.from,
            _loopBeatRange.from + _processBeatSize
        };
        _beatMissCount = 0.0;
    }
}


bool AScheduler::consumeAudioData(std::uint8_t *data, const std::size_t size)
{
    if (!_AudioQueue.tryPopRange(data, data + size)) {
        std::memset(data, 0, size);
        return false;
    }
    // Compute the audio elapsed beat
    auto blockBeatSize = _audioBlockBeatSize.load();
    _audioBlockBeatMissCount += _audioBlockBeatMissOffset;
    if (_audioBlockBeatMissCount <= -1.0) {
        _audioBlockBeatMissCount += 1.0;
        --blockBeatSize;
    } else if (_audioBlockBeatMissCount >= 1.0) {
        _audioBlockBeatMissCount -= 1.0;
        ++blockBeatSize;
    }
    _audioElapsedBeat += blockBeatSize;
    return true;
}

void AScheduler::setProcessParamByBlockSize(const BlockSize processBlockSize, const SampleRate sampleRate) noexcept
{
    const auto tempo = project()->tempo();

    _sampleRate = sampleRate;
    _processBlockSize = processBlockSize;
    _processBeatSize = ComputeBeatSize(processBlockSize, tempo, _sampleRate, _beatMissOffset);
    _audioBlockBeatSize = ComputeBeatSize(_audioBlockSize, tempo, _sampleRate, _audioBlockBeatMissOffset);
    for (auto &cache : _graphs)
        cache.currentBeatRange = { cache.currentBeatRange.from, cache.currentBeatRange.from + _processBeatSize };
}

void AScheduler::setBPM(const BPM bpm) noexcept
{
    project()->setBPM(bpm);
    const auto tempo = project()->tempo();
    _processBeatSize = ComputeBeatSize(_processBlockSize, tempo, _sampleRate, _beatMissOffset);
    _audioBlockBeatSize = ComputeBeatSize(_audioBlockSize, tempo, _sampleRate, _audioBlockBeatMissOffset);
    for (auto &cache : _graphs)
        cache.currentBeatRange = { cache.currentBeatRange.from, cache.currentBeatRange.from + _processBeatSize };
}

void AScheduler::setAudioBlockSize(const BlockSize blockSize) noexcept
{
    const auto tempo = project()->tempo();
    _audioBlockSize = blockSize;
    _audioBlockBeatSize = ComputeBeatSize(_audioBlockSize, tempo, _sampleRate, _audioBlockBeatMissOffset);
}
