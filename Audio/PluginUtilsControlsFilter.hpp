/**
 * @ Author: Pierre Veysseyre
 * @ Description: Plugin control macros utils for equalizer
 */

#pragma once

#include "PluginUtils.hpp"

/**
 * @brief Helper for filter controls
 */
#define CONTROL_FILTER_CUTOFF_DEFAULT_RANGE() CONTROL_RANGE_STEP(10.0, 20'000.0, 1.0)
#define CONTROL_EQUALIZER_BAND_DEFAULT_RANGE() CONTROL_RANGE_STEP(-12.0, 12.0, 0.1)
#define CONTROL_EQUALIZER_BAND_DEFAULT_VALUE() 0.0

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
