/**
 * @file LambdaFilter.ipp
 * @brief Lambda filter implementation
 *
 * @author Pierre V
 * @date 2021-04-23
 */

#include <Audio/DSP/Merge.hpp>

inline void Audio::LambdaFilter::onAudioGenerationStarted(const BeatRange &range)
{
    UNUSED(range);
    _filter.init(
        DSP::Filter::FIRSpec {
            static_cast<DSP::Filter::BasicType>(filterType()),
            DSP::Filter::WindowType::Hanning,
            63ul,
            static_cast<float>(audioSpecs().sampleRate),
            { static_cast<float>(cutoffFrequencyFrom()), static_cast<float>(cutoffFrequencyTo()) },
            1.0f
        }
    );
    _cache.resize(GetFormatByteLength(audioSpecs().format) * audioSpecs().processBlockSize, audioSpecs().sampleRate, audioSpecs().channelArrangement, audioSpecs().format);
    _cache.clear();
}
constexpr auto PrintRangeClip = [](const Audio::BufferView buffer) {
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

inline void Audio::LambdaFilter::receiveAudio(BufferView output)
{
    float *out = output.data<float>();
    if (static_cast<bool>(byBass())) {
        std::memcpy(out, _cache.data<float>(), output.size<std::uint8_t>());
        return;
    }

    const DB outGain = ConvertDecibelToRatio(static_cast<float>(outputVolume()));

    // _filter._setGain(ConvertDecibelToRatio(static_cast<DB>(toto())));
    _filter.setCutoffs(static_cast<float>(cutoffFrequencyFrom()), static_cast<float>(cutoffFrequencyTo()));
    _filter.filter(_cache.data<float>(), audioSpecs().processBlockSize, out, outGain);

    // PrintRangeClip(output);
}

inline void Audio::LambdaFilter::sendAudio(const BufferViews &inputs)
{
    if (!inputs.size())
        return;
    const DB inGain = ConvertDecibelToRatio(static_cast<float>(
        static_cast<bool>(byBass()) ? inputGain() + outputVolume() : inputGain()
    ));
    DSP::Merge<float>(inputs, _cache, inGain, true);
}
