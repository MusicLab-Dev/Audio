/**
 * @ Author: Pierre Veysseyre
 * @ Description: Sampler
 */

#pragma once

#include <Core/FlatVector.hpp>

#include <Audio/PluginUtils.hpp>
#include "Managers/NoteManager.hpp"

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
    static constexpr std::size_t KeysPerOctave = 12u;

    using IndexList = std::array<std::size_t, KeyCount>;
    using OctaveBuffer = std::array<Buffer, KeysPerOctave>;

    virtual Flags getFlags(void) const noexcept;

    virtual void sendAudio(const BufferViews &inputs) noexcept {}
    virtual void receiveAudio(BufferView output) noexcept;

    virtual void sendNotes(const NoteEvents &notes) noexcept;
    virtual void receiveNotes(NoteEvents &notes) noexcept {}

    virtual void onAudioParametersChanged(void);

    // virtual void sendControls(const ControlEvents &controls) noexcept {
    // }

    virtual void onAudioGenerationStarted(const BeatRange &range) noexcept {}

public:
    template<typename T>
    void loadSample(const std::string &path);

    [[nodiscard]] const OctaveBuffer &getBuffers(void) const noexcept { return _buffers; }

private:
    // Cacheline 1
    Gain _outputGain { 0.5f };
    IndexList _readIndex { 0u };
    // Cacheline 1 & 2
    OctaveBuffer _buffers {};
    // Cacheline 3 & 4
    NoteManager _noteManager {};


    /** @brief Increment the read index of given key */
    void incrementReadIndex(Key key, std::size_t amount = 1u) noexcept;
};

#include "Sampler.ipp"
