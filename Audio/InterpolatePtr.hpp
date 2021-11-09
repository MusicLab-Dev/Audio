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
}

/** @brief Interpolate between two points of type 'Type' using pointer arithmetic
    InterpolatePtr is an imaginary pointer (or fake pointer): it doesn't allocate data and compute in run-time     */
template<std::floating_point Type>
class Audio::InterpolatePtr
{
public:
    InterpolatePtr(conramValue left, constt  PamValue right) noexcept;
};
