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

#include <iostream>

template<typename T>
inline void Audio::Sampler::loadSample(const std::string &path)
{
    Buffer buf = SampleLoader::LoadWAV<T>(path);

    std::cout << "SIZE: " << buf.size<T>() << std::endl;
    auto a = DSP::Resampler<T>::ResampleSemitone(BufferView(buf), -11);
    // _buffers.push(a);

    for (auto i = 0u; i < 11; ++i) {
        // _buffers.push(DSP::Resampler::Decimate<float>(v, static_cast<Semitone>(i)));
    }

}
