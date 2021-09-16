/**
 * @ Author: Pierre Veysseyre
 * @ Description: Snare
 */

#pragma once

#include <Core/FlatVector.hpp>

#include <Audio/PluginUtilsControlsFM.hpp>
#include <Audio/PluginUtilsControlsMapping.hpp>


#include "Managers/NoteManager.hpp"

namespace Audio
{
    class Snare;
}

class Audio::Snare final : public Audio::IPlugin
{
    REGISTER_PLUGIN(
        /* Plugin's name */
        TR_TABLE(
            TR(English, "Snare"),
            TR(French, "Snare")
        ),
        /* Plugin description */
        TR_TABLE(
            TR(English, "Snare allow to generate spicy kicks"),
            TR(French, "Snare allow to generate spicy kicks")
        ),
        /* Plugin flags */
        FLAGS(AudioOutput, NoteInput, NoChildren),
        /* Plugin tags */
        TAGS(Drum),
        /* Control list */
        REGISTER_CONTROL_OUTPUT_VOLUME(
            outputVolume,
            DefaultPluginOutputVolume,
            CONTROL_DEFAULT_OUTPUT_VOLUME_RANGE()
        ),
        REGISTER_CONTROL_FLOATING(
            duration, 0.3, CONTROL_RANGE_STEP(0.0, 1.0, 0.001),
            TR_TABLE(
                TR(English, "Duration"),
            ),
            TR_TABLE(
                TR(English, "Duration"),
            ),
            TR_TABLE(
                TR(English, "Dur")
            ),
            TR_TABLE(
                TR(English, "s")
            )
        ),
        REGISTER_CONTROL_FLOATING(
            color, 0.0, CONTROL_RANGE_STEP(0.0, 1.0, 0.01),
            TR_TABLE(
                TR(English, "Color"),
            ),
            TR_TABLE(
                TR(English, "Color"),
            ),
            TR_TABLE(
                TR(English, "Color")
            ),
            TR_TABLE(
                TR(English, "%")
            )
        ),
        REGISTER_CONTROL_FLOATING(
            snapLevel, 0.5, CONTROL_RANGE_STEP(0.0, 1.0, 0.01),
            TR_TABLE(
                TR(English, "Snap level"),
            ),
            TR_TABLE(
                TR(English, "Snap level"),
            ),
            TR_TABLE(
                TR(English, "S. Lvl")
            ),
            TR_TABLE(
                TR(English, "%")
            )
        ),
        REGISTER_CONTROL_FLOATING(
            snapDuration, 0.25, CONTROL_RANGE_STEP(0.0, 1.0, 0.01),
            TR_TABLE(
                TR(English, "Snap duration"),
            ),
            TR_TABLE(
                TR(English, "Snap duration"),
            ),
            TR_TABLE(
                TR(English, "S. Dec")
            ),
            TR_TABLE(
                TR(English, "%")
            )
        ),
        REGISTER_CONTROL_FLOATING(
            snapTone, 0.25, CONTROL_RANGE_STEP(0.0, 1.0, 0.01),
            TR_TABLE(
                TR(English, "Snap tone"),
            ),
            TR_TABLE(
                TR(English, "Snap tone"),
            ),
            TR_TABLE(
                TR(English, "S. Tone")
            ),
            TR_TABLE(
                TR(English, "%")
            )
        ),
        REGISTER_CONTROL_FLOATING(
            pitchDecay, 0.003, CONTROL_RANGE_STEP(0.0, 0.01, 0.0001),
            TR_TABLE(
                TR(English, "Pitch decay"),
            ),
            TR_TABLE(
                TR(English, "Pitch decay"),
            ),
            TR_TABLE(
                TR(English, "P. dec")
            ),
            TR_TABLE(
                TR(English, "%")
            )
        ),
        REGISTER_CONTROL_FLOATING(
            pitchLevel, 0.6, CONTROL_RANGE_STEP(0.0, 1.0, 0.01),
            TR_TABLE(
                TR(English, "Pitch level"),
            ),
            TR_TABLE(
                TR(English, "Pitch level"),
            ),
            TR_TABLE(
                TR(English, "P. lvl")
            ),
            TR_TABLE(
                TR(English, "%")
            )
        ),
        REGISTER_CONTROL_FLOATING(
            attack, 0.0035, CONTROL_RANGE_STEP(0.0, 0.1, 0.001),
            TR_TABLE(
                TR(English, "Attack"),
            ),
            TR_TABLE(
                TR(English, "Attack"),
            ),
            TR_TABLE(
                TR(English, "Atck")
            ),
            TR_TABLE(
                TR(English, "%")
            )
        )    )

public:
    /** @brief Plugin constructor */
    Snare(const IPluginFactory *factory) noexcept : IPlugin(factory) {}

    virtual void receiveAudio(BufferView output);

    virtual void sendNotes(const NoteEvents &notes, const BeatRange &range);

    virtual void setExternalPaths(const ExternalPaths &paths);

    virtual void onAudioParametersChanged(void);

    // virtual void sendControls(const ControlEvents &controls) noexcept {
    // }

    virtual void onAudioGenerationStarted(const BeatRange &range);

private:
    NoteManagerDefault _noteManager {};
    DSP::FM::Schema<4u, DSP::FM::AlgorithmType::Drum, true> _fm {};
};

#include "Snare.ipp"
