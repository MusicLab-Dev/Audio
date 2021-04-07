/**
 * @ Author: Pierre Veysseyre
 * @ Description: AScheduler
 */

#pragma once

#include <functional>
#include <atomic>
#include <future>

#include <Core/Functor.hpp>
#include <Core/SPSCQueue.hpp>
#include <Flow/Scheduler.hpp>

#include "Project.hpp"
#include "Buffer.hpp"
#include "SchedulerTask.hpp"

namespace Audio
{
    class AScheduler;

    using ApplyFunctor = Core::Functor<void(void)>;
    using NotifyFunctor = Core::Functor<void(void)>;
};

class alignas_cacheline Audio::AScheduler
{
public:
    /** @brief Internal play state */
    enum class State : int {
        Pause, Play
    };


    /** @brief Structure of an internal event */
    struct Event
    {
        ApplyFunctor apply {}; // The apply event is called when the scheduler is IDLE
        NotifyFunctor notify {}; // The notify event is called when the scheduler is RUNNING, after 'apply' dispatcher
    };


    /** @brief Cache of a playback mode graph */
    struct alignas_quarter_cacheline PlaybackGraph
    {
        Flow::Graph graph {};
        BeatRange currentBeatRange {};
    };

    /** @brief Caches of every playback mode */
    using PlaybackGraphs = std::array<PlaybackGraph, Audio::PlaybackModeCount>;


    /** @brief Default constructor (will crash if you play without project !) */
    AScheduler(void);

    /** @brief Constructor */
    AScheduler(ProjectPtr &&project);

    /** @brief Virtual destructor */
    virtual ~AScheduler(void) = default;

    /** @brief Get / Set internal project */
    [[nodiscard]] ProjectPtr &project(void) noexcept { return _project; }
    void setProject(ProjectPtr &&project) noexcept { _project = std::move(project); }


    /** @brief Get / set internal state */
    [[nodiscard]] State state(void) const noexcept { return _state.load(); }
    bool setState(const State state) noexcept;


    /** @brief Get internal process beat size */
    [[nodiscard]] std::uint32_t processBeatSize(void) const noexcept { return _processBeatSize; }

    /** @brief Get the process block size */
    [[nodiscard]] std::size_t processBlockSize(void) const noexcept { return _processBlockSize; }


    /** @brief Get / Set the loop beat range */
    [[nodiscard]] BeatRange loopBeatRange(void) const noexcept { return _loopBeatRange; }
    void setLoopBeatRange(const BeatRange loopBeatRange) noexcept { _loopBeatRange = loopBeatRange; }

    /** @brief Get / Set the loop status */
    [[nodiscard]] bool isLooping(void) const noexcept { return _isLooping; }
    void setIsLooping(const bool isLooping) noexcept { _isLooping = isLooping; }


    /** @brief Get / Set the playback mode */
    [[nodiscard]] PlaybackMode playbackMode(void) const noexcept { return _playbackMode; }
    void setPlaybackMode(const PlaybackMode mode) noexcept { _playbackMode = mode; }


    /** @brief Get a generation graph */
    template<PlaybackMode Playback>
    [[nodiscard]] Flow::Graph &graph(void) noexcept { return _graphs[static_cast<std::size_t>(Playback)].graph; }
    template<PlaybackMode Playback>
    [[nodiscard]] const Flow::Graph &graph(void) const noexcept { return _graphs[static_cast<std::size_t>(Playback)].graph; }

    /** @brief Get an internal current beat range */
    template<PlaybackMode Playback>
    [[nodiscard]] const BeatRange &currentBeatRange(void) const noexcept { return _graphs[static_cast<std::size_t>(Playback)].currentBeatRange; }
    template<PlaybackMode Playback>
    [[nodiscard]] BeatRange &currentBeatRange(void) noexcept { return _graphs[static_cast<std::size_t>(Playback)].currentBeatRange; }


    /** @brief Get / Set the partition node */
    [[nodiscard]] Node *partitionNode(void) const noexcept { return _partitionNode; }
    void setPartitionNode(Node * const partitionNode) noexcept { _partitionNode = partitionNode; }

    /** @brief Get / Set the partition index */
    [[nodiscard]] std::uint32_t partitionIndex(void) const noexcept { return _partitionIndex; }
    void setPartitionIndex(const std::uint32_t partitionIndex) noexcept { _partitionIndex = partitionIndex; }


    /** @brief Setup processBeatSize & processBlockSize parameters with a desired processBeatSize */
    void setProcessParamByBeatSize(const Beat processBeatSize, const SampleRate sampleRate);

