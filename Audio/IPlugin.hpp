/*
 * Project: MusicLab
 * Author: Pierre Veysseyre
 * Description: IPlugin.hpp
 */

#pragma once

#include <Core/FlatVector.hpp>

#include "Buffer.hpp"
#include "Control.hpp"
#include "Note.hpp"
#include "IPluginFactory.hpp"

namespace Audio
{
    class IPlugin;

    /** @brief Languages tags */
    constexpr auto English = "EN";
    constexpr auto French = "FR";

    /** @brief A list of points events */
    using ControlEvents = Core::TinyVector<ControlEvent>;

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
    };

    struct ControlMetaData
    {
        TranslationMetaData translations;
    };

    using ControlMetaDataList = Core::TinyVector<ControlMetaData>;

    struct PluginMetaData
    {
        TranslationMetaData translations;
        ControlMetaDataList controls;
    };
};

class Audio::IPlugin
{
public:
    using Flags = IPluginFactory::Flags;

    /** @brief Virtual destructor */
    virtual ~IPlugin(void) noexcept = default;

    // virtual ParameterDescriptors getParameterDescriptors(void) const = 0;

    /** @brief  */
    static PluginFactoryPtr GetFactory(void);
    /** @brief  */
    static void SetFactory(PluginFactoryPtr factory);

    /** @brief  */
    virtual Flags getFlags(void) const noexcept = 0;


    /** @brief  */
    virtual void sendAudio(const BufferViews &inputs) noexcept = 0;
    /** @brief  */
    virtual void receiveAudio(BufferView output) noexcept = 0;

    /** @brief  */
    virtual void sendNotes(const NoteEvents &notes) noexcept = 0;
    /** @brief  */
    virtual void receiveNotes(NoteEvents &notes) noexcept = 0;

    /** @brief  */
    virtual void sendControls(const ControlEvents &controls) noexcept = 0;


    /** @brief Signal called when the generation of the audio block start */
    virtual void onAudioGenerationStarted(const BeatRange &range) noexcept = 0;


    /** @brief Various flags helpers */
    [[nodiscard]] inline bool hasAudioInput(void) const noexcept    { return static_cast<std::size_t>(getFlags()) & static_cast<std::size_t>(Flags::AudioInput); }
    [[nodiscard]] inline bool hasAudioOutput(void) const noexcept   { return static_cast<std::size_t>(getFlags()) & static_cast<std::size_t>(Flags::AudioOutput); }
    [[nodiscard]] inline bool hasNoteInput(void) const noexcept     { return static_cast<std::size_t>(getFlags()) & static_cast<std::size_t>(Flags::NoteInput); }
    [[nodiscard]] inline bool hasNoteOutput(void) const noexcept    { return static_cast<std::size_t>(getFlags()) & static_cast<std::size_t>(Flags::NoteOutput); }
    [[nodiscard]] inline bool hasControlInput(void) const noexcept  { return static_cast<std::size_t>(getFlags()) & static_cast<std::size_t>(Flags::ControlInput); }

public: // See REGISTER_PLUGIN in PluginUtils
    /** @brief Get a control value by serial ID (DO NOT REIMPLEMENT MANUALLY, see REGISTER_PLUGIN !) */
    [[nodiscard]] virtual ParamValue &getControl(const ParamID id) noexcept = 0;
    [[nodiscard]] virtual ParamValue getControl(const ParamID id) const noexcept = 0;

    /** @brief Get static meta-data about the plugin (DO NOT REIMPLEMENT MANUALLY, see REGISTER_PLUGIN !) */
    [[nodiscard]] virtual const PluginMetaData &getMetaData(void) const noexcept = 0;
};
