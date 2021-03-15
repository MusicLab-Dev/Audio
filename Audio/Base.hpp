/*
 * @ Author: Pierre Veysseyre
 * @ Description: A collection of Audio library builtins
 */

#pragma once

#include <cstdint>

#include <Core/Vector.hpp>
#include <Core/FlatVector.hpp>
#include <Core/FlatString.hpp>
#include <Core/SortedFlatVector.hpp>

#include "BaseDevice.hpp"
#include "BaseIndex.hpp"

namespace Audio
{
    /** @brief BPM (beat per seconds) */
    using BPM = float;

    /** @brief Tempo (beat per seconds) */
    using Tempo = float;

    /** @brief Color value */
    using Color = std::uint32_t;

    /** @brief Index of a note */
    using NoteIndex = std::uint16_t;

    /** @brief Key of a note */
    using Key = std::uint8_t;

    /** @brief Semitone */
    using Semitone = std::int8_t;

    /** @brief Number of different handled keys */
    constexpr Key KeyCount = 128u;

    /** @brief Velocity of a note */
    using Velocity = std::uint16_t;

    /** @brief Note pitch tuning */
    using Tuning = uint16_t;

    /** @brief Midi note channels */
    using MidiChannels = std::uint8_t;

    /** @brief Parameter ID */
    using ParamID = std::uint32_t;

    /** @brief Parameter value */
    using ParamValue = double;

    /** @brief Gain value */
    using Gain = float;

    /** @brief Audio channels arrangement */
    enum class ChannelArrangement : std::uint8_t {
        Mono = 1,
        Stereo = 2
    };

    /** @brief Audio channels */
    enum class Channel {
        Mono = 0,
        Left = Mono,
        Right
    };

    /** @brief A sorted list of beat ranges */
    using BeatRanges = Core::SortedFlatVector<BeatRange>;

    /** @brief A sorted list of time ranges */
    using TimeRanges = Core::SortedFlatVector<TimeRange>;


    /** @brief Decribe the audio device behavior */
    struct AudioSpecs
    {
        SampleRate sampleRate { 0u };
        ChannelArrangement channelArrangement { 0u };
        Format format { Format::Floating32 };
        std::size_t processBlockSize { 0u };

        /** @brief Equality operators */
        [[nodiscard]] bool operator==(const AudioSpecs &other) const noexcept
            { return sampleRate == other.sampleRate && channelArrangement == other.channelArrangement &&
                format == other.format && processBlockSize == other.processBlockSize; }
        [[nodiscard]] bool operator!=(const AudioSpecs &other) const noexcept { return !(*this == other); }
    };
};
