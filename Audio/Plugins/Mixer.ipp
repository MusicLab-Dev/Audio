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
        for (auto i = 0u; i < size; ++i) {
            if (!k)
                to[i] = from[i] / _cache.size();
            else
                to[i] += from[i] / _cache.size();
        }
        // return;
    }
}

inline void Audio::Mixer::sendAudio(const BufferViews &inputs)
{
    _cache = inputs;
}
