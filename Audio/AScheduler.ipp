/**
 * @ Author: Pierre Veysseyre
 * @ Description: AScheduler
 */

inline Audio::AScheduler::AScheduler(ProjectPtr &&project)
    : _project(std::move(project))
{
    _graph.setRepeatCallback([this](void) -> bool {
        onAudioBlockGenerated();
        return state() == State::Play;
    });
}

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
    _graph.clear();
    if (_project)
        buildProjectGraph();
}

inline void Audio::AScheduler::wait(void) noexcept_ndebug
{
    coreAssert(state() == State::Pause,
        throw std::logic_error("Audio::AScheduler::wait: Scheduler must be in paused mode before wait is called"));
    _graph.wait();
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
    _scheduler->schedule(_graph);
}

inline void Audio::AScheduler::onAudioProcessStarted(const BeatRange &beatRange)
{
    _project->onAudioGenerationStarted(beatRange);
}
