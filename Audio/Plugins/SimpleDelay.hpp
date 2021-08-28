/**
 * @ Author: Pierre Veysseyre
 * @ Description: SimpleDelay
 */

#pragma once

#include <Audio/PluginUtils.hpp>
#include <Audio/DSP/Delay.hpp>

namespace Audio
{
    class SimpleDelay;
}

class Audio::SimpleDelay final : public Audio::IPlugin
{
    REGISTER_PLUGIN(
        /* Plugin's name */
        TR_TABLE(
            TR(English, "Delay"),
            TR(French, "Delay")
        ),
        /* Plugin description */
        TR_TABLE(
            TR(English, "SimpleDelay allow to mix multiple audio source"),
            TR(French, "Le mixeur permet de mixer plusieurs sources sonore")
        ),
        /* Plugin flags */
        FLAGS(AudioInput, AudioOutput),
        /* Plugin tags */
        TAGS(Delay),
        /* Control list */
        REGISTER_CONTROL_INPUT_GAIN(
            inputGain,
            0.0,
            CONTROL_DEFAULT_INPUT_GAIN_RANGE()
        ),
        REGISTER_CONTROL_OUTPUT_VOLUME(
            outputVolume,
            0.0,
            CONTROL_DEFAULT_OUTPUT_VOLUME_RANGE()
        ),
        REGISTER_CONTROL_EFFECT_BYPASS(
            bypass
        ),
        REGISTER_CONTROL_FLOATING(
            delayTime,
            0.0,
            // CONTROL_RANGE_STEP(0.0, 0.001, 0.0001),
            CONTROL_RANGE_STEP(0.0, 1.0, 0.001),
            TR_TABLE(
                TR(English, "Delay time"),
                TR(French, "Durée du delay")
            ),
            TR_TABLE(
                TR(English, "Delay time"),
                TR(French, "Durée du delay")
            ),
            TR_TABLE(
                TR(English, "Del")
            ),
            TR_TABLE(
                TR(English, "seconds")
            )
        ),
        REGISTER_CONTROL_EFFECT_SYNC_TEMPO(
            sync
        ),
        REGISTER_CONTROL_FLOATING(
            feedbackAmount,
            0.0,
            CONTROL_RANGE_STEP(0.0, 1.0, 0.01),
            TR_TABLE(
                TR(English, "Delay feedback"),
                TR(French, "Feed-back du delay")
            ),
            TR_TABLE(
                TR(English, "Delay feedback"),
                TR(French, "Feed-back du delay")
            ),
            TR_TABLE(
                TR(English, "FB")
            ),
            TR_TABLE(
                TR(English, "%")
            )
        ),
        REGISTER_CONTROL_FLOATING(
            mixRate,
            0.5,
            CONTROL_RANGE_STEP(0.0, 1.0, 0.01),
            TR_TABLE(
                TR(English, "Delay mix"),
                TR(French, "Mix du delay")
            ),
            TR_TABLE(
                TR(English, "Delay mix"),
                TR(French, "Mix du delay")
            ),
            TR_TABLE(
                TR(English, "Mix")
            ),
            TR_TABLE(
                TR(English, "%")
            )
        )
    )

public:
    /** @brief Plugin constructor */
    SimpleDelay(const IPluginFactory *factory) noexcept : IPlugin(factory) {}

    virtual void sendAudio(const BufferViews &inputs);
    virtual void receiveAudio(BufferView output);

    virtual void onAudioGenerationStarted(const BeatRange &range);

    virtual void onAudioParametersChanged(void);

private:
    DSP::DelayLineUnique<float, DSP::InternalPath::Both> _delay;
    DSP::DelayLineUnique<float, DSP::InternalPath::Default> _feedforward;
    DSP::DelayLineUnique<float, DSP::InternalPath::Feedback> _feedback;
    Buffer _inputCache;
};

#include "SimpleDelay.ipp"
