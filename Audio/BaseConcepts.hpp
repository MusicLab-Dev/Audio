/**
 * @file BaseConcepts.hpp
 * @brief C++ 20 concepts helpers
 *
 * @author Pierre V
 * @date 2021-11-24
 */

#pragma once

#include <concepts>

namespace Audio
{
    template<typename Type>
    concept IsFloating = std::is_floating_point_v<Type>;

    template<typename Type>
    concept IsInteger = std::is_integral_v<Type>;
}
