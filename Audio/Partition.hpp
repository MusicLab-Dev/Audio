/**
 * @ Author: Pierre Veysseyre
 * @ Description: Partition
 */

#pragma once

#include "Note.hpp"

namespace Audio
{
    /** @brief Store all notes of a partition */
    using Partition = Core::SortedTinyFlatVector<Note>;
}