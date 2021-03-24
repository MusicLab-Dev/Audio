/**
 * @ Author: Pierre Veysseyre
 * @ Description: AScheduler
 */

#include <iostream>

inline Audio::AScheduler::AScheduler(void)
{
    _graph.setRepeatCallback([this](void) -> bool {
        // std::cout << "setRepeatCallback !!!!!" << std::endl;
        // std::cout << "_currentBeatRange " << _currentBeatRange << std::endl;
        // std::cout << "_processBeatSize " << _processBeatSize << std::endl;
        // std::cout << "___currentBeatRange " << _currentBeatRange << std::endl;
        // std::cout << "___processBeatSize " << _processBeatSize << std::endl;

        static auto SampleCpt = 0u;



        if (_overflowCache) {
            onAudioQueueBusy();
        } else {
            // std::cout << "Range " << _currentBeatRange << std::endl;
            // std::cout << "SampleCpt " << SampleCpt << std::endl;
            if (produceAudioData(_project->master()->cache())) {
                onAudioBlockGenerated();
            } else {
                onAudioQueueBusy();
            }
            _currentBeatRange.increment(_processBeatSize);
            processBeatMiss();
            processLooping();
            SampleCpt += _processBlockSize;
        }
        if (state() == State::Pause)
            std::cout << "State:" << static_cast<int>(state()) << std::endl;
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

inline bool Audio::AScheduler::produceAudioData(const BufferView output)
{
    const auto cacheSize = output.size<std::uint8_t>();
    const bool ok = _AudioQueue.tryPushRange(
        output.byteData(),
        output.byteData() + cacheSize
    );

    if (!ok) {
        _overflowCache.copy(output);
        // std::cout << " - produce audio failed\n";
        return false;
    } else {
        // std::cout << " - produce audio success\n";
        return true;
    }
}

inline bool Audio::AScheduler::flushOverflowCache(void)
{
    const auto cacheSize = _overflowCache.size<std::uint8_t>();
    // std::cout << "flush: Queue size: " << _AudioQueue.size() << std::endl;
    const bool ok = _AudioQueue.tryPushRange(
        _overflowCache.byteData(),
        _overflowCache.byteData() + cacheSize
    );

    return ok;
}