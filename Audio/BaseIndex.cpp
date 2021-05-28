/**
 * @ Author: Pierre Veysseyre
 * @ Description: BaseIndex.cpp
 */

#include "BaseIndex.hpp"

using namespace Audio;

std::ostream &operator<<(std::ostream &out, const BeatRange &range)
{
    return out << "(" << range.from << ":" << range.to << ")";
}

std::ostream &operator<<(std::ostream &out, const TimeRange &range)
{
    return out << "(" << range.from << ":" << range.to << ")";
}

Beat Audio::MakeBeat(const std::uint32_t unitCount, const NoteType beatType) noexcept
{
    return unitCount * (BeatPrecision  / static_cast<Beat>(beatType));
}

BeatRange Audio::MakeBeatRange(const std::uint32_t from, const std::uint32_t to, const NoteType beatType) noexcept
{
    return BeatRange { MakeBeat(from, beatType), MakeBeat(to, beatType) };
}
