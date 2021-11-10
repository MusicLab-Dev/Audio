/**
 * @ Author: Pierre Veysseyre
 * @ Description: InterpolatePtr
 */

#pragma once

#include <concepts>

#include "Base.hpp"

namespace Audio
{
    template<std::floating_point Type>
    class InterpolatePtr;

    template<std::floating_point Type>
    class InterpolatePtr2;
}

/** @brief Interpolate between two points of type 'Type' using pointer arithmetic
    InterpolatePtr is an imaginary pointer (or fake pointer): it doesn't allocate data and compute in run-time     */
template<std::floating_point Type>
class Audio::InterpolatePtr
{
public:
    constexpr InterpolatePtr(const Type from, const Type to, const Type minDelta, const std::size_t sampleCount) noexcept
    {
        const auto range = (to - from);
        const auto points = range / minDelta;
        if (points < sampleCount) {
            _stepIndex = std::ceil(sampleCount / points);
            _stepValue = minDelta;
        } else {
            _stepIndex = 1u;
            _stepValue = range / sampleCount;
        }
    }

    [[nodiscard]] constexpr Type operator[](const std::size_t index) const noexcept
    {
        const auto realIndex = _currentIndex + index;
        const auto pointIndex = realIndex / _stepIndex;

        return _value + _stepValue * pointIndex;
    }

    constexpr InterpolatePtr &operator++(void) noexcept { ++_currentIndex; }

private:
    std::size_t _currentIndex { 0u };
    std::size_t _stepIndex { 0u };
    Type _value {};
    Type _stepValue {};
};

template<std::floating_point Type>
class Audio::InterpolatePtr2
{
public:
    constexpr InterpolatePtr2(const Type from, const Type to, const Type minDelta, const std::size_t sampleCount) noexcept
    {
        const auto range = (to - from);
        const auto points = range / minDelta;

        if (points < sampleCount) {
            _stepIndex = std::ceil(sampleCount / points);
            _stepValue = minDelta;
        } else {
            _stepIndex = 1u;
            _stepValue = range / sampleCount;
        }
    }

    [[nodiscard]] constexpr Type operator*(void) const noexcept
    {
        return _value;
    }

    constexpr InterpolatePtr2 &operator++(void) noexcept
    {
        if (!(++_currentIndex % _stepIndex)) {
            _value += _stepValue;
        }
        return *this;
    }

private:
    std::size_t _currentIndex { 0u };
    std::size_t _stepIndex { 0u };
    Type _value {};
    Type _stepValue {};
};
