/**
 * @ Author: Pierre Veysseyre
 * @ Description: Sampler implementation
 */

#include <iomanip>

#include <Audio/DSP/Merge.hpp>
#include <Audio/DSP/FIR.hpp>

inline void Audio::Mixer::receiveAudio(BufferView output)
{
    const auto size = output.size<float>();
    const float *out = output.data<float>();

    // std::cout << "MIXER SIZE: " << _cache.size() << std::endl;

    DSP::Merge<float>(_cache, output, size, true);

    constexpr auto PrintGain = [](const float outGain) {
        const std::size_t gainNorm = outGain * 100.f;
        std::cout << "<";
        for (auto i = 0ul; i < 100ul; ++i) {
            std::cout << (i < gainNorm ? "=" : " ");
        }
        std::cout << ">" << std::endl;
    };
    constexpr auto ApplyRatio = [](BufferView buffer, const float ratio) {
        const auto size = buffer.size<float>();
        float *data = buffer.data<float>();
        std::for_each(data, data + size, [ratio](float &x) { x *= ratio; });
    };

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
        else
            std::cout << std::setprecision(4) << "min: " << min << ", max: " << max << std::endl;
    };

    // DSP::FIR::Filter<float>(
    //     DSP::FilterSpecs {
    //         DSP::Filter::FilterType::LowPass,
    //         DSP::Filter::WindowType::Hanning,
    //         // size
    //     }
    // );

    PrintRangeClip(output);
}

inline void Audio::Mixer::sendAudio(const BufferViews &inputs)
{
    _cache = inputs;
}
