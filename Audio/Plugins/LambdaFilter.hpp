/**
 * @file LambdaFilter.hpp
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
    class LambdaFilter;
}

class Audio::LambdaFilter final : public Audio::IPlugin
{
    REGISTER_PLUGIN(
        /* Plugin's name */
        TR_TABLE(
            TR(English, "LambdaFilter"),
            TR(French, "Filtre lambda")
        ),
        /* Plugin description */
        TR_TABLE(
            TR(English, "LambdaFilter allow to filter audio signal"),
            TR(French, "Le Filtre lambda permet de filtre de l'audio")
        ),
        /* Plugin flags */
        FLAGS(AudioInput, AudioOutput),
        /* Plugin tags */
        TAGS(Filter),
        /* Control list */
        REGISTER_CONTROL_OUTPUT_VOLUME(
            outputVolume,
            1.0,
            CONTROL_RANGE(0.0, 1.0)
        ),
        REGISTER_CONTROL(
            /* Control type */
            Floating,
            /* Control variable / getter / setter name */
            cutoffFrequencyFrom,
            /* Control's range */
            CONTROL_RANGE(0.0, 1.0),
            /* Control's default value */
            0.5,
            /* Control name */
            TR_TABLE(
                TR(English, "First cutoff frequency"),
                TR(French, "Première fréquence de coupure")
            ),
            /* Control's description */
            TR_TABLE(
                TR(English, "First cutoff frequency"),
                TR(French, "Première fréquence de coupure")
            )
        ),
        REGISTER_CONTROL(
            /* Control type */
            Floating,
            /* Control variable / getter / setter name */
            cutoffFrequencyTo,
            /* Control's range */
            CONTROL_RANGE(0.0, 1.0),
            /* Control's default value */
            0.8,
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
        ,REGISTER_CONTROL_ENUM(
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
    LambdaFilter(const IPluginFactory *factory) noexcept : IPlugin(factory) {}

    virtual void sendAudio(const BufferViews &inputs);
    virtual void receiveAudio(BufferView output);

    virtual void onAudioGenerationStarted(const BeatRange &range);

private:
    DSP::FIRFilter<float> _firFilter;
    Buffer _cache;
};

#include "LambdaFilter.ipp"
