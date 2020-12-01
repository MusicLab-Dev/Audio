/**
 * @ Author: Pierre Veysseyre
 * @ Description: Unit tests of AScheduler class
 */

#include <gtest/gtest.h>

#include <Audio/AScheduler.hpp>
#include <Audio/Plugins/Oscillator.hpp>
#include <Audio/Plugins/SimpleDelay.hpp>
#include <Audio/Plugins/Mixer.hpp>

using namespace Audio;

class Scheduler : public AScheduler
{
public:
    Scheduler(ProjectPtr &&project) : AScheduler(std::move(project)) {}

    void onAudioBlockGenerated(void) { std::cout << "onAudioBlockGenerated\n"; }
};

TEST(AScheduler, Initialization)
{
    PluginTable::Init();
    {
        auto *mixer = new Mixer();
        auto *osc = new Oscillator();
        auto *delay = new SimpleDelay(4096, ChannelArrangement::Mono, 1);

        auto project = std::make_shared<Project>(Core::FlatString("project"));
        project->master() = std::make_unique<Node>(PluginPtr((IPlugin *)(mixer)));
        // project->master()->cache() = Buffer(4096, ChannelArrangement::Mono);

        // auto p = PluginPtr((IPlugin *)(delay));
        // parent->setPlugin(std::move(p));

        Scheduler scheduler(std::move(project));

        scheduler.invalidateProjectGraph();

    }
    PluginTable::Destroy();
}
