/**
 * @file PluginControlUtils.hpp
 * @brief Plugin control macros utils
 *
 * @author Pierre V
 * @date 2021-04-23
 */

#include "PluginUtils.hpp"

/**
 * @brief Helper for enveloppe generator controls
 */
#define REGISTER_CONTROL_ENVELOPPE_ATTACK(Name, Value, Range) \
    REGISTER_CONTROL( \
        Floating, Name, Value, Range, \
        TR_TABLE( \
            TR(English, "Enveloppe attack"), \
            TR(French, "Attaque de l'enveloppe") \
        ), \
        TR_TABLE( \
            TR(English, "Attack duration used by the enveloppe to determine volume gain"), \
            TR(French, "Durée pour atteindre le volume maximum") \
        ) \
    )

#define REGISTER_CONTROL_ENVELOPPE_DECAY(Name, Value, Range) \
    REGISTER_CONTROL( \
        Floating, Name, Value, Range, \
        TR_TABLE( \
            TR(English, "Enveloppe decay"), \
            TR(French, "Chute de l'enveloppe") \
        ), \
        TR_TABLE( \
            TR(English, "Decay duration used by the enveloppe to determine volume gain"), \
            TR(French, "Durée pour atteindre le volume intermédiaire (sustain)") \
        ) \
    )

#define REGISTER_CONTROL_ENVELOPPE_SUSTAIN(Name, Value, Range) \
    REGISTER_CONTROL( \
        Floating, Name, Value, Range, \
        TR_TABLE( \
            TR(English, "Enveloppe sustain"), \
            TR(French, "Niveau intermédiaire de l'enveloppe") \
        ), \
        TR_TABLE( \
            TR(English, "Sustain duration used by the enveloppe to determine volume gain"), \
            TR(French, "Volume intermédiaire utilisé tant qu'une note est jouée") \
        ) \
    )

#define REGISTER_CONTROL_ENVELOPPE_RELEASE(Name, Value, Range) \
    REGISTER_CONTROL( \
        Floating, Name, Value, Range, \
        TR_TABLE( \
            TR(English, "Enveloppe release"), \
            TR(French, "Extinction de l'enveloppe") \
        ), \
        TR_TABLE( \
            TR(English, "Release duration used by the enveloppe to determine volume gain"), \
            TR(French, "Durée pour atteindre un volume nul utilisée lorsqu'une note s'arrête de jouer") \
        ) \
    )

#define REGISTER_CONTROL_ENVELOPPE_ADSR(NameA, ValueA, RangeA, NameD, ValueD, RangeD, Name, ValueS, RangeS, NameR, ValueR, RangeR) REGISTER_CONTROL_ENVELOPPE_ATTACK(NameA, ValueA, RangeA), REGISTER_CONTROL_ENVELOPPE_DECAY(NameD, ValueD, RangeD), REGISTER_CONTROL_ENVELOPPE_SUSTAIN(Name, ValueS, RangeS), REGISTER_CONTROL_ENVELOPPE_RELEASE(NameR, ValueR, RangeR)
#define REGISTER_CONTROL_ENVELOPPE_AD(NameA, ValueA, RangeA, NameD, ValueD, RangeD) REGISTER_CONTROL_ENVELOPPE_ATTACK(NameA, ValueA, RangeA), REGISTER_CONTROL_ENVELOPPE_DECAY(NameD, ValueD, RangeD)
#define REGISTER_CONTROL_ENVELOPPE_AR(NameA, ValueA, RangeA, NameR, ValueR, RangeR) REGISTER_CONTROL_ENVELOPPE_ATTACK(NameA, ValueA, RangeA), REGISTER_CONTROL_ENVELOPPE_RELEASE(NameR, ValueR, RangeR)

/**
 * @brief Helper for volume controls
 */
#define CONTROL_DEFAULT_OUTPUT_VOLUME_RANGE() CONTROL_RANGE(-48.0, 12.0)
#define CONTROL_DEFAULT_INPUT_GAIN_RANGE() CONTROL_RANGE(-48.0, +48.0)

