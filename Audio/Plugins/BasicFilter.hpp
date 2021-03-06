/**
 * @file BasicFilter.hpp
 * @brief Basic filter plugin using FIR method
 *
 * @author Pierre V
 * @date 2021-04-23
 */

#pragma once

#include <Audio/PluginUtils.hpp>
#include <Audio/DSP/FIR.hpp>

namespace Audio
{
    class BasicFilter;
}

class Audio::BasicFilter final : public Audio::IPlugin
{
    REGISTER_PLUGIN(
        /* Plugin's name */
        TR_TABLE(
            TR(English, "BasicFilter"),
            TR(French, "Filtre basique")
        ),
        /* Plugin description */
        TR_TABLE(
            TR(English, "Basic Filter allow to filter audio signal"),
            TR(French, "Le filtre basique permet de filtrer de l'audio")
        ),
        /* Plugin flags */
        FLAGS(AudioInput, AudioOutput),
        /* Plugin tags */
        TAGS(Filter),
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
        REGISTER_CONTROL_FILTER_CUTOFF(
            cutoffFrequency,
            5'000.0,
            CONTROL_RANGE_STEP(50.0, 22'000, 1.0)
        ),
        REGISTER_CONTROL_EFFECT_BYPASS(
            byBass
        ),
        REGISTER_CONTROL_ENUM(
            /* Control variable / getter / setter name */
            filterType,
            /* Control's range */
            CONTROL_ENUM_RANGE(
                TR_TABLE(
                    TR(English, "Low-pass"),
                    TR(French, "Passe bas")
                ),
                TR_TABLE(
                    TR(English, "High-pass"),
                    TR(French, "Passe haut")
                )
            ),
            /* Control name */
            TR_TABLE(
                TR(English, "Filter type"),
                TR(French, "Type de filtre")
            ),
            /* Control's description */
            TR_TABLE(
                TR(English, "Filter type"),
                TR(French, "Type de filtre")
            ),
            /* Control's short name */
            TR_TABLE(
                TR(English, "Type")
            ),
            /* Control's unit */
            TR_TABLE(
                TR(English, "hertz")
            )
        )
    )

public:
    /** @brief Plugin constructor */
    BasicFilter(const IPluginFactory *factory) noexcept : IPlugin(factory) {}

    virtual void sendAudio(const BufferViews &inputs);
    virtual void receiveAudio(BufferView output);

    virtual void onAudioGenerationStarted(const BeatRange &range);

private:
    DSP::FIR::BasicFilter<float> _filter;
    Buffer _cache;
};

#include "BasicFilter.ipp"
