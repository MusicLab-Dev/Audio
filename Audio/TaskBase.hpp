/**
 * @ Author: Pierre Veysseyre
 * @ Description: TaskBase
 */

#pragma once

#include "Buffer.hpp"

#include <iostream>

namespace Audio
{
    class AScheduler;
    class Node;

    namespace Internal
    {
        class TaskScheduler;
    };
    class TaskBase;
    class TaskBaseScheduler;
};

class Audio::Internal::TaskScheduler
{
public:
    TaskScheduler(const AScheduler *scheduler) : _scheduler(scheduler) {}

    [[nodiscard]] const AScheduler &scheduler(void) const noexcept { return *_scheduler; }

private:
    const AScheduler *_scheduler { nullptr };

};

class Audio::TaskBase
{
public:
    TaskBase(Node *node) : _node(node) {}

    [[nodiscard]] const Node &node(void) const noexcept { return *_node; }
    [[nodiscard]] Node &node(void) noexcept { return *_node; }

private:
    Node *_node { nullptr };
};

class Audio::TaskBaseScheduler : public Audio::TaskBase, public Audio::Internal::TaskScheduler
{
public:
    TaskBaseScheduler(Node *node, const AScheduler *scheduler) : TaskBase(node), TaskScheduler(scheduler) {}

    // void f() {
    // }
};
