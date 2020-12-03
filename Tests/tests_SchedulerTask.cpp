/**
 * @ Author: Pierre Veysseyre
 * @ Description: Unit tests of Scheduler task
 */

#include <gtest/gtest.h>

#include <Audio/SchedulerTask.hpp>

TEST(SchedulerTask, Basics)
{
    Audio::IPlugin::Flags flags = static_cast<std::size_t>(Audio::IPlugin::Flags::AudioInput)
        static_cast<std::size_t>(Audio::IPlugin::Flags::AudioOutput);
    Node node;
    auto &task = Audio::MakeSchedulerTask<false, false>(flags, nullptr, nullptr, nullptr);
}