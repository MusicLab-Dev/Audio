/**
 * @ Author: Pierre Veysseyre
 * @ Description: Project implementation
 */

inline void Audio::Project::onAudioGenerationStarted(const BeatRange &range)
{
    master()->onAudioGenerationStarted(range);
}

inline bool Audio::Project::setPlaybackMode(const PlaybackMode mode) noexcept
{
    if (_playbackMode == mode)
        return false;
    _playbackMode = mode;
    return true;
}

#include <iostream>

inline bool Audio::Project::setBPM(const BPM bpm) noexcept
{
    if (_bpm == bpm)
        return false;
    if (bpm > MaxBpmSupported) {
        _bpm = MaxBpmSupported;
        return true;
    }
    if (bpm < MinBpmSupported) {
        _bpm = MinBpmSupported;
        return true;
    }
    std::cout << "salut\n";
    _bpm = bpm;
    return true;
}

inline bool Audio::Project::setTempo(const Tempo tempo) noexcept
{
    return setBPM(tempo * 60.f);
}