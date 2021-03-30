/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Notes
 */

#pragma once

#include "Note.hpp"

namespace Audio
{
    /** @brief A list of NoteEvent */
    using NoteEvents = Core::TinyVector<NoteEvent>;

    /** @brief A flat vector that contains Note of a partition and has a cache of notes on the fly in its header */
    using Notes = Core::SortedTinyFlatVector<Note>;
};
