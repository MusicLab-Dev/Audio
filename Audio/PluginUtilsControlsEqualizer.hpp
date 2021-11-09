/**
 * @ Author: Pierre Veysseyre
 * @ Description: Plugin control macros utils for equalizer
 */

#pragma once

#include "PluginUtils.hpp"

/**
 * @brief Helper for equalizer controls
 */
#define REGISTER_CONTROL_EQUALIZER_BAND_VOLUME(Name, Value, Range) \
    REGISTER_CONTROL_NUMERIC( \
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
