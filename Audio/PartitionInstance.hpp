/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: PartitionInstance
 */

#pragma once

#include <Core/SortedVector.hpp>

#include "Base.hpp"

namespace Audio
{
    /** @brief Instance of a partition */
    struct alignas_quarter_cacheline PartitionInstance
    {
        std::uint32_t partitionIndex { 0u };
        Beat offset { 0u };
        BeatRange range {};

        /** @brief Comparison operator for Sorted vector */
        [[nodiscard]] bool operator<(const PartitionInstance &other) const noexcept
            { return other.range.from < other.range.from; }
    };

    /** @brief A list containing all instances of all partitions */
    using PartitionInstances = Core::SortedTinyVector<PartitionInstance>;
}