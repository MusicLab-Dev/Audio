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