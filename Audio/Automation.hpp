/*
 * @ Author: Pierre Veysseyre
 * @ Description: Automation
 */

#pragma once

#include "Point.hpp"

namespace Audio
{
    /** @brief Header of an automation */
    struct AutomationHeader
    {
        bool muted { false };
    };

    /** @brief A list of point with an AutomationHeader */
    using Automation = Core::SortedTinyFlatVector<Point, std::less<Point>, AutomationHeader>;
}