/**
 * @ Author: Pierre Veysseyre
 * @ Description: AScheduler
 */

#include <iostream>

#include <Core/StringUtils.hpp>

#include "AScheduler.hpp"

using namespace Audio;

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
        scheduleCurrentGraph();
        break;
    }
    return true;
}

void AScheduler::processBeatMiss(void)
{
    auto &range = getCurrentBeatRange();

    _beatMissCount += _beatMissOffset;
    if (_beatMissCount <= -1) {
        _beatMissCount += 1;
        range = {
            range.from,
            range.to - 1
        };
    } else if (_beatMissCount >= 1) {
        _beatMissCount -= 1;
        range = {
            range.from,
            range.to + 1
        };
    }
}
void AScheduler::processLooping(void)
{
    auto &range = getCurrentBeatRange();

    if ((range.to > _loopBeatRange.to)) {
        range = {
            0u,
            _processBeatSize
        };
        _beatMissCount = 0.0;
    }
}

void AScheduler::setProcessParamByBeatSize(const Beat processBeatSize, const SampleRate sampleRate)
{

}

void AScheduler::setProcessParamByBlockSize(const std::size_t processBlockSize, const SampleRate sampleRate)
{
    const double beats = static_cast<double>(processBlockSize) / sampleRate * project()->tempo() * Audio::BeatPrecision;
    const double beatsFloor = std::floor(beats);
    const double beatsCeil = std::ceil(beats);

    _processBlockSize = processBlockSize;
    if (auto ceilDt = beatsCeil - beats, floorDt = beats - beatsFloor; ceilDt < floorDt) {
        _beatMissOffset = -ceilDt;
        _processBeatSize = beatsCeil;
    } else {
        _beatMissOffset = floorDt;
        _processBeatSize = beatsFloor;
    }
    _beatMissCount = 0.0;
    for (auto &cache : _graphs)
        cache.currentBeatRange = { 0u, _processBeatSize };
}
