/**
 * @ Author: Pierre Veysseyre
 * @ Description: Base Ranges
 */

#pragma once

#include <cstdint>

namespace Audio
{
    /** @brief Beat index */
    using Beat = std::uint32_t;

    /** @brief Time index */
    using TimeIndex = std::uint32_t;

    enum class TimeSignatureType : Beat {
        WholeNote = 1,                  // Ronde
        HalfNote = 2,                   // Blanche
        QuarterNote = 4,                // Noire
        EighthNote = 8,                 // Croche
        SixteenthNote = 16,             // Double croche
        ThirtySecondNote = 32,          // Triple croche
        SixtyFourthNote = 64,           // Quadruple croche
        HundredTwentyEighthNote = 128   // Quintuple croche
    };

    /** @brief Bar (mesure solfege) precision */
    static constexpr auto BarPrecision = 128u;

    /** @brief Max beat index */
    static constexpr auto BeatIndexMax = std::numeric_limits<Beat>::max() / BarPrecision;

    /** @brief Retrieve a beat depending of a time signature */
    // static constexpr auto GetBeatBySignature = [](std::size_t beat)


    struct BeatRange;
    struct TimeRange;
}

/** @brief Beat range */
struct alignas(8) Audio::BeatRange
{
    Beat from {};
    Beat to {};

    [[nodiscard]] inline bool operator==(const BeatRange &other) const noexcept { return (from == other.from) && (to == other.to); }
    [[nodiscard]] inline bool operator!=(const BeatRange &other) const noexcept { return !(operator==(other)); }

    inline BeatRange &operator+=(const Beat size) { from += size; to += size; return *this; }
};

static_assert_fit(Audio::BeatRange, 8);

/** @brief Time range */
struct alignas(8) Audio::TimeRange
{
    TimeIndex from {};
    TimeIndex to {};

    [[nodiscard]] inline bool operator==(const TimeRange &other) const noexcept { return (from == other.from) && (to == other.to); }
    [[nodiscard]] inline bool operator!=(const TimeRange &other) const noexcept { return !(operator==(other)); }
};

static_assert_fit(Audio::TimeRange, 8);
