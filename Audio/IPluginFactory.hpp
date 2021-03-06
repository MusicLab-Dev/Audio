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
        MultipleExternalInputs  = 1 << 6
    };

    enum class SDK : std::uint32_t {
        Internal, External
    };

    enum class Tags : std::uint32_t {
        None            = 0,
        Effect          = 1,
        Analyzer        = 1 << 1,
        Delay           = 1 << 2,
        Distortion      = 1 << 3,
        Dynamics        = 1 << 4,
        EQ              = 1 << 5,
        Filter          = 1 << 6,
        Spatial         = 1 << 7,
        Generator       = 1 << 8,
        Mastering       = 1 << 9,
        Modulation      = 1 << 10,
        PitchShift      = 1 << 11,
        Restoration     = 1 << 12,
        Reverb          = 1 << 13,
        Surround        = 1 << 14,
        Tools           = 1 << 15,
        Network         = 1 << 16,
        Drum            = 1 << 17,
        Instrument      = 1 << 18,
        Piano           = 1 << 20,
        Sampler         = 1 << 21,
        Synth           = 1 << 22,
        External        = 1 << 23,
        Sequencer       = 1 << 24
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
