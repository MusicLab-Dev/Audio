/*
 * @ Author: Pierre Veysseyre
 * @ Description: Point
 */

#pragma once

#include "Base.hpp"

namespace Audio
{
    struct Point;
}

/** @brief Represent a point in an automation curve */
struct alignas_quarter_cacheline Audio::Point
{
    /** @brief Describe the interpolation type between points */
    enum class CurveType : std::uint8_t {
        Linear, Fast, Slow
    };

    using CurveRate = std::int16_t;

    // /** @brief POD semantics */
    // Point(void) noexcept = default;
    // Point(const ParamValue value_) noexcept : value(value_) {}
    // Point(const Beat beat_, const ParamValue value_) noexcept : beat(beat_), value(value_) {}
    // Point(const Point &other) noexcept = default;
    // Point(Point &&other) noexcept = default;
    // Point &operator=(const Point &other) noexcept = default;
    // Point &operator=(Point &&other) noexcept = default;

    /** @brief Equality operators */
    [[nodiscard]] bool operator==(const Point &other) const noexcept
        { return beat == other.beat && type == other.type && curveRate == other.curveRate && value == other.value; }
    [[nodiscard]] bool operator!=(const Point &other) const noexcept { return !(*this == other); }

    /** @brief Comparison operators */
    [[nodiscard]] bool operator<(const Point &other) const noexcept { return beat < other.beat; }
    [[nodiscard]] bool operator<=(const Point &other) const noexcept { return beat <= other.beat; }
    [[nodiscard]] bool operator>(const Point &other) const noexcept { return beat > other.beat; }
    [[nodiscard]] bool operator>=(const Point &other) const noexcept { return beat >= other.beat; }

    Beat                    beat {};
    alignas(2) CurveType    type { CurveType::Linear };
    CurveRate               curveRate {}; // We may change this to unsigned 24bits for better precision
    ParamValue              value {};
};