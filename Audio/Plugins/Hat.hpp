/**
 * @ Author: Pierre Veysseyre
 * @ Description: Hat
 */

#pragma once

#include <Core/FlatVector.hpp>

#include <Audio/PluginUtilsControlsFM.hpp>
#include <Audio/PluginUtilsControlsMapping.hpp>

#include <Audio/DSP/FM.hpp>
// #include <Audio/DSP/Filter.hpp>
#include <Audio/DSP/Biquad.hpp>
#include <Audio/DSP/Delay.hpp>

#include "Managers/NoteManager.hpp"

namespace Audio
{
    class Hat;
}

class Audio::Hat final : public Audio::IPlugin
{
    REGISTER_PLUGIN(
        /* Plugin's name */
        TR_TABLE(
            TR(English, "Hat"),
            TR(French, "Hat")
        ),
        /* Plugin description */
        TR_TABLE(
            TR(English, "Hat allow to generate spicy kicks"),
            TR(French, "Hat allow to generate spicy kicks")
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
        REGISTER_CONTROL_NUMERIC(
            attack, 0.0, CONTROL_RANGE_STEP(0.0, 0.1, 0.001),
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
        ),
        REGISTER_CONTROL_NUMERIC(
            duration, 0.120, CONTROL_RANGE_STEP(0.01, 0.6, 0.001),
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
        REGISTER_CONTROL_NUMERIC(
            bandFreq, 6'000.0, CONTROL_RANGE_STEP(1'000, 20'000, 100.0),
            TR_TABLE(
                TR(English, "BandPass"),
            ),
            TR_TABLE(
                TR(English, "BandPass"),
            ),
            TR_TABLE(
                TR(English, "BandPass")
            ),
            TR_TABLE(
                TR(English, "%")
            )
        ),
        REGISTER_CONTROL_NUMERIC(
            highFreq, 4'000.0, CONTROL_RANGE_STEP(1'000, 20'000, 100.0),
            TR_TABLE(
                TR(English, "HighPass"),
            ),
            TR_TABLE(
                TR(English, "HighPass"),
            ),
            TR_TABLE(
                TR(English, "HighPass")
            ),
            TR_TABLE(
                TR(English, "%")
            )
        ),
        REGISTER_CONTROL_NUMERIC(
            combLevel, 0.1, CONTROL_RANGE_STEP(0.0, 0.707, 0.01),
            TR_TABLE(
                TR(English, "Spice"),
            ),
            TR_TABLE(
                TR(English, "Spice"),
            ),
            TR_TABLE(
                TR(English, "Spice")
            ),
            TR_TABLE(
                TR(English, "%")
            )
        ),
        REGISTER_CONTROL_NUMERIC(
            combSpread, 0.0, CONTROL_RANGE_STEP(0.0, 0.02, 0.0001),
            TR_TABLE(
                TR(English, "Spread"),
            ),
            TR_TABLE(
                TR(English, "Spread"),
            ),
            TR_TABLE(
                TR(English, "Spread")
            ),
            TR_TABLE(
                TR(English, "%")
            )
        )
    )

public:
    /** @brief Plugin constructor */
    Hat(const IPluginFactory *factory) noexcept : IPlugin(factory) {}

    virtual void receiveAudio(BufferView output);

    virtual void sendNotes(const NoteEvents &notes, const BeatRange &range);

    virtual void setExternalPaths(const ExternalPaths &paths);

    virtual void onAudioParametersChanged(void);

    // virtual void sendControls(const ControlEvents &controls) noexcept {
    // }

    virtual void onAudioGenerationStarted(const BeatRange &range);

private:
    NoteManagerDefault _noteManager {};
    DSP::FM::SchemaHelper<DSP::FM::AlgorithmType::Hat, false> _fm {};
    DSP::Biquad::Filter<DSP::Biquad::Internal::Form::Transposed2> _bandPassFilter;
    DSP::Biquad::Filter<DSP::Biquad::Internal::Form::Transposed2> _highPassFilter;
    DSP::DelayLineUnique<float, DSP::DelayLineDesign::Default> _combFilter;
    Buffer _cache;
};

#include "Hat.ipp"
