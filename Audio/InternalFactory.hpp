/**
 * @ Author: Pierre Veysseyre
 * @ Description: InternalFactory.hpp
 */

#pragma once

#include "IPluginFactory.hpp"

namespace Audio
{
    template<typename Plugin>
    class InternalFactory;
}

template<typename Plugin>
class Audio::InternalFactory final : public Audio::IPluginFactory
{
public:
    static inline const std::string Path = [] {
        return std::string("__internal__:/") += Plugin::MetaData().translations.getName(Audio::English);
    }();

    InternalFactory(void) = default;
    ~InternalFactory(void) final = default;

    virtual std::string_view getName(const std::string_view &lang) const final { return Plugin::MetaData().translations.getName(lang); }

    virtual std::string_view getDescription(const std::string_view &lang) const final { return Plugin::MetaData().translations.getDescription(lang); }

    virtual std::string_view getPath(void) const final { return std::string_view(Path); }

    virtual Flags getFlags(void) const final { return Plugin::MetaData().flags; }

    virtual Tags getTags(void) const final { return Plugin::MetaData().tags; }

    // virtual Capabilities getCapabilities(void) final;

    virtual SDK getSDK(void) const final { return SDK::Internal; }

    [[nodiscard]] virtual IPlugin *instantiate(void) final { return new Plugin(this); }

private:
};
