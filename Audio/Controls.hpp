/*
 * @ Author: Matthieu Moinvaziri
 * @ Description: Controls
 */

#pragma once

#include "Control.hpp"

namespace Audio
{
    class Control;
    struct ControlEvent;

    /** @brief A list of NoteEvent */
    using ControlEvents = Core::TinyVector<ControlEvent>;

    /** @brief A list of NoteEvent that are generated temporarily */
    using ControlsOnTheFly = Core::TinySmallVector<ControlEvent, (Core::CacheLineSize - sizeof(ControlEvents) / sizeof(ControlEvent))>;

    /** @brief A flat vector that contains Note of a partition and has a cache of notes on the fly in its header */
    using Controls = Core::TinyFlatVector<Control, ControlsOnTheFly>;
}
