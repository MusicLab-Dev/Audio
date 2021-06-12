/**
 * @ Author: Pierre Veysseyre
 * @ Description: Plugin control macros utils
 */

#include "PluginUtils.hpp"

/**
 * @brief Helper for enveloppe generator controls
 */
#define REGISTER_CONTROL_ENVELOPPE_ATTACK(Name, Value, Range) \
    REGISTER_CONTROL_FLOATING( \
        Name, Value, Range, \
        TR_TABLE( \
            TR(English, "Envelope attack"), \
            TR(French, "Attaque de l'enveloppe") \
        ), \
        TR_TABLE( \
            TR(English, "Attack duration used by the enveloppe to determine volume gain"), \
            TR(French, "Durée pour atteindre le volume maximum") \
        ), \
        TR_TABLE( \
            TR(English, "Atk") \
        ), \
        TR_TABLE( \
            TR(English, "seconds") \
        ) \
    )

#define REGISTER_CONTROL_ENVELOPPE_DECAY(Name, Value, Range) \
    REGISTER_CONTROL_FLOATING( \
        Name, Value, Range, \
        TR_TABLE( \
            TR(English, "Envelope decay"), \
            TR(French, "Chute de l'enveloppe") \
        ), \
        TR_TABLE( \
            TR(English, "Decay duration used by the enveloppe to determine volume gain"), \
            TR(French, "Durée pour atteindre le volume intermédiaire (sustain)") \
        ), \
        TR_TABLE( \
            TR(English, "Dec") \
        ), \
        TR_TABLE( \
            TR(English, "seconds") \
        ) \
    )

#define REGISTER_CONTROL_ENVELOPPE_SUSTAIN(Name, Value, Range) \
    REGISTER_CONTROL_FLOATING( \
        Name, Value, Range, \
        TR_TABLE( \
            TR(English, "Envelope sustain"), \
            TR(French, "Niveau intermédiaire de l'enveloppe") \
        ), \
        TR_TABLE( \
            TR(English, "Sustain duration used by the enveloppe to determine volume gain"), \
            TR(French, "Volume intermédiaire utilisé tant qu'une note est jouée") \
        ), \
        TR_TABLE( \
            TR(English, "Sus") \
        ), \
        TR_TABLE( \
            TR(English, "%") \
        ) \
    )

#define REGISTER_CONTROL_ENVELOPPE_RELEASE(Name, Value, Range) \
    REGISTER_CONTROL_FLOATING( \
        Name, Value, Range, \
        TR_TABLE( \
            TR(English, "Envelope release"), \
            TR(French, "Extinction de l'enveloppe") \
        ), \
        TR_TABLE( \
            TR(English, "Release duration used by the enveloppe to determine volume gain"), \
            TR(French, "Durée pour atteindre un volume nul utilisée lorsqu'une note s'arrête de jouer") \
        ), \
        TR_TABLE( \
            TR(English, "Rel") \
        ), \
        TR_TABLE( \
            TR(English, "seconds") \
        ) \
    )

#define REGISTER_CONTROL_ENVELOPPE_ADSR(NameA, ValueA, RangeA, NameD, ValueD, RangeD, Name, ValueS, RangeS, NameR, ValueR, RangeR) REGISTER_CONTROL_ENVELOPPE_ATTACK(NameA, ValueA, RangeA), REGISTER_CONTROL_ENVELOPPE_DECAY(NameD, ValueD, RangeD), REGISTER_CONTROL_ENVELOPPE_SUSTAIN(Name, ValueS, RangeS), REGISTER_CONTROL_ENVELOPPE_RELEASE(NameR, ValueR, RangeR)
#define REGISTER_CONTROL_ENVELOPPE_AD(NameA, ValueA, RangeA, NameD, ValueD, RangeD) REGISTER_CONTROL_ENVELOPPE_ATTACK(NameA, ValueA, RangeA), REGISTER_CONTROL_ENVELOPPE_DECAY(NameD, ValueD, RangeD)
#define REGISTER_CONTROL_ENVELOPPE_AR(NameA, ValueA, RangeA, NameR, ValueR, RangeR) REGISTER_CONTROL_ENVELOPPE_ATTACK(NameA, ValueA, RangeA), REGISTER_CONTROL_ENVELOPPE_RELEASE(NameR, ValueR, RangeR)

/**
 * @brief Helper for FM controls
 */
