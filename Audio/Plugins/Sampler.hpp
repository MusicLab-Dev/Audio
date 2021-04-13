/**
 * @ Author: Pierre Veysseyre
 * @ Description: Sampler
 */

#pragma once

#include <Core/FlatVector.hpp>

#include <Audio/PluginUtils.hpp>
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
        REGISTER_CONTROL(
            /* Control variable / getter / setter name */
            outputGain,
            /* Control name */
            TR_TABLE(
                TR(English, "Output gain"),
                TR(French, "Volume de sortie")
            ),
            /* Control's description */
            TR_TABLE(
                TR(English, "Output gain of the sampler"),
                TR(French, "Volume de sortie du sampleur")
            )
        ),
        REGISTER_CONTROL(
            /* Control variable / getter / setter name */
            pitch,
            /* Control name */
            TR_TABLE(
                TR(English, "Pitch"),
                TR(French, "Hauteur")
            ),
            /* Control's description */
            TR_TABLE(
                TR(English, "Base pitch of the loaded note"),
                TR(French, "Hauteur de référence la note chargée")
            )
        ),
        REGISTER_CONTROL(
            /* Control variable / getter / setter name */
            enveloppeAttack,
            /* Control name */
            TR_TABLE(
                TR(English, "Enveloppe attack"),
                TR(French, "Attaque de l'enveloppe")
            ),
            /* Control's description */
            TR_TABLE(
                TR(English, "Attack duration used by the enveloppe to determine volume gain"),
                TR(French, "Hauteur de référence la note chargée")
            )
        ),
        REGISTER_CONTROL(
            /* Control variable / getter / setter name */
            enveloppeRelease,
            /* Control name */
            TR_TABLE(
                TR(English, "Enveloppe release"),
                TR(French, "Extinction de l'enveloppe")
            ),
            /* Control's description */
            TR_TABLE(
                TR(English, "Release duration used by the enveloppe to determine volume gain"),
                TR(French, "Hauteur de référence la note chargée")
            )
        )
    )

public:
    virtual void receiveAudio(BufferView output);

    virtual void sendNotes(const NoteEvents &notes);

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
    NoteManager _noteManager {};

    Buffer _lastBlock;
    Buffer _cacheBlock;
    bool _hasLastBlockLoaded { false };
};

#include "Sampler.ipp"
