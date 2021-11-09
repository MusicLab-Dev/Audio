/**
 * @file Compressor.hpp
 * @brief Basic filter plugin using FIR method
 *
 * @author Pierre V
 * @date 2021-04-23
 */

#pragma once

#include <Audio/PluginUtils.hpp>

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
            TR(French, "Compresseur")
        ),
        /* Plugin description */
        TR_TABLE(
            TR(English, "Compressor allow you to reduce the audio signal's dynamic range"),
            TR(French, "Le compresseur vous permet de réduire la plage dynamique du signal audio")
        ),
        /* Plugin flags */
        FLAGS(AudioInput, AudioOutput),
        /* Plugin tags */
        TAGS(Mastering),
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
        REGISTER_CONTROL_NUMERIC(
            threshold, 0.0, CONTROL_RANGE_STEP(-48.0, 6.0, 0.1),
            TR_TABLE(
                TR(English, "Threshold level"),
                TR(French, "Niveau de seuil")
            ),
            TR_TABLE(
                TR(English, "Threshold level"),
                TR(French, "Niveau de seuil")
            ),
            TR_TABLE(
                TR(English, "THLD")
            ),
            TR_TABLE(
                TR(English, "dB")
            )
        ),
        REGISTER_CONTROL_NUMERIC(
            attack, 0.001, CONTROL_RANGE_STEP(0.0, 500.0, 0.1),
            TR_TABLE(
                TR(English, "Attack duration"),
                TR(French, "Temps d'attaque")
            ),
            TR_TABLE(
                TR(English, "Duration to reach the maximum gain reduction"),
                TR(French, "Durée pour atteindre le maximum de réduction du volume")
            ),
            TR_TABLE(
                TR(English, "Atk")
            ),
            TR_TABLE(
                TR(English, "ms")
            )
        ),
        REGISTER_CONTROL_NUMERIC(
            release, 20.0, CONTROL_RANGE_STEP(0.0, 500.0, 0.1),
            TR_TABLE(
                TR(English, "Release duration"),
                TR(French, "Temps de relachement")
            ),
            TR_TABLE(
                TR(English, "Duration to reach the minimum gain reduction"),
                TR(French, "Durée pour atteindre le minimum de réduction du volume")
            ),
            TR_TABLE(
                TR(English, "Rel")
            ),
            TR_TABLE(
                TR(English, "ms")
            )
        ),
        REGISTER_CONTROL_EFFECT_BYPASS(
            bypass
        ),
        REGISTER_CONTROL_NUMERIC(
            /* Control variable / getter / setter name */
            ratio, 4.0, CONTROL_RANGE_STEP(1.0, 20.0, 0.1),
            /* Control name */
            TR_TABLE(
                TR(English, "Compression ratio"),
                TR(French, "Facteur de compression")
            ),
            /* Control's description */
            TR_TABLE(
                TR(English, "The amount of gain reduction"),
                TR(French, "Facteur de reduction de volume")
            ),
            /* Control's short name */
            TR_TABLE(
                TR(English, "Ratio")
            ),
            /* Control's unit */
            TR_TABLE(
                TR(English, "")
            )
        ),
        REGISTER_CONTROL_ENUM(
            mode,
            CONTROL_ENUM_RANGE(
                TR_TABLE(
                    TR(English, "Peak"),
                    TR(French, "Peak")
                ),
                TR_TABLE(
                    TR(English, "RMS"),
                    TR(French, "RMS")
                )
            ),
            TR_TABLE(
                TR(English, "Compressor mode"),
                TR(French, "Mode du compresseur")
            ),
            TR_TABLE(
                TR(English, "Compressor mode"),
                TR(French, "Mode du compresseur")
            ),
            TR_TABLE(
                TR(English, "Mode")
            ),
            TR_TABLE(
                TR(English, "")
            )
        )
    )

public:

    struct Comp
    {
        float inputLevel { 0.0f };
        float envelopeLevel { 0.0f };
        float attackRate { 0.0f };
        float releaseRate { 0.0f };
        float thresholdDelta { 0.0f };
    };

    /** @brief Plugin constructor */
    Compressor(const IPluginFactory *factory) noexcept : IPlugin(factory) {}

    virtual void sendAudio(const BufferViews &inputs);
    virtual void receiveAudio(BufferView output);

    virtual void onAudioGenerationStarted(const BeatRange &range);

private:
    Buffer _cache;
    Comp _compressor;
};

#include "Compressor.ipp"
