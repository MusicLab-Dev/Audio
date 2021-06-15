/**
 * @file Chords.hpp
 * @brief
 *
 * @author Pierre V
 * @date 2021-05-21
 */

#pragma once

#include <Audio/PluginUtils.hpp>
#include <Audio/Plugins/Managers/ChordsManager.hpp>

namespace Audio
{
    class Chords;
}

class Audio::Chords final : public Audio::IPlugin
{
    REGISTER_PLUGIN(
        /* Plugin's name */
        TR_TABLE(
            TR(English, "Chords"),
            TR(French, "Accords")
        ),
        /* Plugin description */
        TR_TABLE(
            TR(English, "Chords allow to generate chords"),
            TR(French, "L'arpégiateur permet de généner des notes à partir d'autres notes")
        ),
        /* Plugin flags */
        FLAGS(NoteInput, NoteOutput),
        /* Plugin tags */
        TAGS(Sequencer),
        /* Control list */
        REGISTER_CONTROL_EFFECT_BYPASS(
            byPass
        ),
        REGISTER_CONTROL_ENUM(
            chordType,
            CONTROL_ENUM_RANGE(
                TR_TABLE(
                    TR(English, "Major")
                ),
                TR_TABLE(
                    TR(English, "Minor")
                ),
                TR_TABLE(
                    TR(English, "Diminished")
                ),
                TR_TABLE(
                    TR(English, "Augmented")
                )
            ),
            TR_TABLE(
                TR(English, "Chord type"),
                TR(French, "Type d'accord")
            ),
            /* Control's description */
            TR_TABLE(
                TR(English, "Chord type"),
                TR(French, "Type d'accord")
            ),
            /* Control's short name */
            TR_TABLE(
                TR(English, "Type")
            ),
            /* Control's unit */
            TR_TABLE(
                TR(English, "")
            )
        ),
        REGISTER_CONTROL_INTEGER(
            noteOffset,
            0,
            CONTROL_RANGE_STEP(-12, 12, 1),
            /* Control's description */
            TR_TABLE(
                TR(English, "Note offset"),
                TR(English, "Note offset")
            ),
            TR_TABLE(
                TR(English, "Note offset"),
                TR(English, "Note offset")
            ),
            /* Control's short name */
            TR_TABLE(
                TR(English, "Dt")
            ),
            /* Control's unit */
            TR_TABLE(
                TR(English, "semitone")
            )
        )
    )

public:
    /** @brief Plugin constructor */
    Chords(const IPluginFactory *factory) noexcept : IPlugin(factory) {}

    virtual void sendNotes(const NoteEvents &notes, const BeatRange &range);
    virtual void receiveNotes(NoteEvents &notes);

    virtual void onAudioGenerationStarted(const BeatRange &range);

private:
    ChordsManager _cache;
    NoteEvents _byPassCache;
};

#include "Chords.ipp"
