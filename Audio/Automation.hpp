/*
 * @ Author: Pierre Veysseyre
 * @ Description: Automation
 */

#pragma once

#include "Point.hpp"

namespace Audio
{
    class Automation;

    /** @brief A sorted list of points */
    using Points = Core::SortedFlatVector<Point>;
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


    /** @brief Get the automation name */
    [[nodiscard]] const Core::FlatString &name(void) const noexcept { return _name; }

    /** @brief Set the automation name */
    bool setName(Core::FlatString &&name) noexcept;


private:
    Points _points {};
    BeatRanges _instances {};
    Core::FlatString _name {};
    bool _muted { false };
};

static_assert_fit_half_cacheline(Audio::Automation);

#include "Automation.ipp"
