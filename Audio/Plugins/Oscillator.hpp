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
        FLAGS(AudioOutput, NoteInput),
        /* Plugin tags */
        TAGS(Synth),
        /* Control list */
        REGISTER_CONTROL(
            /* Control type */
            Floating,
            /* Control variable / getter / setter name */
            outputVolume,
            /* Control's range */
            CONTROL_RANGE(0.0, 1.0),
            /* Control's default value */
            1.0,
            /* Control name */
            TR_TABLE(
                TR(English, "Output volume"),
                TR(French, "Volume de sortie")
            ),
            /* Control's description */
            TR_TABLE(
                TR(English, "Output volume of the Oscillator"),
                TR(French, "Volume de sortie du Oscillateur")
            )
        ),
        REGISTER_CONTROL(
            /* Control type */
            Floating,
            /* Control variable / getter / setter name */
            enveloppeAttack,
            /* Control's range */
            CONTROL_RANGE(0.0, 1.0),
            /* Control's default value */
            0.01,
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
            /* Control type */
            Floating,
            /* Control variable / getter / setter name */
            enveloppeRelease,
            /* Control's range */
            CONTROL_RANGE(0.0, 1.0),
            /* Control's default value */
            0.01,
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
    NoteManager<DSP::EnveloppeType::AR> _noteManager {};
    Osc _oscillator;

    float getEnveloppeGain(const Key key, const std::size_t index, const bool isTrigger) noexcept
    {
        return _noteManager.getEnveloppeGain(key, index, isTrigger, 0.f, enveloppeAttack(), 0.f, 0.f, 0.f, enveloppeRelease(), audioSpecs().sampleRate);
    }

    template<typename Type>
    void generateWaveform(const Osc &oscillator, Type *output, const std::size_t outputSize,
            const float frequency, const SampleRate sampleRate, const std::size_t phaseOffset, const Key key, const bool trigger) noexcept;

    template<typename Type>
    void generateSine(Type *output, const std::size_t outputSize,
            const float frequency, const SampleRate sampleRate, const std::size_t phaseOffset, const Key key, const bool trigger) noexcept;

    template<typename Type>
    void generateSquare(Type *output, const std::size_t outputSize,
            const float frequency, const SampleRate sampleRate, const std::size_t phaseOffset, const Key key, const bool trigger) noexcept;

    template<typename Type>
    void generateTriangle(Type *output, const std::size_t outputSize,
            const float frequency, const SampleRate sampleRate, const std::size_t phaseOffset, const Key key, const bool trigger) noexcept;

    template<typename Type>
    void generateSaw(Type *output, const std::size_t outputSize,
            const float frequency, const SampleRate sampleRate, const std::size_t phaseOffset, const Key key, const bool trigger) noexcept;
};

#include "Oscillator.ipp"
