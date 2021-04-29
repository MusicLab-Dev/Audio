/*
 * Project: MusicLab
 * Author: Pierre Veysseyre
 * Description: IPlugin.hpp
 */

#pragma once

#include <Core/FlatVector.hpp>

#include "Buffer.hpp"
#include "Controls.hpp"
#include "Notes.hpp"
#include "IPluginFactory.hpp"

namespace Audio
{
    class IPlugin;

    /** @brief Languages tags */
    constexpr auto English = "EN";
    constexpr auto French = "FR";

    /** @brief A list of paths */
    using ExternalPaths = Core::TinyVector<std::string>;

    struct TranslationPair
    {
        std::string_view lang {};
        std::string_view text {};
    };

    using TranslationTable = Core::TinyVector<TranslationPair>;

    /** @brief Try to find a translation */
    [[nodiscard]] inline std::string_view FindTranslation(const TranslationTable &table, const std::string_view &lang)
    {
        if (const auto it = table.find([&lang](const auto &elem) { return elem.lang == lang; }); it != table.end())
            return it->text;
        return std::string_view();
    }

    struct TranslationMetaData
    {
        TranslationTable names {};
        TranslationTable descriptions {};

        /** @brief Try to find a name */
        [[nodiscard]] std::string_view getName(const std::string_view &lang) const noexcept
        {
            std::string_view str;
            if (names.empty())
                str = std::string_view("Name-Error");
            else {
                str = FindTranslation(names, lang);
                if (str.empty())
                    str = names[0].text;
            }
            return str;
        }

        /** @brief Try to find a description */
        [[nodiscard]] std::string_view getDescription(const std::string_view &lang) const noexcept
        {
            std::string_view str;
            if (descriptions.empty())
                str = std::string_view("Description-Error");
            else {
                str = FindTranslation(names, lang);
                if (str.empty())
                    str = descriptions[0].text;
            }
            return str;
        }
    };

    struct ControlMetaData
    {
        using RangeNames = Core::TinyVector<TranslationTable>;

        struct RangeValues
        {
            ParamValue min {};
            ParamValue max {};
            ParamValue step {};
        };

        TranslationMetaData translations;
        ParamType type;
        ParamValue defaultValue;
        RangeValues rangeValues;
        RangeNames rangeNames;
    };

    using ControlMetaDataList = Core::TinyVector<ControlMetaData>;

    struct alignas_cacheline PluginMetaData
    {
        TranslationMetaData translations;
        ControlMetaDataList controls;
        Audio::IPluginFactory::Flags flags;
        Audio::IPluginFactory::Tags tags;
    };

    static_assert_fit_cacheline(PluginMetaData);
}

class Audio::IPlugin
{
public:
    using Flags = IPluginFactory::Flags;

    /** @brief Plugin constructor */
    IPlugin(const IPluginFactory *factory) noexcept : _factory(factory) {}

    /** @brief Virtual destructor */
    virtual ~IPlugin(void) noexcept = default;


    /** @brief Get related factory */
    [[nodiscard]] const IPluginFactory *factory(void) const noexcept { return _factory; }


    /** @brief Get the audio specs of this plugin */
    [[nodiscard]] AudioSpecs &audioSpecs(void) noexcept { return _specs; }
    [[nodiscard]] const AudioSpecs &audioSpecs(void) const noexcept { return _specs; }

    /** @brief Update internal audio specs */
    void updateAudioSpecs(const AudioSpecs &specs)
    {
        if (_specs == specs)
            return;
        _specs = specs;
        onAudioParametersChanged();
    }

    /** @brief Virtual callback called on audio parameter changes */
    virtual void onAudioParametersChanged(void) {}


    /** @brief Get the sample rate in plugin's cache */
    [[nodiscard]] inline SampleRate sampleRate(void) const noexcept { return _specs.sampleRate; }

    /** @brief Get the channel arrangement in plugin's cache */
    [[nodiscard]] inline ChannelArrangement channelArrangement(void) const noexcept { return _specs.channelArrangement; }

    /** @brief Get the channel arrangement in plugin's cache */
    [[nodiscard]] inline Format format(void) const noexcept { return _specs.format; }


    /** @brief  */
    [[nodiscard]] Flags getFlags(void) const noexcept { return getMetaData().flags; };


    /** @brief  */
    virtual void sendAudio(const BufferViews &inputs) { UNUSED(inputs); throw std::runtime_error("IPlugin::sendAudio: Not implemented"); }

    /** @brief  */
    virtual void receiveAudio(BufferView output) { UNUSED(output); throw std::runtime_error("IPlugin::receiveAudio: Not implemented"); }

    /** @brief  */
    virtual void sendNotes(const NoteEvents &notes) { UNUSED(notes); throw std::runtime_error("IPlugin::sendNotes: Not implemented"); }

    /** @brief  */
    virtual void receiveNotes(NoteEvents &notes) { UNUSED(notes); throw std::runtime_error("IPlugin::receiveNotes: Not implemented"); }

    /** @brief  */
    virtual void sendControls(const ControlEvents &controls) { UNUSED(controls); throw std::runtime_error("IPlugin::sendControls: Not implemented"); }


    /** @brief Get / Set a plugin's external paths (if flag SingleExternalInput or MultipleExternalInputs is set) */
    virtual const ExternalPaths &getExternalPaths(void) const { throw std::runtime_error("IPlugin::getExternalPaths: Not implemented"); }
    virtual void setExternalPaths(const ExternalPaths &paths) { UNUSED(paths); throw std::runtime_error("IPlugin::setExternalPaths: Not implemented"); }


    /** @brief Signal called when the generation of the audio block start */
    virtual void onAudioGenerationStarted(const BeatRange &range) = 0;


    /** @brief Various flags helpers */
    [[nodiscard]] inline bool hasAudioInput(void) const noexcept    { return static_cast<std::size_t>(getFlags()) & static_cast<std::size_t>(Flags::AudioInput); }
    [[nodiscard]] inline bool hasAudioOutput(void) const noexcept   { return static_cast<std::size_t>(getFlags()) & static_cast<std::size_t>(Flags::AudioOutput); }
    [[nodiscard]] inline bool hasNoteInput(void) const noexcept     { return static_cast<std::size_t>(getFlags()) & static_cast<std::size_t>(Flags::NoteInput); }
    [[nodiscard]] inline bool hasNoteOutput(void) const noexcept    { return static_cast<std::size_t>(getFlags()) & static_cast<std::size_t>(Flags::NoteOutput); }

public: // See REGISTER_PLUGIN in PluginUtils
    /** @brief Get a control value by serial ID (DO NOT REIMPLEMENT MANUALLY, see REGISTER_PLUGIN !) */
    [[nodiscard]] virtual ParamValue &getControl(const ParamID id) noexcept = 0;
    [[nodiscard]] virtual ParamValue getControl(const ParamID id) const noexcept = 0;

    /** @brief Get static meta-data about the plugin (DO NOT REIMPLEMENT MANUALLY, see REGISTER_PLUGIN !) */
    [[nodiscard]] virtual const PluginMetaData &getMetaData(void) const noexcept = 0;

private:
    const IPluginFactory *_factory { nullptr };
    AudioSpecs _specs;
};
