/**
 * @ Author: Pierre Veysseyre
 * @ Description: Unit tests of Automations class
 */

#include <gtest/gtest.h>

#include <Audio/Automations.hpp>

using namespace Audio;

TEST(Automations, Basics)
{
    Automations automations;

    automations.resize(4);
    automations.headerCustomType().controlsOnTheFly.push(ControlEvent(2, 42.0));
    ASSERT_EQ(automations.size(), 4);
    ASSERT_EQ(automations.headerCustomType().controlsOnTheFly.front().paramID, 2);
    ASSERT_EQ(automations.headerCustomType().controlsOnTheFly.front().value, 42.0);
}
