/**
 * @ Author: Pierre Veysseyre
 * @ Description: Compressor
 */

#pragma once

#include <Audio/PluginUtilsControlsVolume.hpp>
#include <Core/FlatVector.hpp>

namespace Audio
{
    class Compressor;
}

class Audio::Compressor final : public Audio::IPlugin
{
    REGISTER_PLUGIN(
        /* Plugin's name */
        TR_TABLE(
            TR(English, "Compressor"),
        ),
        /* Plugin description */
        TR_TABLE(
            TR(English, "Compressor allow to mix multiple audio source"),
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
        ),
        REGISTER_CONTROL_EFFECT_BYPASS(
            bypass
        ),
        REGISTER_CONTROL_FLOATING(
            threshold, 0.0, CONTROL_RANGE_STEP(-60.0, 0.0, 0.01),
            TR_TABLE(
                TR(English, "Threshold level"),
            ),
            TR_TABLE(
                TR(English, "Threshold level"),
            ),
            TR_TABLE(
                TR(English, "Thd")
            ),
            TR_TABLE(
                TR(English, "dB")
            )
        ),
        REGISTER_CONTROL_FLOATING(
            ratio, 2.0, CONTROL_RANGE_STEP(1.0, 10.0, 0.1),
            TR_TABLE(
                TR(English, "Compression ratio"),
            ),
            TR_TABLE(
                TR(English, "Compression ratio"),
            ),
            TR_TABLE(
                TR(English, "Rt")
            ),
            TR_TABLE(
                TR(English, "/1")
            )
        ),
        REGISTER_CONTROL_FLOATING(
            attack, 5.0, CONTROL_RANGE_STEP(0.01, 500.0, 0.01),
            TR_TABLE(
                TR(English, "Attack duration"),
            ),
            TR_TABLE(
                TR(English, "Attack duration"),
            ),
            TR_TABLE(
                TR(English, "Atk")
            ),
            TR_TABLE(
                TR(English, "ms")
            )
        ),
        REGISTER_CONTROL_FLOATING(
            release, 100.0, CONTROL_RANGE_STEP(1.0, 1'000.0, 0.1),
            TR_TABLE(
                TR(English, "Release duration"),
            ),
            TR_TABLE(
                TR(English, "Release duration"),
            ),
            TR_TABLE(
                TR(English, "Rel")
            ),
            TR_TABLE(
                TR(English, "ms")
            )
        ),
        REGISTER_CONTROL_EFFECT_BYPASS(
            smooth
        ),
        REGISTER_CONTROL_EFFECT_BYPASS(
            apply
        )
    )

public:
    /** @brief Plugin constructor */
    Compressor(const IPluginFactory *factory) noexcept : IPlugin(factory) {}

    virtual void sendAudio(const BufferViews &inputs);
    virtual void receiveAudio(BufferView output);

    virtual void onAudioGenerationStarted(const BeatRange &range);

private:
    Buffer _cache;
    Core::FlatVector<float> _levels;
    float _lastLevel {};
};

#include "Compressor.ipp"
