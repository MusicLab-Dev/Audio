/**
 * @ Author: Pierre Veysseyre
 * @ Description: Scheduler Task
 */

#pragma once

#include <Flow/Flow/Graph.hpp>

#include "IPlugin.hpp"

namespace Audio
{
    class AScheduler;

    template<IPlugin::Flags Flags, bool ProcessNotesAndControls, bool ProcessAudio, Audio::PlaybackMode Playback>
    class SchedulerTask;

    /** @brief Make a task from a runtime flags */
    template<Audio::PlaybackMode Playback, bool ProcessNotesAndControls, bool ProcessAudio, IPlugin::Flags Deduced = IPlugin::Flags::None,
            IPlugin::Flags Begin = IPlugin::Flags::AudioInput, IPlugin::Flags End = IPlugin::Flags::NoteOutput>
    [[nodiscard]] std::pair<Flow::Task, const NoteEvents *> MakeSchedulerTask(Flow::Graph &graph, const IPlugin::Flags flags,
            const AScheduler *scheduler, Node *node, const NoteEvents * const parentNoteStack);
}

template<Audio::IPlugin::Flags Flags, bool ProcessNotesAndControls, bool ProcessAudio, Audio::PlaybackMode Playback>
class alignas_cacheline Audio::SchedulerTask
{
public:
    static constexpr bool HasNoteInput = static_cast<std::size_t>(Flags) & static_cast<std::size_t>(IPlugin::Flags::NoteInput);
    static constexpr bool HasNoteOutput = static_cast<std::size_t>(Flags) & static_cast<std::size_t>(IPlugin::Flags::NoteOutput);
    static constexpr bool HasAudioInput = static_cast<std::size_t>(Flags) & static_cast<std::size_t>(IPlugin::Flags::AudioInput);
    static constexpr bool HasAudioOutput = static_cast<std::size_t>(Flags) & static_cast<std::size_t>(IPlugin::Flags::AudioOutput);

    /** @brief Construct the task from a node, a scheduler and a parent note stack */
    SchedulerTask(const AScheduler *scheduler, Node *node, const NoteEvents * const parentNoteStack) noexcept
        : _scheduler(scheduler), _node(node), _parentNoteStack(parentNoteStack) {}

    /** @brief Move constructor */
    SchedulerTask(SchedulerTask &&other) noexcept = default;

    /** @brief Move assignment */
    SchedulerTask &operator=(SchedulerTask &&other) noexcept = default;

    /** @brief Execution operator */
    void operator()(void) noexcept;

    /** @brief Get the internal note stack */
    [[nodiscard]] const NoteEvents *noteStack(void) const noexcept { return _noteStack.get(); }

    /** @brief Clear the internal note stack */
    void clearNoteStack(void) noexcept { _noteStack->clear(); }

private:
    const AScheduler *_scheduler { nullptr };
    Node *_node { nullptr };
    std::unique_ptr<NoteEvents> _noteStack { std::make_unique<NoteEvents>() };
    const NoteEvents *_parentNoteStack { nullptr };
    BufferViews _bufferStack {};
    ControlEvents _controlStack {};

    /** @brief Get the internal scheduler*/
    [[nodiscard]] const AScheduler &scheduler(void) const noexcept { return *_scheduler; }

    /** @brief Get the internal node */
    [[nodiscard]] const Node &node(void) const noexcept { return *_node; }
    [[nodiscard]] Node &node(void) noexcept { return *_node; }

    /** @brief Collect every controls of the current frame */
    [[nodiscard]] bool collectControls(const BeatRange &beatRange) noexcept;

    /** @brief Collect a single point from an interpolation */
    void collectInterpolatedPoint(const BeatRange &beatRange, const ParamID paramID, const Point * const left, const Point &right);

    /** @brief Collect every notes of the current frame */
    [[nodiscard]] bool collectPartitions(const BeatRange &beatRange) noexcept;

    /** @brief Collect every notes within the current offset */
    void collectPartition(const Partition &partition, const BeatRange &beatRange, const double beatToSampleRatio, const double beatMissOffset, const PartitionInstance &instance = PartitionInstance()) noexcept;

    /** @brief Collect every cached children buffer of the current frame */
    bool collectBuffers(void) noexcept;
};