    /** @brief Setup processBeatSize & processBlockSize parameters with a desired processBlockSize. Return true if processBlockSize is used */
    void setProcessParamByBlockSize(const std::size_t processBlockSize, const SampleRate sampleRate);

    /** @brief Add apply event to be dispatched */
    template<typename Apply>
    void addEvent(Apply &&apply);

    /** @brief Add apply and notify events to be dispatched */
    template<typename Apply, typename Notify>
    void addEvent(Apply &&apply, Notify &&notify);


    /** @brief Get the currently used graph (depend of playback mode) */
    [[nodiscard]] const Flow::Graph &getCurrentGraph(void) const noexcept;
    [[nodiscard]] Flow::Graph &getCurrentGraph(void) noexcept
        { return const_cast<Flow::Graph &>(const_cast<const AScheduler *>(this)->getCurrentGraph()); }

    /** @brief Get the currently used beat range (depend of playback mode) */
    [[nodiscard]] const BeatRange &getCurrentBeatRange(void) const noexcept;
    [[nodiscard]] BeatRange &getCurrentBeatRange(void) noexcept
        { return const_cast<BeatRange &>(const_cast<const AScheduler *>(this)->getCurrentBeatRange()); }

    /** @brief Invalidates a graph */
    template<PlaybackMode Playback>
    void invalidateGraph(void);
    template<bool SetDirty = true>
    void invalidateCurrentGraph(void);


    /** @brief Callback called when scheduler is set to play */
    void onAudioProcessStarted(const BeatRange &beatRange);

    /** @brief Virtual callback called when a frame is generated
     *  @return true if the current graph has to stop */
    [[nodiscard]] virtual bool onAudioBlockGenerated(void) = 0;

    /** @brief Virtual callback called
     *  @return true if the current graph has to stop */
    [[nodiscard]] virtual bool onAudioQueueBusy(void) = 0;

    /** @brief Will wait until the processing graph is completed
     *  Never call this without setting state to 'Pause' during the whole wait call */
    void wait(void) noexcept_ndebug;

    /** @brief Init internal cache */
    void prepareCache(const AudioSpecs &specs);

    /** @brief Clear the overflow cache */
    void clearOverflowCache(void);

    /** @brief Clear the audio queue */
    void clearAudioQueue(void);


    /** @brief Will consume audio data from the global queue */
    static inline std::size_t ConsumeAudioData(std::uint8_t *data, const std::size_t size)
        { return _AudioQueue.popRange(data, data + size); }

protected:
    /** @brief Dispatch apply events without clearing event list */
    void dispatchApplyEvents(void);

    /** @brief Dispatch notify events and clear event list */
    void dispatchNotifyEvents(void);

private:
    // Cacheline 1
    // Virtual table pointer
    std::unique_ptr<Flow::Scheduler> _scheduler { std::make_unique<Flow::Scheduler>() };
    Core::TinyVector<Event> _events {};
    ProjectPtr _project {};
    Buffer _overflowCache {};
    PlaybackMode _playbackMode { PlaybackMode::Production };
    std::atomic<State> _state { State::Pause };

    // Cacheline 2
    std::size_t _processBlockSize { 0u };
    Audio::BeatRange _loopBeatRange {};
    bool _isLooping { false };
    Beat _processBeatSize { 0u };
    double _beatMissCount { 0.0 };
    double _beatMissOffset { 0.0 };
    Node *_partitionNode { nullptr };
    std::uint32_t _partitionIndex { 0 };
    std::array<bool, Audio::PlaybackModeCount> _dirtyFlags {};
    // 8 bytes

    // Cacheline 3
    PlaybackGraphs _graphs {};


    /** @brief Audio callback queue */
    static inline Core::SPSCQueue<std::uint8_t> _AudioQueue { 2048 * 4 * 4 };


    /** @brief Build a graph */
    template<Audio::PlaybackMode Playback>
    void buildGraph(void);

    /** @brief Build a node in a graph */
    template<Audio::PlaybackMode Playback>
    void buildNodeTask(const Node *node, std::pair<Flow::Task, const NoteEvents *> &parentNoteTask, std::pair<Flow::Task, const NoteEvents *> &parentAudioTask);


    /** @brief Will feed audio data into the global queue */
    bool produceAudioData(const BufferView output);

    bool flushOverflowCache(void);

    /** @brief Process looping, it modify the currentBeatRange */
    void processLooping(void);

    /** @brief Process the beat misses, it modify the incrementation of the currentBeatRange */
    void processBeatMiss(void);


    /** @brief Schedule the current graph */
    void scheduleCurrentGraph(void);
};

static_assert_sizeof(Audio::AScheduler, Core::CacheLineSize * 3);

#include "SchedulerTask.ipp"
#include "AScheduler.ipp"
