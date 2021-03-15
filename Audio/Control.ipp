/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Control
 */

inline bool Audio::Control::setParamID(const ParamID paramID) noexcept
{
    if (paramID == _paramID)
        return false;
    _paramID = paramID;
    return true;
}

inline bool Audio::Control::setMuted(const bool muted) noexcept
{
    if (_muted == muted)
        return false;
    _muted = muted;
    return true;
}

inline bool Audio::Control::setManualMode(const bool manualMode) noexcept
{
    if (_manualMode == manualMode)
        return false;
    _manualMode = manualMode;
    return true;
}

inline bool Audio::Control::setManualPoint(const Point &manualPoint) noexcept
{
    if (_manualPoint == manualPoint)
        return false;
    _manualPoint = manualPoint;
    return true;
}