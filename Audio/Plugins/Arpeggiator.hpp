/**
 * @file Arpeggiator.hpp
 * @brief
 *
 * @author Pierre V
 * @date 2021-05-21
 */

#pragma once

#include <Audio/PluginUtils.hpp>
#include <Audio/Plugins/Managers/ArpeggiatorManager.hpp>

namespace Audio
{
    class Arpeggiator;
}

class Audio::Arpeggiator final : public Audio::IPlugin
{
    REGISTER_PLUGIN(
        /* Plugin's name */
        TR_TABLE(
            TR(English, "Arpeggiator"),
            TR(French, "Arpégiateur")
        ),
        /* Plugin description */
        TR_TABLE(
            TR(English, "Generates note sequences"),
            TR(French, "Produit des séquences de notes")
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
            arpeggiatorMode,
            CONTROL_ENUM_RANGE(
                TR_TABLE(
                    TR(English, "Up")
                ),
                TR_TABLE(
                    TR(English, "Down")
                ),
                TR_TABLE(
                    TR(English, "UpDown")
                ),
                TR_TABLE(
                    TR(English, "DownUp")
                ),
                TR_TABLE(
                    TR(English, "Random")
                )
            ),
            TR_TABLE(
                TR(English, "Mode"),
                TR(French, "Mode")
            ),
            /* Control's description */
            TR_TABLE(
                TR(English, "Notes sequence mode"),
                TR(French, "Mode de séquence des notes")
            ),
            /* Control's short name */
            TR_TABLE(
                TR(English, "Mode")
            ),
            /* Control's unit */
            TR_TABLE(
                TR(English, "")
            )
        ),
        REGISTER_CONTROL_ENUM(
            noteLength,
            CONTROL_ENUM_RANGE(
                TR_TABLE(
                    TR(English, "1/1")
                ),
                TR_TABLE(
                    TR(English, "1/2")
                ),
                TR_TABLE(
                    TR(English, "1/4")
                ),
                TR_TABLE(
                    TR(English, "1/8")
                ),
                TR_TABLE(
                    TR(English, "1/16")
                ),
                TR_TABLE(
                    TR(English, "1/32")
                ),
                TR_TABLE(
                    TR(English, "1/64")
                ),
                TR_TABLE(
                    TR(English, "1/128")
                )
            ),
            TR_TABLE(
                TR(English, "Note length"),
                TR(French, "Longueur des notes")
            ),
            /* Control's description */
            TR_TABLE(
                TR(English, "Note length in the whole sequence"),
                TR(French, "Longueur des notes dans la séquence générée")
            ),
            /* Control's short name */
            TR_TABLE(
                TR(English, "Length")
            ),
            /* Control's unit */
            TR_TABLE(
                TR(English, "")
            )
        ),
        REGISTER_CONTROL_INTEGER(
            noteCount,
            3,
            CONTROL_RANGE_STEP(1, 8, 1),
            /* Control's description */
            TR_TABLE(
                TR(English, "Note count"),
                TR(French, "Nombre de notes")
            ),
            TR_TABLE(
                TR(English, "Note count used for sequence generation"),
                TR(French, "Nombre de notes utilisé pour la séquence générée")
            ),
            /* Control's short name */
            TR_TABLE(
                TR(English, "Size")
            ),
            /* Control's unit */
            TR_TABLE(
                TR(English, "")
            )
        )
    )

public:
    /** @brief Plugin constructor */
    Arpeggiator(const IPluginFactory *factory) noexcept : IPlugin(factory) {}

    virtual void sendNotes(const NoteEvents &notes, const BeatRange &range);
    virtual void receiveNotes(NoteEvents &notes);

    virtual void onAudioGenerationStarted(const BeatRange &range);

private:
    ArpeggiatorManager _cache;
    NoteEvents _byPassCache;
};

#include "Arpeggiator.ipp"
