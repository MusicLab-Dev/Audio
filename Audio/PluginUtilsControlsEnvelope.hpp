/**
 * @ Author: Pierre Veysseyre
 * @ Description: Plugin control macros utils for envelope
 */

#pragma once

#include "PluginUtils.hpp"

/**
 * @brief Helper for enveloppe generator controls
 */
#define REGISTER_CONTROL_ENVELOPPE_DELAY(Name, Value, Range) \
    REGISTER_CONTROL_FLOATING( \
        Name, Value, Range, \
        TR_TABLE( \
            TR(English, "Envelope delay"), \
            TR(French, "Delai de l'enveloppe") \
        ), \
        TR_TABLE( \
            TR(English, "Delay duration used by the enveloppe to determine when to start"), \
            TR(French, "Durée avant que l'enveloppe commence") \
        ), \
        TR_TABLE( \
            TR(English, "Delay") \
        ), \
        TR_TABLE( \
            TR(English, "seconds") \
        ) \
    )

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

#define REGISTER_CONTROL_ENVELOPPE_ADSR(NameA, ValueA, RangeA, NameDecay, ValueDecay, RangeDecay, Name, ValueS, RangeS, NameR, ValueR, RangeR) REGISTER_CONTROL_ENVELOPPE_ATTACK(NameA, ValueA, RangeA), REGISTER_CONTROL_ENVELOPPE_DECAY(NameDecay, ValueDecay, RangeDecay), REGISTER_CONTROL_ENVELOPPE_SUSTAIN(Name, ValueS, RangeS), REGISTER_CONTROL_ENVELOPPE_RELEASE(NameR, ValueR, RangeR)
#define REGISTER_CONTROL_ENVELOPPE_DADSR(NameDelay, ValueDelay, RangeDelay, NameA, ValueA, RangeA, NameDecay, ValueDecay, RangeDecay, Name, ValueS, RangeS, NameR, ValueR, RangeR) REGISTER_CONTROL_ENVELOPPE_DELAY(NameDelay, ValueDelay, RangeDelay), REGISTER_CONTROL_ENVELOPPE_ATTACK(NameA, ValueA, RangeA), REGISTER_CONTROL_ENVELOPPE_DECAY(NameDecay, ValueDecay, RangeDecay), REGISTER_CONTROL_ENVELOPPE_SUSTAIN(Name, ValueS, RangeS), REGISTER_CONTROL_ENVELOPPE_RELEASE(NameR, ValueR, RangeR)
#define REGISTER_CONTROL_ENVELOPPE_AD(NameA, ValueA, RangeA, NameDecay, ValueDecay, RangeDecay) REGISTER_CONTROL_ENVELOPPE_ATTACK(NameA, ValueA, RangeA), REGISTER_CONTROL_ENVELOPPE_DECAY(NameDecay, ValueDecay, RangeDecay)
#define REGISTER_CONTROL_ENVELOPPE_AR(NameA, ValueA, RangeA, NameR, ValueR, RangeR) REGISTER_CONTROL_ENVELOPPE_ATTACK(NameA, ValueA, RangeA), REGISTER_CONTROL_ENVELOPPE_RELEASE(NameR, ValueR, RangeR)
