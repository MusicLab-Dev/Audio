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
    enum class State : std::uint32_t {
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


    /** @brief Get the currently used graph (depend of playback mode) */
    [[nodiscard]] const Flow::Graph &graph(void) const noexcept { return _graphCache.graph; }
    [[nodiscard]] Flow::Graph &graph(void) noexcept { return _graphCache.graph; }

    /** @brief Get the currently used beat range (depend of playback mode) */
    [[nodiscard]] const BeatRange &currentBeatRange(void) const noexcept { return _graphCache.currentBeatRange; }
    [[nodiscard]] BeatRange &currentBeatRange(void) noexcept { return _graphCache.currentBeatRange; }


    /** @brief Get / Set the partition node */
    [[nodiscard]] Node *partitionNode(void) const noexcept { return _partitionNode; }
    void setPartitionNode(Node * const partitionNode) noexcept { _partitionNode = partitionNode; }

    /** @brief Get / Set the partition index */
    [[nodiscard]] std::uint32_t partitionIndex(void) const noexcept { return _partitionIndex; }
    void setPartitionIndex(const std::uint32_t partitionIndex) noexcept { _partitionIndex = partitionIndex; }

    /** @brief Get the sample rate */
    [[nodiscard]] SampleRate sampleRate(void) const noexcept { return _sampleRate; }

    /** @brief Setup processBeatSize & processBlockSize parameters with a desired processBlockSize */
    void setProcessParams(const BlockSize processBlockSize, const SampleRate sampleRate, const std::uint32_t cachedAudioFrames) noexcept;


    /** @brief Get the BPM / tempo */
    [[nodiscard]] BPM bpm(void) const noexcept { return _bpm; }
    [[nodiscard]] Tempo tempo(void) const noexcept { return _bpm / 60.0f; }

    /** @brief Update bpm */
    void setBPM(const BPM bpm) noexcept;


    /** @brief Add apply event to be dispatched */
    template<typename Apply>
    void addEvent(Apply &&apply);

    /** @brief Add apply and notify events to be dispatched */
    template<typename Apply, typename Notify>
    void addEvent(Apply &&apply, Notify &&notify);


    /** @brief Invalidates graph */
    template<PlaybackMode Playback>
    void invalidateGraph(void);
    template<bool SetDirty = true>
    void invalidateCurrentGraph(void);


    /** @brief Callback called when scheduler is set to play */
    void onAudioProcessStarted(const BeatRange &beatRange);

    /** @brief Virtual callback called when a frame is generated
     *  @return true if the current graph has to stop */
    [[nodiscard]] virtual bool onAudioBlockGenerated(void) = 0;

    /** @brief Virtual callback called when a frame couldn't be inserted into the audio queue
     *  @return true if the current graph has to stop */
    [[nodiscard]] virtual bool onAudioQueueBusy(void) = 0;

    /** @brief Virtual callback called when an export block has been generated
     *  @return true if the export graph has to stop */
    [[nodiscard]] virtual bool onExportBlockGenerated(void) = 0;

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


    /** @brief Get the current beat miss offset / count */
    [[nodiscard]] float beatMissOffset(void) const noexcept { return _beatMissOffset; }
    [[nodiscard]] float beatMissCount(void) const noexcept { return _beatMissCount; }
    [[nodiscard]] float beatMissCorrection(void) const noexcept { return _beatMissCorrection; }


    /** @brief Start the exportation of the project */
    void exportProject(void) noexcept;


    /** @brief Compute a beat size out of a sample size */
    [[nodiscard]] static Beat ComputeBeatSize(const std::uint32_t sampleSize, const Tempo tempo, const SampleRate sampleRate, float &beatMissOffset) noexcept;

    /** @brief Compute a sample size out of a beat size */
    [[nodiscard]] static std::uint32_t ComputeSampleSize(const Beat beatSize, const Tempo tempo, const SampleRate sampleRate, const float beatMissOffset, const float beatMissCount) noexcept;

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
    std::uint32_t _partitionIndex { 0u };
    Node *_partitionNode { nullptr };
    float _beatMissCount { 0.0f };
    float _beatMissOffset { 0.0f };
    float _beatMissCorrection { 0.0f };
    std::uint32_t _processLoopCrop { 0u };
    BPM _bpm { 0.0f };
    PlaybackGraph _graphCache {};

    // Cacheline 3 - High frequency atomic read / write
    std::atomic<Beat> _audioElapsedBeat { 0u }; // Represent elapsed beat since last play
    std::atomic<Beat> _audioBlockBeatSize { 0u }; // Used by the audio callback to determine how many beat elapsed
    BlockSize _audioBlockSize { 0u };
    std::uint32_t _cachedAudioFrames { 0u };
    float _audioBlockBeatMissCount { 0.0f };
    float _audioBlockBeatMissOffset { 0.0f };

    /** @brief Audio callback queue */
    static inline Core::SPSCQueue<std::uint8_t> _AudioQueue {};


    /** @brief Build a graph */
    template<Audio::PlaybackMode Playback>
    void buildGraph(void);

    /** @brief Build a node in a graph */
    template<Audio::PlaybackMode Playback>
    void buildNodeTask(const Node *node, std::pair<Flow::Task, const NoteEvents *> &parentNoteTask, std::pair<Flow::Task, const NoteEvents *> &parentAudioTask);


    /** @brief Will feed audio data into the global queue */
    [[nodiscard]] bool produceAudioData(const BufferView output);

    /** @brief Tries to flush to overflow cache */
    [[nodiscard]] bool flushOverflowCache(void);

    /** @brief Process looping, it modify the currentBeatRange */
    void processLooping(void) noexcept;

    /** @brief Process the beat misses, it modify the incrementation of the currentBeatRange */
    void processBeatMiss(void) noexcept;


    /** @brief Schedule the current graph */
    void scheduleCurrentGraph(void);

    /** @brief Callback emited by graph when completed */
    [[nodiscard]] bool onPlaybackGraphCompleted(void) noexcept;

    /** @brief Callback emited by graph when completed */
    [[nodiscard]] bool onExportGraphCompleted(void) noexcept;
};

static_assert_alignof_cacheline(Audio::AScheduler);
static_assert_sizeof(Audio::AScheduler, Core::CacheLineSize * 3);

#include "SchedulerTask.ipp"
#include "AScheduler.ipp"
