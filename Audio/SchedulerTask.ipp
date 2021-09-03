/**
 * @ Author: Pierre Veysseyre
 * @ Description: Scheduler Task
 */

#include <Audio/DSP/Merge.hpp>
#include <iostream>

template<Audio::PlaybackMode Playback, bool ProcessNotesAndControls, bool ProcessAudio, Audio::IPlugin::Flags Deduced, Audio::IPlugin::Flags Begin, Audio::IPlugin::Flags End>
inline std::pair<Flow::Task, const Audio::NoteEvents *> Audio::MakeSchedulerTask(Flow::Graph &graph, const IPlugin::Flags flags,
        const AScheduler *scheduler, Node *node, const NoteEvents * const parentNoteStack)
{
    if constexpr (Begin > End) {
        Audio::SchedulerTask<Deduced, ProcessNotesAndControls, ProcessAudio, Playback> schedulerTask(scheduler, node, parentNoteStack);
        return std::make_pair(
            graph.emplace(std::move(schedulerTask)),
            schedulerTask.noteStack()
        );
    } else {
        if (static_cast<std::size_t>(flags) & static_cast<std::size_t>(Begin)) {
            return MakeSchedulerTask<
                Playback,
                ProcessNotesAndControls,
                ProcessAudio,
                static_cast<IPlugin::Flags>(static_cast<std::size_t>(Deduced) | static_cast<std::size_t>(Begin)),
                static_cast<IPlugin::Flags>(static_cast<std::size_t>(Begin) << 1),
                End
            >(graph, flags, scheduler, node, parentNoteStack);
        } else {
            return MakeSchedulerTask<
                Playback,
                ProcessNotesAndControls,
                ProcessAudio,
                Deduced,
                static_cast<IPlugin::Flags>(static_cast<std::size_t>(Begin) << 1),
                End
            >(graph, flags, scheduler, node, parentNoteStack);
        }
    }
}

template<Audio::IPlugin::Flags Flags, bool ProcessNotesAndControls, bool ProcessAudio, Audio::PlaybackMode Playback>
inline void Audio::SchedulerTask<Flags, ProcessNotesAndControls, ProcessAudio, Playback>::operator()(void) noexcept
{
    if (_parentNoteStack) // @todo verify that this copy should happend for each task
        *_noteStack = *_parentNoteStack;
    else
        _noteStack->clear();
    if (node().muted()) {
        node().cache().clear();
        return;
    }
    const auto beatRange = scheduler().currentBeatRange();
    auto &plugin = *node().plugin();
    if constexpr (ProcessNotesAndControls) {
        auto realBeatRange = beatRange;
        if (scheduler().isLooping()) {
            if (const auto max = scheduler().loopBeatRange().to; realBeatRange.to > max)
                realBeatRange.to = max;
        }
        if (collectControls(realBeatRange)) {
            plugin.sendControls(_controlStack);
            _controlStack.clear();
        }
        if (collectPartitions(realBeatRange) || _noteStack) {
            if constexpr (HasNoteInput) {
                plugin.sendNotes(*_noteStack, realBeatRange);
                _noteStack->clear();
            }
        }
        if constexpr (HasNoteOutput) {
            plugin.receiveNotes(*_noteStack);
        }
    }
    if constexpr (ProcessAudio) {
        if (collectBuffers() && HasAudioInput) {
            plugin.sendAudio(_bufferStack);
            _bufferStack.clear();
        }
        if constexpr (HasAudioOutput) {
            node().cache().clear();
            plugin.receiveAudio(node().cache());
        } else {
            // Merge audio
            node().cache().clear();
            DSP::Merge<float>(_bufferStack, node().cache(), 1.0f, false);
            _bufferStack.clear();
        }
        if (node().analysisRequestCount())
            node().cache().template updateVolumeCache<float>();
    }
}

template<Audio::IPlugin::Flags Flags, bool ProcessNotesAndControls, bool ProcessAudio, Audio::PlaybackMode Playback>
inline bool Audio::SchedulerTask<Flags, ProcessNotesAndControls, ProcessAudio, Playback>::collectControls(const BeatRange &beatRange) noexcept
{
    auto &automations = node().automations();

    if (!automations.isSafe())
        return false;
    auto &automationsHeader = automations.headerCustomType();
    if (auto &events = automationsHeader.controlsOnTheFly; events) {
        _controlStack.insert(_controlStack.end(), events.beginUnsafe(), events.endUnsafe());
        events.clearUnsafe();
    }
    if constexpr (Playback == PlaybackMode::Production || Playback == PlaybackMode::Export) {
        ParamID paramID = 0u;
        for (const auto &automation : automations) {
            if (!automation.isSafe() || automation.headerCustomType().muted) {
                ++paramID;
                continue;
            }
            const Point *last = nullptr;
            for (const auto &point : automation) {
                if (point.beat < beatRange.from) {
                    last = &point;
                } else {
                    collectInterpolatedPoint(beatRange, paramID, last, point);
                    break;
                }
            }
            // if (last)
            //     _controlStack.push(paramID, last->value);
            ++paramID;
        }
    }
    return _controlStack;
}

