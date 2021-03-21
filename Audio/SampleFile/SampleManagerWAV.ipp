/**
 * @ Author: Pierre Veysseyre
 * @ Description: SampleManagerWAV.cpp
 */

#include <filesystem>
#include <fstream>

template<typename Type>
inline void Audio::SampleManagerWAV::WriteToBufferImpl(const Type *input, Type *output, const ChannelArrangement channelArrangement, const std::size_t size) noexcept
{
    const auto arrangement = static_cast<std::size_t>(channelArrangement);
    for (auto iChannel = 0u; iChannel < arrangement; ++iChannel) {
        for (auto i = 0u; i < size; ++i) {
            output[i * (iChannel + 1)] = input[i * arrangement];
        }
    }
}

template<typename Type>
inline void Audio::SampleManagerWAV::WriteFromBufferImpl(const Type *input, Type *output, const ChannelArrangement channelArrangement, const std::size_t size) noexcept
{
    const auto arrangement = static_cast<std::size_t>(channelArrangement);
    for (auto iChannel = 0u; iChannel < arrangement; ++iChannel) {
        for (auto i = 0u; i < size; ++i) {
            output[i * arrangement] = input[i * (iChannel + 1)];
        }
    }
}