#define REGISTER_CONTROL_FM_OPERATOR(BaseName) \
    _REGISTER_CONTROL_FM_OUTPUT_VOLUME(BaseName##volume, 0.0, CONTROL_FM_OPERATOR_OUTPUT_VOLUME_RANGE()), \
    REGISTER_CONTROL_ENVELOPPE_ADSR( \
        BaseName##attack, 0.1, CONTROL_RANGE(0.0, 10.0), \
        BaseName##decay, 0.2, CONTROL_RANGE(0.0, 10.0), \
        BaseName##sustain, 0.8, CONTROL_RANGE(0.0, 1.0), \
        BaseName##release, 0.2, CONTROL_RANGE(0.0, 10.0) \
    ), \
    _REGISTER_CONTROL_FM_RATIO(BaseName##ratio, CONTROL_FM_RATIO_DEFAULT_VALUE(), CONTROL_FM_RATIO_DEFAULT_RANGE()), \
    _REGISTER_CONTROL_FM_DETUNE(BaseName##detune, CONTROL_FM_DETUNE_DEFAULT_VALUE(), CONTROL_FM_DETUNE_DEFAULT_RANGE()), \
    _REGISTER_CONTROL_FM_FEEDBACK(BaseName##feedback, 0.0, CONTROL_FM_OPERATOR_FEEDBACK_RANGE())


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

#define CONTROL_FM_RATIO_DEFAULT_RANGE() CONTROL_RANGE_STEP(1.0, 15.0, 0.1)
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


/**
 * @brief Helper for volume controls
 */
#define CONTROL_DEFAULT_OUTPUT_VOLUME_RANGE() CONTROL_RANGE_STEP(-48.0, 12.0, 0.1)
#define CONTROL_DEFAULT_INPUT_GAIN_RANGE() CONTROL_RANGE_STEP(-48.0, 12.0, 0.1)

#define REGISTER_CONTROL_OUTPUT_VOLUME(Name, Value, Range) \
    REGISTER_CONTROL_FLOATING( \
        Name, Value, Range, \
        TR_TABLE( \
            TR(English, "Output volume"), \
            TR(French, "Volume de sortie") \
        ), \
        TR_TABLE( \
            TR(English, "Output volume of the plugin"), \
            TR(French, "Volume de sortie du plugin") \
        ), \
        TR_TABLE( \
            TR(English, "Out") \
        ), \
        TR_TABLE( \
            TR(English, "decibels") \
        ) \
    )

#define REGISTER_CONTROL_INPUT_GAIN(Name, Value, Range) \
    REGISTER_CONTROL_FLOATING( \
        Name, Value, Range, \
        TR_TABLE( \
            TR(English, "Input gain"), \
            TR(French, "Gain d'entrée") \
        ), \
        TR_TABLE( \
            TR(English, "Input gain of the plugin"), \
            TR(French, "Gain d'entrée du plugin") \
        ), \
        TR_TABLE( \
            TR(English, "In") \
        ), \
        TR_TABLE( \
            TR(English, "decibels") \
        ) \
    )

/**
 * @brief Helper for filter/equalizer controls
 */
#define CONTROL_FILTER_CUTOFF_DEFAULT_RANGE() CONTROL_RANGE_STEP(10.0, 20'000.0, 1.0)
#define CONTROL_EQUALIZER_BAND_DEFAULT_RANGE() CONTROL_RANGE_STEP(-12.0, 12.0, 0.1)
#define CONTROL_EQUALIZER_BAND_DEFAULT_VALUE() 0.0

#define REGISTER_CONTROL_EFFECT_BYPASS(Name) \
    REGISTER_CONTROL_BOOLEAN( \
        Name, 0, \
        TR_TABLE( \
            TR(English, "By-pass the effect"), \
            TR(French, "Désactive l'effet") \
        ), \
        TR_TABLE( \
            TR(English, "By-pass the effect"), \
            TR(French, "Désactive l'effet") \
        ), \
        TR_TABLE( \
            TR(English, "Byp") \
        ), \
        TR_TABLE() \
    )

#define REGISTER_CONTROL_FILTER_CUTOFF(Name, Value, Range) \
    REGISTER_CONTROL_FLOATING( \
        Name, Value, Range, \
        TR_TABLE( \
            TR(English, "Cutoff frequency"), \
            TR(French, "Fréquence de coupure") \
        ), \
        TR_TABLE( \
            TR(English, "Cutoff frequency"), \
            TR(French, "Fréquence de coupure") \
        ), \
        TR_TABLE( \
            TR(English, "Cut") \
        ), \
        TR_TABLE( \
            TR(English, "hertz") \
        ) \
    )

#define REGISTER_CONTROL_FILTER_CUTOFF_DESCRIPTION(Name, Value, Range, Description) \
    REGISTER_CONTROL_FLOATING( \
        Name, Value, Range, \
        TR_TABLE( \
            TR(English, "Cutoff frequency " Description), \
            TR(French, "Fréquence de coupure " Description) \
        ), \
        TR_TABLE( \
            TR(English, "Cutoff frequency " Description), \
            TR(French, "Fréquence de coupure " Description) \
        ), \
        TR_TABLE( \
            TR(English, "Cut") \
        ), \
        TR_TABLE( \
            TR(English, "hertz") \
        ) \
    )

#define REGISTER_CONTROL_EQUALIZER_BAND_VOLUME(Name, Value, Range) \
    REGISTER_CONTROL_FLOATING( \
        Name, Value, Range, \
        TR_TABLE( \
            TR(English, "Volume"), \
            TR(French, "Volume") \
        ), \
        TR_TABLE( \
            TR(English, "Volume of frequency band"), \
            TR(French, "Volume d'une plage de fréquence") \
        ), \
        TR_TABLE( \
            TR(English, "Vol") \
        ), \
        TR_TABLE( \
            TR(English, "decibels") \
        ) \
    )

#define FOR_EACH_EQUALIZER_BAND(N, what, Name, Range)    _FOR_EACH_EQUALIZER_BAND_##N(what, Name, Range)
#define _FOR_EACH_EQUALIZER_BAND_0(what)
#define _FOR_EACH_EQUALIZER_BAND_1(what, Name, Range)    what(Name##_0, CONTROL_EQUALIZER_BAND_DEFAULT_VALUE(), Range)
#define _FOR_EACH_EQUALIZER_BAND_2(what, Name, Range)    what(Name##_1, CONTROL_EQUALIZER_BAND_DEFAULT_VALUE(), Range), _FOR_EACH_EQUALIZER_BAND_1(what, Name, Range)
#define _FOR_EACH_EQUALIZER_BAND_3(what, Name, Range)    what(Name##_2, CONTROL_EQUALIZER_BAND_DEFAULT_VALUE(), Range), _FOR_EACH_EQUALIZER_BAND_2(what, Name, Range)
#define _FOR_EACH_EQUALIZER_BAND_4(what, Name, Range)    what(Name##_3, CONTROL_EQUALIZER_BAND_DEFAULT_VALUE(), Range), _FOR_EACH_EQUALIZER_BAND_3(what, Name, Range)
#define _FOR_EACH_EQUALIZER_BAND_5(what, Name, Range)    what(Name##_4, CONTROL_EQUALIZER_BAND_DEFAULT_VALUE(), Range), _FOR_EACH_EQUALIZER_BAND_4(what, Name, Range)
#define _FOR_EACH_EQUALIZER_BAND_6(what, Name, Range)    what(Name##_5, CONTROL_EQUALIZER_BAND_DEFAULT_VALUE(), Range), _FOR_EACH_EQUALIZER_BAND_5(what, Name, Range)
#define _FOR_EACH_EQUALIZER_BAND_7(what, Name, Range)    what(Name##_6, CONTROL_EQUALIZER_BAND_DEFAULT_VALUE(), Range), _FOR_EACH_EQUALIZER_BAND_6(what, Name, Range)
#define _FOR_EACH_EQUALIZER_BAND_8(what, Name, Range)    what(Name##_7, CONTROL_EQUALIZER_BAND_DEFAULT_VALUE(), Range), _FOR_EACH_EQUALIZER_BAND_7(what, Name, Range)
#define _FOR_EACH_EQUALIZER_BAND_9(what, Name, Range)    what(Name##_8, CONTROL_EQUALIZER_BAND_DEFAULT_VALUE(), Range), _FOR_EACH_EQUALIZER_BAND_8(what, Name, Range)
#define _FOR_EACH_EQUALIZER_BAND_10(what, Name, Range)   what(Name##_9, CONTROL_EQUALIZER_BAND_DEFAULT_VALUE(), Range), _FOR_EACH_EQUALIZER_BAND_9(what, Name, Range)
#define _FOR_EACH_EQUALIZER_BAND_11(what, Name, Range)   what(Name##_10, CONTROL_EQUALIZER_BAND_DEFAULT_VALUE(), Range), _FOR_EACH_EQUALIZER_BAND_10(what, Name, Range)
#define _FOR_EACH_EQUALIZER_BAND_12(what, Name, Range)   what(Name##_11, CONTROL_EQUALIZER_BAND_DEFAULT_VALUE(), Range), _FOR_EACH_EQUALIZER_BAND_11(what, Name, Range)
#define _FOR_EACH_EQUALIZER_BAND_13(what, Name, Range)   what(Name##_12, CONTROL_EQUALIZER_BAND_DEFAULT_VALUE(), Range), _FOR_EACH_EQUALIZER_BAND_12(what, Name, Range)
#define _FOR_EACH_EQUALIZER_BAND_14(what, Name, Range)   what(Name##_13, CONTROL_EQUALIZER_BAND_DEFAULT_VALUE(), Range), _FOR_EACH_EQUALIZER_BAND_13(what, Name, Range)
#define _FOR_EACH_EQUALIZER_BAND_15(what, Name, Range)   what(Name##_14, CONTROL_EQUALIZER_BAND_DEFAULT_VALUE(), Range), _FOR_EACH_EQUALIZER_BAND_14(what, Name, Range)
#define _FOR_EACH_EQUALIZER_BAND_16(what, Name, Range)   what(Name##_15, CONTROL_EQUALIZER_BAND_DEFAULT_VALUE(), Range), _FOR_EACH_EQUALIZER_BAND_15(what, Name, Range)


#define REGISTER_CONTROL_EQUALIZER_BANDS(Name, Count, Range) \
    FOR_EACH_EQUALIZER_BAND(Count, REGISTER_CONTROL_EQUALIZER_BAND_VOLUME, Name, Range)
