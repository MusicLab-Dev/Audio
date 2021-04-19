/**
 * @ Author: Pierre Veysseyre
 * @ Description: Oscillator
 */

#pragma once

#include <Core/FlatVector.hpp>

#include <Audio/PluginUtils.hpp>
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
            TR(English, "Oscillator allow to generate audio waveforms and play them as notes"),
            TR(French, "Le Oscillateur permet de générer des formes d'ondes audio et de les jouer comme des notes")
        ),
        /* Plugin flags */
        FLAGS(AudioOutput, NoteInput, SingleExternalInput),
        /* Plugin tags */
        TAGS(Synth),
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
                TR(English, "Output gain of the Oscillator"),
                TR(French, "Volume de sortie du Oscillateur")
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

    struct Osc
    {
        enum class Waveform : std::uint8_t {
            Sine, Square, Triangle, Saw
        };

        // inline float get

        float tunning { 0.f };
        Waveform waveform { Waveform::Sine };
    };

public:

    virtual void receiveAudio(BufferView output);

    virtual void sendNotes(const NoteEvents &notes);

    virtual void setExternalPaths(const ExternalPaths &paths);

    virtual void onAudioParametersChanged(void);

    // virtual void sendControls(const ControlEvents &controls) noexcept {
    // }

    virtual void onAudioGenerationStarted(const BeatRange &range);


public:
private:
    NoteManager<DSP::EnveloppeType::ADSR> _noteManager {};
    Osc _oscillator;

    template<typename Type>
    void generateWaveform(const Osc &oscillator, Type *output, const std::size_t outputSize, const float frequency, const SampleRate sampleRate, const std::size_t phaseOffset) noexcept;

    template<typename Type>
    void generateSine(Type *output, const std::size_t outputSize, const float frequency, const SampleRate sampleRate, const std::size_t phaseOffset) noexcept;

    template<typename Type>
    void generateSquare(Type *output, const std::size_t outputSize, const float frequency, const SampleRate sampleRate, const std::size_t phaseOffset) noexcept;

    template<typename Type>
    void generateTriangle(Type *output, const std::size_t outputSize, const float frequency, const SampleRate sampleRate, const std::size_t phaseOffset) noexcept;

    template<typename Type>
    void generateSaw(Type *output, const std::size_t outputSize, const float frequency, const SampleRate sampleRate, const std::size_t phaseOffset) noexcept;
};

#include "Oscillator.ipp"
