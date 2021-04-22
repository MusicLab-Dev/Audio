/**
 * @ Author: Pierre Veysseyre
 * @ Description: Unit tests of Sampler plugin
 */

#include <gtest/gtest.h>
#include <iostream>

#include <Audio/Plugins/Sampler.hpp>

using namespace Audio;

TEST(Sampler, MetaData)
{
    const auto &meta = Sampler::GetMetaData();
    Sampler sampler;
    const auto &cSampler = sampler;

    ASSERT_EQ(meta.translations.names[0].lang, English);
    ASSERT_EQ(meta.translations.names[0].text, "Sampler");
    ASSERT_EQ(meta.translations.names[1].lang, French);
    ASSERT_EQ(meta.translations.names[1].text, "Sampleur");
    ASSERT_EQ(meta.controls.size(), Sampler::ControlCount);

    ASSERT_DOUBLE_EQ(cSampler.outputVolume(), 1.0);
    sampler.outputVolume() = 42.0;
    ASSERT_DOUBLE_EQ(cSampler.outputVolume(), 42.0);

    ASSERT_DOUBLE_EQ(cSampler.getControl(0), 42.0);
    sampler.getControl(0) = 0.24;
    ASSERT_DOUBLE_EQ(sampler.getControl(0), 0.24);

    ASSERT_DOUBLE_EQ(cSampler.getControl(1), 0.001);
    sampler.getControl(1) = 42.0;
    ASSERT_DOUBLE_EQ(sampler.getControl(1), 42.0);

    IPlugin &plugin(sampler);
    const IPlugin &cPlugin(plugin);

    ASSERT_DOUBLE_EQ(cPlugin.getControl(0), 0.24);
    plugin.getControl(0) = 42.0;
    ASSERT_DOUBLE_EQ(plugin.getControl(0), 42.0);

    ASSERT_DOUBLE_EQ(cPlugin.getControl(1), 42.0);
    plugin.getControl(1) = 0.24;
    ASSERT_DOUBLE_EQ(plugin.getControl(1), 0.24);

    const auto &pMeta = cPlugin.getMetaData();

    ASSERT_EQ(pMeta.translations.names[0].lang, English);
    ASSERT_EQ(pMeta.translations.names[0].text, "Sampler");
    ASSERT_EQ(pMeta.translations.names[1].lang, French);
    ASSERT_EQ(pMeta.translations.names[1].text, "Sampleur");
    ASSERT_EQ(pMeta.controls.size(), Sampler::ControlCount);

}
