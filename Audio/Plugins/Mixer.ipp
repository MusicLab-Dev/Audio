/**
 * @ Author: Pierre Veysseyre
 * @ Description: Sampler implementation
 */

#include <Audio/DSP/Merge.hpp>

inline void Audio::Mixer::receiveAudio(BufferView output)
{
    const auto size = output.size<float>();

    // std::cout << "MIXER SIZE: " << _cache.size() << std::endl;

    for (auto k = 0u; k < _cache.size(); ++k) {
        const float *from = _cache[k].data<float>();
        float *to = output.data<float>();
        to[0] = from[0] / _cache.size();
        for (auto i = 1u; i < size; ++i) {
            to[i] += from[i] / _cache.size();
        }
        // return;
    }
}

inline void Audio::Mixer::sendAudio(const BufferViews &inputs)
{
    _cache = inputs;
}
