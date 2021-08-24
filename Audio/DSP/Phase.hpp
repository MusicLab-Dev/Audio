/**
 * @ Author: Pierre Veysseyre
 * @ Description: Phase handler
 */

#pragma once

#include <Audio/MathConstants.hpp>

namespace Audio::DSP
{
    template<typename Type>
    class Phase;
}

template<typename Type>
class Audio::DSP::Phase
{
public:
    Phase(void) = default;
    Phase(const Type phase) : _phase(phase) {}

    void reset(void) noexcept { _phase = Type(0); }

    void setPhase(const Type phase) noexcept { _phase = phase; }
    [[nodiscard]] Type phase(void) const noexcept { return _phase; }

    void increment(const Type inc) noexcept
    {
        _phase += inc;
        while (_phase >= Pi2)
            _phase -= Pi2;

    }

private:
    Type _phase { 0 };
};
