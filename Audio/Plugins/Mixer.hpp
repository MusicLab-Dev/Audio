/**
 * @ Author: Pierre Veysseyre
 * @ Description: Mixer
 */

#pragma once

#include <Audio/PluginUtilsControlsVolume.hpp>

namespace Audio
{
    class Mixer;
}

class Audio::Mixer final : public Audio::IPlugin
{
    REGISTER_PLUGIN(
        /* Plugin's name */
        TR_TABLE(
            TR(English, "Mixer"),
            TR(French, "Mixeur")
        ),
        /* Plugin description */
        TR_TABLE(
            TR(English, "Mix multiple audio source"),
            TR(French, "Mélange plusieurs sources audio")
        ),
        /* Plugin flags */
        FLAGS(AudioInput, AudioOutput),
        /* Plugin tags */
        TAGS(Mastering),
        /* Control list */
        REGISTER_CONTROL_OUTPUT_VOLUME(
            outputVolume,
            0.0,
            CONTROL_DEFAULT_OUTPUT_VOLUME_RANGE()
        ),
        REGISTER_CONTROL_INPUT_GAIN(
            inputGain,
            0.0,
            CONTROL_DEFAULT_INPUT_GAIN_RANGE()
        )
    )

public:
    /** @brief Plugin constructor */
    Mixer(const IPluginFactory *factory) noexcept : IPlugin(factory) {}

    virtual void sendAudio(const BufferViews &inputs);
    virtual void receiveAudio(BufferView output);

    virtual void onAudioGenerationStarted(const BeatRange &range);

private:
    Buffer _cache;
};

#include "Mixer.ipp"
