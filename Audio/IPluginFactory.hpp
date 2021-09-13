/*
 * @ Author: Pierre Veysseyre
 * @ Description: IPluginFactory
 */

#pragma once

#include <memory>

#include "Base.hpp"

namespace Audio
{
    class IPlugin;
    class IPluginFactory;

    using PluginFactoryPtr = std::unique_ptr<IPluginFactory>;

    using DLLSignature = IPluginFactory *(*)(void);
}

class Audio::IPluginFactory
{
public:
    enum class Flags : std::uint16_t {
        None                    = 0,
        AudioInput              = 1,
        AudioOutput             = 1 << 1,
        NoteInput               = 1 << 2,
        NoteOutput              = 1 << 3,
        SingleExternalInput     = 1 << 5,
        MultipleExternalInputs  = 1 << 6,
        NoChildren              = 1 << 7
    };

    enum class SDK : std::uint32_t {
        Internal, External
    };

    enum class Tags : std::uint32_t {
        None            = 0,

        // Groups
        Group           = 1,
        Mastering       = static_cast<int>(Group) | 1 << 1,
        Sequencer       = static_cast<int>(Group) | 1 << 2,

        // Instruments
        Instrument      = 1 << 10,
        Synth           = static_cast<int>(Instrument) | 1 << 11,
        Drum            = static_cast<int>(Instrument) | 1 << 12,
        Sampler         = static_cast<int>(Instrument) | 1 << 13,

        // Effects
        Effect          = 1 << 20,
        Filter          = static_cast<int>(Effect) | 1 << 21,
        Reverb          = static_cast<int>(Effect) | 1 << 22,
        Delay           = static_cast<int>(Effect) | 1 << 23,
        Distortion      = static_cast<int>(Effect) | 1 << 24,
    };

    static constexpr std::string_view DefaultLang = "EN";

    virtual ~IPluginFactory(void) = default;

    virtual std::string_view getName(const std::string_view &lang = DefaultLang) const = 0;
    virtual std::string_view getDescription(const std::string_view &lang = DefaultLang) const = 0;

    virtual std::string_view getPath(void) const = 0;

    virtual Flags getFlags(void) const = 0;

    virtual Tags getTags(void) const = 0;

    // virtual Capabilities getCapabilities(void) = 0;

    virtual SDK getSDK(void) const = 0;

    /** @brief Instantiate a new plugin */
    virtual IPlugin *instantiate(void) = 0;
};

// static_assert(alignof(Audio::IPluginFactory) == 16, "IPluginFactory must be aligned to 16 bytes !");
// static_assert(sizeof(Audio::IPluginFactory) == 16, "IPluginFactory must take 16 bytes !");
