/**
 * @ Author: Pierre Veysseyre
 * @ Description: Sampler
 */

#pragma once

#include <Core/FlatVector.hpp>

#include <Audio/PluginControlUtils.hpp>
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
        FLAGS(AudioOutput, NoteInput, SingleExternalInput),
        /* Plugin tags */
        TAGS(Sampler),
        /* Control list */
        REGISTER_CONTROL_OUTPUT_VOLUME(
            outputVolume,
            DefaultPluginOutputVolume,
            CONTROL_DEFAULT_OUTPUT_VOLUME_RANGE()
        ),
        /* Enveloppe controls (attack, release) */
        REGISTER_CONTROL_ENVELOPPE_AR(
            enveloppeAttack, 0.001, CONTROL_RANGE(0.0, 10.0),
            enveloppeRelease, 0.001, CONTROL_RANGE(0.0, 10.0)
        )
    )

public:
    /** @brief Plugin constructor */
    Sampler(const IPluginFactory *factory) noexcept : IPlugin(factory) {}

    virtual void receiveAudio(BufferView output);

    virtual void sendNotes(const NoteEvents &notes);

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
    NoteManager<DSP::EnveloppeType::AR> _noteManager {};

    Buffer _tmp;
    ExternalPaths _externalPaths;

    float getEnveloppeGain(const Key key, const std::uint32_t index, const bool isTrigger) noexcept
    {
        return _noteManager.getEnveloppeGain<false>(key, index, isTrigger,
                0.0f,
                static_cast<float>(enveloppeAttack()),
                0.0f,
                0.0f,
                0.0f,
                static_cast<float>(enveloppeRelease()),
        audioSpecs().sampleRate);
    }
};

#include "Sampler.ipp"
