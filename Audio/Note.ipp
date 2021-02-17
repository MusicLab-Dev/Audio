/**
 * @ Author: Pierre Veysseyre
 * @ Description: Note implementation
 */

inline bool Audio::Note::operator==(const Note &other) const noexcept
{
    return (
        (range == other.range) &&
        (key == other.key) &&
        (velocity == other.velocity) &&
        (tuning == other.tuning)
    );
}

inline bool Audio::Note::operator!=(const Note &other) const noexcept
{
    return !(operator==(other));
}

inline bool Audio::Note::operator>(const Note &other) const noexcept
{
    if (range.from > other.range.from)
        return true;
    if (range.from == other.range.from && range.to > other.range.to)
        return true;
    return false;
}

inline bool Audio::Note::operator>=(const Note &other) const noexcept
{
    if (range.from > other.range.from)
        return true;
    if (range.from == other.range.from && range.to >= other.range.to)
        return true;
    return false;
}

inline bool Audio::Note::operator<(const Note &other) const noexcept
{
    if (range.from < other.range.from)
        return true;
    if (range.from == other.range.from && range.to < other.range.to)
        return true;
    return false;
}

inline bool Audio::Note::operator<=(const Note &other) const noexcept
{
    if (range.from < other.range.from)
        return true;
    if (range.from == other.range.from && range.to <= other.range.to)
        return true;
    return false;
}
