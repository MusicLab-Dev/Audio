/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Notes
 */

#pragma once

#include "Partition.hpp"

namespace Audio
{
    /** @brief A list of NoteEvent that are generated temporarily */
    using NotesOnTheFly = Core::TinySmallVector<NoteEvent, (Core::CacheLineSize - sizeof(NoteEvents) / sizeof(NoteEvent))>;

    /** @brief A flat vector that contains partitions and has a cache of notes on the fly in its header */
    using Partitions = Core::TinyFlatVector<Partition, NotesOnTheFly>;
}
