/**
 * @ Author: Pierre Veysseyre
 * @ Description: BaseDevice
 */

#pragma once

#include <cstdint>

namespace Audio
{
    /** @brief Format describe the bit depth (e.g. Audio headroom) of the Device
     *  Note that the CPU works increase with the size of the format ! */
    enum class Format : std::uint8_t {
        Unknown = 0, Fixed8, Fixed16, Fixed32, Floating32
    };

    /** @brief Get the byte length of a specific format */
    [[nodiscard]] constexpr std::size_t GetFormatByteLength(const Format format) noexcept
    {
        switch (format) {
        case Format::Unknown:
            return 0;
        case Format::Fixed8:
            return sizeof(std::int8_t);
        case Format::Fixed16:
            return sizeof(std::int16_t);
        case Format::Fixed32:
            return sizeof(std::int32_t);
        case Format::Floating32:
            return sizeof(float);
        default:
            return 0;
        }
    }

    /** @brief Sample per second */
    using SampleRate = std::uint32_t;

    /** @brief Size of an audio block */
    using BlockSize = std::uint16_t;
};