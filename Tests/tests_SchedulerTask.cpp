// /**
//  * @ Author: Pierre Veysseyre
//  * @ Description: Unit tests of Scheduler task
//  */

#include <gtest/gtest.h>

#include <Audio/SchedulerTask.hpp>

TEST(SchedulerTask, Basics)
{
    Flow::Graph graph;
    Audio::IPlugin::Flags flags = static_cast<Audio::IPlugin::Flags>(static_cast<std::size_t>(Audio::IPlugin::Flags::AudioInput)
        | static_cast<std::size_t>(Audio::IPlugin::Flags::AudioOutput));

    auto res = Audio::MakeSchedulerTask<false, false>(graph, flags, nullptr, nullptr, nullptr);
}