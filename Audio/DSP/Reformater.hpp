/**
 * @ Author: Pierre Veysseyre
 * @ Description: Reformat algorithm
 */

#pragma once

namespace Audio::DSP::Reformater
{
    namespace Internal
    {
        template<typename InType, typename OutType>
        void TrivialFormat(const InType *input, OutType *output, const std::size_t inputSize) noexcept;

        template<typename InType, typename OutType>
        void FromFloating(const InType *input, OutType *output, const std::size_t inputSize) noexcept;

        template<typename InType, typename OutType, bool InputOffset>
        void FromFloatingImpl(const InType *input, OutType *output, const std::size_t inputSize) noexcept;

    }

    template<typename InType, typename OutType>
    void Reformat(const InType *input, OutType *output, const std::size_t inputSize) noexcept;
}

#include "Reformater.ipp"
