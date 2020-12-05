/**
 * @ Author: Pierre Veysseyre
 * @ Description: AScheduler
 */

template<typename Apply, typename Notify>
inline void Audio::AScheduler::addEvent(Apply &&apply, Notify &&notify)
{
    if (state() == State::Play)
        _events.push(std::forward<Apply>(apply), std::forward<Notify>(notify));
    else {
        apply();
        notify();
    }
}

inline void Audio::AScheduler::invalidateProjectGraph(void)
{
    _graph->clear();
    if (_project)
        buildProjectGraph();
}

inline void Audio::AScheduler::dispatchApplyEvents(void)
{
    for (const auto &event : _events)
        event.apply();
}

inline void Audio::AScheduler::dispatchNotifyEvents(void)
{
    for (const auto &event : _events)
        event.notify();
    _events.clear();
}

inline void Audio::AScheduler::scheduleProjectGraph(void)
{
    onAudioProcessStarted(currentBeatRange());
    _scheduler->run_until(*_graph, [this](void) -> bool {
        onAudioBlockGenerated();
        return state() == State::Play;
    });
}

inline void Audio::AScheduler::onAudioProcessStarted(const BeatRange &beatRange)
{
    _project->onAudioGenerationStarted(beatRange);
}
