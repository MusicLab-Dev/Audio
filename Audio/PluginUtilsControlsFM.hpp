/**
 * @ Author: Pierre Veysseyre
 * @ Description: Plugin control macros utils for FM
 */

#pragma once

#include "PluginUtilsControlsEnvelope.hpp"

/**
 * @brief Helper for FM controls
 */

#define REGISTER_CONTROL_FM_OPERATOR_EACH(prefix, ...)          _REGISTER_CONTROL_FM_OPERATOR_EACH(prefix, VA_ARGC(__VA_ARGS__), __VA_ARGS__)
#define _REGISTER_CONTROL_FM_OPERATOR_EACH(prefix, N, ...)      _CONCATENATE(_REGISTER_CONTROL_FM_OPERATOR_EACH_, N)(prefix __VA_OPT__(,) __VA_ARGS__)
#define _REGISTER_CONTROL_FM_OPERATOR_EACH_0()
#define _REGISTER_CONTROL_FM_OPERATOR_EACH_1(prefix, x)         REGISTER_CONTROL_FM_OPERATOR_##prefix(x)
#define _REGISTER_CONTROL_FM_OPERATOR_EACH_2(prefix, x, ...)    REGISTER_CONTROL_FM_OPERATOR_##prefix(x), _REGISTER_CONTROL_FM_OPERATOR_EACH_1(prefix, __VA_ARGS__)
#define _REGISTER_CONTROL_FM_OPERATOR_EACH_3(prefix, x, ...)    REGISTER_CONTROL_FM_OPERATOR_##prefix(x), _REGISTER_CONTROL_FM_OPERATOR_EACH_2(prefix, __VA_ARGS__)
#define _REGISTER_CONTROL_FM_OPERATOR_EACH_4(prefix, x, ...)    REGISTER_CONTROL_FM_OPERATOR_##prefix(x), _REGISTER_CONTROL_FM_OPERATOR_EACH_3(prefix, __VA_ARGS__)
#define _REGISTER_CONTROL_FM_OPERATOR_EACH_5(prefix, x, ...)    REGISTER_CONTROL_FM_OPERATOR_##prefix(x), _REGISTER_CONTROL_FM_OPERATOR_EACH_4(prefix, __VA_ARGS__)
#define _REGISTER_CONTROL_FM_OPERATOR_EACH_6(prefix, x, ...)    REGISTER_CONTROL_FM_OPERATOR_##prefix(x), _REGISTER_CONTROL_FM_OPERATOR_EACH_5(prefix, __VA_ARGS__)
#define _REGISTER_CONTROL_FM_OPERATOR_EACH_7(prefix, x, ...)    REGISTER_CONTROL_FM_OPERATOR_##prefix(x), _REGISTER_CONTROL_FM_OPERATOR_EACH_6(prefix, __VA_ARGS__)

#define REGISTER_CONTROL_FM_ALGORITHM_DEFAULT(...)              REGISTER_CONTROL_FM_OPERATOR_EACH(DEFAULT, __VA_ARGS__)
#define REGISTER_CONTROL_FM_ALGORITHM_DELAY(...)                REGISTER_CONTROL_FM_OPERATOR_EACH(DELAY, __VA_ARGS__)
#define REGISTER_CONTROL_FM_ALGORITHM_DEFAULT_PITCH(...) \
    REGISTER_CONTROL_FM_OPERATOR_EACH(DEFAULT, __VA_ARGS__), \
    REGISTER_CONTROL_ENVELOPPE_ADSR( \
        pitchEnvattack, 0.1, CONTROL_RANGE_STEP(0.0, 5.0, 0.0001), \
        pitchEnvdecay, 0.2, CONTROL_RANGE_STEP(0.0, 5.0, 0.001), \
        pitchEnvsustain, 0.8, CONTROL_RANGE_STEP(0.0, 1.0, 0.01), \
        pitchEnvrelease, 0.2, CONTROL_RANGE_STEP(0.0, 5.0, 0.001) \
    ), \

