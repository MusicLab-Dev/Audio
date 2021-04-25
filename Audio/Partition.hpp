/**
 * @ Author: Pierre Veysseyre
 * @ Description: Partition
 */

#pragma once

#include "Notes.hpp"

namespace Audio
{
    class Partition;
}

class alignas_half_cacheline Audio::Partition
{
public:
    /** @brief Get the internal notes */
    [[nodiscard]] inline const Notes &notes(void) const noexcept { return _notes; }

    /** @brief Get the internal notes */
    [[nodiscard]] inline Notes &notes(void) noexcept { return _notes; }


    /** @brief Get the internal intances */
    [[nodiscard]] BeatRanges &instances(void) noexcept { return _instances; }

    /** @brief Set the internal intances */
    [[nodiscard]] const BeatRanges &instances(void) const noexcept { return _instances; }


    /** @brief Check if the partition is muted (not active) or not */
    [[nodiscard]] bool muted(void) const noexcept { return _muted; }

    /** @brief Set the muted state of the partition */
    void setMuted(const bool muted) noexcept { _muted = muted; }


    /** @brief Get the internal midiChannels */
    [[nodiscard]] MidiChannels midiChannels(void) const noexcept { return _midiChannels; }

    /** @brief Set the internal midiChannels */
    void setMidiChannels(const MidiChannels midiChannels) noexcept { _midiChannels = midiChannels; }


    /** @brief Get the name of the partition */
    [[nodiscard]] const Core::FlatString &name(void) const noexcept { return _name; }

    /** @brief Set the partition name, return true if the name changed */
    void setName(Core::FlatString &&name) noexcept { _name = name; }

private:
    Notes               _notes {};
    BeatRanges          _instances {};
    Core::FlatString    _name {};
    NoteIndex           _lastID {};
    MidiChannels        _midiChannels {};
    bool                _muted { false };
};

static_assert_fit_half_cacheline(Audio::Partition);
