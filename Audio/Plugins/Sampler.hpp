/**
 * @ Author: Pierre Veysseyre
 * @ Description: Sampler
 */

#pragma once

#include <Audio/PluginUtils.hpp>

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
        /* Control list */
        REGISTER_CONTROL(
            /* Control variable / getter / setter name */
            volume,
            /* Control name */
            TR_TABLE(
                TR(English, "Volume"),
                TR(French, "Volume")
            ),
            /* Control's description */
            TR_TABLE(
                TR(English, "Output volume of the sampler"),
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
        )
    )

public:
    virtual Flags getFlags(void) const noexcept;

    virtual void sendAudio(const BufferViews &inputs) noexcept {}
    virtual void receiveAudio(BufferView output) noexcept {}

    virtual void sendNotes(const NoteEvents &notes) noexcept {}
    virtual void receiveNotes(NoteEvents &notes) noexcept {}

    virtual void sendControls(const ControlEvents &controls) noexcept {
    }

    virtual void onAudioGenerationStarted(const BeatRange &range) noexcept {}

public:
    template<typename T>
    void loadSample(const std::string &path);

    [[nodiscard]] const BufferViews &getBuffers(void) const noexcept { return _buffers; }

    float           _volume { 1.f };
private:
    BufferViews     _buffers;
};

#include "Sampler.ipp"
