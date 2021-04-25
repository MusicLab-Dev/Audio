/**
 * @ Author: Pierre V
 * @ Description: SampleManagerSpecs
 */

#pragma once

#include <cstdint>

namespace Audio
{
    struct SampleSpecs {
        std::uint16_t format { 0u }; // 1: PCM, 3: IEEE float
        std::uint16_t channels { 0u };
        std::uint32_t sampleRate { 0u };
        std::uint16_t bytePerSample { 0u };
    };
}
