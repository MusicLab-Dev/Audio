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
