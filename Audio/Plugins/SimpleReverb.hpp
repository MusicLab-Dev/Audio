/**
 * @ Author: Pierre Veysseyre
 * @ Description: SimpleReverb
 */

#pragma once

#include <Audio/PluginUtils.hpp>
#include <Audio/DSP/Delay.hpp>
#include <Audio/DSP/Reverb.hpp>

namespace Audio
{
    class SimpleReverb;
}

class Audio::SimpleReverb final : public Audio::IPlugin
{
    REGISTER_PLUGIN(
        /* Plugin's name */
        TR_TABLE(
            TR(English, "Reverb"),
            TR(French, "Reverb")
        ),
        /* Plugin description */
        TR_TABLE(
            TR(English, "Add reverberation to audio"),
            TR(French, "Ajoute de la reverbération à l'audio")
        ),
        /* Plugin flags */
        FLAGS(AudioInput, AudioOutput),
        /* Plugin tags */
        TAGS(Reverb),
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
        ),
        REGISTER_CONTROL_EFFECT_BYPASS(
            bypass
        ),
        REGISTER_CONTROL_FLOATING(
            reverbTime,
            1.0,
            CONTROL_RANGE_STEP(0.0, 5.0, 0.01),
            TR_TABLE(
                TR(English, "Reverb time"),
                TR(French, "Durée du reverb")
            ),
            TR_TABLE(
                TR(English, "Reverb time"),
                TR(French, "Durée du reverb")
            ),
            TR_TABLE(
                TR(English, "Dur")
            ),
            TR_TABLE(
                TR(English, "seconds")
            )
        ),
        REGISTER_CONTROL_FLOATING(
            mixRate,
            0.5,
            CONTROL_RANGE_STEP(0.0, 1.0, 0.01),
            TR_TABLE(
                TR(English, "Reverb mix"),
                TR(French, "Mix du reverb")
            ),
            TR_TABLE(
                TR(English, "Reverb mix"),
                TR(French, "Mix du reverb")
            ),
            TR_TABLE(
                TR(English, "Mix")
            ),
            TR_TABLE(
                TR(English, "%")
            )
        ),
        REGISTER_CONTROL_FLOATING(
            preDelayTime,
            0.08,
            CONTROL_RANGE_STEP(0.0, 0.5, 0.001),
            TR_TABLE(
                TR(English, "Pre-delay duration"),
                TR(French, "Delai avant premières réflexions")
            ),
            TR_TABLE(
                TR(English, "Pre-delay duration"),
                TR(French, "Delai avant premières réflexions")
            ),
            TR_TABLE(
                TR(English, "Pre")
            ),
            TR_TABLE(
                TR(English, "seconds")
            )
        )
    )

public:
    /** @brief Plugin constructor */
    SimpleReverb(const IPluginFactory *factory) noexcept : IPlugin(factory) {}

    virtual void sendAudio(const BufferViews &inputs);
    virtual void receiveAudio(BufferView output);

    virtual void onAudioGenerationStarted(const BeatRange &range);

    virtual void onAudioParametersChanged(void);

private:
    Buffer _inputCache;
    DSP::Reverb::Basic<float> _reverb;
};

#include "SimpleReverb.ipp"
