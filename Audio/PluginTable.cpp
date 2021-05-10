/**
 * @ Author: Pierre Veysseyre
 * @ Description: PluginTable.cpp
 */

#include <stdexcept>

#include <Core/Assert.hpp>

#include "Project.hpp"
#include "PluginTable.hpp"

void Audio::PluginTable::addPlugin(IPlugin *plugin) noexcept_ndebug
{
    _instances.push(plugin);
    _counters.push(1u);
}

void Audio::PluginTable::incrementRefCount(IPlugin *plugin) noexcept_ndebug
{
    auto i = 0u;
    for (auto &instance : _instances) {
        if (plugin != instance) {
            ++i;
            continue;
        }
        ++_counters[i];
    }
}

void Audio::PluginTable::decrementRefCount(IPlugin *plugin) noexcept_ndebug
{
    auto i = 0u;
    for (auto instance : _instances) {
        if (plugin != instance) {
            ++i;
            continue;
        }
        if (!--_counters[i]) {
            delete _instances.at(i);
            _instances.erase(_instances.begin() + i);
            _counters.erase(_counters.begin() + i);
        }
    }
}

#include <Audio/Plugins/Mixer.hpp>
#include <Audio/Plugins/Sampler.hpp>
#include <Audio/Plugins/Oscillator.hpp>
#include <Audio/Plugins/LambdaFilter.hpp>
#include <Audio/Plugins/BasicFilter.hpp>
#include <Audio/Plugins/BandFilter.hpp>
#include <Audio/Plugins/SigmaFilter.hpp>
#include <Audio/Plugins/GammaEqualizer.hpp>
#include <Audio/Plugins/SimpleDelay.hpp>

Audio::PluginTable::PluginTable(void)
{
    registerFactory<Audio::Mixer>();
    registerFactory<Audio::Sampler>();
    registerFactory<Audio::Oscillator>();
    registerFactory<Audio::BandFilter>();
    registerFactory<Audio::BasicFilter>();
    registerFactory<Audio::SimpleDelay>();
    registerFactory<Audio::GammaEqualizer>();
    // registerFactory<Audio::LambdaFilter>();
    // registerFactory<Audio::SigmaFilter>();
}
