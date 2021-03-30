/**
 * @ Author: Pierre Veysseyre
 * @ Description: AScheduler
 */

#include <iostream>

inline Audio::AScheduler::AScheduler(void)
{
    for (auto &cache : _graphs) {
        cache.graph.setRepeatCallback([this](void) -> bool {
            static auto SampleCpt = 0u;

            if (_overflowCache) {
                onAudioQueueBusy();
            } else {
                if (produceAudioData(_project->master()->cache())) {
                    onAudioBlockGenerated();
                } else {
                    onAudioQueueBusy();
                }
                getCurrentBeatRange().increment(_processBeatSize);
                processBeatMiss();
                if (isLooping())
                    processLooping();
                SampleCpt += _processBlockSize;
            }
            if (state() == State::Pause)
                std::cout << "State:" << static_cast<int>(state()) << std::endl;
            return state() == State::Play;
        });
    }
}

inline Audio::AScheduler::AScheduler(ProjectPtr &&project)
    : AScheduler()
{
    setProject(std::move(project));
}

template<typename Apply>
inline void Audio::AScheduler::addEvent(Apply &&apply)
{
    if (getCurrentGraph().running())
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
    if (getCurrentGraph().running())
        _events.push(Event {
            apply: std::forward<Apply>(apply),
            notify: std::forward<Notify>(notify)
        });
    else {
        apply();
        notify();
    }
}

inline const Flow::Graph &Audio::AScheduler::getCurrentGraph(void) const noexcept
{
    switch (playbackMode()) {
    case PlaybackMode::Production:
        return graph<PlaybackMode::Production>();
    case PlaybackMode::Live:
        return graph<PlaybackMode::Live>();
    case PlaybackMode::Partition:
        return graph<PlaybackMode::Partition>();
    case PlaybackMode::OnTheFly:
        return graph<PlaybackMode::OnTheFly>();
    default:
        return graph<PlaybackMode::Production>();
    }
}

inline const Audio::BeatRange &Audio::AScheduler::getCurrentBeatRange(void) const noexcept
{
    switch (playbackMode()) {
    case PlaybackMode::Production:
        return currentBeatRange<PlaybackMode::Production>();
    case PlaybackMode::Live:
        return currentBeatRange<PlaybackMode::Live>();
    case PlaybackMode::Partition:
        return currentBeatRange<PlaybackMode::Partition>();
    case PlaybackMode::OnTheFly:
        return currentBeatRange<PlaybackMode::OnTheFly>();
    default:
        return currentBeatRange<PlaybackMode::Production>();
    }
}

template<Audio::PlaybackMode Playback>
inline void Audio::AScheduler::invalidateGraph(void)
{
    getCurrentGraph().clear();
    if (!_project)
        throw std::logic_error("AScheduler::invalidateGraph: Scheduler has no linked project");
    if constexpr (Playback == PlaybackMode::Partition || Playback == PlaybackMode::OnTheFly) {
        if (!_partitionNode)
            throw std::logic_error("AScheduler::invalidateGraph: Scheduler has no linked partition node");
        if (_partitionIndex >= _partitionNode->partitions().size())
            throw std::logic_error("AScheduler::invalidateGraph: Partition node doesn't have a partition at given index");
    }
    buildGraph<Playback>();
}

inline void Audio::AScheduler::invalidateCurrentGraph(void)
{
    switch (playbackMode()) {
    case PlaybackMode::Production:
        return invalidateGraph<PlaybackMode::Production>();
    case PlaybackMode::Live:
        return invalidateGraph<PlaybackMode::Live>();
    case PlaybackMode::Partition:
        return invalidateGraph<PlaybackMode::Partition>();
    case PlaybackMode::OnTheFly:
        return invalidateGraph<PlaybackMode::OnTheFly>();
    }
}

inline void Audio::AScheduler::wait(void) noexcept_ndebug
{
    coreAssert(state() == State::Pause,
        throw std::logic_error("Audio::AScheduler::wait: Scheduler must be in paused mode before wait is called"));
    getCurrentGraph().wait();
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

inline void Audio::AScheduler::scheduleCurrentGraph(void)
{
    onAudioProcessStarted(getCurrentBeatRange());
    _scheduler->schedule(getCurrentGraph());
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


template<Audio::PlaybackMode Playback>
inline void Audio::AScheduler::buildGraph(void)
{

    Node *parent;
    if constexpr (Playback == PlaybackMode::Partition || Playback == PlaybackMode::OnTheFly)
        parent = _partitionNode;
    else
        parent = _project->master().get();

    auto &graph = this->graph<Playback>();
    auto conditional = graph.emplace([this] {
        if (_overflowCache) {
            // The delayed data has been consumed
            if (flushOverflowCache()) {
                _overflowCache.release();
                return true;
            // The delayed data must be re-delayed
            } else
                return false;
        // There is no data delayed
        } else
            return true;
    });
    auto noteTask = MakeSchedulerTask<Playback, true, false>(graph, parent->flags(), this, parent, nullptr);
    noteTask.first.setName(parent->name() + "_control_note");
    auto audioTask = MakeSchedulerTask<Playback, false, true>(graph, parent->flags(), this, parent, nullptr);
    audioTask.first.setName(parent->name() + "_audio");

    auto overflowTask = graph.emplace([]{
        std::this_thread::sleep_for(std::chrono::nanoseconds(5000));
    });
    conditional.precede(overflowTask);
    conditional.precede(noteTask.first);
    // If master is the only node, connect his tasks
    if (parent->children().empty()) {
        noteTask.first.succeed(audioTask.first);
        return;
    }
    for (auto &child : parent->children()) {
        buildNodeTask<Playback>(child.get(), noteTask, audioTask);
    }
    if constexpr (Playback == PlaybackMode::Partition || Playback == PlaybackMode::OnTheFly) {
        while (parent) {
            auto parentAudioTask = MakeSchedulerTask<Playback, false, true>(graph, parent->flags(), this, parent, nullptr);
            parentAudioTask.first.setName(parent->name() + "_audio");
            parentAudioTask.first.succeed(audioTask.first);
            audioTask = parentAudioTask;
            parent = parent->parent();
        }
    }
}

template<Audio::PlaybackMode Playback>
inline void Audio::AScheduler::buildNodeTask(const Node *node,
        std::pair<Flow::Task, const NoteEvents *> &parentNoteTask, std::pair<Flow::Task, const NoteEvents *> &parentAudioTask)
{
    if (node->children().empty()) {
        auto task = MakeSchedulerTask<Playback, true, true>(graph<Playback>(), node->flags(), this, const_cast<Node *>(node), parentNoteTask.second);
        task.first.setName(node->name().toStdString() + "_control_note_audio");
        task.first.succeed(parentNoteTask.first);
        task.first.precede(parentAudioTask.first);
        return;
    }
    auto noteTask = MakeSchedulerTask<Playback, true, false>(graph<Playback>(), node->flags(), this, const_cast<Node *>(node), parentNoteTask.second);
    noteTask.first.setName(node->name().toStdString() + "_control_note");
    auto audioTask = MakeSchedulerTask<Playback, false, true>(graph<Playback>(), node->flags(), this, const_cast<Node *>(node), parentNoteTask.second);
    audioTask.first.setName(node->name().toStdString() + "_audio");
    noteTask.first.succeed(parentNoteTask.first);

    for (auto &child : node->children()) {
        buildNodeTask<Playback>(child.get(), noteTask, audioTask);
    }
    audioTask.first.precede(parentAudioTask.first);
}