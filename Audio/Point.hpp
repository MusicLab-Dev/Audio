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

    /** @brief Curve rate of a point */
    using CurveRate = std::int16_t;

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

static_assert_fit_quarter_cacheline(Audio::Point);
