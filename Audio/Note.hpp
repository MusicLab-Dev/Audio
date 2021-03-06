/**
 * @ Author: Pierre Veysseyre
 * @ Description: Note
 */

#pragma once

#include <cstdint>
#include <vector>

#include <Core/SmallVector.hpp>

#include "Base.hpp"

namespace Audio
{
    struct NoteEvent;
    struct Note;

    /** @brief Modifiers of a note */
    struct alignas_eighth_cacheline NoteModifiers
    {
        Velocity velocity { 0u };
        Tuning tuning { 0u };
        BlockSize sampleOffset { 0u };
    };

    static_assert_fit_eighth_cacheline(NoteModifiers);

    /** @brief A list of NoteEvent */
    using NoteEvents = Core::TinyVector<NoteEvent>;
}

/** @brief Default Note event */
struct alignas_eighth_cacheline Audio::NoteEvent
{
    enum class EventType : std::uint8_t {
        On, Off, OnOff, PolyPressure
    };

    EventType   type { EventType::On };
    Key         key { 0u };
    Velocity    velocity { 0u };
    Tuning      tuning { 0u };
    BlockSize   sampleOffset { 0u };
};

static_assert_fit_eighth_cacheline(Audio::NoteEvent);

/** @brief Default Note */
struct alignas_quarter_cacheline Audio::Note
{
    enum class EventType : std::uint8_t {
        On, Off, PolyPressure
    };

    static const std::vector<const char *> NoteNamesSharp;

    Note(void) = default;

    Note(const BeatRange &range_, Key key_ = 69, Velocity velocity_ = 0xFFFF, Tuning tuning_ = 0u)
        : range(range_), key(key_), velocity(velocity_), tuning(tuning_) {}

    /** @brief Check if an another note is the same */
    [[nodiscard]] inline bool operator==(const Note &other) const noexcept;

    /** @brief Check if an another note is different */
    [[nodiscard]] inline bool operator!=(const Note &other) const noexcept;

    /** @brief Check if this note come after an another one */
    [[nodiscard]] inline bool operator>(const Note &other) const noexcept;

    /** @brief Check if this note come after an another one or in same time */
    [[nodiscard]] inline bool operator>=(const Note &other) const noexcept;

    /** @brief Check if this note come before an another one */
    [[nodiscard]] inline bool operator<(const Note &other) const noexcept;

    /** @brief Check if this note come before an another one or in same time */
    [[nodiscard]] inline bool operator<=(const Note &other) const noexcept;


    BeatRange   range {};
    Key         key { 0u };
    Velocity    velocity { 0u };
    Tuning      tuning { 0u };
};

std::ostream &operator<<(std::ostream &out, const Audio::Note &note);
std::ostream &operator<<(std::ostream &out, const Audio::NoteEvent &note);

// static_assert_fit_quarter_cacheline(Audio::NoteEvent);

#include "Note.ipp"
