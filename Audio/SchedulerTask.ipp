/**
 * @ Author: Pierre Veysseyre
 * @ Description: Scheduler Task
 */

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
    const auto beatRange = scheduler().template currentBeatRange<Playback>();
    auto &plugin = *node().plugin();
    if constexpr (ProcessNotesAndControls) {
        if (collectControls(beatRange)) {
            plugin.sendControls(_controlStack);
            _controlStack.clear();
        }
        if (collectPartitions(beatRange) || _noteStack) {
            if constexpr (HasNoteInput) {
                plugin.sendNotes(*_noteStack);
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
            std::cout << "Forget to merge lol" << std::endl;
            // Merge audio
        }
    }
}

template<Audio::IPlugin::Flags Flags, bool ProcessNotesAndControls, bool ProcessAudio, Audio::PlaybackMode Playback>
inline bool Audio::SchedulerTask<Flags, ProcessNotesAndControls, ProcessAudio, Playback>::collectControls(const BeatRange &beatRange) noexcept
{
    auto &controls = node().controls();

    if (!controls.isSafe())
        return false;
    if (auto &events = controls.headerCustomType(); events) {
        _controlStack.insert(_controlStack.end(), events.beginUnsafe(), events.endUnsafe());
        events.clearUnsafe();
    }
    if constexpr (Playback == PlaybackMode::Production) {
        for (const auto &control : controls) {
            if (control.muted())
                continue;
            for (const auto &automation : control.automations()) {
                if (automation.muted())
                    continue;
                for (const auto &instance : automation.instances()) {
                    if (instance.to < beatRange.from)
                        continue;
                    else if (instance.from > beatRange.to)
                        break;
                    const Point *last = nullptr;
                    for (const auto &point : automation.points()) {
                        if (instance.from + point.beat < beatRange.to) {
                            last = &point;
                        }
                        else {
                            collectInterpolatedPoint(beatRange, control.paramID(), last, point);
                            break;
                        }
                    }
                }
            }
        }
    }
    return _controlStack;
}


template<Audio::IPlugin::Flags Flags, bool ProcessNotesAndControls, bool ProcessAudio, Audio::PlaybackMode Playback>
inline void Audio::SchedulerTask<Flags, ProcessNotesAndControls, ProcessAudio, Playback>::collectInterpolatedPoint(
        const BeatRange &beatRange, const ParamID paramID, const Point * const left, const Point &right)
{
    ParamValue value = left ? 0.0 : right.value;
    switch (right.type) {
    case Point::CurveType::Linear:
    case Point::CurveType::Fast:
    case Point::CurveType::Slow:
        value = ((right.value - left->value) / (right.beat - left->beat)) * (beatRange.to - beatRange.from);
        break;
    }
    _controlStack.push(paramID, value);
}

template<Audio::IPlugin::Flags Flags, bool ProcessNotesAndControls, bool ProcessAudio, Audio::PlaybackMode Playback>
inline bool Audio::SchedulerTask<Flags, ProcessNotesAndControls, ProcessAudio, Playback>::collectPartitions(const BeatRange &beatRange) noexcept
{
    auto &partitions = node().partitions();

    if (!partitions.isSafe())
        return false;
    if (auto &events = partitions.headerCustomType(); events) {
        _noteStack->insert(_noteStack->end(), events.beginUnsafe(), events.endUnsafe());
        events.clearUnsafe();
    }
    if constexpr (Playback == PlaybackMode::Production) {
        for (const auto &partition : partitions) {
            if (partition.muted())
                continue;
            for (const auto &instance : partition.instances()) {
                if (instance.to <= beatRange.from || instance.from >= beatRange.to)
                    continue;
                collectPartition(partition, beatRange, instance);
            }
        }
    } else if constexpr (Playback == PlaybackMode::Partition) {
        if (&node() == _scheduler->partitionNode())
            collectPartition(partitions[_scheduler->partitionIndex()], beatRange);
    }
    return *_noteStack;
}

template<Audio::IPlugin::Flags Flags, bool ProcessNotesAndControls, bool ProcessAudio, Audio::PlaybackMode Playback>
inline void Audio::SchedulerTask<Flags, ProcessNotesAndControls, ProcessAudio, Playback>::collectPartition(const Partition &partition, const BeatRange &beatRange, const BeatRange &offset) noexcept
{
    const double beatToSampleRatio = static_cast<double>(_scheduler->sampleRate())
            / (static_cast<double>(_scheduler->tempo()) * static_cast<double>(Audio::BeatPrecision));

    for (const auto &note : partition.notes()) {
        const auto noteFrom = offset.from + note.range.from;
        const auto noteTo = offset.from + note.range.to;
        if (noteTo <= beatRange.from || noteFrom >= beatRange.to)
            continue;
        if (noteFrom >= beatRange.from && noteTo <= beatRange.to) {
            _noteStack->push(NoteEvent {
                NoteEvent::EventType::OnOff,
                note.key,
                note.velocity,
                note.tuning,
                static_cast<BlockSize>(static_cast<double>(noteFrom - beatRange.from) * beatToSampleRatio)
            });
        } else if (noteFrom >= beatRange.from) {
            _noteStack->push(NoteEvent {
                NoteEvent::EventType::On,
                note.key,
                note.velocity,
                note.tuning,
                static_cast<BlockSize>(static_cast<double>(noteFrom - beatRange.from) * beatToSampleRatio)
            });
        } else if (noteTo <= beatRange.to) {
            _noteStack->push(NoteEvent {
                NoteEvent::EventType::Off,
                note.key,
                note.velocity,
                note.tuning,
                static_cast<BlockSize>(static_cast<double>(noteTo - beatRange.from) * beatToSampleRatio)
            });
        }
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
