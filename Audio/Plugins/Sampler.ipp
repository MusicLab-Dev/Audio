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

template<typename T>
inline void Audio::Sampler::loadSample(const std::string &path)
{
    auto buf = SampleLoader::LoadWAV<T>(path);

    Buffer b(buf.channelByteSize(), buf.sampleRate(), buf.channelArrangement());
    // for (auto i = 0u; i
    // _buffers.push(buf);
    for (auto i = 0u; i < 11; ++i) {
        // BufferView v(Buffer(buf));
        // _buffers.push(DSP::Resampler::Decimate<float>(v, static_cast<Semitone>(i)));
    }

}
