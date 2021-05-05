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
}

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
    [[nodiscard]] const ProjectPtr &project(void) const noexcept { return _project; }
    void setProject(ProjectPtr &&project) noexcept { _project = std::move(project); }


    /** @brief Get / set internal state */
    [[nodiscard]] State state(void) const noexcept { return _state.load(); }
    bool setState(const State state) noexcept;


    /** @brief Get internal process beat size */
    [[nodiscard]] Beat processBeatSize(void) const noexcept { return _processBeatSize; }

    /** @brief Get the process block size */
    [[nodiscard]] BlockSize processBlockSize(void) const noexcept { return _processBlockSize; }


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

    /** @brief Setup processBeatSize & processBlockSize parameters with a desired processBlockSize */
    void setProcessParamByBlockSize(const BlockSize processBlockSize, const SampleRate sampleRate) noexcept;

    /** @brief Update tempo */
    void setBPM(const BPM bpm) noexcept;

    /** @brief Set the taget audio block size */
    void setAudioBlockSize(const BlockSize blockSize) noexcept;


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

    /** @brief Set all dirty flags to true */
    void setDirtyFlags(void) noexcept;

    /** @brief Will wait until the processing graph is completed
     *  Never call this without setting state to 'Pause' during the whole wait call */
    void wait(void) noexcept_ndebug;

    /** @brief Init internal cache */
    void prepareCache(const AudioSpecs &specs);

    /** @brief Clear the overflow cache */
    void clearOverflowCache(void);

    /** @brief Clear the audio queue */
    void clearAudioQueue(void);


    /** @brief Check if the graph is exited */
    [[nodiscard]] bool hasExitedGraph(void) const noexcept { return _hasExitedGraph; }


    /** @brief Will consume audio data from the global queue */
    bool consumeAudioData(std::uint8_t *data, const std::size_t size);

    /** @brief Get current elapsed beat from the last play */
    [[nodiscard]] Beat audioElapsedBeat(void) const noexcept { return _audioElapsedBeat.load(); }

protected:
    /** @brief Dispatch apply events without clearing event list */
    void dispatchApplyEvents(void);

    /** @brief Dispatch notify events and clear event list */
    void dispatchNotifyEvents(void);

    /** @brief Function that indicates that the graph is stopped
     *  This function must be called inside derived class
     *  A such mechanism is needed on top of Flow::Graph::running because running can be still true while
     * shutting down, thus an UI event can happen just before it exited and glitch the program */
    void graphExited(void) { _hasExitedGraph = true; }

private:
    // Cacheline 1
    // Virtual table pointer
    std::unique_ptr<Flow::Scheduler> _scheduler { std::make_unique<Flow::Scheduler>() };
    Core::TinyVector<Event> _events {}; // @todo Use two vectors instead of one
    ProjectPtr _project {};
    Buffer _overflowCache {};
    PlaybackMode _playbackMode { PlaybackMode::Production };
    std::atomic<State> _state { State::Pause };

    // Cacheline 2
    Audio::BeatRange _loopBeatRange {};
    Beat _processBeatSize { 0u };
    SampleRate _sampleRate { 0u };
    BlockSize _processBlockSize { 0u };
    bool _isLooping { false };
    bool _hasExitedGraph { true };
    std::uint32_t _partitionIndex { 0 };
    Node *_partitionNode { nullptr };
    double _beatMissCount { 0.0 };
    double _beatMissOffset { 0.0 };
    std::array<bool, Audio::PlaybackModeCount> _dirtyFlags {};
    // 8 bytes

    // Cacheline 3
    PlaybackGraphs _graphs {};

    // Cacheline 4 - High frequency atomic read / write
    std::atomic<Beat> _audioElapsedBeat { 0u }; // Represent elapsed beat since last play
    std::atomic<Beat> _audioBlockBeatSize { 0u }; // Used by the audio callback to determine how many beat elapsed
    BlockSize _audioBlockSize { 0u };
    double _audioBlockBeatMissCount { 0.0 };
    double _audioBlockBeatMissOffset { 0.0 };

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
    void processLooping(void) noexcept;

    /** @brief Process the beat misses, it modify the incrementation of the currentBeatRange */
    void processBeatMiss(void) noexcept;


    /** @brief Schedule the current graph */
    void scheduleCurrentGraph(void);

    /** @brief Compute a beat size */
    [[nodiscard]] Beat ComputeBeatSize(const BlockSize blockSize, const Tempo tempo, const SampleRate sampleRate, double &beatMissOffset) noexcept;

};

static_assert_sizeof(Audio::AScheduler, Core::CacheLineSize * 4);

#include "SchedulerTask.ipp"
#include "AScheduler.ipp"
