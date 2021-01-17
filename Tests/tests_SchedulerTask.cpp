// /**
//  * @ Author: Pierre Veysseyre
//  * @ Description: Unit tests of Scheduler task
//  */

#include <gtest/gtest.h>

#include <Audio/SchedulerTask.hpp>

#include <Audio/Plugins/Oscillator.hpp>
#include <Audio/Plugins/SimpleDelay.hpp>
#include <Audio/Plugins/Mixer.hpp>

#include <Core/FlatString.hpp>

using namespace Audio;

    class Scheduler : public AScheduler
{
public:
    Scheduler(ProjectPtr &&project) : AScheduler(std::move(project)) {}

    virtual ~Scheduler(void) override = default;

    virtual void onAudioBlockGenerated(void) { std::cout << " <onAudioBlockGenerated> \n"; }
};

static constexpr SampleRate SR = 48000;
static constexpr BlockSize BS = 4096;


// TEST(SchedulerTask, Basics)
// {
//     Flow::Graph graph;
//     Audio::IPlugin::Flags flags = static_cast<Audio::IPlugin::Flags>(static_cast<std::size_t>(Audio::IPlugin::Flags::AudioInput)
//         | static_cast<std::size_t>(Audio::IPlugin::Flags::AudioOutput));

//     auto res = Audio::MakeSchedulerTask<false, false>(graph, flags, nullptr, nullptr, nullptr);
// }

// TEST(SchedulerTask, OneMixer)
// {
//     PluginTable::Init();
//     {
//         auto master = std::make_unique<Node>(PluginPtr(new Mixer()));
//         auto mixer1 = std::make_unique<Node>(PluginPtr(new Mixer()));
//         auto osc1 = std::make_unique<Node>(PluginPtr(new Oscillator()));
//         auto osc2 = std::make_unique<Node>(PluginPtr(new Oscillator()));
//         // auto delay1 = std::make_unique<Node>(PluginPtr(new SimpleDelay(BS, SR, ChannelArrangement::Mono, 1)));
//         // auto delay2 = std::make_unique<Node>(PluginPtr(new SimpleDelay(BS, SR, ChannelArrangement::Mono, 1)));

//         auto project = std::make_shared<Project>(Core::FlatString("project"));

//         // std::cout << "master: " << master.get() << std::endl;
//         // std::cout << "mixer1: " << mixer1.get() << std::endl;
//         // std::cout << "osc1: " << osc1.get() << std::endl;
//         // std::cout << "osc2: " << osc2.get() << std::endl;


//         /**
//          * Internal node tree:
//          *
//          *      Master
//          *        |
//          *      Mixer1
//          *     /     \
//          *  Osc1     Osc2
//          *
//          *
//          *
//          * Flow node tree:
//          *
//          *            Master
//          *            (notes)
//          *              |
//          *              |
//          *        ____Mixer1 ____
//          *       /     (notes)   \
//          *      |                |
//          *    Osc1              Osc2
//          *(note&audio)       (note&audio)
//          *     |                  |
//          *     \__________________/
//          *              |
//          *           Mixer1
//          *          (audio)
//          *              |
//          *              |
//          *            Master
//          *           (audio)
//          */
//         mixer1->children().push(std::move(osc1));
//         mixer1->children().push(std::move(osc2));
//         master->children().push(std::move(mixer1));
//         project->master() = std::move(master);

//         Scheduler scheduler(std::move(project));
//         scheduler.invalidateProjectGraph();

//         auto g = scheduler.graph();

//         EXPECT_EQ(g.size(), 6);
//         auto rootNode = g.begin()->node();
//         auto rootLinksFrom = rootNode->linkedFrom;
//         auto rootLinksTo = rootNode->linkedTo;
//         EXPECT_EQ(rootLinksFrom.size(), 0);
//         EXPECT_EQ(rootLinksTo.size(), 1);

//         auto mixer1node = rootLinksTo[0];
//         EXPECT_EQ(mixer1node->linkedFrom.size(), 1);
//         EXPECT_EQ(mixer1node->linkedTo.size(), 2);

//         // Parent node (master) check
//         EXPECT_EQ(mixer1node->linkedFrom[0]->linkedFrom.size(), 0);
//         EXPECT_EQ(mixer1node->linkedFrom[0]->linkedTo.size(), 1);

