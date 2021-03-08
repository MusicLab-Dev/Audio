/**
 * @ Author: Pierre Veysseyre
 * @ Description: AScheduler
 */

inline Audio::AScheduler::AScheduler(void)
{
    _graph.setRepeatCallback([this](void) -> bool {
        onAudioBlockGenerated();
        _currentBeatRange += _processBeatSize;
        return state() == State::Play;
    });
}

inline Audio::AScheduler::AScheduler(ProjectPtr &&project)
    : AScheduler()
{
    setProject(std::move(project));
}

template<typename Apply>
inline void Audio::AScheduler::addEvent(Apply &&apply)
{
    if (_graph.running())
        _events.push(Event {
            apply: std::forward<Apply>(apply),
            notify: NotifyFunctor()
        });
    else {
        apply();
    }
}

template<typename Apply, typename Notify>
inline void Audio::AScheduler::addEvent(Apply &&apply, Notify &&notify)
{
    if (_graph.running())
        _events.push(Event {
            apply: std::forward<Apply>(apply),
            notify: std::forward<Notify>(notify)
        });
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
    std::cout << "< scheduleProjectGraph > " << currentBeatRange().from << " - " << currentBeatRange().to << std::endl;
    onAudioProcessStarted(currentBeatRange());
    _scheduler->schedule(_graph);
}

inline void Audio::AScheduler::prepareCache(const AudioSpecs &specs)
{
    _project->master()->prepareCache(specs);
}

inline void Audio::AScheduler::onAudioProcessStarted(const BeatRange &beatRange)
{
    _project->onAudioGenerationStarted(beatRange);
}
