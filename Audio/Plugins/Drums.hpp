/**
 * @ Author: Pierre Veysseyre
 * @ Description: Drums
 */

#pragma once

#include <Core/FlatVector.hpp>

#include <Audio/PluginUtilsControlsFM.hpp>
#include <Audio/PluginUtilsControlsMapping.hpp>


#include "Managers/FMManager.hpp"

namespace Audio
{
    class Drums;
}

class Audio::Drums final : public Audio::IPlugin
{
    REGISTER_PLUGIN(
        /* Plugin's name */
        TR_TABLE(
            TR(English, "Drums"),
            TR(French, "Drums")
        ),
        /* Plugin description */
        TR_TABLE(
            TR(English, "Drums allow to generate audio waveforms and play them as notes"),
            TR(French, "Drums allow to generate audio waveforms and play them as notes")
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
        REGISTER_CONTROL_FM_ALGORITHM_DEFAULT_PITCH(
            opA, opB, opC, opD
        ),

        // // Sub section env
        // REGISTER_CONTROL_ENVELOPE_ADSR(
        //     subAttack, 0.1, CONTROL_RANGE_STEP(0.0, 5.0, 0.0001),
        //     subDecay, 0.2, CONTROL_RANGE_STEP(0.0, 5.0, 0.001),
        //     subSustain, 0.8, CONTROL_RANGE_STEP(0.0, 1.0, 0.01),
        //     BaseName##release, 0.2, CONTROL_RANGE_STEP(0.0, 5.0, 0.001)
        // ),
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
    Drums(const IPluginFactory *factory) noexcept : IPlugin(factory) {}

    virtual void receiveAudio(BufferView output);

    virtual void sendNotes(const NoteEvents &notes, const BeatRange &range);

    virtual void setExternalPaths(const ExternalPaths &paths);

    virtual void onAudioParametersChanged(void);

    // virtual void sendControls(const ControlEvents &controls) noexcept {
    // }

    virtual void onAudioGenerationStarted(const BeatRange &range);


public:
private:
    FMManager<DSP::EnvelopeType::DADSR, 4u, DSP::FM::AlgorithmType::KickDrum, true> _fmManager {};
};

#include "Drums.ipp"
