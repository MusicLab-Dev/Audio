/**
 * @ Author: Pierre Veysseyre
 * @ Description: Sampler
 */

#pragma once

#include <Audio/IPlugin.hpp>

namespace Audio
{
    class Sampler;
};

class Audio::Sampler final : public Audio::IPlugin
{
public:

    virtual Flags getFlags(void) const noexcept;

    virtual void sendAudio(const BufferViews &inputs) noexcept {}
    virtual void receiveAudio(BufferView output) noexcept {}

    virtual void sendNotes(const Notes &notes) noexcept {}
    virtual void receiveNotes(Notes &notes) noexcept {}

    virtual void sendControls(const ControlEvents &controls) noexcept {}

    virtual void onAudioGenerationStarted(const BeatRange &range) noexcept;

    virtual void onAudioBlockGenerated(void) noexcept {}

private:
};

#include "Sampler.ipp"
