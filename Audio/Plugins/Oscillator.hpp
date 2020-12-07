/**
 * @ Author: Pierre Veysseyre
 * @ Description: Oscillator
 */

#pragma once

#include <Audio/IPlugin.hpp>
#include <Audio/DSP/EnveloppeGenerator.hpp>

#include "Managers/NoteManager.hpp"

namespace Audio
{
    class Oscillator;

    using Enveloppe = DSP::EnveloppeGenerator<DSP::GeneratorType::ADSR>;
};

class alignas_cacheline Audio::Oscillator final : public Audio::IPlugin
{
public:
    /** @brief Describe the waveform of the oscillator */
    enum class Waveform : std::uint8_t {
        Sine, Triangle, Saw, Square
    };

    virtual Flags getFlags(void) const noexcept;

    virtual void sendAudio(const BufferViews &inputs) noexcept;
    virtual void receiveAudio(BufferView output) noexcept;

    virtual void sendNotes(const NoteEvents &notes) noexcept;
    virtual void receiveNotes(NoteEvents &notes) noexcept;

    virtual void sendControls(const ControlEvents &controls) noexcept;

    virtual void onAudioGenerationStarted(const BeatRange &range) noexcept {}


    const NoteManager *noteManager(void) const noexcept { return _noteManager.get(); }
    NoteManager *noteManager(void) noexcept { return _noteManager.get(); }

    const Enveloppe &enveloppe(void) const noexcept { return _enveloppe; }
    Enveloppe &enveloppe(void) noexcept { return _enveloppe; }

private:
    NoteManagerPtr  _noteManager;
    Enveloppe       _enveloppe;
};

static_assert_fit_cacheline(Audio::Oscillator);

#include "Oscillator.ipp"
