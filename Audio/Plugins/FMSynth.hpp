/**
 * @ Author: Pierre Veysseyre
 * @ Description: FMX
 */

#pragma once

#include <Core/FlatVector.hpp>

#include <Audio/PluginUtilsControlsFM.hpp>
// #include <Audio/Volume.hpp>
// #include <Audio/Modifier.hpp>

#include "Managers/FMManager.hpp"

namespace Audio
{
    class FMX;
}

class Audio::FMX final : public Audio::IPlugin
{
    REGISTER_PLUGIN(
        /* Plugin's name */
        TR_TABLE(
            TR(English, "FMX"),
            TR(French, "FMX")
        ),
        /* Plugin description */
        TR_TABLE(
            TR(English, "FMX allow to generate audio waveforms and play them as notes"),
            TR(French, "FMX allow to generate audio waveforms and play them as notes")
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
        REGISTER_CONTROL_FM_OPERATOR(opA),
        REGISTER_CONTROL_FM_OPERATOR(opB),
        REGISTER_CONTROL_FM_OPERATOR(opC),
        REGISTER_CONTROL_FM_OPERATOR(opD),
        REGISTER_CONTROL_FM_OPERATOR(opE),
        REGISTER_CONTROL_FM_OPERATOR(opF)
    )

public:
    /** @brief Plugin constructor */
    FMX(const IPluginFactory *factory) noexcept : IPlugin(factory) {}

    virtual void receiveAudio(BufferView output);

    virtual void sendNotes(const NoteEvents &notes, const BeatRange &range);

    virtual void setExternalPaths(const ExternalPaths &paths);

    virtual void onAudioParametersChanged(void);

    // virtual void sendControls(const ControlEvents &controls) noexcept {
    // }

    virtual void onAudioGenerationStarted(const BeatRange &range);


public:
private:
    FMManager<DSP::EnvelopeType::ADSR, 6u> _fmManager {};
};

#include "FMSynth.ipp"