#define REGISTER_CONTROL_FM_OPERATOR_DEFAULT(BaseName) \
    _REGISTER_CONTROL_FM_OUTPUT_VOLUME(BaseName##volume, -70.0, CONTROL_FM_OPERATOR_OUTPUT_VOLUME_RANGE()), \
    REGISTER_CONTROL_ENVELOPPE_ADSR( \
        BaseName##attack, 0.1, CONTROL_RANGE_STEP(0.0, 5.0, 0.0001), \
        BaseName##decay, 0.2, CONTROL_RANGE_STEP(0.0, 5.0, 0.001), \
        BaseName##sustain, 0.8, CONTROL_RANGE_STEP(0.0, 1.0, 0.01), \
        BaseName##release, 0.2, CONTROL_RANGE_STEP(0.0, 5.0, 0.001) \
    ), \
    _REGISTER_CONTROL_FM_RATIO(BaseName##ratio, CONTROL_FM_RATIO_DEFAULT_VALUE(), CONTROL_FM_RATIO_DEFAULT_RANGE()), \
    _REGISTER_CONTROL_FM_DETUNE(BaseName##detune, CONTROL_FM_DETUNE_DEFAULT_VALUE(), CONTROL_FM_DETUNE_DEFAULT_RANGE()), \
    _REGISTER_CONTROL_FM_FEEDBACK(BaseName##feedback, 0.0, CONTROL_FM_OPERATOR_FEEDBACK_RANGE()), \
    _REGISTER_CONTROL_FM_KEY_FOLLOW(BaseName##keyAmountLeft, 0.0, CONTROL_FM_OPERATOR_KEY_FOLLOW_RANGE()), \
    _REGISTER_CONTROL_FM_KEY_FOLLOW(BaseName##keyAmountRight, 0.0, CONTROL_FM_OPERATOR_KEY_FOLLOW_RANGE())

#define REGISTER_CONTROL_FM_OPERATOR_DELAY(BaseName) \
    _REGISTER_CONTROL_FM_OUTPUT_VOLUME(BaseName##volume, -70.0, CONTROL_FM_OPERATOR_OUTPUT_VOLUME_RANGE()), \
    REGISTER_CONTROL_ENVELOPPE_DADSR( \
        BaseName##delay, 0.0, CONTROL_RANGE_STEP(0.0, 0.5, 0.001), \
        BaseName##attack, 0.1, CONTROL_RANGE_STEP(0.0, 5.0, 0.001), \
        BaseName##decay, 0.2, CONTROL_RANGE_STEP(0.0, 5.0, 0.001), \
        BaseName##sustain, 0.8, CONTROL_RANGE_STEP(0.0, 1.0, 0.01), \
        BaseName##release, 0.2, CONTROL_RANGE_STEP(0.0, 5.0, 0.001) \
    ), \
    _REGISTER_CONTROL_FM_RATIO(BaseName##ratio, CONTROL_FM_RATIO_DEFAULT_VALUE(), CONTROL_FM_RATIO_DEFAULT_RANGE()), \
    _REGISTER_CONTROL_FM_DETUNE(BaseName##detune, CONTROL_FM_DETUNE_DEFAULT_VALUE(), CONTROL_FM_DETUNE_DEFAULT_RANGE()), \
    _REGISTER_CONTROL_FM_FEEDBACK(BaseName##feedback, 0.0, CONTROL_FM_OPERATOR_FEEDBACK_RANGE()), \
    _REGISTER_CONTROL_FM_KEY_FOLLOW(BaseName##keyAmountLeft, 0.0, CONTROL_FM_OPERATOR_KEY_FOLLOW_RANGE()), \
    _REGISTER_CONTROL_FM_KEY_FOLLOW(BaseName##keyAmountRight, 0.0, CONTROL_FM_OPERATOR_KEY_FOLLOW_RANGE())

/**
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
**/

#define CONTROL_FM_OPERATOR_OUTPUT_VOLUME_RANGE() CONTROL_RANGE_STEP(-70.0, 12.0, 0.1)
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
            TR(English, "dB") \
        ) \
    )

#define CONTROL_FM_RATIO_DEFAULT_RANGE() CONTROL_RANGE_STEP(0.5, 15.0, 0.01)
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

#define CONTROL_FM_DETUNE_DEFAULT_RANGE() CONTROL_RANGE_STEP(-100.0, 100.0, 0.1)
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

#define CONTROL_FM_OPERATOR_KEY_FOLLOW_RANGE() CONTROL_RANGE_STEP(-100, 100, 1)
#define _REGISTER_CONTROL_FM_KEY_FOLLOW(Name, Value, Range) \
    REGISTER_CONTROL_INTEGER( \
        Name, Value, Range, \
        TR_TABLE( \
            TR(English, #Name " key follow"), \
            TR(French, "Intensité de key follow de l'opérateur") \
        ), \
        TR_TABLE( \
            TR(English, #Name " key follow"), \
            TR(French, "Intensité de key follow de l'opérateur") \
        ), \
        TR_TABLE( \
            TR(English, "Key") \
        ), \
        TR_TABLE( \
            TR(English, "") \
        ) \
    )
