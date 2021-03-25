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

#include <iostream>

inline void Audio::Mixer::receiveAudio(BufferView output) noexcept
{
    const auto size = output.size<float>();

    // std::cout << "MIXER SIZE: " << _cache.size() << std::endl;

    for (auto k = 0u; k < _cache.size(); ++k) {
        const float *from = _cache[k].data<float>();
        float *to = output.data<float>();
        for (auto i = 0u; i < size; ++i) {
            if (!k)
                to[i] = from[i] / _cache.size();
            else
                to[i] += from[i] / _cache.size();
        }
        // return;
    }
}

inline void Audio::Mixer::sendAudio(const BufferViews &inputs) noexcept
{
    _cache = inputs;
}
