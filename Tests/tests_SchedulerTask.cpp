// /**
//  * @ Author: Pierre Veysseyre
//  * @ Description: Unit tests of Scheduler task
//  */

#include <gtest/gtest.h>

#include <Audio/SchedulerTask.hpp>

#include <Audio/Plugins/Sampler.hpp>

#include <Core/FlatString.hpp>

using namespace Audio;

    class Scheduler : public AScheduler
{
public:
    Scheduler(ProjectPtr &&project) : AScheduler(std::move(project)) {}

    virtual ~Scheduler(void) override = default;

    virtual bool onAudioBlockGenerated(void) {
        std::cout << " <onAudioBlockGenerated> \n";
        return false;
    }
};

static constexpr SampleRate SR = 48000;
static constexpr BlockSize BS = 4096;


#include "DummyPlugin.hpp"

#define MAKE_DUMMY(PluginClass) \
    std::make_unique<Node>(PluginPtr(new PluginClass()));

#define MAKE_DUMMY_BIND(PluginClass, Parent) \
    parent.push(std::make_unique<Node>(PluginPtr(new PluginClass())));


TEST(SchedulerTask, NotesCollection)
{
    static constexpr auto MaxFrames = 2u;
    static constexpr auto FrameSize = 4u;

    GlobalCounter = 0;
    PluginTable::Init();
    {

        auto sampler = MAKE_DUMMY(Sampler);
        auto samplerP = reinterpret_cast<Sampler *>(sampler->plugin());

        // samplerP->loadSample<float>("/home/Pedro/Documents/AUDIO/Kick.wav");
        // samplerP->loadSample("/home/Pedro/Musique/Zaå Wezs.wav");


        // auto samplerBuffer = BufferView(samplerP->getBuffers()[0]);


        // return;
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
        master->children().push(std::move(mixer1));
        project->master() = std::move(master);

        Scheduler scheduler(std::move(project));

        scheduler.invalidateCurrentGraph();

        auto &graph = scheduler.getCurrentGraph();
        EXPECT_EQ(graph.size(), 6);


        // scheduler.graph().setRunning(true);
        for (auto i = 0u; i < MaxFrames; ++i) {
            std::cout << "=====\n\n";
            scheduler.getCurrentBeatRange() = BeatRange({ i * FrameSize, (i + 1) * FrameSize });
            scheduler.setState(AScheduler::State::Play);
            scheduler.setState(AScheduler::State::Pause);
            scheduler.wait();
            GlobalCounter++;
        }

        // return;

        auto ptrMaster = reinterpret_cast<DummyAudioIO *>(scheduler.project()->master()->plugin().get());
        EXPECT_EQ(ptrMaster->audioData.size(), MaxFrames * 2);
        auto ptrMixer1 = reinterpret_cast<DummyAudioIO *>(scheduler.project()->master()->children()[0]->plugin().get());
        EXPECT_EQ(ptrMixer1->audioData.size(), MaxFrames * 2);
        auto ptrOsc1 = reinterpret_cast<DummyNoteInAudioOut *>(scheduler.project()->master()->children()[0]->children()[0]->plugin().get());
        EXPECT_EQ(ptrOsc1->audioData.size(), MaxFrames);
        EXPECT_EQ(ptrOsc1->noteData.size(), MaxFrames);
        auto ptrOsc2 = reinterpret_cast<DummyNoteInAudioOut *>(scheduler.project()->master()->children()[0]->children()[1]->plugin().get());
        EXPECT_EQ(ptrOsc2->audioData.size(), MaxFrames);
        EXPECT_EQ(ptrOsc2->noteData.size(), MaxFrames);

        for (auto &i : ptrOsc1->noteData) {
            std::cout << "frame: " << i.frame << std::endl;
            std::cout << "dir: " << (i.dir == TestDataBase::Dir::In ? "in" : "out") << std::endl;
            std::cout << "notes: " << std::endl;
            for (auto n : i.noteData)
                std::cout << "  - " << n.key << " " << (n.type == NoteEvent::EventType::On ? "on" : (n.type == NoteEvent::EventType::Off ? "off" : "on&off")) << std::endl;
        }

        // EXPECT_EQ(scheduler.project()->master()->cache().size<float>(), 1);

        // for (auto i = 0; i < 10; ++i) {
        //     std::cout << scheduler.project()->master()->cache().data<float>(Channel::Mono)[i] << std::endl;
        // }

        // scheduler.project()->setBPM(60);

        // for (auto &child : scheduler.project()->master()->children()) {
        //     auto plugin = reinterpret_cast<std::size_t *>(child->plugin());
        //     auto dummy = reinterpret_cast<DummyPluginBase *>(&(plugin[1]));
        //     std::cout << child->name().toStdView() << ": " << std::endl;
        //     std::cout << "notes: " << dummy->noteData.size() <<std::endl;
        //     std::cout << "audio: " << dummy->audioData.size() <<std::endl;
        // }
    }
    PluginTable::Destroy();
}

// TEST(SchedulerTask, Controls)
// {
//     PluginTable::Init();
//     {
//         auto sampler = MAKE_DUMMY(Sampler);
//         sampler->setName(Core::FlatString("sampler1"));
//         auto samplerP = reinterpret_cast<Sampler *>(sampler->plugin());

//         auto master = MAKE_DUMMY(DummyAudioIO);
//         master->setName(Core::FlatString("master"));
//         auto &nodePtr = master->children().push(std::move(sampler));
//         Control control(0, 10);
//         Automation automation;
//         automation.points().push(0, 0);
//         automation.points().push(1, 3);
//         automation.points().push(2, 9);
//         automation.instances().push(BeatRange({ 0, 1 }));
//         control.automations().push(automation);
//         nodePtr->controls().push(std::move(control));

//         auto project = std::make_shared<Project>(Core::FlatString("project"));
//         project->master() = std::move(master);

//         Scheduler scheduler(std::move(project));

//         for (auto i = 0; i < 1; i++) {
//             scheduler.setBeatRange(BeatRange({ i * 3, (i + 1) * 3 }));
//             scheduler.invalidateProjectGraph();
//             scheduler.setState(Scheduler::State::Play);
//             scheduler.setState(Scheduler::State::Pause);
//             scheduler.wait();

//         }

//     }
//     PluginTable::Destroy();
// }
