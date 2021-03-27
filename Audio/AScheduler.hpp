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

namespace Audio
{
    class AScheduler;

    using ApplyFunctor = Core::Functor<void(void)>;
    using NotifyFunctor = Core::Functor<void(void)>;
};

class Audio::AScheduler
{
public:
    enum class State {
        Pause, Play
    };
    /** @brief Structure of an internal event */
    struct Event
    {
        ApplyFunctor apply {}; // The apply event is called when the scheduler is IDLE
        NotifyFunctor notify {}; // The notify event is called when the scheduler is RUNNING, after 'apply' dispatcher
    };

    /** @brief Default constructor (will crash if you play without project !) */
    AScheduler(void);

    /** @brief Constructor */
    AScheduler(ProjectPtr &&project);

    /** @brief Virtual destructor */
    virtual ~AScheduler(void) = default;

    /** @brief Get / Set internal project */
    [[nodiscard]] ProjectPtr &project(void) noexcept { return _project; }
    void setProject(ProjectPtr &&project) noexcept { _project = std::move(project); }


    /** @brief Get the generation graph */
    [[nodiscard]] Flow::Graph &graph(void) noexcept { return _graph; }


    /** @brief Get / set internal state */
    [[nodiscard]] State state(void) const noexcept { return _state.load(); }
    bool setState(const State state) noexcept;

    /** @brief Get / set internal current beat range */
    [[nodiscard]] BeatRange currentBeatRange(void) const noexcept { return _currentBeatRange; }
    void setBeatRange(const BeatRange beatRange) noexcept;

    /** @brief Get / set internal process beat size */
    [[nodiscard]] std::uint32_t processBeatSize(void) const noexcept { return _processBeatSize; }
    // void setProcessBeatSize(const std::uint32_t size) noexcept;

    /** @brief Get / Set the process block size */
    [[nodiscard]] std::size_t processBlockSize(void) const noexcept { return _processBlockSize; }
    // bool setProcessBlockSize(const std::size_t processBlockSize) noexcept;

    /** @brief Get / Set the loop beat range */
    [[nodiscard]] BeatRange loopBeatRange(void) const noexcept { return _loopBeatRange; }
    void setLoopBeatRange(const BeatRange loopBeatRange) noexcept { _loopBeatRange = loopBeatRange; }

    /** @brief Get / Set the loop status */
    [[nodiscard]] bool isLooping(void) const noexcept { return _isLooping; }
    void setIsLooping(const bool isLooping) noexcept { _isLooping = isLooping; }


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


    /** @brief Invalidates the project graph */
    void invalidateProjectGraph(void);


    /** @brief Callback called when scheduler is set to play */
    void onAudioProcessStarted(const BeatRange &beatRange);

    /** @brief Virtual callback called when a frame is generated */
    virtual void onAudioBlockGenerated(void) = 0;

    /** @brief Virtual callback called  */
    virtual void onAudioQueueBusy(void) = 0;

    /** @brief Will wait until the processing graph is completed
     *  Never call this without setting state to 'Pause' during the whole wait call */
    void wait(void) noexcept_ndebug;

    /** @brief Init internal cache */
    void prepareCache(const AudioSpecs &specs);


    /** @brief Will consome audio data from the global queue */
    static inline std::size_t ConsumeAudioData(std::uint8_t *data, const std::size_t size)
        { return _AudioQueue.popRange(data, data + size); }

protected:
    /** @brief Dispatch apply events without clearing event list */
    void dispatchApplyEvents(void);

    /** @brief Dispatch notify events and clear event list */
    void dispatchNotifyEvents(void);

private:
    std::unique_ptr<Flow::Scheduler> _scheduler { std::make_unique<Flow::Scheduler>() };
    Flow::Graph _graph {};
    Core::TinyVector<Event> _events {};
    ProjectPtr _project {};
    std::atomic<State> _state { State::Pause };
    Buffer _overflowCache {};

    BeatRange _currentBeatRange {};
    Beat _processBeatSize { 0u };
    std::size_t _processBlockSize { 0u };

    /** @brief Used to loop over a specific BeatRange */
    Audio::BeatRange _loopBeatRange {};
    bool _isLooping { true };

    /** @brief Used to balance time between BeatRanges & sample */
    double _beatMissCount { 0.0 };
    double _beatMissOffset { 0.0 };

protected:
    static inline Core::SPSCQueue<std::uint8_t> _AudioQueue { 65536 };
private:
    /** @brief Cache stucture used to store recursive parameters */
    struct SpecsParams
    {
        std::uint32_t channelByteSize;
        SampleRate sampleRate;
        ChannelArrangement channelArrangement;
        Format format;
    };

    /** @brief Build the project graph */
    void buildProjectGraph(void);

    void buildNodeTask(const Node *node, std::pair<Flow::Task, const NoteEvents *> &parentNoteTask, std::pair<Flow::Task, const NoteEvents *> &parentAudioTask);


    /** @brief Will feed audio data into the global queue */
    bool produceAudioData(const BufferView output);

    bool flushOverflowCache(void);

    /** @brief Process looping, it modify the currentBeatRange */
    void processLooping(void);

    /** @brief Process the beat misses, it modify the incrementation of the currentBeatRange */
    void processBeatMiss(void);


public:
    /** @brief Schedule the project graph */
    void scheduleProjectGraph(void);
};

#include "AScheduler.ipp"
