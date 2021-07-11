/**
 * @ Author: Pierre Veysseyre
 * @ Description: Plugin control macros utils for volume
 */

#pragma once

#include <cmath>

#include "PluginUtils.hpp"

/**
 * @brief Helper to map controls
 */

// bright[0:100] -> apBvolume[20:80]

#define CONTROL_MAP(Src, Dest, DestFrom, DestTo) \
    Dest(std::lerp(DestFrom, DestTo, Src()));