//         // Check link for osc1 & osc2
//         for (const auto &link : mixer1node->linkedTo) {
//             EXPECT_EQ(link->linkedFrom.size(), 1);
//             EXPECT_EQ(link->linkedTo.size(), 1);
//             // Mixer1 note node
//             EXPECT_EQ(link->linkedFrom[0]->linkedFrom.size(), 1);
//             EXPECT_EQ(link->linkedFrom[0]->linkedTo.size(), 2);
//             // Mixer1 audio node
//             EXPECT_EQ(link->linkedTo[0]->linkedFrom.size(), 2);
//             EXPECT_EQ(link->linkedTo[0]->linkedTo.size(), 1);
//             // Master audio node
//             EXPECT_EQ(link->linkedTo[0]->linkedTo[0]->linkedFrom.size(), 1);
//             EXPECT_EQ(link->linkedTo[0]->linkedTo[0]->linkedTo.size(), 0);
//         }
//     }
//     PluginTable::Destroy();
// }

// TEST(SchedulerTask, TwoMixer)
// {
//     PluginTable::Init();
//     {
//         auto master = std::make_unique<Node>(PluginPtr(new Mixer()));
//         auto mixer1 = std::make_unique<Node>(PluginPtr(new Mixer()));
//         auto mixer2 = std::make_unique<Node>(PluginPtr(new Mixer()));
//         auto osc1 = std::make_unique<Node>(PluginPtr(new Oscillator()));
//         auto osc2 = std::make_unique<Node>(PluginPtr(new Oscillator()));
//         auto project = std::make_shared<Project>(Core::FlatString("project"));
//         // std::cout << "master: " << master.get() << std::endl;
//         // std::cout << "mixer1: " << mixer1.get() << std::endl;
//         // std::cout << "osc1: " << osc1.get() << std::endl;
//         // std::cout << "osc2: " << osc2.get() << std::endl;
//         // std::cout << std::endl;

//         /**
//          * Internal node tree:
//          *
//          *      Master
//          *        |
//          *      Mixer2
//          *        |
//          *      Mixer1
//          *     /     \
//          *  Osc1     Osc2
//          *
//          *
//          *
//          * Flow node tree:
//          *
//          *            Master
//          *            (notes)
//          *              |
//          *            Mixer2
//          *            (notes)
//          *              |
//          *        ____Mixer1 ____
//          *       /     (notes)   \
//          *      |                |
//          *    Osc1              Osc2
//          *(note&audio)       (note&audio)
//          *     |                  |
//          *     \__________________/
//          *              |
//          *           Mixer1
//          *          (audio)
//          *              |
//          *           Mixer2
//          *          (audio)
//          *              |
//          *            Master
//          *           (audio)
//          */
//         mixer1->children().push(std::move(osc1));
//         mixer1->children().push(std::move(osc2));
//         mixer2->children().push(std::move(mixer1));
//         master->children().push(std::move(mixer2));
//         project->master() = std::move(master);

//         Scheduler scheduler(std::move(project));
//         scheduler.invalidateProjectGraph();

//         auto g = scheduler.graph();

//         EXPECT_EQ(g.size(), 8);
//         auto rootNode = g.begin()->node();
//         auto rootLinksFrom = rootNode->linkedFrom;
//         auto rootLinksTo = rootNode->linkedTo;
//         EXPECT_EQ(rootLinksFrom.size(), 0);
//         EXPECT_EQ(rootLinksTo.size(), 1);

//         auto mixer2node = rootLinksTo[0];
//         EXPECT_EQ(mixer2node->linkedFrom.size(), 1);
//         EXPECT_EQ(mixer2node->linkedTo.size(), 1);

//         // Parent node (master) check
//         EXPECT_EQ(mixer2node->linkedFrom[0]->linkedFrom.size(), 0);
//         EXPECT_EQ(mixer2node->linkedFrom[0]->linkedTo.size(), 1);

//         auto mixer1node = rootLinksTo[0]->linkedTo[0];

