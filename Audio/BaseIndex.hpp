/**
 * @ Author: Pierre Veysseyre
 * @ Description: Base Ranges
 */

#pragma once

#include <ostream>
#include <cstdint>
#include <limits>

#include <Flow/Core/Core/Utils.hpp>

namespace Audio
{
    /** @brief Beat index */
    using Beat = std::uint32_t;

    /** @brief Time index */
    using TimeIndex = std::uint32_t;

    /** @brief Bar (mesure solfege) precision */
    static constexpr auto BeatPrecision = 128u;

    /** @brief Max beat index */
    static constexpr auto BeatIndexMax = std::numeric_limits<Beat>::max() / BeatPrecision;

    struct BeatRange;
    struct TimeRange;
}

/** @brief Beat range */
struct alignas_eighth_cacheline Audio::BeatRange
{
    Beat from {};
    Beat to {};

    [[nodiscard]] inline bool operator==(const BeatRange &other) const noexcept { return (from == other.from) && (to == other.to); }
    [[nodiscard]] inline bool operator!=(const BeatRange &other) const noexcept { return !(operator==(other)); }

    [[nodiscard]] inline bool operator>(const BeatRange &other) const noexcept { return (from > other.from && to > other.to); }
    [[nodiscard]] inline bool operator<(const BeatRange &other) const noexcept { return (from < other.to && to < other.to); }
    [[nodiscard]] inline bool operator<=(const BeatRange &other) const noexcept { return !(*this > other) || (from <= other.from && to <= other.to); }
    [[nodiscard]] inline bool operator>=(const BeatRange &other) const noexcept { return !(operator<(other)); }

    inline BeatRange operator+(const Beat size) noexcept { return BeatRange({ from + size, to + size }); }
    inline BeatRange &operator+=(const Beat size) noexcept { from += size; to += size; return *this; }
    inline BeatRange operator-(const Beat size) noexcept { return BeatRange({ from - size, to - size }); }
    inline BeatRange &operator-=(const Beat size) noexcept { from -= size; to -= size; return *this; }

    inline void increment(const Beat inc) noexcept { from = to; to += inc; }
};

static_assert_fit_eighth_cacheline(Audio::BeatRange);

/** @brief Time range */
struct alignas_eighth_cacheline Audio::TimeRange
{
    TimeIndex from {};
    TimeIndex to {};

    [[nodiscard]] inline bool operator==(const TimeRange &other) const noexcept { return (from == other.from) && (to == other.to); }
    [[nodiscard]] inline bool operator!=(const TimeRange &other) const noexcept { return !(operator==(other)); }
};

static_assert_fit_eighth_cacheline(Audio::TimeRange);


std::ostream &operator<<(std::ostream &out, const Audio::BeatRange &range);
std::ostream &operator<<(std::ostream &out, const Audio::TimeRange &range);
