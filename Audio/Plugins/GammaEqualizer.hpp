/**
 * @file GammaEqualizer.hpp
 * @brief Lambda filter plugin using FIR method
 *
 * @author Pierre V
 * @date 2021-04-23
 */

#pragma once

#include <Audio/PluginUtils.hpp>
#include <Audio/DSP/FIR.hpp>

namespace Audio
{
    class GammaEqualizer;
}

class Audio::GammaEqualizer final : public Audio::IPlugin
{
    REGISTER_PLUGIN(
        TR_TABLE(
            TR(English, "GammaEqualizer"),
            TR(French, "Equaliseur Gamma")
        ),
        TR_TABLE(
            TR(English, "Gamma equalizer allow to filter frequencies bands of an audio signal"),
            TR(French, "L'équaliseur Gamma permet de filtrer des plages de fréquences de l'audio")
        ),
        FLAGS(AudioInput, AudioOutput),
        TAGS(EQ),
        REGISTER_CONTROL_OUTPUT_VOLUME(
            outputVolume,
            DefaultPluginOutputVolume,
            CONTROL_OUTPUT_VOLUME_RANGE()
        )
    )

public:
    /** @brief Plugin constructor */
    GammaEqualizer(const IPluginFactory *factory) noexcept : IPlugin(factory) {}

    virtual void sendAudio(const BufferViews &inputs);
    virtual void receiveAudio(BufferView output);

    virtual void onAudioGenerationStarted(const BeatRange &range);

private:
    DSP::FIR::MultiFilter<2ul, float> _filter;
    Buffer _cache;
};

#include "GammaEqualizer.ipp"
