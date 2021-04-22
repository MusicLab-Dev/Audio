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
            1.0 / 2,
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
        /* Enveloppe controls (attack, decay, sustain, release) */
        REGISTER_CONTROL_ENVELOPPE_ADSR(
            enveloppeAttack, 0.1, CONTROL_RANGE(0.0, 10.0),
            enveloppeDecay, 0.2, CONTROL_RANGE(0.0, 10.0),
            enveloppeSustain, 0.8, CONTROL_RANGE(0.0, 1.0),
            enveloppeRelease, 0.2, CONTROL_RANGE(0.0, 10.0)
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

    float getEnveloppeGain(const Key key, const std::size_t index, const bool isTrigger) noexcept
    {
        return _noteManager.enveloppe().adsr(key, index, isTrigger, enveloppeAttack(), enveloppeDecay(), enveloppeSustain(), enveloppeRelease(), audioSpecs().sampleRate);
        // return _noteManager.getEnveloppeGain(key, index, isTrigger, 0ul, enveloppeAttack(), 0ul, enveloppeDecay(), enveloppeSustain(), enveloppeRelease(), audioSpecs().sampleRate);
    }

    template<bool Accumulate = true, typename Type>
    void generateWaveform(const Osc &oscillator, Type *output, const std::size_t outputSize,
            const float frequency, const SampleRate sampleRate, const std::size_t phaseOffset, const Key key, const bool trigger, const float gain) noexcept;

    template<bool Accumulate, typename Type>
    void generateSine(Type *output, const std::size_t outputSize,
            const float frequency, const SampleRate sampleRate, const std::size_t phaseOffset, const Key key, const bool trigger, const float gain) noexcept;

    template<bool Accumulate, typename Type>
    void generateSquare(Type *output, const std::size_t outputSize,
            const float frequency, const SampleRate sampleRate, const std::size_t phaseOffset, const Key key, const bool trigger, const float gain) noexcept;

    template<bool Accumulate, typename Type>
    void generateTriangle(Type *output, const std::size_t outputSize,
            const float frequency, const SampleRate sampleRate, const std::size_t phaseOffset, const Key key, const bool trigger, const float gain) noexcept;

    template<bool Accumulate, typename Type>
    void generateSaw(Type *output, const std::size_t outputSize,
            const float frequency, const SampleRate sampleRate, const std::size_t phaseOffset, const Key key, const bool trigger, const float gain) noexcept;
};

#include "Oscillator.ipp"
