/**
 * @ Author: Pierre Veysseyre
 * @ Description: Interpreter
 */

#pragma once

#include <Audio/Base.hpp>

enum class NoteType : Audio::Beat {
    WholeNote = 1,                  // Ronde
    HalfNote = 2,                   // Blanche
    QuarterNote = 4,                // Noire
    EighthNote = 8,                 // Croche
    SixteenthNote = 16,             // Double croche
    ThirtySecondNote = 32,          // Triple croche
    SixtyFourthNote = 64,           // Quadruple croche
    HundredTwentyEighthNote = 128   // Quintuple croche
};

[[nodiscard]] inline Audio::Beat MakeBeat(const Audio::Beat beatNumber, const NoteType beatType) noexcept
    { return beatNumber * (Audio::BeatPrecision / static_cast<Audio::Beat>(beatType)); }

[[nodiscard]] inline Audio::BeatRange MakeBeatRange(const Audio::Beat from, const Audio::Beat to, const NoteType beatType) noexcept
    { return Audio::BeatRange { MakeBeat(from, beatType), MakeBeat(to, beatType) }; }