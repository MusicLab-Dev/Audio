/**
 * @ Author: Pierre Veysseyre
 * @ Description: AScheduler
 */

#include <iostream>

inline Audio::AScheduler::AScheduler(ProjectPtr &&project)
    : AScheduler()
{
    setProject(std::move(project));
}

template<typename Apply>
inline void Audio::AScheduler::addEvent(Apply &&apply)
{
    if (!_hasExitedGraph)
        _events.push(Event {
            std::forward<Apply>(apply),
            NotifyFunctor()
        });
    else {
        apply();
    }
}

template<typename Apply, typename Notify>
inline void Audio::AScheduler::addEvent(Apply &&apply, Notify &&notify)
{
    if (!_hasExitedGraph)
        _events.push(Event {
            std::forward<Apply>(apply),
            std::forward<Notify>(notify)
        });
    else {
        apply();
        notify();
    }
}

template<Audio::PlaybackMode Playback>
inline void Audio::AScheduler::invalidateGraph(void)
{
    if (!_project)
        throw std::logic_error("AScheduler::invalidateGraph: Scheduler has no linked project");
    graph().clear();
    if constexpr (Playback == PlaybackMode::Partition || Playback == PlaybackMode::OnTheFly) {
        if (!_partitionNode)
            throw std::logic_error("AScheduler::invalidateGraph: Scheduler has no linked partition node");
        if (_partitionIndex >= _partitionNode->partitions().size())
            throw std::logic_error("AScheduler::invalidateGraph: Partition node doesn't have a partition at given index");
    }
    if constexpr (Playback == PlaybackMode::Export)
        graph().setRepeatCallback([this] { return onExportGraphCompleted(); });
    else
        graph().setRepeatCallback([this] { return onPlaybackGraphCompleted(); });
    buildGraph<Playback>();
}

template<bool SetDirty>
inline void Audio::AScheduler::invalidateCurrentGraph(void)
{
    if constexpr (SetDirty)
        graph().clear();
    else {
        switch (playbackMode()) {
        case PlaybackMode::Production:
            return invalidateGraph<PlaybackMode::Production>();
        case PlaybackMode::Live:
            return invalidateGraph<PlaybackMode::Live>();
        case PlaybackMode::Partition:
            return invalidateGraph<PlaybackMode::Partition>();
        case PlaybackMode::OnTheFly:
            return invalidateGraph<PlaybackMode::OnTheFly>();
        case PlaybackMode::Export:
            return invalidateGraph<PlaybackMode::Export>();
        }
    }
}

inline void Audio::AScheduler::wait(void) noexcept_ndebug
{
    coreAssert(state() == State::Pause,
        throw std::logic_error("Audio::AScheduler::wait: Scheduler must be in paused mode before wait is called"));
    graph().wait();
}

inline void Audio::AScheduler::dispatchApplyEvents(void)
{
    for (const auto &event : _events)
        if (event.apply)
            event.apply();
}

inline void Audio::AScheduler::dispatchNotifyEvents(void)
{
    for (const auto &event : _events) {
        if (event.notify)
            event.notify();
    }
    _events.clear();
}

inline void Audio::AScheduler::scheduleCurrentGraph(void)
{
    _hasExitedGraph = false;
    onAudioProcessStarted(currentBeatRange());
    _scheduler->schedule(graph());
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
    const bool ok = _AudioQueue.tryPushRange(
        output.byteData(),
        output.byteData() + output.size<std::uint8_t>() - _processLoopCrop * sizeof(float)
    );

    if (!ok) {
        _overflowCache.copy(output);
        // std::cout << " - produce audio failed\n";
        return false;
    } else {
       _processLoopCrop = 0u;
        // std::cout << " - produce audio success\n";
        return true;
    }
}