#define REGISTER_CONTROL_OUTPUT_VOLUME(Name, Value, Range) \
    REGISTER_CONTROL( \
        Floating, Name, Value, Range, \
        TR_TABLE( \
            TR(English, "Output volume"), \
            TR(French, "Volume de sortie") \
        ), \
        TR_TABLE( \
            TR(English, "Output volume of the plugin"), \
            TR(French, "Volume de sortie du plugin") \
        ) \
    )

#define REGISTER_CONTROL_INPUT_GAIN(Name, Value, Range) \
    REGISTER_CONTROL( \
        Floating, Name, Value, Range, \
        TR_TABLE( \
            TR(English, "Input gain"), \
            TR(French, "Gain d'entrée") \
        ), \
        TR_TABLE( \
            TR(English, "Input gain of the plugin"), \
            TR(French, "Gain d'entrée du plugin") \
        ) \
    )

/**
 * @brief Helper for filter/equalizer controls
 */
#define CONTROL_FILTER_CUTOFF_DEFAULT_RANGE() CONTROL_RANGE(0.0, 1000.0)
#define CONTROL_EQUALIZER_BAND_DEFAULT_RANGE() CONTROL_RANGE(-18.0, 18.0)
#define CONTROL_EQUALIZER_BAND_DEFAULT_VALUE() 0.0

#define REGISTER_CONTROL_FILTER_CUTOFF(Name, Value, Range) \
    REGISTER_CONTROL( \
        Floating, Name, Value, Range, \
        TR_TABLE( \
            TR(English, "Cutoff frequency"), \
            TR(French, "Fréquence de coupure") \
        ), \
        TR_TABLE( \
            TR(English, "Cutoff frequency"), \
            TR(French, "Fréquence de coupure") \
        ) \
    )

#define REGISTER_CONTROL_FILTER_CUTOFF_DESCRIPTION(Name, Value, Range, Description) \
    REGISTER_CONTROL( \
        Floating, Name, Value, Range, \
        TR_TABLE( \
            TR(English, "Cutoff frequency " Description), \
            TR(French, "Fréquence de coupure " Description) \
        ), \
        TR_TABLE( \
            TR(English, "Cutoff frequency " Description), \
            TR(French, "Fréquence de coupure " Description) \
        ) \
    )

