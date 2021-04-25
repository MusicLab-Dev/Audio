/**
 * @ Author: Pierre Veysseyre
 * @ Description: Mixer
 */

#pragma once

#include <Audio/PluginControlUtils.hpp>

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
            TR(English, "Mixer allow to mix multiple audio source"),
            TR(French, "Le mixeur permet de mixer plusieurs sources sonore")
        ),
        /* Plugin flags */
        FLAGS(AudioInput, AudioOutput),
        /* Plugin tags */
        TAGS(Mastering),
        /* Control list */
        REGISTER_CONTROL_OUTPUT_VOLUME(
            masterVolume,
            0.0,
            CONTROL_RANGE(-100.0, 0.0)
        ),
        REGISTER_CONTROL(
            Boolean, ctrl, false, CONTROL_RANGE(false, true),
            TR_TABLE(
                TR(English, "Test Control")
            ),
            TR_TABLE(
                TR(English, "Don't forget to delete me later lol")
            )
        ),
        REGISTER_CONTROL(
            Integer, ctrl2, 0, CONTROL_RANGE_STEP(-100, 100, 15),
            TR_TABLE(
                TR(English, "Test Control 2")
            ),
            TR_TABLE(
                TR(English, "Don't forget to delete me later lol")
            )
        )
    )

public:
    /** @brief Plugin constructor */
    Mixer(const IPluginFactory *factory) noexcept : IPlugin(factory) {}

    virtual void sendAudio(const BufferViews &inputs);
    virtual void receiveAudio(BufferView output);

    virtual void onAudioGenerationStarted(const BeatRange &range) { _cache.clear(); }

private:
    BufferViews _cache;
};

#include "Mixer.ipp"
