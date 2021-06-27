/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Partitions
 */

#pragma once

#include "Partition.hpp"
#include "PartitionInstance.hpp"

namespace Audio
{
    /** @brief Header of the Partitions flat vector */
    struct PartitionsHeader
    {
        /** @brief A list containing all notes passed 'on the fly' */
        using NotesOnTheFly = Core::TinyVector<NoteEvent>;

        NotesOnTheFly notesOnTheFly;
        PartitionInstances instances;
    };

    /** @brief Contains all the data related to partitions */
    using Partitions = Core::TinyFlatVector<Partition, PartitionsHeader>;
}
