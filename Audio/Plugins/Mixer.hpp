/**
 * @ Author: Pierre Veysseyre
 * @ Description: Mixer
 */

#pragma once

#include <Audio/PluginUtils.hpp>

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
        REGISTER_CONTROL(
            /* Control variable / getter / setter name */
            masterVolume,
            /* Control name */
            TR_TABLE(
                TR(English, "Master volume"),
                TR(French, "Volume master")
            ),
            /* Control's description */
            TR_TABLE(
                TR(English, "Output volume of the Mixer"),
                TR(French, "Volume de sortie du sampleur")
            )
        )
    )

public:
    virtual void sendAudio(const BufferViews &inputs);
    virtual void receiveAudio(BufferView output);

    virtual void onAudioGenerationStarted(const BeatRange &range) { _cache.clear(); }

private:
    BufferViews _cache;
};

#include "Mixer.ipp"
