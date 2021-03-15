/**
 * @ Author: Your name
 * @ Description: Automation
 */

inline bool Audio::Automation::setMuted(const bool muted) noexcept
{
    if (_muted == muted)
        return false;
    _muted = muted;
    return true;
}

inline bool Audio::Automation::setName(Core::FlatString &&name) noexcept
{
    if (_name == name)
        return false;
    _name = std::move(name);
    return true;
}