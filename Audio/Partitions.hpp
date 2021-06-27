/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Notes
 */

#pragma once

#include "Partition.hpp"

namespace Audio
{
    /** @brief Instance of a partition */
    struct alignas_quarter_cacheline PartitionInstance
    {
        std::uint32_t partitionIndex { 0u };
        Beat offset { 0u };
        BeatRange range {};

        /** @brief Comparison operator for Sorted vector */
        [[nodiscard]] bool operator<(const PartitionInstance &other)
            { return other.range.from < other.range.from; }
    };

    /** @brief Header of the Partitions flat vector */
    struct PartitionsHeader
    {
        /** @brief A list containing all notes passed 'on the fly' */
        using NotesOnTheFly = Core::TinyVector<NoteEvent>;

        /** @brief A list containing all instances of all partitions */
        using Instances = Core::TinyVector<PartitionInstance>;

        NotesOnTheFly notesOnTheFly;
        Instances instances;
    };

    /** @brief Contains all the data related to partitions */
    using Partitions = Core::TinyFlatVector<Partition, PartitionsHeader>;
}