#define FOR_COMMA_RANGE(N, what, Name, StartFrequency, Count, Range)    _FOR_COMMA_RANGE_##N(what, Name, StartFrequency, Count, Range)
#define _FOR_COMMA_RANGE_0(what)
#define _FOR_COMMA_RANGE_1(what, Name, StartFrequency, Count, Range)    what(Name##_0, CONTROL_EQUALIZER_BAND_DEFAULT_VALUE(), Range, #StartFrequency "Hz")
#define _FOR_COMMA_RANGE_2(what, Name, StartFrequency, Count, Range)    what(Name##_1, CONTROL_EQUALIZER_BAND_DEFAULT_VALUE(), Range, #StartFrequency "Hz"), _FOR_COMMA_RANGE_1(what, Name, StartFrequency * 2, Count, Range)
#define _FOR_COMMA_RANGE_3(what, Name, StartFrequency, Count, Range)    what(Name##_2, CONTROL_EQUALIZER_BAND_DEFAULT_VALUE(), Range, #StartFrequency "Hz"), _FOR_COMMA_RANGE_2(what, Name, StartFrequency * 2, Count, Range)
#define _FOR_COMMA_RANGE_4(what, Name, StartFrequency, Count, Range)    what(Name##_3, CONTROL_EQUALIZER_BAND_DEFAULT_VALUE(), Range, #StartFrequency "Hz"), _FOR_COMMA_RANGE_3(what, Name, StartFrequency * 2, Count, Range)
#define _FOR_COMMA_RANGE_5(what, Name, StartFrequency, Count, Range)    what(Name##_4, CONTROL_EQUALIZER_BAND_DEFAULT_VALUE(), Range, #StartFrequency "Hz"), _FOR_COMMA_RANGE_4(what, Name, StartFrequency * 2, Count, Range)
#define _FOR_COMMA_RANGE_6(what, Name, StartFrequency, Count, Range)    what(Name##_5, CONTROL_EQUALIZER_BAND_DEFAULT_VALUE(), Range, #StartFrequency "Hz"), _FOR_COMMA_RANGE_5(what, Name, StartFrequency * 2, Count, Range)
#define _FOR_COMMA_RANGE_7(what, Name, StartFrequency, Count, Range)    what(Name##_6, CONTROL_EQUALIZER_BAND_DEFAULT_VALUE(), Range, #StartFrequency "Hz"), _FOR_COMMA_RANGE_6(what, Name, StartFrequency * 2, Count, Range)
#define _FOR_COMMA_RANGE_8(what, Name, StartFrequency, Count, Range)    what(Name##_7, CONTROL_EQUALIZER_BAND_DEFAULT_VALUE(), Range, #StartFrequency "Hz"), _FOR_COMMA_RANGE_7(what, Name, StartFrequency * 2, Count, Range)
#define _FOR_COMMA_RANGE_9(what, Name, StartFrequency, Count, Range)    what(Name##_8, CONTROL_EQUALIZER_BAND_DEFAULT_VALUE(), Range, #StartFrequency "Hz"), _FOR_COMMA_RANGE_8(what, Name, StartFrequency * 2, Count, Range)
#define _FOR_COMMA_RANGE_10(what, Name, StartFrequency, Count, Range)   what(Name##_9, CONTROL_EQUALIZER_BAND_DEFAULT_VALUE(), Range, #StartFrequency "Hz"), _FOR_COMMA_RANGE_9(what, Name, StartFrequency * 2, Count, Range)
#define _FOR_COMMA_RANGE_11(what, Name, StartFrequency, Count, Range)   what(Name##_10, CONTROL_EQUALIZER_BAND_DEFAULT_VALUE(), Range, #StartFrequency "Hz"), _FOR_COMMA_RANGE_10(what, Name, StartFrequency * 2, Count, Range)
#define _FOR_COMMA_RANGE_12(what, Name, StartFrequency, Count, Range)   what(Name##_11, CONTROL_EQUALIZER_BAND_DEFAULT_VALUE(), Range, #StartFrequency "Hz"), _FOR_COMMA_RANGE_11(what, Name, StartFrequency * 2, Count, Range)
#define _FOR_COMMA_RANGE_13(what, Name, StartFrequency, Count, Range)   what(Name##_12, CONTROL_EQUALIZER_BAND_DEFAULT_VALUE(), Range, #StartFrequency "Hz"), _FOR_COMMA_RANGE_12(what, Name, StartFrequency * 2, Count, Range)
#define _FOR_COMMA_RANGE_14(what, Name, StartFrequency, Count, Range)   what(Name##_13, CONTROL_EQUALIZER_BAND_DEFAULT_VALUE(), Range, #StartFrequency "Hz"), _FOR_COMMA_RANGE_13(what, Name, StartFrequency * 2, Count, Range)
#define _FOR_COMMA_RANGE_15(what, Name, StartFrequency, Count, Range)   what(Name##_14, CONTROL_EQUALIZER_BAND_DEFAULT_VALUE(), Range, #StartFrequency "Hz"), _FOR_COMMA_RANGE_14(what, Name, StartFrequency * 2, Count, Range)
#define _FOR_COMMA_RANGE_16(what, Name, StartFrequency, Count, Range)   what(Name##_15, CONTROL_EQUALIZER_BAND_DEFAULT_VALUE(), Range, #StartFrequency "Hz"), _FOR_COMMA_RANGE_15(what, Name, StartFrequency * 2, Count, Range)


#define REGISTER_CONTROL_EQUALIZER_BANDS(Name, StartFrequency, Count, Range) \
    FOR_COMMA_RANGE(Count, REGISTER_CONTROL_FILTER_CUTOFF_DESCRIPTION, Name, StartFrequency, Count, Range)
