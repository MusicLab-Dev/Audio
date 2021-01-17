/**
 * @ Author: Pierre Veysseyre
 * @ Description: AScheduler
 */

#include "SchedulerTask.hpp"

using namespace Audio;

void AScheduler::setState(const State state) noexcept
{
    switch (state) {
    case State::Pause:
        _state = state;
        break;
    case State::Play:
        for (State expected = State::Pause; !_state.compare_exchange_strong(expected, State::Play);) {
            if (expected == State::Play)
                return;
        }
        scheduleProjectGraph();
        break;
    }
}

#include <iostream>

void AScheduler::setBeatRange(const BeatRange range) noexcept
{
    if (range == _currentBeatRange)
        return;
    _currentBeatRange = range;
    // for (auto it = _graph.begin(); it != _graph.end(); ++it) {
    //     auto workData = std::get_if<0>(&it->node()->workData);
    //     auto schedulerTask = (SchedulerTask<static_cast<IPlugin::Flags>(static_cast<std::size_t>(IPlugin::Flags::NoteInput) |
    //                                                         static_cast<std::size_t>(IPlugin::Flags::AudioOutput) |
    //                                                         static_cast<std::size_t>(IPlugin::Flags::ControlInput))
    //                                             , true, true>*)(&it->node()->workData);
    //     std::cout << workData << std::endl;
    //     std::cout << schedulerTask->noteStack()->size() << std::endl;
    // }
}

// void AScheduler::buildNoteTasks(const Node *node,
//         std::pair<Flow::Task, const NoteEvents *> &parentNoteTask, std::pair<Flow::Task, const NoteEvents *> &parentAudioTask,
//         bool isMaster)
// {
//     std::cout << "Build task: " << node << " - " << static_cast<std::size_t>(node->plugin()->getFlags()) << std::endl;
//     // Actual node has no children, we can create a audio task
//     if (node->children().empty()) {
//         auto task = MakeSchedulerTask<true, true>(_graph, node->flags(), this, const_cast<Node *>(node), parentNoteTask.second);
//         task.first.succeed(parentNoteTask.first);
//         task.first.precede(parentAudioTask.first);
//         std::cout << "\t-> create audio task (final)" << std::endl;
//         return;
//     }

//     // Create note task recursively on children
//     std::cout << "\t-> create note task" << std::endl;
//     if (!isMaster) {
//         auto noteTask = MakeSchedulerTask<true, false>(_graph, node->flags(), this, const_cast<Node *>(node), parentNoteTask.second);
//         noteTask.first.succeed(parentNoteTask.first);

//         std::cout << "\t-> create audio task" << std::endl;
//         auto audioTask = MakeSchedulerTask<false, true>(_graph, node->flags(), this, const_cast<Node *>(node), parentNoteTask.second);

//         std::cout << "\t- begin recursion children: " << node << std::endl;
//         for (auto &child : node->children()) {
//             buildNoteTasks(child.get(), noteTask, audioTask);
//         }
//     } else {
//         for (auto &child : node->children()) {
//             buildNoteTasks(child.get(), parentNoteTask, parentAudioTask);
//         }
//     }
//     std::cout << "\t- end recursion children: " << node << std::endl;

// }

void AScheduler::buildNodeTask(const Node *node,
        std::pair<Flow::Task, const NoteEvents *> &parentNoteTask, std::pair<Flow::Task, const NoteEvents *> &parentAudioTask)
{
    std::cout << node->name().toStdString() << ":\n";
    if (node->children().empty()) {
        // std::cout << "make note&audio\n";
        auto task = MakeSchedulerTask<true, true>(_graph, node->flags(), this, const_cast<Node *>(node), parentNoteTask.second);
        task.first.setName(node->name().toStdView() + "_note&audio");
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
    noteTask.first.setName(node->name().toStdView() + "_note");
    auto audioTask = MakeSchedulerTask<false, true>(_graph, node->flags(), this, const_cast<Node *>(node), parentNoteTask.second);
    audioTask.first.setName(node->name().toStdView() + "_audio");
    noteTask.first.succeed(parentNoteTask.first);
    // std::cout << "bind audio: " << node->name().toStdString() << std::endl;

    for (auto &child : node->children()) {
        buildNodeTask(child.get(), noteTask, audioTask);
    }
    // std::cout << "bind note: " << node->name().toStdString() << std::endl;
    audioTask.first.precede(parentAudioTask.first);
}

void AScheduler::buildNodeTask(const Node *node)
{
    // std::cout << node->name().toStdString() << ":" << std::endl;
    // std::cout << "make note\n";
    // std::cout << "make audio\n";
    auto noteTask = MakeSchedulerTask<true, false>(_graph, node->flags(), this, const_cast<Node *>(node), nullptr);
    noteTask.first.setName(node->name().toStdView() + "_note");
    auto audioTask = MakeSchedulerTask<false, true>(_graph, node->flags(), this, const_cast<Node *>(node), nullptr);
    audioTask.first.setName(node->name().toStdView() + "_audio");

    for (auto &child : node->children()) {
        buildNodeTask(child.get(), noteTask, audioTask);
    }
}

void AScheduler::buildProjectGraph(void)
{
    auto *parent = _project->master().get();

    buildNodeTask(parent);

    // Master -> controls / partitions
    // Mixer 1 -> controls / partitions
    // Arpeggiator 1 -> controls / partitions
    // Sampler 1 -> controls / partitions
    // Sampler 1 -> audio
    // Sampler 2 -> controls / partitions
    // Sampler 2 -> audio
    // Arpegiator 1 -> audio:
    /*  -> Check if node has audio ? -> No
          -> Check if node has at least one children with audio ? -> Yes
            -> Merge children
    */
    // Effect 1 -> controls / partitions
    // Sampler 3 -> controls / partitions
    // Sampler 3 -> audio
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