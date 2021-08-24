/**
 * @ Author: Pierre Veysseyre
 * @ Description: SimpleReverb
 */

#pragma once

#include <Audio/PluginUtils.hpp>
#include <Audio/DSP/Delay.hpp>

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
            TR(English, "SimpleReverb allow to mix multiple audio source"),
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
            byBass
        ),
        REGISTER_CONTROL_FLOATING(
            reverbTime,
            0.0,
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
            feedbackRate,
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
    DSP::DelayLineParallel<float, DSP::InternalPath::Default, 4u> _combFilters;
    Core::TinyVector<DSP::DelayLineUnique<float, DSP::InternalPath::Feedback>> _allPassFilters;
    std::array<DSP::DelayLineAllPass<float>, 3u> _allPassTmp;
    Buffer _inputCache;
};

#include "SimpleReverb.ipp"
