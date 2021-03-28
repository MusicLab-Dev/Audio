/*
 * @ Author: Pierre Veysseyre
 * @ Description: Control.hpp
 */

#pragma once

#include "Automation.hpp"

namespace Audio
{
    class Control;
    struct ControlEvent;

    /** @brief A list of automations */
    using Automations = Core::FlatVector<Automation>;

    /** @brief A list of controls */
    using Controls = Core::FlatVector<Control>;
    // using Controls = Core::FlatVector<Control, std::size_t, LiveControlChangeList>;
};

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

/** @brief A control describe how to change a plugin parameter over time */
class alignas_half_cacheline Audio::Control
{
public:
    /** @brief Default constructor */
    Control(void) noexcept = default;

    /** @brief Construct a control out of its parameter ID and value */
    Control(const ParamID paramID) noexcept : _paramID(paramID) {}

    /** @brief Copy assignment */
    Control(const Control &other) noexcept = default;

    /** @brief Move assignment */
    Control(Control &&other) noexcept = default;

    /** @brief Destructor */
    ~Control(void) noexcept = default;


    /** @brief Copy assignment */
    Control &operator=(const Control &other) noexcept = default;

    /** @brief Move assignment */
    Control &operator=(Control &&other) noexcept = default;


    /** @brief Get the automation list */
    [[nodiscard]] Automations &automations(void) noexcept { return _automations; }
    [[nodiscard]] const Automations &automations(void) const noexcept { return _automations; }


    /** @brief Get the paramID associated to this control */
    [[nodiscard]] ParamID paramID(void) const noexcept { return _paramID; }

    /** @brief Set a new ParamID to this control, returns true if the id changed */
    void setParamID(const ParamID paramID) noexcept { _paramID = paramID; }


    /** @brief Check if the control is muted (not active) or not */
    [[nodiscard]] bool muted(void) const noexcept { return _muted; }

    /** @brief Set the muted state of the control */
    void setMuted(const bool muted) noexcept { _muted = muted; }


    /** @brief Check if the control is in manual mode */
    [[nodiscard]] bool manualMode(void) const noexcept { return _manualMode; }

    /** @brief Set the manual mode state of the control */
    void setManualMode(const bool manualMode) noexcept { _manualMode = manualMode; }


    /** @brief Get the control manual point */
    [[nodiscard]] const Point &manualPoint(void) const noexcept { return _manualPoint; }

    /** @brief Set the manual point of the control */
    void setManualPoint(const Point &manualPoint) noexcept { _manualPoint = manualPoint; }

private:
    Point           _manualPoint {};
    Automations     _automations {};
    ParamID         _paramID {};
    bool            _muted { false };
    bool            _manualMode { false };
};

static_assert_fit_half_cacheline(Audio::Control);
