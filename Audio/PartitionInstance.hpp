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

        /** @brief Comparison operators */
        [[nodiscard]] inline bool operator==(const PartitionInstance &other) const noexcept
            { return partitionIndex == other.partitionIndex && offset == other.offset && range == other.range; }
        [[nodiscard]] inline bool operator!=(const PartitionInstance &other) const noexcept
            { return !operator==(other); }
        [[nodiscard]] inline bool operator>(const PartitionInstance &other) const noexcept
            { return range > other.range; }
        [[nodiscard]] inline bool operator>=(const PartitionInstance &other) const noexcept
            { return range >= other.range; }
        [[nodiscard]] inline bool operator<(const PartitionInstance &other) const noexcept
            { return range < other.range; }
        [[nodiscard]] inline bool operator<=(const PartitionInstance &other) const noexcept
            { return range <= other.range; }
    };

    /** @brief A list containing all instances of all partitions */
    using PartitionInstances = Core::SortedTinyVector<PartitionInstance>;
}