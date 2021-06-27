/**
 * @ Author: Pierre Veysseyre
 * @ Description: Unit tests of Automation class
 */

#include <gtest/gtest.h>

#include <Audio/Automation.hpp>

using namespace Audio;

TEST(Automation, Initialization)
{
    Automation automation;

    automation.resize(4);
    automation.headerCustomType().muted = false;
    automation.headerCustomType().paramID = 4u;
    ASSERT_EQ(automation.size(), 4);
    ASSERT_EQ(automation.headerCustomType().muted, false);
    ASSERT_EQ(automation.headerCustomType().paramID, 4u);
}
