/**
 * @file GammaEqualizer.hpp
 * @brief Lambda filter plugin using FIR method
 *
 * @author Pierre V
 * @date 2021-04-23
 */

#pragma once

#include <Audio/PluginUtilsControlsEqualizer.hpp>
#include <Audio/DSP/FIR.hpp>

namespace Audio
{
    class GammaEqualizer;
}

class Audio::GammaEqualizer final : public Audio::IPlugin
{
public:
    static constexpr std::uint32_t BandCount = 10;

    REGISTER_PLUGIN(
        TR_TABLE(
            TR(English, "Equalizer"),
            TR(French, "Equaliseur")
        ),
        TR_TABLE(
            TR(English, "Equalizer allow to filter frequencies bands of an audio signal"),
            TR(French, "L'équaliseur permet de filtrer des plages de fréquences de l'audio")
        ),
        FLAGS(AudioInput, AudioOutput),
        TAGS(Filter),
        REGISTER_CONTROL_OUTPUT_VOLUME(
            outputVolume, 0.0,
            CONTROL_DEFAULT_OUTPUT_VOLUME_RANGE()
        ),
        REGISTER_CONTROL_INPUT_GAIN(
            inputGain, 0.0,
            CONTROL_DEFAULT_INPUT_GAIN_RANGE()
        ),
        REGISTER_CONTROL_EFFECT_BYPASS(
            bypass
        ),
        REGISTER_CONTROL_EQUALIZER_BANDS(
            frequenyBands,
            10, // ::BandCount
            CONTROL_EQUALIZER_BAND_DEFAULT_RANGE()
        )
    )

public:
    /** @brief Plugin constructor */
    GammaEqualizer(const IPluginFactory *factory) noexcept : IPlugin(factory) {}

    virtual void sendAudio(const BufferViews &inputs);
    virtual void receiveAudio(BufferView output);

    virtual void onAudioGenerationStarted(const BeatRange &range);

private:
    DSP::FIR::BandFilter<10u, float> _filter;
    Buffer _cache;
};

#include "GammaEqualizer.ipp"
