/**
 * @file SigmaFilter.hpp
 * @brief Sigma filter plugin using FIR method
 *
 * @author Pierre V
 * @date 2021-04-23
 */

#pragma once

#include <Audio/PluginUtils.hpp>
#include <Audio/DSP/Biquad.hpp>

namespace Audio
{
    class SigmaFilter;
}

class Audio::SigmaFilter final : public Audio::IPlugin
{
    REGISTER_PLUGIN(
        /* Plugin's name */
        TR_TABLE(
            TR(English, "SigmaFilter"),
            TR(French, "Filtre Sigma")
        ),
        /* Plugin description */
        TR_TABLE(
            TR(English, "SigmaFilter allow to filter audio signal"),
            TR(French, "Le filtre Sigma permet de filtrer de l'audio")
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
            cutoffFrequencyFrom,
            120.0,
            CONTROL_FILTER_CUTOFF_DEFAULT_RANGE()
        ),
        REGISTER_CONTROL_FILTER_CUTOFF(
            cutoffFrequencyTo,
            1000.0,
            CONTROL_FILTER_CUTOFF_DEFAULT_RANGE()
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
                ),
                TR_TABLE(
                    TR(English, "Band-pass"),
                    TR(French, "Passe bande")
                ),
                TR_TABLE(
                    TR(English, "Band-stop"),
                    TR(French, "Stop bande")
                )
            ),
            /* Control name */
            TR_TABLE(
                TR(English, "Second cutoff frequency"),
                TR(French, "Deuxième fréquence de coupure")
            ),
            /* Control's description */
            TR_TABLE(
                TR(English, "First cutoff frequency"),
                TR(French, "Deuxième fréquence de coupure")
            )
        )
    )

public:
    /** @brief Plugin constructor */
    SigmaFilter(const IPluginFactory *factory) noexcept : IPlugin(factory) {}

    virtual void sendAudio(const BufferViews &inputs);
    virtual void receiveAudio(BufferView output);

    virtual void onAudioGenerationStarted(const BeatRange &range);

private:
    DSP::Biquad::Filter<DSP::Biquad::Internal::Form::Transposed2> _filter;
    // DSP::X<DSP::Biquad::BiquadParam::InternalForm::Transposed2> _filter;
    Buffer _cache;
};

#include "SigmaFilter.ipp"
