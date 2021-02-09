/**
 * @ Author: Pierre Veysseyre
 * @ Description: Partition implementation
 */

#include <stdexcept>

inline bool Audio::Partition::setMuted(const bool muted) noexcept
{
    if (muted == _muted)
        return false;
    _muted = muted;
    return true;
}

inline bool Audio::Partition::setMidiChannels(const MidiChannels midiChannels) noexcept
{
    if (midiChannels == _midiChannels)
        return false;
    _midiChannels = midiChannels;
    return true;
}

inline bool Audio::Partition::setName(Core::FlatString &&name) noexcept
{
    if (name == _name)
        return false;
    _name = std::move(name);
    return true;
}
