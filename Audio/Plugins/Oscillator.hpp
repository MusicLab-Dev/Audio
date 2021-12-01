/**
 * @ Author: Pierre Veysseyre
 * @ Description: Oscillator
 */

#pragma once

#include <Audio/PluginUtilsControlsVolume.hpp>
#include <Audio/PluginUtilsControlsEnvelope.hpp>
#include <Audio/DSP/Oscillator.hpp>
#include <Audio/DSP/Biquad.hpp>

#include "Managers/NoteManager.hpp"

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
            TR(English, "Audio waveforms synthesizer"),
            TR(French, "Synthétiseur de formes d'onde")
        ),
        /* Plugin flags */
        FLAGS(AudioOutput, NoteInput, NoChildren),
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
                    TR(English, "Square - Digital"),
                    TR(French, "Carré - Digital")
                ),
                TR_TABLE(
                    TR(English, "Triangle"),
                    TR(French, "Triangle")
                ),
                TR_TABLE(
                    TR(English, "Saw - Digital"),
                    TR(French, "Dent de scie - Digital")
                ),
                TR_TABLE(
                    TR(English, "Noise"),
                    TR(French, "Bruit")
                ),
                TR_TABLE(
                    TR(English, "Pulse 1/3 - Digital"),
                    TR(French, "Carré 1/3 - Digital")
                ),
                TR_TABLE(
                    TR(English, "Pulse 1/4 - Digital"),
                    TR(French, "Carré 1/4 - Digital")
                ),
                TR_TABLE(
                    TR(English, "Square - Analog"),
                    TR(French, "Carré - Analog")
                ),
                TR_TABLE(
                    TR(English, "Saw - Analog"),
                    TR(French, "Dent de scie - Analog")
                ),
                TR_TABLE(
                    TR(English, "Pulse 1/3 - Analog"),
                    TR(French, "Carré 1/3 - Analog")
                ),
                TR_TABLE(
                    TR(English, "Pulse 1/4 - Analog"),
                    TR(French, "Carré 1/4 - Analog")
                )
            ),
            /* Control name */
            TR_TABLE(
                TR(English, "Waveform"),
                TR(French, "Forme d'onde")
            ),
            /* Control's description */
            TR_TABLE(
                TR(English, "Waveform"),
                TR(French, "Forme d'onde")
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
            detune, 0.0, CONTROL_RANGE_STEP(0.0, 1.0, 0.01),
            TR_TABLE(
                TR(English, "Detune"),
                TR(French, "Désaccordage")
            ),
            TR_TABLE(
                TR(English, "Internal oscillator detune"),
                TR(French, "Désaccordage entres les oscillateurs interne")
            ),
            TR_TABLE(
                TR(English, "Det")
            ),
            TR_TABLE(
                TR(English, "%")
            )
        ),
        // REGISTER_CONTROL_FLOATING_LOG(
        //     filterCutoff, 100.0, CONTROL_RANGE_STEP(20.0, 20'000.0, 2.0),
        //     TR_TABLE(
        //         TR(English, "Filter cutoff"),
        //     ),
        //     TR_TABLE(
        //         TR(English, "Filter cutoff"),
        //     ),
        //     TR_TABLE(
        //         TR(English, "Cut")
        //     ),
        //     TR_TABLE(
        //         TR(English, "Hz")
        //     )
        // ),
        // REGISTER_CONTROL_FLOATING(
        //     filterResonance, 0.5, CONTROL_RANGE_STEP(0.0, 5.0, 0.1),
        //     TR_TABLE(
        //         TR(English, "Filter resonance"),
        //     ),
        //     TR_TABLE(
        //         TR(English, "Filter resonance"),
        //     ),
        //     TR_TABLE(
        //         TR(English, "Res")
        //     ),
        //     TR_TABLE(
        //         TR(English, "%")
        //     )
        // ),
        // REGISTER_CONTROL_FLOATING(
        //     filterEnv, 0.5, CONTROL_RANGE_STEP(0.0, 1.0, 0.01),
        //     TR_TABLE(
        //         TR(English, "Filter envelope"),
        //     ),
        //     TR_TABLE(
        //         TR(English, "Filter envelope"),
        //     ),
        //     TR_TABLE(
        //         TR(English, "Env")
        //     ),
        //     TR_TABLE(
        //         TR(English, "%")
        //     )
        // ),
        REGISTER_CONTROL_FLOATING(
            panning, 0.0, CONTROL_RANGE_STEP(-1.0, 1.0, 0.01),
            TR_TABLE(
                TR(English, "Panning"),
                TR(French, "Panoramique"),
            ),
            TR_TABLE(
                TR(English, "Stereo panning"),
                TR(English, "Panoramique stéréo"),
            ),
            TR_TABLE(
                TR(English, "Pan")
            ),
            TR_TABLE(
                TR(English, "L/R")
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
    DSP::Oscillator<3u> _oscillator;
    DSP::Biquad::Filter<DSP::Biquad::Internal::Form::Transposed2> _filter;
};

#include "Oscillator.ipp"