inline bool Audio::AScheduler::flushOverflowCache(void)
{
    const auto res = _AudioQueue.tryPushRange(
        _overflowCache.byteData(),
        _overflowCache.byteData() + _overflowCache.size<std::uint8_t>() - _processLoopCrop * sizeof(float)
    );
    if (res)
        _processLoopCrop = 0u;
    return res;
}

inline void Audio::AScheduler::clearAudioQueue(void)
{
    _overflowCache.release();
}

inline void Audio::AScheduler::clearOverflowCache(void)
{
    _AudioQueue.clear();
}

template<Audio::PlaybackMode Playback>
inline void Audio::AScheduler::buildGraph(void)
{
    Node *parent;
    if constexpr (Playback == PlaybackMode::Partition || Playback == PlaybackMode::OnTheFly)
        parent = _partitionNode;
    else
        parent = _project->master().get();

    if (!parent)
        return;

    auto noteTask = MakeSchedulerTask<Playback, true, false>(graph(), parent->flags(), this, parent, nullptr);
    noteTask.first.setName(parent->name() + "_control_note");
    auto audioTask = MakeSchedulerTask<Playback, false, true>(graph(), parent->flags(), this, parent, nullptr);
    audioTask.first.setName(parent->name() + "_audio");

    // If master is the only node, connect his tasks
    if (parent->children().empty()) {
        noteTask.first.precede(audioTask.first);
        if (!parent->parent())
            return;
    } else {
        for (auto &child : parent->children()) {
            buildNodeTask<Playback>(child.get(), noteTask, audioTask);
        }
    }
    if constexpr (Playback == PlaybackMode::Partition || Playback == PlaybackMode::OnTheFly) {
        parent = parent->parent();
        while (parent) {
            auto parentAudioTask = MakeSchedulerTask<Playback, false, true>(graph(), parent->flags(), this, parent, nullptr);
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
        auto task = MakeSchedulerTask<Playback, true, true>(graph(), node->flags(), this, const_cast<Node *>(node), parentNoteTask.second);
        task.first.setName(node->name().toStdString() + "_control_note_audio");
        task.first.succeed(parentNoteTask.first);
        task.first.precede(parentAudioTask.first);
        return;
    }
    auto noteTask = MakeSchedulerTask<Playback, true, false>(graph(), node->flags(), this, const_cast<Node *>(node), parentNoteTask.second);
    noteTask.first.setName(node->name().toStdString() + "_control_note");
    auto audioTask = MakeSchedulerTask<Playback, false, true>(graph(), node->flags(), this, const_cast<Node *>(node), parentNoteTask.second);
    audioTask.first.setName(node->name().toStdString() + "_audio");
    noteTask.first.succeed(parentNoteTask.first);

    for (auto &child : node->children()) {
        buildNodeTask<Playback>(child.get(), noteTask, audioTask);
    }
    audioTask.first.precede(parentAudioTask.first);
}

inline Audio::Beat Audio::AScheduler::ComputeBeatSize(const std::uint32_t sampleSize, const Tempo tempo, const SampleRate sampleRate, float &beatMissOffset) noexcept
{
    const float beats = (static_cast<float>(sampleSize) / sampleRate) * tempo * Audio::BeatPrecision;
    const float beatsFloor = std::floor(beats);

    beatMissOffset = beats - beatsFloor;
    return static_cast<Beat>(beatsFloor);
}

inline std::uint32_t Audio::AScheduler::ComputeSampleSize(const Beat beatSize, const Tempo tempo, const SampleRate sampleRate, const float beatMissOffset, const float beatMissCount) noexcept
{
    if (beatMissOffset > 0.0) {
        return static_cast<std::uint32_t>((static_cast<float>(sampleRate) * (static_cast<float>(beatSize) - beatMissOffset + beatMissCount)) / (tempo * Audio::BeatPrecision));
    } else {
        return static_cast<std::uint32_t>((static_cast<float>(sampleRate) * (static_cast<float>(beatSize) + beatMissOffset - beatMissCount)) / (tempo * Audio::BeatPrecision));
    }
}
