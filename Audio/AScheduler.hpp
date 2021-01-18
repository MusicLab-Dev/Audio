/**
 * @ Author: Pierre Veysseyre
 * @ Description: AScheduler
 */

#pragma once

#include <functional>
#include <atomic>
#include <future>

#include <Flow/Scheduler.hpp>

#include "Project.hpp"
#include "Buffer.hpp"

namespace Audio
{
    class AScheduler;

    using ApplyFunctor = std::function<void(void)>;
    using NotifyFunctor = std::function<void(void)>;
};

class alignas_cacheline Audio::AScheduler
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

    /** @brief Constructor */
    AScheduler(ProjectPtr &&project);

    /** @brief Virtual destructor */
    virtual ~AScheduler(void) = default;

    ProjectPtr &project(void) noexcept { return _project; }


    [[nodiscard]] Flow::Graph &graph(void) noexcept { return _graph; }


    /** @brief Get / set internal state */
    [[nodiscard]] State state(void) const noexcept { return _state.load(); }
    void setState(const State state) noexcept;

    /** @brief Get / set internal current beat range */
    [[nodiscard]] BeatRange currentBeatRange(void) const noexcept { return _currentBeatRange; }
    void setBeatRange(const BeatRange beatRange) noexcept;

    /** @brief Add apply event to be dispatched */
    template<typename Apply, typename Notify>
    void addEvent(Apply &&apply) { addEvent(std::forward<Apply>(), NotifyFunctor()); }

    /** @brief Add apply and notify events to be dispatched */
    template<typename Apply, typename Notify>
    void addEvent(Apply &&apply, Notify &&notify);


    /** @brief Invalidates the project graph */
    void invalidateProjectGraph(void);


    /** @brief Callback called when scheduler is set to play */
    void onAudioProcessStarted(const BeatRange &beatRange);

    /** @brief Virtual callback called when a frame is generated */
    virtual void onAudioBlockGenerated(void) = 0;

    /** @brief Will wait until the processing graph is completed
     *  Never call this without setting state to 'Pause' during the whole wait call */
    void wait(void) noexcept_ndebug;

protected:
    /** @brief Dispatch apply events without clearing event list */
    void dispatchApplyEvents(void);

    /** @brief Dispatch notify events and clear event list */
    void dispatchNotifyEvents(void);

private:
    std::unique_ptr<Flow::Scheduler> _scheduler { std::make_unique<Flow::Scheduler>() };
    Flow::Graph _graph {};
    Core::TinyVector<Event> _events {};
    BeatRange _currentBeatRange {};
    ProjectPtr _project {};
    std::atomic<State> _state { State::Pause };

    /** @brief Build the project graph */
    void buildProjectGraph(void);

    void buildNodeTask(const Node *node, std::pair<Flow::Task, const NoteEvents *> &parentNoteTask, std::pair<Flow::Task, const NoteEvents *> &parentAudioTask);

    /** @brief Schedule the project graph */
    void scheduleProjectGraph(void);
};

#include "AScheduler.ipp"