//         // Check link for osc1 & osc2
//         for (const auto &link : mixer1node->linkedTo) {
//             EXPECT_EQ(link->linkedFrom.size(), 1);
//             EXPECT_EQ(link->linkedTo.size(), 1);
//             // Mixer1 note node
//             EXPECT_EQ(link->linkedFrom[0]->linkedFrom.size(), 1);
//             EXPECT_EQ(link->linkedFrom[0]->linkedTo.size(), 2);
//             // Mixer1 audio node
//             EXPECT_EQ(link->linkedTo[0]->linkedFrom.size(), 2);
//             EXPECT_EQ(link->linkedTo[0]->linkedTo.size(), 1);
//             // Mixer2 audio node
//             EXPECT_EQ(link->linkedTo[0]->linkedTo[0]->linkedFrom.size(), 1);
//             EXPECT_EQ(link->linkedTo[0]->linkedTo[0]->linkedTo.size(), 1);
//             // Master audio node
//             EXPECT_EQ(link->linkedTo[0]->linkedTo[0]->linkedTo[0]->linkedFrom.size(), 1);
//             EXPECT_EQ(link->linkedTo[0]->linkedTo[0]->linkedTo[0]->linkedTo.size(), 0);
//         }
//     }
//     PluginTable::Destroy();
// }


#include "DummyPlugin.hpp"

#define MAKE_DUMMY(PluginClass) \
    std::make_unique<Node>(PluginPtr(new PluginClass()));

TEST(SchedulerTask, NotesCollection)
{
    static constexpr auto MaxFrames = 2u;
    static constexpr auto FrameSize = 4u;

    GlobalCounter = 0;
    PluginTable::Init();
    {
        auto master = MAKE_DUMMY(DummyAudioIO);
        master->setName(Core::FlatString("master"));
        auto mixer1 = MAKE_DUMMY(DummyAudioIO);
        mixer1->setName(Core::FlatString("mixer1"));
        auto osc1 = MAKE_DUMMY(DummyNoteInAudioOut);
        osc1->setName(Core::FlatString("osc1"));
        auto osc2 = MAKE_DUMMY(DummyNoteInAudioOut);
        osc2->setName(Core::FlatString("osc2"));
        // std::cout << "master: " << master.get() << std::endl;
        // std::cout << "mixer1: " << mixer1.get() << std::endl;
        // std::cout << "osc1: " << osc1.get() << std::endl;
        // std::cout << "osc2: " << osc2.get() << std::endl;

        auto project = std::make_shared<Project>(Core::FlatString("project"));

        Partition partition;
        partition.notes().push(Note(BeatRange({ 0, 2 })));
        partition.notes().push(Note(BeatRange({ 4, 6 })));
        partition.notes().push(Note(BeatRange({ 0, 4 }), 70));
        partition.notes().push(Note(BeatRange({ 2, 4 }), 71));
        partition.notes().push(Note(BeatRange({ 3, 5 }), 72));
        partition.instances().push(BeatRange({ 0, 6 }));
        // master->partitions().push(partition);
        mixer1->partitions().push(partition);
        // osc1->partitions().push(partition);
        // osc2->partitions().push(partition);


        mixer1->children().push(std::move(osc1));
        mixer1->children().push(std::move(osc2));
        // master->children().push(std::move(mixer1));
        project->master() = std::move(mixer1);

        Scheduler scheduler(std::move(project));

        scheduler.invalidateProjectGraph();

        auto graph = scheduler.graph();
        // EXPECT_EQ(graph.size(), 4);


        std::cout << "=====\n\n";
        for (auto i = 0u; i < MaxFrames; ++i) {
            scheduler.setBeatRange(BeatRange({ i * FrameSize, (i + 1) * FrameSize }));
            scheduler.setState(AScheduler::State::Play);
            scheduler.setState(AScheduler::State::Pause);
            scheduler.wait();
        }

        for (auto &child : scheduler.project()->master()->children()) {
            auto plugin = reinterpret_cast<std::size_t *>(child->getPlugin());
            auto dummy = reinterpret_cast<DummyPluginBase *>(&(plugin[1]));
            std::cout << child->name().toStdView() << ": " << std::endl;
            std::cout << "notes: " << dummy->noteData.size() <<std::endl;
            std::cout << "audio: " << dummy->audioData.size() <<std::endl;
        }
    }
    PluginTable::Destroy();
}
