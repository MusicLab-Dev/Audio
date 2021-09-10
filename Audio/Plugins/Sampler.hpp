/**
 * @ Author: Pierre Veysseyre
 * @ Description: Sampler
 */

#pragma once

#include <Core/FlatVector.hpp>

#include <Audio/PluginUtilsControlsVolume.hpp>
#include <Audio/PluginUtilsControlsEnvelope.hpp>
#include <Audio/BufferOctave.hpp>
#include "Managers/NoteManager.hpp"
#include <Audio/DSP/FIR.hpp>

namespace Audio
{
    class Sampler;
}

class Audio::Sampler final : public Audio::IPlugin
{
    REGISTER_PLUGIN(
        /* Plugin's name */
        TR_TABLE(
            TR(English, "Sampler"),
            TR(French, "Sampleur")
        ),
        /* Plugin description */
        TR_TABLE(
            TR(English, "Sampler allow to load an audio file and play it as a note"),
            TR(French, "Le sampleur permet de charger un fichier audio et de le jouer comme une note")
        ),
        /* Plugin flags */
        FLAGS(AudioOutput, NoteInput, SingleExternalInput, NoChildren),
        /* Plugin tags */
        TAGS(Sampler),
        /* Control list */
        REGISTER_CONTROL_OUTPUT_VOLUME(
            outputVolume,
            DefaultPluginOutputVolume,
            CONTROL_DEFAULT_OUTPUT_VOLUME_RANGE()
        ),
        /* Envelope controls (attack, release) */
        REGISTER_CONTROL_ENVELOPE_AR(
            attack, 0.001, CONTROL_RANGE_STEP(0.0, 2.0, 0.001),
            release, 0.001, CONTROL_RANGE_STEP(0.0, 2.0, 0.001)
        )
    )

public:
    /** @brief Plugin constructor */
    Sampler(const IPluginFactory *factory) noexcept : IPlugin(factory) {}

    virtual void receiveAudio(BufferView output);

    virtual void sendNotes(const NoteEvents &notes, const BeatRange &range);

    virtual const ExternalPaths &getExternalPaths(void) const { return _externalPaths; }
    virtual void setExternalPaths(const ExternalPaths &paths);

    virtual void onAudioParametersChanged(void);

    // virtual void sendControls(const ControlEvents &controls) noexcept {
    // }

    virtual void onAudioGenerationStarted(const BeatRange &range);


public:
    /** Load a sample file with a specific type */
    template<typename Type>
    void loadSample(const std::string_view &path);

    // [[nodiscard]] const OctaveBuffer &getBuffers(void) const noexcept { return _buffers; }

private:
    // Cacheline 1 & 2
    BufferOctave _buffers {};
    // Cacheline 3 & 4
    NoteManager<DSP::EnvelopeType::AR> _noteManager {};

    ExternalPaths _externalPaths;
};

#include "Sampler.ipp"
