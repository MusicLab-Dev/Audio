/**
 * @ Author: Pierre Veysseyre
 * @ Description: Oscillator
 */

#pragma once

#include <Audio/PluginUtilsControlsVolume.hpp>
#include <Audio/PluginUtilsControlsEnvelope.hpp>
#include <Audio/Volume.hpp>
#include <Audio/Modifier.hpp>
#include <Audio/DSP/Oscillator.hpp>

#include "Managers/NoteManager.hpp"

#include <Audio/DSP/Biquad.hpp>

namespace Audio
{
    class Oscillator;
}

class Audio::Oscillator final : public Audio::IPlugin
{
    REGISTER_PLUGIN(
        /* Plugin's name */
        TR_TABLE(
            TR(English, "Oscillator"),
            TR(French, "Oscillateur")
        ),
        /* Plugin description */
        TR_TABLE(
            TR(English, "Oscillator allow to generate audio waveforms and play them as notes"),
            TR(French, "Le Oscillateur permet de générer des formes d'ondes audio et de les jouer comme des notes")
        ),
        /* Plugin flags */
        FLAGS(AudioOutput, NoteInput),
        /* Plugin tags */
        TAGS(Synth),
        /* Control list */
        REGISTER_CONTROL_OUTPUT_VOLUME(
            outputVolume,
            DefaultPluginOutputVolume,
            CONTROL_DEFAULT_OUTPUT_VOLUME_RANGE()
        ),
        /* Envelope controls (attack, decay, sustain, release) */
        REGISTER_CONTROL_ENVELOPE_ADSR(
            envelopeAttack, 0.1, CONTROL_RANGE_STEP(0.0, 5.0, 0.001),
            envelopeDecay, 0.2, CONTROL_RANGE_STEP(0.0, 5.0, 0.001),
            envelopeSustain, 0.8, CONTROL_RANGE_STEP(0.0, 1.0, 0.01),
            envelopeRelease, 0.2, CONTROL_RANGE_STEP(0.0, 5.0, 0.001)
        ),
        REGISTER_CONTROL_ENUM(
            waveform,
            CONTROL_ENUM_RANGE(
                TR_TABLE(
                    TR(English, "Sine"),
                    TR(French, "Sinus")
                ),
                TR_TABLE(
                    TR(English, "Cosine"),
                    TR(French, "Cosinus")
                ),
                TR_TABLE(
                    TR(English, "Square"),
                    TR(French, "Carré")
                ),
                TR_TABLE(
                    TR(English, "Triangle"),
                    TR(French, "Triangle")
                ),
                TR_TABLE(
                    TR(English, "Saw"),
                    TR(French, "Scie")
                ),
                TR_TABLE(
                    TR(English, "Noise"),
                    TR(French, "Bruit")
                ),
                TR_TABLE(
                    TR(English, "PulseThird"),
                    TR(French, "Carré tiers")
                ),
                TR_TABLE(
                    TR(English, "PulseQuarter"),
                    TR(French, "Carré quart")
                ),
                // TR_TABLE(
                //     TR(English, "Error"),
                //     TR(French, "Error")
                // )
            ),
            /* Control name */
            TR_TABLE(
                TR(English, "Oscillator waveform"),
                TR(French, "Type d'onde de l'oscillateur")
            ),
            /* Control's description */
            TR_TABLE(
                TR(English, "Oscillator waveform"),
                TR(French, "Type d'onde de l'oscillateur")
            ),
            /* Control's short name */
            TR_TABLE(
                TR(English, "Wave")
            ),
            /* Control's unit */
            TR_TABLE(
                TR(English, "")
            )
        ),
        REGISTER_CONTROL_FLOATING(
            tmpCut, 10000.0, CONTROL_RANGE_STEP(20.0, 20000.0, 10.0),
            /* Control name */
            TR_TABLE(
                TR(English, "Oscillator waveform"),
                TR(French, "Type d'onde de l'oscillateur")
            ),
            /* Control's description */
            TR_TABLE(
                TR(English, "Oscillator waveform"),
                TR(French, "Type d'onde de l'oscillateur")
            ),
            /* Control's short name */
            TR_TABLE(
                TR(English, "CUT")
            ),
            /* Control's unit */
            TR_TABLE(
                TR(English, "")
            )
        ),
        REGISTER_CONTROL_FLOATING(
            tmpFilter, 0.0, CONTROL_RANGE_STEP(0.0, 10.0, 0.1),
            /* Control name */
            TR_TABLE(
                TR(English, "Oscillator waveform"),
                TR(French, "Type d'onde de l'oscillateur")
            ),
            /* Control's description */
            TR_TABLE(
                TR(English, "Oscillator waveform"),
                TR(French, "Type d'onde de l'oscillateur")
            ),
            /* Control's short name */
            TR_TABLE(
                TR(English, "FILTER")
            ),
            /* Control's unit */
            TR_TABLE(
                TR(English, "%")
            )
        )
    )

    struct Osc
    {
        enum class Waveform : std::uint8_t {
            Sine, Cosine, Square,
            // Not working !
            Triangle, Saw,
            Noise,
            // Not use this, but fun xD
            Error
        };

        float tunning { 0.f };
        Waveform waveform { Waveform::Sine };
    };

public:
    /** @brief Plugin constructor */
    Oscillator(const IPluginFactory *factory) noexcept : IPlugin(factory) {}

    virtual void receiveAudio(BufferView output);

    virtual void sendNotes(const NoteEvents &notes, const BeatRange &range);

    virtual void setExternalPaths(const ExternalPaths &paths);

    virtual void onAudioParametersChanged(void);

    // virtual void sendControls(const ControlEvents &controls) noexcept {
    // }

    virtual void onAudioGenerationStarted(const BeatRange &range);

private:
    NoteManagerDefault _noteManager {};
    DSP::Oscillator<1u> _oscillator;
    Volume<float> _volumeHandler;
    DSP::Biquad::Filter<DSP::Biquad::Internal::Form::Direct1> _filter;
};

#include "Oscillator.ipp"
