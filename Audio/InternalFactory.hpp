/**
 * @ Author: Pierre Veysseyre
 * @ Description: InternalFactory.hpp
 */

#pragma once

#include "IPluginFactory.hpp"

namespace Audio
{
    template<typename Plugin, const char *Name, Audio::IPluginFactory::Tags FactoryTags>
    class InternalFactory;
};

template<typename Plugin, const char *Name, Audio::IPluginFactory::Tags FactoryTags>
class Audio::InternalFactory final : public Audio::IPluginFactory
{
public:
    static inline const std::string Path = std::string("__internal__:/") + Name;

    InternalFactory(void) {}

    virtual std::string_view getName(void) noexcept final { return Name; }

    virtual std::string_view getPath(void) noexcept final { return std::string_view(Path); }

    virtual Tags getTags(void) noexcept final { return FactoryTags; }

    // virtual Capabilities getCapabilities(void) noexcept final;

    virtual SDK getSDK(void) noexcept final { return SDK::Internal; }

    [[nodiscard]] virtual IPlugin *instantiate(void) noexcept final { return new Plugin(); }

private:
};
