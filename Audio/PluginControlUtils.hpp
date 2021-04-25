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
#define CONTROL_OUTPUT_VOLUME_RANGE() CONTROL_RANGE(-12.0, 12.0)

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

/**
 * @brief Helper for filter/equalizer controls
 */
#define CONTROL_FILTER_CUTOFF_DEFAULT_RANGE() CONTROL_RANGE(0.0, 20'000.0)

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