template<Audio::IPlugin::Flags Flags, bool ProcessNotesAndControls, bool ProcessAudio, Audio::PlaybackMode Playback>
inline void Audio::SchedulerTask<Flags, ProcessNotesAndControls, ProcessAudio, Playback>::collectInterpolatedPoint(
        const BeatRange &beatRange, const ParamID paramID, const Point * const left, const Point &right)
{
    ParamValue value = right.value;

    if (left) {
        switch (right.type) {
        case Point::CurveType::Linear:
        case Point::CurveType::Fast:
        case Point::CurveType::Slow:
            value = ((right.value - left->value) / (right.beat - left->beat)) * (beatRange.from - left->beat) + left->value;
            break;
        default:
            break;
    }
    }
    _controlStack.push(paramID, value);
}

template<Audio::IPlugin::Flags Flags, bool ProcessNotesAndControls, bool ProcessAudio, Audio::PlaybackMode Playback>
inline bool Audio::SchedulerTask<Flags, ProcessNotesAndControls, ProcessAudio, Playback>::collectPartitions(const BeatRange &beatRange) noexcept
{
    const double beatToSampleRatio = static_cast<double>(_scheduler->sampleRate())
            / (static_cast<double>(_scheduler->tempo()) * static_cast<double>(Audio::BeatPrecision));
    const double beatMissOffset = std::fabs(_scheduler->beatMissOffset());

    auto &partitions = node().partitions();

    if (!partitions.isSafe())
        return false;
    auto &partitionsHeader = partitions.headerCustomType();
    if (auto &events = partitionsHeader.notesOnTheFly; events) {
        _noteStack->insert(_noteStack->end(), events.beginUnsafe(), events.endUnsafe());
        events.clearUnsafe();
    }
    if constexpr (Playback == PlaybackMode::Production || Playback == PlaybackMode::Export) {
        if (auto &instances = partitionsHeader.instances; instances.isSafe()) {
            for (auto &instance : instances) {
                if (instance.range.to <= beatRange.from)
                    continue;
                else if (instance.range.from >= beatRange.to)
                    break;
                collectPartition(partitions[instance.partitionIndex], beatRange, beatToSampleRatio, beatMissOffset, instance);
            }
        }
    } else if constexpr (Playback == PlaybackMode::Partition) {
        if (&node() == _scheduler->partitionNode())
            collectPartition(partitions[_scheduler->partitionIndex()], beatRange, beatToSampleRatio, beatMissOffset);
    }
    return *_noteStack;
}

template<Audio::IPlugin::Flags Flags, bool ProcessNotesAndControls, bool ProcessAudio, Audio::PlaybackMode Playback>
inline void Audio::SchedulerTask<Flags, ProcessNotesAndControls, ProcessAudio, Playback>::collectPartition(
        const Partition &partition, const BeatRange &beatRange, const double beatToSampleRatio, const double beatMissOffset, const PartitionInstance &instance) noexcept
{
    UNUSED(beatToSampleRatio);
    for (const auto &note : partition) {
        const auto noteFrom = (note.range.from + instance.range.from) - instance.offset; // Can overflow !
        const auto noteTo = (note.range.to + instance.range.from) - instance.offset; // Can overflow !
        // 'note.range.from < instance.offset' detects an overflow
        if (note.range.from < instance.offset || noteTo <= beatRange.from || noteFrom >= beatRange.to || (noteFrom < beatRange.from && noteTo > beatRange.to))
            continue;
        NoteEvent event;
        event.key = note.key;
        event.velocity = note.velocity;
        event.tuning = note.tuning;
        if (noteFrom >= beatRange.from && noteTo <= beatRange.to) {
            event.type = NoteEvent::EventType::OnOff;
            event.sampleOffset = noteFrom != beatRange.from ? static_cast<BlockSize>((static_cast<double>(noteFrom - beatRange.from) - beatMissOffset) * beatToSampleRatio) : 0u;
        } else if (noteFrom >= beatRange.from) {
            event.type = NoteEvent::EventType::On;
            event.sampleOffset = noteFrom != beatRange.from ? static_cast<BlockSize>((static_cast<double>(noteFrom - beatRange.from) - beatMissOffset) * beatToSampleRatio) : 0u;
        } else { // if (noteTo <= beatRange.to) -> ensured by first check in loop
            event.type = NoteEvent::EventType::Off;
            event.sampleOffset = static_cast<BlockSize>((static_cast<double>(noteTo - beatRange.from) - beatMissOffset) * beatToSampleRatio);
        }
        _noteStack->push(event);
    }
}

template<Audio::IPlugin::Flags Flags, bool ProcessNotesAndControls, bool ProcessAudio, Audio::PlaybackMode Playback>
inline bool Audio::SchedulerTask<Flags, ProcessNotesAndControls, ProcessAudio, Playback>::collectBuffers(void) noexcept
{
    for (auto &child : node().children()) {
        if (!child->muted())
            _bufferStack.push(child->cache());
    }
    return _bufferStack;
}
