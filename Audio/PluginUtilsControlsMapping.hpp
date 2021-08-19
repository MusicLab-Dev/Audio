/**
 * @ Author: Pierre Veysseyre
 * @ Description: Plugin control macros utils for volume
 */

#pragma once

#include <cmath>

/**
 * @brief Helper to map controls
 */

// impact[0:100] -> pitchEnvVolume[0:1]

#define CONTROL_MAP(SrcValue, DestValue, DestRangeFrom, DestRangeTo) \
    DestValue(std::lerp(DestRangeFrom, DestRangeTo, SrcValue()));
