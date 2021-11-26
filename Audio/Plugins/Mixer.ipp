/**
 * @ Author: Pierre Veysseyre
 * @ Description: Sampler implementation
 */

#include <iomanip>

#include <Audio/DSP/Merge.hpp>
#include <Audio/DSP/FIR.hpp>
#include <Audio/DSP/Gain.hpp>

inline void Audio::Mixer::onAudioGenerationStarted(const BeatRange &range)
{
    UNUSED(range);
    _cache.resize(GetFormatByteLength(audioSpecs().format) * audioSpecs().processBlockSize, audioSpecs().sampleRate, audioSpecs().channelArrangement, audioSpecs().format);
    _cache.clear();
}

inline void Audio::Mixer::receiveAudio(BufferView output)
{
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
            std::cout << "<<<CLIPING>>> " << min << ", " << max << std::endl;
        // else
        //     std::cout << std::setprecision(4) << "min: " << min << ", max: " << max << std::endl;
    };

    const float gainOutFrom = ConvertDecibelToRatio(static_cast<float>(getControlPrev(0u)));
    const float gainOutTo = ConvertDecibelToRatio(static_cast<float>(outputVolume()));

    const auto outSize = output.size<float>();
    auto *out = output.data<float>();

    std::memcpy(out, _cache.data<float>(), outSize * sizeof(float));
    DSP::Gain::Apply<float>(out, outSize, gainOutFrom, gainOutTo);
    PrintRangeClip(output);
}

inline void Audio::Mixer::sendAudio(const BufferViews &inputs)
{
    DSP::Merge<float>(inputs, _cache, 1.0f, false);
    DSP::Gain::Apply<float>(
        _cache.data<float>(),
        _cache.size<float>(),
        ConvertDecibelToRatio(static_cast<float>(getControlPrev(1u))),
        ConvertDecibelToRatio(static_cast<float>(inputGain()))
    );
}
