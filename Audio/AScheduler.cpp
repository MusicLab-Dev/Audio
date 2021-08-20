/**
 * @ Author: Pierre Veysseyre
 * @ Description: AScheduler
 */

#include <iostream>

#include <Core/StringUtils.hpp>

#include "AScheduler.hpp"

using namespace Audio;
using namespace std::chrono_literals;


AScheduler::AScheduler(void)
{
    graph().construct();
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
        _beatMissCount = _beatMissOffset;
        _audioBlockBeatMissCount = _beatMissOffset;
        _audioElapsedBeat = 0u;
        if (!graph().running()) {
            if (!graph().size())
                invalidateCurrentGraph<false>();
            scheduleCurrentGraph();
        }
        break;
    }

    return true;
}

void AScheduler::processBeatMiss(void) noexcept
{
    auto &range = currentBeatRange();

    _beatMissCount += _beatMissOffset;
    if (_beatMissCount <= -1.0f) {
        _beatMissCount += 1.0f;
        range = {
            range.from,
            range.to - 1
        };
    } else if (_beatMissCount >= 1.0f) {
        _beatMissCount -= 1.0f;
        range = {
            range.from,
            range.to + 1
        };
    }
}
void AScheduler::processLooping(void) noexcept
{
    auto &range = currentBeatRange();

    if (range.from < _loopBeatRange.to && range.to > _loopBeatRange.to) {
        _processLoopCrop = _processBlockSize - ComputeSampleSize(
            _processBeatSize - (range.to - _loopBeatRange.to),
            tempo(),
            _sampleRate,
            _beatMissOffset,
            _beatMissCount
        );
    } else if (range.to > _loopBeatRange.to || range.from < _loopBeatRange.from) {
        range = {
            _loopBeatRange.from,
            _loopBeatRange.from + _processBeatSize
        };
        _beatMissCount = _beatMissOffset;
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
    if (_audioBlockBeatMissCount <= -1.0f) {
        _audioBlockBeatMissCount += 1.0f;
        --blockBeatSize;
    } else if (_audioBlockBeatMissCount >= 1.0f) {
        _audioBlockBeatMissCount -= 1.0f;
        ++blockBeatSize;
    }
    _audioElapsedBeat += blockBeatSize;
    return true;
}

void AScheduler::setProcessParams(const BlockSize processBlockSize, const SampleRate sampleRate, const std::uint32_t cachedAudioFrames) noexcept
{
    const auto newTempo = tempo();

    _sampleRate = sampleRate;
    _processBlockSize = processBlockSize;
    _audioBlockSize = processBlockSize;
    _cachedAudioFrames = cachedAudioFrames;
    _processBeatSize = ComputeBeatSize(processBlockSize, newTempo, _sampleRate, _beatMissOffset);
    _audioBlockBeatSize = _processBeatSize;
    _audioBlockBeatMissOffset = _beatMissOffset;
    _AudioQueue.resize(_cachedAudioFrames * _audioBlockSize * sizeof(float));
    currentBeatRange().to = currentBeatRange().from + _processBeatSize;
}

void AScheduler::setBPM(const BPM bpm) noexcept
{
    if (_bpm == bpm)
        return;
    _bpm = bpm;
    const auto newTempo = tempo();
    _processBeatSize = ComputeBeatSize(_processBlockSize, newTempo, _sampleRate, _beatMissOffset);
    _audioBlockBeatSize = ComputeBeatSize(_audioBlockSize, newTempo, _sampleRate, _audioBlockBeatMissOffset);
    currentBeatRange().to = currentBeatRange().from + _processBeatSize;
}

void AScheduler::exportProject(void) noexcept
{
    setPlaybackMode(PlaybackMode::Export);
    invalidateGraph<PlaybackMode::Export>();
}

bool AScheduler::onPlaybackGraphCompleted(void) noexcept
{
    bool exited = false;
    currentBeatRange().increment(_processBeatSize);
    processBeatMiss();
    if (isLooping())
        processLooping();
    if (produceAudioData(_project->master()->cache())) {
        exited = onAudioBlockGenerated();
    } else {
        do {
            exited = onAudioQueueBusy();
            if (exited)
                break;
            std::uint32_t sleepFactor;
            switch (_cachedAudioFrames) {
            case 0u:
            case 1u:
                sleepFactor = 1000000u / 8u;
                break;
            case 2u:
                sleepFactor = 1000000u / 4u;
                break;
            case 3u:
                sleepFactor = 1000000u / 2u;
                break;
            default:
                sleepFactor = 1000000u;
                break;
            }
            const std::chrono::nanoseconds sleepDuration((sleepFactor * _processBlockSize) / _sampleRate);
            std::this_thread::sleep_for(sleepDuration);
        } while (!flushOverflowCache());
        if (!exited)
            exited = onAudioBlockGenerated();
    }
    if (exited) {
        std::cout << "[Audio Graph] Shut down" << std::endl;
        clearAudioQueue();
        clearOverflowCache();
        return false;
    } else
        return true;
}

bool AScheduler::onExportGraphCompleted(void) noexcept
{
    bool exited = false;
    currentBeatRange().increment(_processBeatSize);
    processBeatMiss();
    // if (isLooping())
    //     processLooping();
    exited = onExportBlockGenerated();
    if (exited) {
        std::cout << "[Export Graph] Shut down" << std::endl;
        return false;
    } else
        return true;
}
