/**
 * @ Author: Pierre Veysseyre
 * @ Description: Scheduler Task
 */

#include <iostream>

#include <Audio/DSP/Merge.hpp>

template<bool ProcessNotesAndControls, bool ProcessAudio, Audio::IPlugin::Flags Deduced, Audio::IPlugin::Flags Begin, Audio::IPlugin::Flags End>
inline std::pair<Flow::Task, const Audio::NoteEvents *> Audio::MakeSchedulerTask(Flow::Graph &graph, const IPlugin::Flags flags,
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
    // std::cout << "run task: " << node().name().toStdString() << std::endl;
    // std::cout << "_noteStack: " << _noteStack->size() << std::endl;
    // std::cout << "_parentNoteStack: " << _parentNoteStack << std::endl;
    _noteStack->clear();
    if (_parentNoteStack)
        *_noteStack = *_parentNoteStack;
    // std::cout << "_parentNoteStack final: " << _noteStack->size() << std::endl;
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
            int count = 0;
            auto size = node().cache().template size<std::uint8_t>();
            for (auto i = 0; i < size; ++i) {
                if (node().cache().byteData()[i])
                    ++count;
            }
            // std::cout << "SchedulerTask::processAudio: size: " << size << ", count: " << count << std::endl;
        } else {
            // DSP::Merge(node().cache().data<float>())
        }
    }
}

#include <iostream>

template<Audio::IPlugin::Flags Flags, bool ProcessNotesAndControls, bool ProcessAudio>
inline void Audio::SchedulerTask<Flags, ProcessNotesAndControls, ProcessAudio>::collectControls(const BeatRange &beatRange) noexcept
{
    for (const auto &control : node().controls()) {
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

template<Audio::IPlugin::Flags Flags, bool ProcessNotesAndControls, bool ProcessAudio>
inline void Audio::SchedulerTask<Flags, ProcessNotesAndControls, ProcessAudio>::collectInterpolatedPoint(
        const BeatRange &beatRange, const ParamID paramID, const Point * const left, const Point &right)
{
    std::cout << "collectInterpolatedPoint" << std::endl;
    std::cout << "range: " << beatRange.from << ", " << beatRange.to << std::endl;
    std::cout << "paramID: " << paramID << std::endl;
    std::cout << "left: " << left << std::endl;
    std::cout << " --" << left->beat << std::endl;
    std::cout << " -" << left->value << std::endl;
    std::cout << " --" << right.beat << std::endl;
    std::cout << " -" << right.value << std::endl;

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
    // std::cout << "\t-collect notes: " << node().name().toStdView() << std::endl;
    for (const auto &partition : node().partitions()) {
        if (partition.muted())
            continue;
        for (const auto &instance : partition.instances()) {
            // Skip instance ending before the beatrange
            if (instance.to <= beatRange.from)
                continue;
            // Skip instance begining after the beatrange
            if (instance.from >= beatRange.to)
                continue;
            for (const auto &note : partition.notes()) {
                const auto noteFrom = instance.from + note.range.from;
                const auto noteTo = instance.from + note.range.to;
                // Skip note ending after the beatrange
                if (noteTo <= beatRange.from)
                    continue;
                // Skip note begining after the beatrange
                if (noteFrom >= beatRange.to)
                    continue;
                // Note is overlapping the end of the beatrange
                if (noteTo > beatRange.to) {
                    // std::cout << "On\n";
                    _noteStack->push(NoteEvent({
                        type: NoteEvent::EventType::On,
                        key: note.key,
                        velocity: note.velocity,
                        tuning: note.tuning
                    }));
                }
                // Note is overlapping the begining of the beatrange
                else if (noteFrom < beatRange.from) {
                    _noteStack->push(NoteEvent({
                        type: NoteEvent::EventType::Off,
                        key: note.key,
                        velocity: note.velocity,
                        tuning: note.tuning
                    }));
                    // std::cout << "Off\n";
                }
                // Note fits in the beatrange
                else {
                    _noteStack->push(NoteEvent({
                        type: NoteEvent::EventType::OnOff,
                        key: note.key,
                        velocity: note.velocity,
                        tuning: note.tuning
                    }));
                    // std::cout << "OnOff\n";
                }
            }
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