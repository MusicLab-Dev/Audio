/**
 * @ Author: Pierre Veysseyre
 * @ Description: FlatNote
 */

#pragma once

#include "Base.hpp"

namespace Audio
{
    struct FlatNote;
};

/** FlatNote don't store the range and the event type */
struct alignas_eighth_cacheline Audio::FlatNote
{
    /** @brief Default constructor */
    FlatNote(void) = default;

    /** @brief Custom constructor */
    FlatNote(const Key key_, const Velocity velocity_, const Tuning tuning_)
        : key(key_), velocity(velocity_), tuning(tuning_) {}


    Key         key { 69u };
    Velocity    velocity { 0x7F };
    Tuning      tuning {};
    bool        active { false };
};

static_assert_fit_eighth_cacheline(Audio::FlatNote);
