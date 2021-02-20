/**
 * @ Author: Pierre Veysseyre
 * @ Description: Unit tests of Instance class
 */

#include <gtest/gtest.h>

#include <Audio/Plugins/Oscillator.hpp>
#include <Audio/PluginTable.hpp>
#include <Audio/InternalFactory.hpp>

using namespace Audio;

TEST(PluginTable, Instantiation)
{
    PluginTable::Init();
    {
        auto &table = PluginTable::Get();
        auto name = Core::FlatString("osc");
        static const char n1[] { "osc" };

        auto &factory = table.registerFactory<Oscillator, n1, IPluginFactory::Tags::Synth>();
        auto p = table.instantiate(n1);
        auto p1 = table.instantiate(factory);

        EXPECT_EQ(factory.getName(), "osc");
        EXPECT_EQ(factory.getTags(), IPluginFactory::Tags::Synth);
        EXPECT_EQ(factory.getSDK(), IPluginFactory::SDK::Internal);
        EXPECT_EQ(factory.getPath(), "_path");
    }
    PluginTable::Destroy();
}