/**
 * @ Author: Pierre Veysseyre
 * @ Description: Oscillator
 */

#pragma once

#include <Core/FlatVector.hpp>

#include <Audio/PluginControlUtils.hpp>
#include <Audio/Volume.hpp>
#include <Audio/Modifier.hpp>

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
        REGISTER_CONTROL_OUTPUT_VOLUME(
            outputVolume,
            DefaultPluginOutputVolume,
            CONTROL_DEFAULT_OUTPUT_VOLUME_RANGE()
        ),
        /* Enveloppe controls (attack, decay, sustain, release) */
        REGISTER_CONTROL_ENVELOPPE_ADSR(
            enveloppeAttack, 0.1, CONTROL_RANGE(0.0, 10.0),
            enveloppeDecay, 0.2, CONTROL_RANGE(0.0, 10.0),
            enveloppeSustain, 0.8, CONTROL_RANGE(0.0, 1.0),
            enveloppeRelease, 0.2, CONTROL_RANGE(0.0, 10.0)
        ),
        REGISTER_CONTROL_ENUM(
            waveform,
            CONTROL_ENUM_RANGE(
                TR_TABLE(
                    TR(English, "Sine"),
                    TR(French, "Sinus")
                ),
                TR_TABLE(
                    TR(English, "Square"),
                    TR(French, "Carré")
                ),
                TR_TABLE(
                    TR(English, "Triangle"),
                    TR(French, "Triangle")
                ),
                TR_TABLE(
                    TR(English, "Saw"),
                    TR(French, "Scie")
                )
                // ,
                // TR_TABLE(
                //     TR(English, "Noise"),
                //     TR(French, "Bruit")
                // ),
                // TR_TABLE(
                //     TR(English, "Error"),
                //     TR(French, "Error")
                // )
            ),
            /* Control name */
            TR_TABLE(
                TR(English, "Oscillator waveform"),
                TR(French, "Type d'onde de l'oscillateur")
            ),
            /* Control's description */
            TR_TABLE(
                TR(English, "Oscillator waveform"),
                TR(French, "Type d'onde de l'oscillateur")
            ),
            /* Control's short name */
            TR_TABLE(
                TR(English, "Wave")
            ),
            /* Control's unit */
            TR_TABLE(
                TR(English, "pi")
            )
        )
    )

    struct Osc
    {
        enum class Waveform : std::uint8_t {
            Sine, Square,
            // Not working !
            Triangle, Saw,
            Noise,
            // Not use this, but fun xD
            Error
        };

        float tunning { 0.f };
        Waveform waveform { Waveform::Sine };
    };

public:
    /** @brief Default gain for each voice */
    // static constexpr DB DefaultVoiceGain = 0;
    static constexpr DB DefaultVoiceGain = -DBUMax;

    /** @brief Plugin constructor */
    Oscillator(const IPluginFactory *factory) noexcept : IPlugin(factory) {}

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
    Volume<float> _volumeHandler;

    float getEnveloppeGain(const Key key, const std::uint32_t index, const bool isTrigger) noexcept
    {
        return _noteManager.getEnveloppeGain(key, index, isTrigger,
                0.0f,
                static_cast<float>(enveloppeAttack()),
                0.0f,
                static_cast<float>(enveloppeDecay()),
                static_cast<float>(enveloppeSustain()),
                static_cast<float>(enveloppeRelease()),
        audioSpecs().sampleRate);
    }

    template<bool Accumulate = true, typename Type>
    void generateWaveform(const Osc::Waveform waveform, Type *output, const std::size_t outputSize,
            const float frequency, const SampleRate sampleRate, const std::uint32_t phaseOffset, const Key key, const bool trigger, const DB gain) noexcept;

    template<bool Accumulate, typename Type>
    void generateSine(Type *output, const std::size_t outputSize,
            const float frequency, const SampleRate sampleRate, const std::uint32_t phaseOffset, const Key key, const bool trigger, const DB gain) noexcept;

    template<bool Accumulate, typename Type>
    void generateSquare(Type *output, const std::size_t outputSize,
            const float frequency, const SampleRate sampleRate, const std::uint32_t phaseOffset, const Key key, const bool trigger, const DB gain) noexcept;

    template<bool Accumulate, typename Type>
    void generateTriangle(Type *output, const std::size_t outputSize,
            const float frequency, const SampleRate sampleRate, const std::uint32_t phaseOffset, const Key key, const bool trigger, const DB gain) noexcept;

    template<bool Accumulate, typename Type>
    void generateSaw(Type *output, const std::size_t outputSize,
            const float frequency, const SampleRate sampleRate, const std::uint32_t phaseOffset, const Key key, const bool trigger, const DB gain) noexcept;

    template<bool Accumulate, typename Type>
    void generateError(Type *output, const std::size_t outputSize,
            const float frequency, const SampleRate sampleRate, const std::uint32_t phaseOffset, const Key key, const bool trigger, const DB gain) noexcept;

    template<bool Accumulate, typename Type>
    void generateNoise(Type *output, const std::size_t outputSize,
            const float frequency, const SampleRate sampleRate, const std::uint32_t phaseOffset, const Key key, const bool trigger, const DB gain) noexcept;
};

#include "Oscillator.ipp"
