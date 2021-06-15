/**
 * @ Author: Pierre Veysseyre
 * @ Description: Plugin control macros utils for FM
 */

#pragma once

#include "PluginUtilsControlsEnvelope.hpp"

/**
 * @brief Helper for FM controls
 */
#define REGISTER_CONTROL_FM_OPERATOR(BaseName) \
    _REGISTER_CONTROL_FM_OUTPUT_VOLUME(BaseName##volume, 0.0, CONTROL_FM_OPERATOR_OUTPUT_VOLUME_RANGE()), \
    _REGISTER_CONTROL_FM_OUTPUT_VOLUME_RATIO(BaseName##volumeRatio, 1.0, CONTROL_FM_OPERATOR_OUTPUT_VOLUME_RATIO_RANGE()), \
    REGISTER_CONTROL_ENVELOPPE_ADSR( \
        BaseName##attack, 0.1, CONTROL_RANGE_STEP(0.0, 5.0, 0.001), \
        BaseName##decay, 0.2, CONTROL_RANGE_STEP(0.0, 5.0, 0.001), \
        BaseName##sustain, 0.8, CONTROL_RANGE_STEP(0.0, 1.0, 0.01), \
        BaseName##release, 0.2, CONTROL_RANGE_STEP(0.0, 5.0, 0.001) \
    ), \
    _REGISTER_CONTROL_FM_RATIO(BaseName##ratio, CONTROL_FM_RATIO_DEFAULT_VALUE(), CONTROL_FM_RATIO_DEFAULT_RANGE()), \
    _REGISTER_CONTROL_FM_DETUNE(BaseName##detune, CONTROL_FM_DETUNE_DEFAULT_VALUE(), CONTROL_FM_DETUNE_DEFAULT_RANGE()), \
    _REGISTER_CONTROL_FM_FEEDBACK(BaseName##feedback, 0.0, CONTROL_FM_OPERATOR_FEEDBACK_RANGE())


#define CONTROL_FM_OPERATOR_OUTPUT_VOLUME_RATIO_RANGE() CONTROL_RANGE_STEP(1.0, 10.0, 0.1)
#define _REGISTER_CONTROL_FM_OUTPUT_VOLUME_RATIO(Name, Value, Range) \
    REGISTER_CONTROL_FLOATING( \
        Name, Value, Range, \
        TR_TABLE( \
            TR(English, "Operator volume ratio"), \
            TR(French, "Volume de l'opérateur") \
        ), \
        TR_TABLE( \
            TR(English, "Operator volume ratio"), \
            TR(French, "Volume de l'opérateur") \
        ), \
        TR_TABLE( \
            TR(English, "Vol Rate") \
        ), \
        TR_TABLE( \
            TR(English, "%") \
        ) \
    )

#define CONTROL_FM_OPERATOR_OUTPUT_VOLUME_RANGE() CONTROL_RANGE_STEP(0.0, 1.0, 0.01)
#define _REGISTER_CONTROL_FM_OUTPUT_VOLUME(Name, Value, Range) \
    REGISTER_CONTROL_FLOATING( \
        Name, Value, Range, \
        TR_TABLE( \
            TR(English, "Operator volume"), \
            TR(French, "Volume de l'opérateur") \
        ), \
        TR_TABLE( \
            TR(English, "Operator volume"), \
            TR(French, "Volume de l'opérateur") \
        ), \
        TR_TABLE( \
            TR(English, "Vol") \
        ), \
        TR_TABLE( \
            TR(English, "%") \
        ) \
    )

#define CONTROL_FM_RATIO_DEFAULT_RANGE() CONTROL_RANGE_STEP(1.0, 15.0, 0.01)
#define CONTROL_FM_RATIO_DEFAULT_VALUE() 1.0
#define _REGISTER_CONTROL_FM_RATIO(Name, Value, Range) \
    REGISTER_CONTROL_FLOATING( \
        Name, Value, Range, \
        TR_TABLE( \
            TR(English, "ratio"), \
            TR(French, "ratio") \
        ), \
        TR_TABLE( \
            TR(English, "ratio"), \
            TR(French, "ratio") \
        ), \
        TR_TABLE( \
            TR(English, "Rate") \
        ), \
        TR_TABLE( \
            TR(English, "") \
        ) \
    )

#define CONTROL_FM_DETUNE_DEFAULT_RANGE() CONTROL_RANGE_STEP(-11.0, 11.0, 1)
#define CONTROL_FM_DETUNE_DEFAULT_VALUE() 0.0
#define _REGISTER_CONTROL_FM_DETUNE(Name, Value, Range) \
    REGISTER_CONTROL_FLOATING(Name, Value, Range, \
        TR_TABLE( \
            TR(English, "Detune"), \
            TR(French, "Detune") \
        ), \
        TR_TABLE( \
            TR(English, "Detune"), \
            TR(French, "Detune") \
        ), \
        TR_TABLE( \
            TR(English, "Det") \
        ), \
        TR_TABLE( \
            TR(English, "semitone") \
        ) \
    )

#define CONTROL_FM_OPERATOR_FEEDBACK_RANGE() CONTROL_RANGE_STEP(0.0, 8.0, 1.0)
#define _REGISTER_CONTROL_FM_FEEDBACK(Name, Value, Range) \
    REGISTER_CONTROL_INTEGER( \
        Name, Value, Range, \
        TR_TABLE( \
            TR(English, #Name " feedback"), \
            TR(French, "Intensité de feedback de l'opérateur") \
        ), \
        TR_TABLE( \
            TR(English, #Name " feedback"), \
            TR(French, "Intensité de feedback de l'opérateur") \
        ), \
        TR_TABLE( \
            TR(English, "Fdbk") \
        ), \
        TR_TABLE( \
            TR(English, "") \
        ) \
    )
