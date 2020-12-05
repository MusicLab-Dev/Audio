/**
 * @ Author: Pierre Veysseyre
 * @ Description: Scheduler Task
 */

template<bool ProcessNotesAndControls, bool ProcessAudio, IPlugin::Flags Deduced, IPlugin::Flags Begin, IPlugin::Flags End>
inline std::pair<tf::Task, const NoteEvents *> Audio::MakeSchedulerTask(Flow::Graph &graph, const IPlugin::Flags flags,
        const AScheduler *scheduler, Node *node, const NoteEvents * const parentNoteStack)
{
    if constexpr (Begin > End) {
        Audio::SchedulerTask<Deduced, ProcessNotesAndControls, ProcessAudio> schedulerTask(scheduler, node, parentNoteStack);
        const auto * const noteStack = schedulerTask.noteStack();
        return std::make_pair(
            graph.emplace(std::move(schedulerTask)),
            noteStack
        );
    } else {
        if (static_cast<std::size_t>(flags) & static_cast<std::size_t>(Begin)) {
            return MakeSchedulerTask<
                ProcessNotesAndControls,
                ProcessAudio,
                static_cast<IPlugin::Flags>(static_cast<std::size_t>(Deduced) | static_cast<std::size_t>(Begin)),
                static_cast<IPlugin::Flags>(static_cast<std::size_t>(Begin) << 1),
                End
            >(graph, flags, scheduler, node, parentNoteStack);
        } else {
            return MakeSchedulerTask<
                ProcessNotesAndControls,
                ProcessAudio,
                Deduced,
                static_cast<IPlugin::Flags>(static_cast<std::size_t>(Begin) << 1),
                End
            >(graph, flags, scheduler, node, parentNoteStack);
        }
    }
}

template<Audio::IPlugin::Flags Flags, bool ProcessNotesAndControls, bool ProcessAudio>
inline void Audio::SchedulerTask<Flags, ProcessNotesAndControls, ProcessAudio>::operator()(void) noexcept
{
    *_noteStack = *_parentNoteStack;
    const auto beatRange = scheduler().currentBeatRange();
    auto &plugin = *node().plugin();
    if (ProcessNotesAndControls) {
        if constexpr (HasControlInput) {
            collectControls(beatRange);
            plugin.sendControls(_controlStack);
        }
        collectNotes(beatRange);
        if constexpr (HasNoteInput) {
            if (!_noteStack->empty()) {
                plugin.sendNotes(*_noteStack);
                _noteStack->clear();
            }
        }
        if constexpr (HasNoteOutput) {
            plugin.receiveNotes(*_noteStack);
        }
    }
    if (ProcessAudio) {
        collectBuffers();
        if constexpr (HasAudioInput) {
            plugin.sendAudio(_bufferStack);
        }
        if constexpr (HasAudioOutput) {
            plugin.receiveAudio(node().cache());
        } else {
            //mergeBufferStack(); '_buffersCache' -> node().cache()
        }
    }
}

template<Audio::IPlugin::Flags Flags, bool ProcessNotesAndControls, bool ProcessAudio>
inline void Audio::SchedulerTask<Flags, ProcessNotesAndControls, ProcessAudio>::collectControls(const BeatRange &beatRange) noexcept
{
    for (const auto &control : node().controls()) {
        if (control.muted())
            continue;
        auto i = 0ul;
        for (const auto &automation : control.automations()) {
            if (control.isAutomationMuted(i++))
                continue;
            for (const auto &instance : automation.instances()) {
                if (instance.to < beatRange.from)
                    continue;
                else if (instance.from > beatRange.to)
                    break;
                const Point *last = nullptr;
                for (const auto &point : automation.points()) {
                    if (instance.begin + point.beat < beatRange.to)
                        last = &point;
                    else {
                        collectInterpolatedPoint(beatRange, control.paramID(), last, point);
                        break;
                    }
                }
            }
        }
    }
}

template<Audio::IPlugin::Flags Flags, bool ProcessNotesAndControls, bool ProcessAudio>
inline void Audio::SchedulerTask<Flags, ProcessNotesAndControls, ProcessAudio>::collectInterpolatedPoint(
        const BeatRange &beatRange, const ParamID paramID, const Point * const left, const Point &right)
{
    ParamValue value = left ? 0.0 : right.value;
    switch (right.type) {
    case Point::CurveType::Linear:
        value = ((right.value - left->value) / (right.beat - left->beat)) * (beatRange.to - beatRange.from);
        break;
    case Point::CurveType::Fast:
        // value =
        break;
    case Point::CurveType::Slow:
        // value =
        break;
    }
    _controlStack.push(paramID, value);
}

template<Audio::IPlugin::Flags Flags, bool ProcessNotesAndControls, bool ProcessAudio>
inline void Audio::SchedulerTask<Flags, ProcessNotesAndControls, ProcessAudio>::collectNotes(const BeatRange &beatRange) noexcept
{
    for (const auto &partition : node().partitions()) {
        if (partition.muted())
            continue;
        for (const auto &instance : partition.instances()) {
            //
            //_noteStack.push()
        }
    }
}

template<Audio::IPlugin::Flags Flags, bool ProcessNotesAndControls, bool ProcessAudio>
inline void Audio::SchedulerTask<Flags, ProcessNotesAndControls, ProcessAudio>::collectBuffers(void) noexcept
{
    for (auto &child : node().children()) {
        if (!child->muted())
            _bufferStack.push(child->cache());
    }
}