/**
 * @ Author: Pierre Veysseyre
 * @ Description: Mixer
 */

#pragma once

#include <Audio/IPlugin.hpp>

namespace Audio
{
    class Mixer;
};

class Audio::Mixer : public Audio::IPlugin
{
public:
    using Cache = Core::TinyVector<BufferViews>;
    using Index = std::uint16_t;

    /** @brief */
    Mixer(float mixGain = 1.f) : _mixGain(mixGain) {}

    virtual Flags getFlags(void) const noexcept;

    virtual void sendAudio(const BufferViews &inputs) noexcept;
    virtual void receiveAudio(BufferView output) noexcept;

    virtual void sendNotes(const NoteEvents &notes) noexcept {}
    virtual void receiveNotes(NoteEvents &notes) noexcept {}

    virtual void sendControls(const Controls &controls) noexcept {}

    virtual void sendSync(const Tempo &tempo) noexcept {}
    virtual void receiveSync(Tempo &tempo) noexcept {}

    virtual void onAudioGenerationStarted(const BeatRange &range) noexcept;


private:
    float   _mixGain { 1.0 };
};

// static_assert_fit_half_cacheline(Audio::Mixer);

#include "Mixer.ipp"
