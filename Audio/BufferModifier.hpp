/**
 * @file BufferModifier.hpp
 * @brief Handler to easy apply ratio of functor to buffers
 *
 * @author Pierre V
 * @date 2021-04-26
 */

#pragma once

#include "Buffer.hpp"

namespace Audio
{
    template<typename Type>
    struct Modifier;
}

template<typename Type>
struct Audio::Modifier
{
    using RangeFunctor = Core::Functor<Type(const std::size_t index)>;

    static void ApplyVolume(Type *data, const std::size_t size, const DB volume) noexcept
    {
        std::for_each(data, data + size, [volume](Type &x) { x *= volume; });
    }

    static void ApplyRangeFunctor(Type *data, const std::size_t size, const std::uint32_t offset, RangeFunctor &&functor) noexcept
    {
        const auto end = size + offset;
        auto k = 0u;
        for (auto i = offset; i < end; ++i, ++k) {
            data[k] = functor(i);
        }
    }
};
