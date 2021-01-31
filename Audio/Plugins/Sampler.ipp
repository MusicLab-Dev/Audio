/**
 * @ Author: Pierre Veysseyre
 * @ Description: Sampler implementation
 */

#include <Audio/DSP/Resampler.hpp>
#include "Managers/SampleLoader.hpp"

inline Audio::IPlugin::Flags Audio::Sampler::getFlags(void) const noexcept
{
    return static_cast<Flags>(
        static_cast<std::size_t>(Flags::AudioOutput) |
        static_cast<std::size_t>(Flags::ControlInput) |
        static_cast<std::size_t>(Flags::NoteInput)
    );
}

inline void Audio::Sampler::loadSample(const std::string &path)
{
    auto buf = SampleLoader::LoadWAV(path);

    _buffers.push(buf);
    for (auto i = 0u; i < 11; ++i) {
        // _buffers.push(DSP::Resampler::Decimate(buf, b));
    }

}
