/*
 * @ Author: Pierre Veysseyre
 * @ Description: Control
 */

#pragma once

#include <Core/SmallVector.hpp>

#include "Automation.hpp"

namespace Audio
{
    struct ControlEvent;

    /** @brief A list of control events */
    using ControlEvents = Core::TinyVector<ControlEvent>;
}

/** @brief Represent a control change event in an audio block space */
struct alignas_quarter_cacheline Audio::ControlEvent
{
    /** @brief POD semantics */
    ControlEvent(void) noexcept = default;
    ControlEvent(const ParamID paramID_, const ParamValue value_) noexcept
        : paramID(paramID_), value(value_) {}
    ControlEvent(const ControlEvent &other) noexcept = default;
    ControlEvent(ControlEvent &&other) noexcept = default;
    ControlEvent &operator=(const ControlEvent &other) noexcept = default;
    ControlEvent &operator=(ControlEvent &&other) noexcept = default;

    ParamID paramID {};
    ParamValue value {};
};

static_assert_fit_quarter_cacheline(Audio::ControlEvent);
