/*
 * @ Author: Matthieu Moinvaziri
 * @ Description: Controls
 */

#pragma once

#include "ControlEvent.hpp"
#include "Automation.hpp"

namespace Audio
{
    /** @brief A list of NoteEvent */
    using ControlEvents = Core::TinyVector<ControlEvent>;

    /** @brief Header of the Automations flat vector */
    struct AutomationsHeader
    {
        /** @brief A list of ControlEvent that are generated temporarily */
        using ControlsOnTheFly = Core::TinySmallVector<ControlEvent, (Core::CacheLineSize - sizeof(ControlEvents) / sizeof(ControlEvent))>;

        ControlsOnTheFly controlsOnTheFly;
    };

    /** @brief Contains all the data related to automations */
    using Automations = Core::TinyFlatVector<Automation, AutomationsHeader>;
}
