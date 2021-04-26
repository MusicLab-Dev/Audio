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
public:
    static constexpr auto BandCount = 10ul;

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
        //(10) 32 - 64 - 128 - 256 - 512 - 1024 - 2048 - 4096 - 8192 - 16384
        // (9) 64 - 128 - 256 - 512 - 1024 - 2048 - 4096 - 8192 - 16384
        // (8) 128 - 256 - 512 - 1024 - 2048 - 4096 - 8192 - 16384
        // (8) 71 - 142 - 284 - 569 - 1186 - 2371 - 4743 - 9487
        // (7) 128 - 256 - 512 - 1024 - 2048 - 4096 - 8192 - 16384
        // (6) 128 - 256 - 512 - 1024 - 2048 - 4096 - 8192 - 16384
        // (5) 128 - 256 - 512 - 1024 - 2048 - 4096 - 8192 - 16384
        // (4) 128 - 256 - 512 - 1024 - 2048 - 4096 - 8192 - 16384
        // (3) 128 - 256 - 512 - 1024 - 2048 - 4096 - 8192 - 16384
        // (2) 128 - 256 - 512 - 1024 - 2048 - 4096 - 8192 - 16384
        // REGISTER_CONTROL_FILTER_CUTOFF(
        //     cutoffLow,
        //     CONTROL_EQUALIZER_BAND_DEFAULT_VALUE(),
        //     CONTROL_EQUALIZER_BAND_DEFAULT_RANGE()
        // ),

        // REGISTER_CONTROL_FILTER_CUTOFF(
        //     cutoffHigh,
        //     CONTROL_EQUALIZER_BAND_DEFAULT_VALUE(),
        //     CONTROL_EQUALIZER_BAND_DEFAULT_RANGE()
        // ),
        // REGISTER_CONTROL_FILTER_CUTOFF_DESCRIPTION(
        //     cutoff,
        //     CONTROL_EQUALIZER_BAND_DEFAULT_VALUE(),
        //     CONTROL_EQUALIZER_BAND_DEFAULT_RANGE(),
        //     "32Hz"
        // )
        // ,
        REGISTER_CONTROL_EQUALIZER_BANDS(
            frequenyBands,
            128,
            8,
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
    DSP::FIR::SerieFilter<BandCount, float> _filter;
    Buffer _cache;
};

#include "GammaEqualizer.ipp"
