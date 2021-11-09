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
#include <Audio/Plugins/CutoffFilter.hpp>
#include <Audio/Plugins/BandFilter.hpp>
#include <Audio/Plugins/SigmaFilter.hpp>
#include <Audio/Plugins/GammaEqualizer.hpp>
#include <Audio/Plugins/SimpleDelay.hpp>
#include <Audio/Plugins/SimpleReverb.hpp>
#include <Audio/Plugins/Arpeggiator.hpp>
#include <Audio/Plugins/Chords.hpp>
// #include <Audio/Plugins/FMSynth.hpp>
#include <Audio/Plugins/Piano.hpp>
// #include <Audio/Plugins/Drums.hpp>
#include <Audio/Plugins/Kick.hpp>
#include <Audio/Plugins/Snare.hpp>
#include <Audio/Plugins/Hat.hpp>

#include <Audio/Plugins/Compressor.hpp>

Audio::PluginTable::PluginTable(void)
{
    // Groups
    registerFactory<Audio::Mixer>();
    registerFactory<Audio::Arpeggiator>();

    // Instruments
    registerFactory<Audio::Oscillator>();
    registerFactory<Audio::Sampler>();
    registerFactory<Audio::Piano>();
    registerFactory<Audio::Kick>();
    registerFactory<Audio::Snare>();
    registerFactory<Audio::Hat>();
    // registerFactory<Audio::FMX>();
    // registerFactory<Audio::Drums>();
    // registerFactory<Audio::Chords>();

    // Effects
    registerFactory<Audio::SimpleDelay>();
    registerFactory<Audio::SimpleReverb>();
    registerFactory<Audio::CutoffFilter>();
    registerFactory<Audio::BandFilter>();
    registerFactory<Audio::GammaEqualizer>();
    // registerFactory<Audio::SigmaFilter>();
    // registerFactory<Audio::LambdaFilter>();

    // Mixing
    registerFactory<Audio::Compressor>();
}
