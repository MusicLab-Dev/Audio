/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Merge algorithm
 */

#pragma once

#include <Core/Utils.hpp>

#include <Audio/Buffer.hpp>

namespace Audio::DSP
{
    /** @brief Merge a list of buffer into a separated one.
     *  If normalize is true, all input buffers will be normalize to the number of input.
        Otherwise the inputs are simply summed. */
    template<typename Type>
    void Merge(const BufferViews inputs, BufferView output, const DB ratio, const bool normalize = false) noexcept;

    template<typename Type>
    void Merge(const BufferView input, BufferView output, const DB ratio, const bool normalize = false) noexcept;

    template<typename Unit, std::size_t BufferSize, typename ...Args>
    void Merge(Unit * const output, Args &&...args) noexcept;

    namespace Internal
    {
        template<typename Unit, std::size_t BufferSize, std::size_t Index, std::size_t InputCount, typename Tuple>
        std::enable_if_t<std::is_convertible_v<std::tuple_element_t<Index, Tuple>, Unit * const>, void>
            MergeUnroll(Unit * const output, const Tuple &tuple) noexcept;

        template<typename Unit, std::size_t Count, std::size_t InputCount>
        void UnrolledCopy(Unit * const output, const Unit * const input, const Unit ratio) noexcept;
    }
}

#include "Merge.ipp"
