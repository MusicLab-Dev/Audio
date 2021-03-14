/**
 * @ Author: Your name
 * @ Description: Automation
 */

inline bool Audio::Automation::setMuted(const bool value) noexcept
{
    if (_muted == value)
        return false;
    _muted = value;
    return true;
}
