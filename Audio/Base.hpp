/*
 * @ Author: Pierre Veysseyre
 * @ Description: A collection of Audio library builtins
 */

#pragma once

#include <cstdint>

#define _USE_MATH_DEFINES
#include <math.h>
#include <cmath>

#include <Core/Vector.hpp>
#include <Core/FlatVector.hpp>
#include <Core/FlatString.hpp>
#include <Core/SortedFlatVector.hpp>

#include "BaseConcepts.hpp"
#include "BaseDevice.hpp"
#include "BaseIndex.hpp"
#include "BaseVolume.hpp"
#include "BaseNote.hpp"

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

    /** @brief Midi note channels */
    using MidiChannels = std::uint8_t;

    /** @brief Parameter ID */
    using ParamID = std::uint32_t;

    /** @brief Parameter type */
    enum class ParamType : int {
        Boolean,
        Integer,
        Floating,
        Enum
    };

    /** @brief Parameter interpolation type */
    enum class ParamInterpType : int {
        Linear,     // Default one
        Log         // Used for frequencies
    };

    /** @brief Parameter value */
    using ParamValue = double;

    /** @brief Audio channels arrangement */
    enum class ChannelArrangement : std::uint8_t {
        Mono = 1,
        Stereo = 2,
        All
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
        ChannelArrangement channelArrangement { ChannelArrangement::Mono };
        Format format { Format::Floating32 };
        std::uint32_t processBlockSize { 0u };

        /** @brief Equality operators */
        [[nodiscard]] bool operator==(const AudioSpecs &other) const noexcept
            { return sampleRate == other.sampleRate && channelArrangement == other.channelArrangement &&
                format == other.format && processBlockSize == other.processBlockSize; }
        [[nodiscard]] bool operator!=(const AudioSpecs &other) const noexcept { return !(*this == other); }
    };

    /** @brief Decribe the sample specs behavior */
    struct SampleSpecs
    {
        SampleRate sampleRate { 0u };
        ChannelArrangement channelArrangement { ChannelArrangement::Mono };
        Format format { Format::Floating32 };
        std::size_t channelByteSize { 0u };
    };

    /** @brief Make a combination of flags out of an enum class */
    template<typename Output, typename Intermediate, typename ...Inputs>
    [[nodiscard]] inline Output MakeFlags(const Inputs ...inputs) noexcept
        { return static_cast<Output>((... | static_cast<Intermediate>(inputs))); }


    /** @brief PlaybackMode describe how to project should be computed */
    enum class PlaybackMode : std::uint32_t {
        Production,
        Live,
        Partition,
        OnTheFly,
        Export
    };

    /** @brief Count of playback modes */
    constexpr std::size_t PlaybackModeCount = static_cast<std::size_t>(PlaybackMode::OnTheFly) + 1;

    /** @brief Get normalized frequency */
    static inline float GetFrequencyNorm(const float frequency, const SampleRate sampleRate) noexcept
        { return 2.0f * static_cast<float>(M_PI) * frequency / static_cast<float>(sampleRate); }
}
