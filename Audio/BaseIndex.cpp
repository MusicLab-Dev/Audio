/**
 * @ Author: Pierre Veysseyre
 * @ Description: BaseIndex.cpp
 */

#include "BaseIndex.hpp"

std::ostream &operator<<(std::ostream &out, const Audio::BeatRange &range)
{
    return out << "(" << range.from << ":" << range.to << ")";
}

std::ostream &operator<<(std::ostream &out, const Audio::TimeRange &range)
{
    return out << "(" << range.from << ":" << range.to << ")";
}
