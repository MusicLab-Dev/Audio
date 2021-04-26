/**
 * @ Author: Pierre Veysseyre
 * @ Description: Sampler implementation
 */

#include <iomanip>

#include <Audio/DSP/Merge.hpp>
#include <Audio/DSP/FIR.hpp>

inline void Audio::Mixer::onAudioGenerationStarted(const BeatRange &range)
{
    UNUSED(range);
    _cache.clear();
}

inline void Audio::Mixer::receiveAudio(BufferView output)
{
    // const auto size = output.size<float>();
    // const float *out = output.data<float>();

    // std::cout << "MIXER SIZE: " << _cache.size() << std::endl;

    DSP::Merge<float>(_cache, output, false);

    constexpr auto PrintRangeClip = [](const BufferView buffer) {
        const auto size = buffer.size<float>();
        const float *out = buffer.data<float>();
        auto min = 1000.f;
        auto max = -1000.f;
        for (auto i = 0ul; i < size; ++i) {
            min = std::min(out[i], min);
            max = std::max(out[i], max);
        }
        if (min <= -1 || max >= 1)
            std::cout << "<<<CLIPING>>>" << std::endl;
        // else
        //     std::cout << std::setprecision(4) << "min: " << min << ", max: " << max << std::endl;
    };

    PrintRangeClip(output);
}

inline void Audio::Mixer::sendAudio(const BufferViews &inputs)
{
    _cache = inputs;
}
