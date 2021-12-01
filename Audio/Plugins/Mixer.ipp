/**
 * @ Author: Pierre Veysseyre
 * @ Description: Sampler implementation
 */

#include <iomanip>

#include <Audio/DSP/Merge.hpp>
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

    const float gainFrom = ConvertDecibelToRatio(static_cast<float>(getControlPrev(0u)));
    const float gainTo = ConvertDecibelToRatio(static_cast<float>(outputVolume()));

    const auto outChannelSize = audioSpecs().processBlockSize;
    auto *out = output.data<float>();

    const auto channels = static_cast<std::size_t>(audioSpecs().channelArrangement);

    std::memcpy(out, _cache.data<float>(), outChannelSize * channels * sizeof(float));

    // static float Phase { 0.0f };
    // for (auto i = 0u; i < outChannelSize; ++i) {
    //     // LRLR
    //     // out[2 * i] = std::sin(Phase) / 2.0f;
    //     // out[2 * i + 1] = std::sin(Phase) >= 0.0 ? 0.25f : 0.25f;

    //     // LLRR
    //     out[i] = std::sin(Phase) / 2.0f;
    //     out[i + outChannelSize] = std::sin(Phase) / 4.0f;
    //     // out[i + outChannelSize] = std::sin(Phase) >= 0.0 ? 0.25f : 0.25f;

    //     Phase += 2.0f * M_PI * 0.01f;
    //     while (Phase >= 2.0f * M_PI)
    //         Phase -= 2.0f * M_PI;
    // }


    DSP::Gain::ApplyStereo<float>(out, outChannelSize, channels, gainFrom, gainTo);
    PrintRangeClip(output);
}

inline void Audio::Mixer::sendAudio(const BufferViews &inputs)
{
    DSP::Merge<float>(inputs, _cache, 1.0f, false);
    DSP::Gain::ApplyStereo<float>(
        _cache.data<float>(),
        audioSpecs().processBlockSize,
        static_cast<std::size_t>(audioSpecs().channelArrangement),
        ConvertDecibelToRatio(static_cast<float>(getControlPrev(1u))),
        ConvertDecibelToRatio(static_cast<float>(inputGain()))
    );
}
