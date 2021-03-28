/**
 * @ Author: Pierre Veysseyre
 * @ Description: Project implementation
 */

inline void Audio::Project::onAudioGenerationStarted(const BeatRange &range)
{
    master()->onAudioGenerationStarted(range);
}