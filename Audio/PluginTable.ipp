/**
 * @ Author: Pierre Veysseyre
 * @ Description: PluginTable template implementation
 */

#include "InternalFactory.hpp"

inline void Audio::PluginTable::updateAudioSpecs(const AudioSpecs &audioSpecs)
{
    for (auto &instance : _instances)
        instance->updateAudioSpecs(audioSpecs);
}

template<typename Type, const char *Name, Audio::IPluginFactory::Tags FactoryTags>
Audio::IPluginFactory &Audio::PluginTable::registerFactory(void)
{
    _factories.push(std::make_unique<Audio::InternalFactory<Type, Name, FactoryTags>>());
    return *_factories.at(_factories.size() - 1);
}

inline Audio::PluginPtr Audio::PluginTable::instantiate(const std::string_view &path)
{
    if (auto factory = find(path); !factory)
        throw std::logic_error("Audio::PluginTable::instantiate: Plugin '" + std::string(path) + "' not found");
    else
        return instantiate(*factory);
}

inline Audio::PluginPtr Audio::PluginTable::instantiate(IPluginFactory &factory)
{
    auto * const ptr = factory.instantiate();

    return PluginPtr(ptr);
}

inline const Audio::IPluginFactory *Audio::PluginTable::find(const std::string_view &path) const noexcept
{
    for (auto &factory : _factories) {
        if (factory->getPath() != path)
            continue;
        return factory.get();
    }
    return nullptr;
}
