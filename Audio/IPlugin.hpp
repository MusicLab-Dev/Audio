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
    using ExternalPaths = Core::TinyVector<std::string_view>;

    struct TranslationPair
    {
        std::string_view lang {};
        std::string_view text {};
    };

    using TranslationTable = Core::TinyVector<TranslationPair>;

    struct TranslationMetaData
    {
        TranslationTable names {};
        TranslationTable descriptions {};

        /** @brief Try to find a name */
        [[nodiscard]] std::string_view getName(const std::string_view &lang) const noexcept
        {
            if (const auto it = names.find([&lang](const auto &elem) { return elem.lang == lang; }); it != names.end())
                return it->text;
            return names.empty() ? std::string_view("Name-Error") : names[0].text;
        }

        /** @brief Try to find a description */
        [[nodiscard]] std::string_view getDescription(const std::string_view &lang) const noexcept
        {
            if (const auto it = descriptions.find([&lang](const auto &elem) { return elem.lang == lang; }); it != descriptions.end())
                return it->text;
            return descriptions.empty() ? std::string_view("Description-Error") : descriptions[0].text;
        }
    };

    struct ControlMetaData
    {
        TranslationMetaData translations;
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
};

class Audio::IPlugin
{
public:
    using Flags = IPluginFactory::Flags;

    /** @brief Virtual destructor */
    virtual ~IPlugin(void) noexcept = default;


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
    virtual void sendAudio(const BufferViews &inputs) { throw std::runtime_error("IPlugin::sendAudio: Not implemented"); }

    /** @brief  */
    virtual void receiveAudio(BufferView output) { throw std::runtime_error("IPlugin::receiveAudio: Not implemented"); }

    /** @brief  */
    virtual void sendNotes(const NoteEvents &notes) { throw std::runtime_error("IPlugin::sendNotes: Not implemented"); }

    /** @brief  */
    virtual void receiveNotes(NoteEvents &notes) { throw std::runtime_error("IPlugin::receiveNotes: Not implemented"); }

    /** @brief  */
    virtual void sendControls(const ControlEvents &controls) { throw std::runtime_error("IPlugin::sendControls: Not implemented"); }


    /** @brief Set a plugin's external paths (if flag SingleExternalInput or MultipleExternalInputs is set) */
    virtual void setExternalPaths(const ExternalPaths &paths) { throw std::runtime_error("IPlugin::setExternalPaths: Not implemented"); }


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
    AudioSpecs _specs;
};
