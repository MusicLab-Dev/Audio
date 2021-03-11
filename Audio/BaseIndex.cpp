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
