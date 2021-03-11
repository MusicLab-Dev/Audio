/**
 * @ Author: Pierre Veysseyre
 * @ Description: Sampler implementation
 */


#include <Audio/DSP/Merge.hpp>

inline Audio::IPlugin::Flags Audio::Mixer::getFlags(void) const noexcept
{
    return static_cast<Flags>(
        static_cast<std::size_t>(Flags::AudioInput) |
        static_cast<std::size_t>(Flags::AudioOutput) |
        static_cast<std::size_t>(Flags::ControlInput)
    );
}

inline void Audio::Mixer::receiveAudio(BufferView output) noexcept
{
    const auto size = output.size<std::uint8_t>();
    const auto from =_cache[0].byteData();
    const auto to = output.byteData();
    for (auto i = 0; i < size; ++i)
        to[i] = from[i];
}

void Audio::Mixer::sendAudio(const BufferViews &inputs) noexcept
{
    _cache = inputs;
}
