/**
 * @ Author: Pierre Veysseyre
 * @ Description: Plugin control macros utils for volume
 */

#pragma once

#include "PluginUtils.hpp"

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
