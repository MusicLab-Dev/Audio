/**
 * @ Author: Pierre Veysseyre
 * @ Description: PluginTable template implementation
 */

#include "InternalFactory.hpp"

inline void Audio::PluginTable::updateAudioParameters(const SampleRate sampleRate, const ChannelArrangement channelArrangement)
{
    for (auto &instance : _instances)
        instance->updateAudioParameters(sampleRate, channelArrangement);
}

template<typename Type, const char *Name, Audio::IPluginFactory::Tags FactoryTags>
Audio::IPluginFactory &Audio::PluginTable::registerFactory(void)
{
    _factories.push(std::make_unique<Audio::InternalFactory<Type, Name, FactoryTags>>());
    return *_factories.at(_factories.size() - 1);
}

inline Audio::PluginPtr Audio::PluginTable::instantiate(const std::string_view &view, const SampleRate sampleRate, const ChannelArrangement channelArrangement)
{
    for (auto &factory : _factories) {
        if (factory->getName() != view)
            continue;
        return instantiate(*factory, sampleRate, channelArrangement);
    }
    throw std::logic_error("Audio::PluginTable::instantiate: Plugin '" + std::string(view) + "' not found");
}

inline Audio::PluginPtr Audio::PluginTable::instantiate(IPluginFactory &factory, const SampleRate sampleRate, const ChannelArrangement channelArrangement)
{
    auto * const ptr = factory.instantiate();

    ptr->updateAudioParameters(sampleRate, channelArrangement);
    return PluginPtr(ptr);
}