/**
 * @ Author: Pierre Veysseyre
 * @ Description: AScheduler
 */

#include "SchedulerTask.hpp"

using namespace Audio;

bool AScheduler::setState(const State state) noexcept
{
    switch (state) {
    case State::Pause:
        for (State expected = State::Play; !_state.compare_exchange_strong(expected, State::Pause);) {
            if (expected == State::Pause)
                return false;
        }
        break;
    case State::Play:
        for (State expected = State::Pause; !_state.compare_exchange_strong(expected, State::Play);) {
            if (expected == State::Play)
                return false;
        }
        scheduleProjectGraph();
        break;
    }
    return true;
}

#include <iostream>

void AScheduler::setProcessBeatSize(const std::uint32_t size) noexcept
{
    if (size == _processBeatSize)
        return;
    _processBeatSize = size;
}

void AScheduler::setBeatRange(const BeatRange range) noexcept
{
    if (range == _currentBeatRange)
        return;
    _currentBeatRange = range;
}

void AScheduler::buildNodeTask(const Node *node,
        std::pair<Flow::Task, const NoteEvents *> &parentNoteTask, std::pair<Flow::Task, const NoteEvents *> &parentAudioTask)
{
    std::cout << node->name().toStdString() << ":\n";
    if (node->children().empty()) {
        // std::cout << "make note&audio\n";
        auto task = MakeSchedulerTask<true, true>(_graph, node->flags(), this, const_cast<Node *>(node), parentNoteTask.second);
        task.first.setName(node->name().toStdString() + "_note&audio");
        task.first.succeed(parentNoteTask.first);
        task.first.precede(parentAudioTask.first);
        // std::cout << "bind note&audio: " << node->name().toStdString() << std::endl;
        // std::cout << "\t-> create audio (final)" << std::endl;
        // std::cout << "\t-> bind note&audio" << std::endl;
        return;
    }
    // std::cout << "make note\n";
    // std::cout << "make audio\n";
    auto noteTask = MakeSchedulerTask<true, false>(_graph, node->flags(), this, const_cast<Node *>(node), parentNoteTask.second);
    noteTask.first.setName(node->name().toStdString() + "_note");
    auto audioTask = MakeSchedulerTask<false, true>(_graph, node->flags(), this, const_cast<Node *>(node), parentNoteTask.second);
    audioTask.first.setName(node->name().toStdString() + "_audio");
    noteTask.first.succeed(parentNoteTask.first);
    // std::cout << "bind audio: " << node->name().toStdString() << std::endl;

    for (auto &child : node->children()) {
        buildNodeTask(child.get(), noteTask, audioTask);
    }
    // std::cout << "bind note: " << node->name().toStdString() << std::endl;
    audioTask.first.precede(parentAudioTask.first);
}

void AScheduler::buildProjectGraph(void)
{
    auto *parent = _project->master().get();

    auto conditional = _graph.emplace([this] {
        if (_overflowCache) {
            // std::cout << "overflow check\n";
            // The delayed data has been consumed
            if (flushOverflowCache()) {
                std::cout << " - flush successsss\n";
                // std::cout << _AudioQueue.size() << std::endl;
                _overflowCache.release();
                // std::cout << _overflowCache.operator bool() << std::endl;
                return true;
            // The delayed data must be re-delayed
            } else {
                std::cout << " - flush failed\n";
                return false;
            }
        // There is no data delayed
        } else {
            // std::cout << "do nothing\n";
            return true;
        }
    });
    auto noteTask = MakeSchedulerTask<true, false>(_graph, parent->flags(), this, parent, nullptr);
    noteTask.first.setName(parent->name().toStdString() + "_note");
    auto audioTask = MakeSchedulerTask<false, true>(_graph, parent->flags(), this, parent, nullptr);
    audioTask.first.setName(parent->name().toStdString() + "_audio");

    auto overflowTask = _graph.emplace([]{ /* @todo: test  sleep(50ns) */
        std::this_thread::sleep_for(std::chrono::nanoseconds(100));
        // std::cout << "<overflow>\n";
    });
    conditional.precede(overflowTask);
    conditional.precede(noteTask.first);
    // If master is the only node, connect his tasks
    if (parent->children().empty()) {
        noteTask.first.succeed(audioTask.first);
        return;
    }
    for (auto &child : parent->children()) {
        buildNodeTask(child.get(), noteTask, audioTask);
    }

}

/*

    // Static task
    _flow->emplace([](void) -> void {
    });


    // Switch task
    auto switchNode = _flow->emplace([](void) -> int {
        return 2; // Only case2 will follow
    });
    auto [case0, case1, case2] = _flow->emplace(
        [] {},
        [] {},
        [] {}
    );
    switchNode.precede(case0, case1, case2, switchNode);
*/