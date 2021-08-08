/**
 * @ Author: Pierre Veysseyre
 * @ Description: Kick Plugin
 */

#include <Audio/PluginUtils.hpp>

namespace Audio
{
    class Kick;
}

class Audio::Kick final : public Audio::IPlugin
{
    REGISTER_PLUGIN(
        /* Plugin's name */
        TR_TABLE(
            TR(English, "Kick"),
            TR(French, "Kick")
        ),
        /* Plugin description */
        TR_TABLE(
            TR(English, "Generate on-demand kicks to play as notes"),
            TR(French, "Generate on-demand kicks to play as notes")
        ),
        /* Plugin flags */
        FLAGS(AudioOutput, NoteInput),
        /* Plugin tags */
        TAGS(Synth),
        /* Control list */
        REGISTER_CONTROL_OUTPUT_VOLUME(
            outputVolume,
            0.0,
            CONTROL_DEFAULT_OUTPUT_VOLUME_RANGE()
        )
    )

public:
    /** @brief Plugin constructor */
    Kick(const IPluginFactory *factory) noexcept : IPlugin(factory) {}

    /** @brief Generate an audio frame of the instrument */
    virtual void receiveAudio(BufferView output);

    /** @brief Send notes to the instrument */
    virtual void sendNotes(const NoteEvents &notes, const BeatRange &range);


    /** @brief Notify that audio parameters has changed */
    virtual void onAudioParametersChanged(void);

    /** @brief Notify that the audio generation starts now */
    virtual void onAudioGenerationStarted(const BeatRange &range);

private:

};