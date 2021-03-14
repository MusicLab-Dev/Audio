/*
 * @ Author: Pierre Veysseyre
 * @ Description: Automation
 */

#pragma once

#include "Base.hpp"

namespace Audio
{
    class Automation;
    struct Point;

    /** @brief A sorted list of points */
    using Points = Core::SortedFlatVector<Point>;
};

/** @brief Represent a point in an automation curve */
struct alignas_quarter_cacheline Audio::Point
{
    /** @brief Describe the interpolation type between points */
    enum class CurveType : std::uint8_t {
        Linear, Fast, Slow
    };

    /** @brief POD semantics */
    Point(void) noexcept = default;
    Point(const ParamValue value_) noexcept : value(value_) {}
    Point(const Beat beat_, const ParamValue value_) noexcept : beat(beat_), value(value_) {}
    Point(const Point &other) noexcept = default;
    Point(Point &&other) noexcept = default;
    Point &operator=(const Point &other) noexcept = default;
    Point &operator=(Point &&other) noexcept = default;

    /** @brief Equality operators */
    [[nodiscard]] bool operator==(const Point &other) const noexcept
        { return beat == other.beat && type == other.type && curveRate == other.curveRate && value == other.value; }
    [[nodiscard]] bool operator!=(const Point &other) const noexcept { return !(*this == other); }

    Beat                    beat {};
    alignas(2) CurveType    type { CurveType::Linear };
    std::int16_t            curveRate {}; // We may change this to unsigned 24bits for better precision
    ParamValue              value {};
};

static_assert_fit_quarter_cacheline(Audio::Point);

/** @brief An automation hold a curve used to change parameters over time, it may contains multiple instances which describe where the automation appears over time */
class alignas_half_cacheline Audio::Automation
{
public:
    /** @brief Get a reference to automation points */
    [[nodiscard]] Points &points(void) noexcept { return _points; }
    [[nodiscard]] const Points &points(void) const noexcept { return _points; }

    /** @brief Get a reference to automation instances */
    [[nodiscard]] BeatRanges &instances(void) noexcept { return _instances; }
    [[nodiscard]] const BeatRanges &instances(void) const noexcept { return _instances; }

    /** @brief Get the muted state associated to this automation */
    [[nodiscard]] bool muted(void) const noexcept { return _muted; }

    /** @brief Set a new muted state to this automation, returns true if the state changed */
    bool setMuted(const bool value) noexcept;

private:
    Points _points {};
    BeatRanges _instances {};
    Core::FlatString _name {};
    bool _muted { false };
};

static_assert_fit_half_cacheline(Audio::Automation);
