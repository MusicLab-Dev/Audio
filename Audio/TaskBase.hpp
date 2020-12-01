/**
 * @ Author: Pierre Veysseyre
 * @ Description: TaskBase
 */

#pragma once


#include <iostream>

#include "Node.hpp"

namespace Audio
{
    class AScheduler;

    namespace Internal
    {
        class TaskBase;
    };
};


class Audio::Internal::TaskBase
{
public:
    /** @brief Construct the task from a node and a scheduler */
    TaskBase(const AScheduler *scheduler, Node *node) noexcept : _scheduler(scheduler), _node(node) {}

    /** @brief POD semantics */
    TaskBase(const TaskBase &other) noexcept = default;
    TaskBase(TaskBase &&other) noexcept = default;
    TaskBase &operator=(const TaskBase &other) noexcept = default;
    TaskBase &operator=(TaskBase &&other) noexcept = default;

    /** @brief Get the internal scheduler*/
    [[nodiscard]] const AScheduler &scheduler(void) const noexcept { return *_scheduler; }

    /** @brief Get the internal node */
    [[nodiscard]] const Node &node(void) const noexcept { return *_node; }
    [[nodiscard]] Node &node(void) noexcept { return *_node; }

private:
    const AScheduler *_scheduler { nullptr };
    Node *_node { nullptr };
};
