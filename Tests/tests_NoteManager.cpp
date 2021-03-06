/**
 * @ Author: Pierre Veysseyre
 * @ Description: Unit tests of NoteManager class
 */

#include <gtest/gtest.h>

#include <Audio/Partition.hpp>
#include <Audio/Plugins/Managers/NoteManager.hpp>
#include <Audio/AScheduler.hpp>

using namespace Audio;

TEST(NoteManager, Initialization)
{
    NoteManager<DSP::EnvelopeType::ADSR> manager;

    Partition input {
        Note({ 0, 1 }, 0, 10, 0),
        Note({ 1, 2 }, 1, 20, 1),
        Note({ 2, 3 }, 2, 30, 2),
        Note({ 3, 4 }, 3, 40, 3)
    };

    // AScheduler sc;


    // manager.feedNotes(input);
    // auto activeNotes = manager.getActiveNotes();

    // EXPECT_EQ(activeNotes.size(), 4);

    // for (auto i = 0; i < 4; ++i) {
    //     EXPECT_EQ(activeNotes[i].active, true);
    //     EXPECT_EQ(activeNotes[i].velocity, (i + 1) * 10);
    //     EXPECT_EQ(activeNotes[i].tuning, i);
    //     EXPECT_EQ(activeNotes[i].key, i);
    // }

    // manager.resetCache();
    // activeNotes = manager.getActiveNotes();
    // EXPECT_EQ(activeNotes.size(), 0);
}
