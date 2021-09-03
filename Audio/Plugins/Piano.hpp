/**
 * @ Author: Pierre Veysseyre
 * @ Description: Piano
 */

#pragma once

#include <Core/FlatVector.hpp>

#include <Audio/PluginUtilsControlsMapping.hpp>
#include <Audio/DSP/FIR.hpp>
#include <Audio/DSP/Biquad.hpp>

namespace Audio
{
    class Piano;
}

class Audio::Piano final : public Audio::IPlugin
{
    REGISTER_PLUGIN(
        /* Plugin's name */
        TR_TABLE(
            TR(English, "Piano"),
            TR(French, "Piano")
        ),
        /* Plugin description */
        TR_TABLE(
            TR(English, "Piano allow to generate audio waveforms and play them as notes"),
            TR(French, "Piano allow to generate audio waveforms and play them as notes")
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
        REGISTER_CONTROL_FLOATING(
            brightness, 0.0, CONTROL_RANGE_STEP(0.0, 1.0, 0.01),
            TR_TABLE(
                TR(English, "Brightness"),
            ),
            TR_TABLE(
                TR(English, "Brightness"),
            ),
            TR_TABLE(
                TR(English, "Bright")
            ),
            TR_TABLE(
                TR(English, "%")
            )
        ),
        REGISTER_CONTROL_FLOATING(
            filterCutoff, 1000.0, CONTROL_RANGE_STEP(20.0, 8000.0, 20.0),
            /* Control name */
            TR_TABLE(
                TR(English, "Filter frequency cutoff"),
                TR(French, "Type d'onde de l'oscillateur")
            ),
            /* Control's description */
            TR_TABLE(
                TR(English, "Filter frequency cutoff"),
                TR(French, "Type d'onde de l'oscillateur")
            ),
            /* Control's short name */
            TR_TABLE(
                TR(English, "Cut")
            ),
            /* Control's unit */
            TR_TABLE(
                TR(English, "")
            )
        ),
        REGISTER_CONTROL_FLOATING(
            filterEnvAmount, 0.0, CONTROL_RANGE_STEP(0.0, 17.0, 0.01),
            /* Control name */
            TR_TABLE(
                TR(English, "Envelope amount to the filter cutoff"),
                TR(French, "Type d'onde de l'oscillateur")
            ),
            /* Control's description */
            TR_TABLE(
                TR(English, "Envelope amount to the filter cutoff"),
                TR(French, "Type d'onde de l'oscillateur")
            ),
            /* Control's short name */
            TR_TABLE(
                TR(English, "Env")
            ),
            /* Control's unit */
            TR_TABLE(
                TR(English, "%")
            )
        ),
        REGISTER_CONTROL_FLOATING(
            filterResonance, 0.0, CONTROL_RANGE_STEP(0.0, 1.0, 0.01),
            /* Control name */
            TR_TABLE(
                TR(English, "Filter resonance amount"),
                TR(French, "Type d'onde de l'oscillateur")
            ),
            /* Control's description */
            TR_TABLE(
                TR(English, "Filter resonance amount"),
                TR(French, "Type d'onde de l'oscillateur")
            ),
            /* Control's short name */
            TR_TABLE(
                TR(English, "Q")
            ),
            /* Control's unit */
            TR_TABLE(
                TR(English, "%")
            )
        ),
        REGISTER_CONTROL_FLOATING(
            filterKeyFollow, 0.0, CONTROL_RANGE_STEP(-1.0, 1.0, 0.01),
            /* Control name */
            TR_TABLE(
                TR(English, "Key folow amount to filter cutoff"),
                TR(French, "Type d'onde de l'oscillateur")
            ),
            /* Control's description */
            TR_TABLE(
                TR(English, "Key folow amount to filter cutoff"),
                TR(French, "Type d'onde de l'oscillateur")
            ),
            /* Control's short name */
            TR_TABLE(
                TR(English, "KeyFollow")
            ),
            /* Control's unit */
            TR_TABLE(
                TR(English, "%")
            )
        ),
        REGISTER_CONTROL_FLOATING(
            detuneAmount, 0.0, CONTROL_RANGE_STEP(0.0, 1.0, 0.01),
            /* Control name */
            TR_TABLE(
                TR(English, "Detune osc amount"),
                TR(French, "Type d'onde de l'oscillateur")
            ),
            /* Control's description */
            TR_TABLE(
                TR(English, "Detune osc amount"),
                TR(French, "Type d'onde de l'oscillateur")
            ),
            /* Control's short name */
            TR_TABLE(
                TR(English, "Det")
            ),
            /* Control's unit */
            TR_TABLE(
                TR(English, "%")
            )
        )
    )

public:
    /** @brief Plugin constructor */
    Piano(const IPluginFactory *factory) noexcept : IPlugin(factory) {}

    virtual void receiveAudio(BufferView output);

    virtual void sendNotes(const NoteEvents &notes, const BeatRange &range);

    virtual void setExternalPaths(const ExternalPaths &paths);

    virtual void onAudioParametersChanged(void);

    // virtual void sendControls(const ControlEvents &controls) noexcept {
    // }

    virtual void onAudioGenerationStarted(const BeatRange &range);


private:
    NoteManagerDefault _noteManager {};
    DSP::Oscillator<3u> _oscillators;
    DSP::Biquad::Filter<DSP::Biquad::Internal::Form::Transposed2> _filter;
    DSP::EnvelopeDefaultLinear<DSP::EnvelopeType::ADSR, 1u> _filterEnv;

    Buffer _cache;
};

#include "Piano.ipp"
