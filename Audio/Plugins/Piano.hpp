/**
 * @ Author: Pierre Veysseyre
 * @ Description: Piano
 */

#pragma once

#include <Core/FlatVector.hpp>

#include <Audio/PluginUtilsControlsFM.hpp>
#include <Audio/PluginUtilsControlsMapping.hpp>
#include <Audio/DSP/FIR.hpp>

#include "Managers/FMManager.hpp"

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
        /* FM controls */
        REGISTER_CONTROL_FM_ALGORITHM_DEFAULT_PITCH(opA, opB, opC, opD),
        // REGISTER_CONTROL_FM_OPERATOR_DEFAULT(opA),
        // REGISTER_CONTROL_FM_OPERATOR_DEFAULT(opB),
        // REGISTER_CONTROL_FM_OPERATOR_DEFAULT(opC),
        // REGISTER_CONTROL_FM_OPERATOR_DEFAULT(opD),
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


public:
private:
    FMManager<DSP::EnvelopeType::ADSR, 4u, DSP::FM::AlgorithmType::Default, true> _fmManager {};
    DSP::FIR::BasicFilter<float> _filter;
    Buffer _cache;
};

#include "Piano.ipp"
